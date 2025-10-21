#include "status_checker_osc.h"
#include "oscilloscope_manager.h"

#include <QDebug>

/**
 * @brief конструктор StatusCheckerOsc
 * @param ip айпи осциллографа
 * @param parent родительский объект
 */
StatusCheckerOsc::StatusCheckerOsc(const QString &ip, QObject *parent)
    : QThread(parent), oscilloscopeIP(ip)
{
}

/**
 * @brief метод для выполнения проверки соединения с осциллографом в фоновом потоке
*/
void StatusCheckerOsc::run()
{
    OscilloscopeManager oscilloscope;
    const int MAX_ST = 512;        ///< макс размер буфера для отправки данных
    char outbuf[MAX_ST] = "*IDN?\n"; ///< буфер для отправки idn
    char inbuf[MAX_ST] = {0};      ///< буфер для получения данных
    int sockfd;

    while (true)
    {
        sockfd = -1;

        // пытаемся подключиться
        if (sockfd == -1)
        {
            sockfd = oscilloscope.connectToScope(oscilloscopeIP.toUtf8().data(), MAX_TCP_CONNECT);
            if (sockfd > 0)
            {
                //qDebug() << "1";  // соединение установлено
            }
            else
            {
                emit connectionLost();  // сигнал о потере соединения
                //qDebug() << "2";
                oscilloscope.disconnectFromScope(sockfd);  // закрываем соединение
                sockfd = -1;  // сбрасываем сокет
            }
        }

        // если соединение установлено - команда и ответ
        if (sockfd != -1)
        {
            oscilloscope.writeData(sockfd, outbuf);   // отправляем команду
            oscilloscope.readData(sockfd, inbuf, sizeof(inbuf), MAX_TCP_READ);  // читаем ответ

            if (strlen(inbuf) == 0)  // если нет ответа -соединение
            {
                //qDebug() << "3";
                emit connectionLost();  // -соединения
                oscilloscope.disconnectFromScope(sockfd);  // закрываем соединение
            }
            else
            {
                //qDebug() << "4";
                emit connectionDetected();  // успешное подключении
                oscilloscope.disconnectFromScope(sockfd);  // закрываем соединение
            }
        }

        msleep(2500);  // проверка каждый 2.5 сек
    }
}
