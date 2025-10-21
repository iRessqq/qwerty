#include "oscilloscope_manager.h"

#include <QString>

/**
 * @brief конструктор OscilloscopeManager
 */
OscilloscopeManager::OscilloscopeManager() : connectedFlag(FALSE) {}

/**
 * @brief деструктор OscilloscopeManager
 */
OscilloscopeManager::~OscilloscopeManager() {}

/**
 * @brief завершение при длительном ожидании соединения
 * @param sig номер сигнала
 */
void OscilloscopeManager::boredNow(int /*sig*/) {
    printf("Слишком долгое ожидание подключения, завершаем работу\n");
    exit(1);
}

/**
 * @brief отправка данных на осциллограф
 * @param sockfd дескриптор сокета
 * @param buf кказатель на буфер с данными
 * @return 0 при успехе, -1 при ошибке
 */
int OscilloscopeManager::writeData(int sockfd, char *buf) {
    LECROY_TCP_HEADER header;
    int result, bytes_more, bytes_xferd, tmp;
    char *idxPtr;

    BOOL eoi_flag = TRUE;
    int len = strlen(buf);

    if (connectedFlag != TRUE) return -1;

    // формируем заголовок пакета
    header.bEOI_Flag = LECROY_DATA_FLAG;
    header.bEOI_Flag |= (eoi_flag) ? LECROY_EOI_FLAG : 0;
    header.reserved[0] = 1;
    header.reserved[1] = 0;
    header.reserved[2] = 0;
    header.iLength = htonl(len);

    // отправляем заголовок
    tmp = write(sockfd, (char*)&header, sizeof(LECROY_TCP_HEADER));
    if (tmp != sizeof(LECROY_TCP_HEADER)) {
        printf("Не удалось записать заголовок, вернулось: %d\n", tmp);
        return -1;
    }

    // отправляем основные данные
    bytes_more = len;
    idxPtr = buf;
    bytes_xferd = 0;
    while (bytes_more > 0) {
        idxPtr = buf + bytes_xferd;
        result = write(sockfd, idxPtr, bytes_more);
        if (result < 0) {
            printf("Не удалось записать блок полностью, вернулось: %d\n", result);
            return -1;
        }
        bytes_xferd += result;
        bytes_more -= result;
    }

    return 0;
}

/**
 * @brief чтение данных с осциллографа
 * @param sockfd дескриптор сокета
 * @param buf указатель на буфер для хранения полученных данных
 * @param len размер буфера
 * @param allowable_time время ожидания данных в сек
 * @return 0 при успехе, -1 при ошибке
 */
int OscilloscopeManager::readData(int sockfd, char *buf, int len, int allowable_time) {
    LECROY_TCP_HEADER header;
    char tmpStr[512];
    int result, accum, space_left, bytes_more, buf_count;
    char *idxPtr;

    fd_set rfds;
    struct timeval tval;

    tval.tv_sec = allowable_time;
    tval.tv_usec = 0;

    if (connectedFlag != TRUE) return -1;

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    if (buf == NULL) return -1;

    memset(buf, 0, len);
    buf_count = 0;
    space_left = len;

    while (true) {
        result = select(sockfd + 1, &rfds, NULL, NULL, &tval);
        if (result < 0) {
            disconnectFromScope(sockfd);
            printf("Таймаут при чтении\n");
            return -1;
        }

        accum = 0;
        while (true) {
            memset(&header, 0, sizeof(LECROY_TCP_HEADER));

            result = read(sockfd, (char*)&header + accum, sizeof(header) - accum);
            if (result < 0) {
                disconnectFromScope(sockfd);
                printf("Ошибка при получении заголовка\n");
                return -1;
            }

            accum += result;
            if ((size_t)accum >= sizeof(header)) break;
        }

        header.iLength = ntohl(header.iLength);
        if (header.iLength < 1) return 0;

        if (header.iLength > space_left) {
            header.iLength = space_left;
            sprintf(tmpStr, "Размер буфера чтения (%d байт) слишком мал\n", len);
            printf(tmpStr);
        }

        accum = 0;
        while (true) {
            idxPtr = buf + (buf_count + accum);
            bytes_more = header.iLength - accum;
            if ((space_left - accum) < LECROY_TCP_MINIMUM_PACKET_SIZE) {
                disconnectFromScope(sockfd);
                printf("Размер буфера чтения слишком мал, минимум %d байт\n", LECROY_TCP_MINIMUM_PACKET_SIZE);
                return -1;
            }
            result = read(sockfd, idxPtr, (bytes_more > 2048) ? 2048 : bytes_more);
            if (result < 0) {
                disconnectFromScope(sockfd);
                printf("Ошибка при получении данных\n");
                return -1;
            }

            accum += result;
            if (accum >= header.iLength) break;
            if ((accum + buf_count) >= len) break;
        }
        buf_count += accum;
        space_left -= accum;

        if (header.bEOI_Flag & LECROY_EOI_FLAG) break;
        if (space_left <= 0) break;
    }

    return 0;
}

/**
 * @brief подключение к осциллографу
 * @param ip_address айпи осциллографа
 * @param allowable_delay время ожидания подключения в сек
 * @return дескриптор сокета при успехе, -1 ошибке, -2 уже подключено
 */
int OscilloscopeManager::connectToScope(char *ip_address, int allowable_delay) {
    int sockfd, tmp;
    struct sockaddr_in addr;

    if (connectedFlag == TRUE) {
        printf("Ошибка, уже подключено\n");
        return -2;
    }

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(LECROY_SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(ip_address);

    signal(SIGALRM, [](int sig){ OscilloscopeManager().boredNow(sig); });
    alarm(allowable_delay);

    tmp = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    alarm(0);

    if (tmp < 0) {
        printf("Ошибка, не удалось подключиться к осциллографу\n");
        return -1;
    }

    connectedFlag = TRUE;
    writeData(sockfd, (char*)"CORD LO\n");

    return sockfd;
}

/**
 * @brief отключение от осциллографа
 * @param sockfd дескриптор сокета
 * @return 0 при успехе, -1 если уже отключено
 */
int OscilloscopeManager::disconnectFromScope(int sockfd) {
    if (connectedFlag != TRUE) return -1;
    close(sockfd);
    connectedFlag = FALSE;
    return 0;
}

/**
 * @brief получение айпи осциллографа
 * @return айпи осциллографа в виде строки
 */
QString OscilloscopeManager::getIp() {
    return OUR_SCOPE_IP;
}
