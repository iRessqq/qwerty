#ifndef OSCILLOSCOPEMANAGER_H
#define OSCILLOSCOPEMANAGER_H

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>

#include <QString>

#define OUR_SCOPE_IP                  "172.18.56.236"
#define LECROY_SERVER_PORT            1861

#define CMD_BUF_LEN                   8192
#define MAX_TCP_CONNECT               5
#define MAX_TCP_READ                  3

#define BOOL                          int
#define TRUE                          1
#define FALSE                         0

#define LECROY_EOI_FLAG               0x01
#define LECROY_SRQ_FLAG               0x08
#define LECROY_CLEAR_FLAG             0x10
#define LECROY_LOCKOUT_FLAG           0x20
#define LECROY_REMOTE_FLAG            0x40
#define LECROY_DATA_FLAG              0x80

#define LECROY_READ_TIME_OUT          10
#define LECROY_TCP_MINIMUM_PACKET_SIZE 64

/**
 * @brief cтруктура заголовка lecroy tcp пакета
 */
typedef struct {
    unsigned char   bEOI_Flag;   ///< флаги управления
    unsigned char   reserved[3]; ///< заpарезервированные байты
    int             iLength;     ///< длина передаваемых данных
} LECROY_TCP_HEADER;

/**
 * @brief класс для управления подключением и обменом данными
 */
class OscilloscopeManager {
public:
    /**
     * @brief конструктор OscilloscopeManager
     */
    OscilloscopeManager();

    /**
     * @brief деструктор OscilloscopeManager
     */
    ~OscilloscopeManager();

    /**
     * @brief обработчик сигнала при длительном ожидании соединения
     * @param sig номер сигнала
     */
    void boredNow(int sig);

    /**
     * @brief отправка данных на осциллограф
     * @param sockfd дескриптор сокета
     * @param buf кказатель на буфер с данными
     * @return 0 при успехе, -1 при ошибке
     */
    int writeData(int sockfd, char *buf);

    /**
     * @brief чтение данных с осциллографа
     * @param sockfd дескриптор сокета
     * @param buf указатель на буфер для хранения полученных данных
     * @param len размер буфера
     * @param allowable_time время ожидания в сек
     * @return 0 при успехе, -1 при ошибке
     */
    int readData(int sockfd, char *buf, int len, int allowable_time);

    /**
     * @brief кодключение к осциллографу
     * @param ip_address айпи осциллографа
     * @param allowable_delay время ожидания подключения в сек
     * @return дескриптор сокета при успехе, -1 ошибке, -2 уже подключено
     */
    int connectToScope(char *ip_address, int allowable_delay);

    /**
     * @brief отключение от осциллографа
     * @param sockfd дескриптор сокета
     * @return 0 при успехе, -1 если уже отключено
     */
    int disconnectFromScope(int sockfd);

    /**
     * @brief получение айпи осциллографа
     * @return айпи осциллографа в виде строки
     */
    QString getIp();

private:
    BOOL connectedFlag; ///< флаг состояния подключения
};

#endif // OSCILLOSCOPE_MANAGER_H
