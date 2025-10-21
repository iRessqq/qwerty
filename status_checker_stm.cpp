#include "status_checker_stm.h"

#include <QSerialPortInfo>
#include <QDebug>

/**
 * @brief класс проверки подключения к порту платы
 */
StatusCheckerStm::StatusCheckerStm(const QString &port, QObject *parent)
    : QThread(parent), portName(port)
{
    // конструктор сохраняет имя порта
}

/**
 * @brief поток при запуске
 */
void StatusCheckerStm::run()
{
    // флаг для ранее обнаруженного порта
    bool wasPresent = false;

    // цикл проверки
    while (true) {
        // получение доступных портов и сравнение с нужным
        bool found = false;
        for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
            if (info.systemLocation() == portName) {
                found = true;
                break;
            }
        }

        // если порт появился
        if (found && !wasPresent) {
            wasPresent = true;
            // сигнал о подключении
            emit connectionDetected();
            //qDebug() << "stm подключена";
        }
        // если порт исчез
        else if (!found && wasPresent) {
            wasPresent = false;
            // сигнал о потере соединения
            emit connectionLost();
            //qDebug() << "stm отключена";
        }

        // 1 сек между проверками
        sleep(1);
    }
}
