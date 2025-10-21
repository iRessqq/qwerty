#include "stm_manager.h"
#include "history_window.h"

#include <QThread>
#include <QDebug>
//#include <boost/crc.hpp>

/**
 * @brief конструктор класса работы с stm
 */
StmManager::StmManager(QObject *parent) : QObject(parent)
{
    // скорость передачи 115200
    serial.setBaudRate(QSerialPort::Baud115200);
    // 8 бит данных
    serial.setDataBits(QSerialPort::Data8);
    // без контроля четности
    serial.setParity(QSerialPort::NoParity);
    // 1 стоп-бит
    serial.setStopBits(QSerialPort::OneStop);
    // без управления потоком
    serial.setFlowControl(QSerialPort::NoFlowControl);
}

/**
 * @brief деструктор класса работы с stm
 */
StmManager::~StmManager()
{
    if (serial.isOpen())
        serial.close();
}

/**
 * @brief подключиться к порту
 * @param portName имя порта
 * @return true - удалось, else false
 */
bool StmManager::connectToPort(const QString &portName)
{
    // закрывает если открыт(так надо)
    if (serial.isOpen())
        serial.close();

    // устанавливает имя порта
    serial.setPortName(portName);
    // пробуем открыть порт в режиме чтения/записи
    if (serial.open(QIODevice::ReadWrite)) {
        //qDebug() << "подключено к порту:" << portName;
        return true;
    } else {
        qDebug() << "не удалось открыть порт:" << serial.errorString();
        return false;
    }
}

/**
 * @brief проверка открытия порта
 * @return true - удалось, else false
 */
bool StmManager::isConnected() const
{
    return serial.isOpen();
}

/**
 * @brief отправка команды на плату и получение ответа
 * @param operation операция (1 байт)
 * @param object объект (3 байта)
 * @param data данные (N байт)
 * @return ответ от платы или пустой массив
 */
QByteArray StmManager::sendCommand(quint8 operation, quint32 object, const QByteArray &data)
{
    // если порт не открыт
    if (!serial.isOpen())
        return {};

    // очистка буфера
    serial.clear(QSerialPort::Input);

    // формируем пакет
    QByteArray packet = buildPacket(operation, object, data);
    //qDebug() << "-> send:" << packet.toHex(' ').toUpper();
    serial.write(packet);

    // запись в порт (таймаут 100 мс)
    if (!serial.waitForBytesWritten(100)) {
        qDebug() << "ошибка записи:" << serial.errorString();
        return {};
    }

    // данные во входном буфере (таймаут 100 мс)
    if (!serial.waitForReadyRead(100)) {
        qDebug() << "нет ответа от STM";
        return {};
    }

    // данные из порта
    QByteArray response = serial.readAll();
    // чтение до таймаута
    while (serial.waitForReadyRead(100))
        response += serial.readAll();

    // команду и ответ и прибор в бд
    HistoryWindow::logCommand(
        QString::fromLatin1(packet.toHex(' ').toUpper()),
        QString::fromLatin1(response.toHex(' ').toUpper()),
        QStringLiteral("STM")
        );

    //qDebug() << "<- resp:" << response.toHex(' ').toUpper();
    return response;
}

/**
 * @brief формирование пакета для передачи плате
 * @param operation операция (1 байт)
 * @param object объект (3 байта)
 * @param data данные (N байт)
 * @return сформированный пакет
 */
QByteArray StmManager::buildPacket(quint8 operation, quint32 object, const QByteArray &data)
{
    QByteArray p;
    // DST(4) + SRC(4)
    p.append(char(0x01)); p.append(char(0x00));
    p.append(char(0x01)); p.append(char(0x10));
    p.append(char(0x00)); p.append(char(0x00));
    p.append(char(0x00)); p.append(char(0x01));

    // длина: op(1) + obj(3) + data(N) + CRC(2)
    quint16 totalLength = 1 + 3 + data.size() + 2;
    if (totalLength < 8) totalLength = 8;
    // длина в пакет
    p.append(char(totalLength & 0xFF));
    p.append(char((totalLength >> 8) & 0xFF));

    // операция
    p.append(char(operation));

    // объект в формате LSB → MSB (3 байта)
    p.append(char( object        & 0xFF)); // obj0 - LSB
    p.append(char((object >>  8) & 0xFF)); // obj1
    p.append(char((object >> 16) & 0xFF)); // obj2

    // добавляем данные
    p.append(data);

    // расчет crc по всему пакету и добавление
    quint16 crc = calculateCrc(p);
    p.append(char(crc & 0xFF));
    p.append(char((crc >> 8) & 0xFF));

    return p;
}

/**
 * @brief расчет crc
 * @param data массив байт для вычисления crc
 * @return 16-битное значение crc
 */
quint16 StmManager::calculateCrc(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (auto raw_b : data) {
        crc ^= static_cast<quint8>(raw_b);
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}
