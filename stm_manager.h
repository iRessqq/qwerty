#ifndef STM_MANAGER_H
#define STM_MANAGER_H

#include <QObject>
#include <QSerialPort>

/**
 * @brief менеджер для работы с платой
 */
class StmManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief конструктор
     */
    explicit StmManager(QObject *parent = nullptr);
    /**
     * @brief деструктор класса работы с stm
     */
    ~StmManager();

    /**
     * @brief подключиться к порту
     * @param portName имя порта
     * @return true - удалось, else false
     */
    bool connectToPort(const QString &portName);

    /**
     * @brief проверка открытия порта
     * @return true - удалось, else false
     */
    bool isConnected() const;

    /**
     * @brief отправка команды на плату и получение ответа
     * @param operation операция (1 байт)
     * @param object объект (3 байта)
     * @param data данные (N байт)
     * @return ответ от платы или пустой массив
     */
    QByteArray sendCommand(quint8 operation, quint32 object, const QByteArray &data);

    /**
     * @brief формирование пакета для передачи плате
     * @param operation операция (1 байт)
     * @param object объект (3 байта)
     * @param data данные (N байт)
     * @return сформированный пакет
     */
    QByteArray buildPacket(quint8 operation, quint32 object, const QByteArray &data);

private:
    QSerialPort serial;  ///< объект для работы с ком портом

    /**
     * @brief расчет crc
     * @param data массив байт для вычисления crc
     * @return 16-битное значение crc
     */
    quint16 calculateCrc(const QByteArray &data);
};

#endif // STM_MANAGER_H
