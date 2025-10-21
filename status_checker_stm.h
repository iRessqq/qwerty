#ifndef STATUS_CHECKER_STM_H
#define STATUS_CHECKER_STM_H

#include <QThread>
#include <QString>

/**
 * @brief класс проверки подключения к порту платы
 */
class StatusCheckerStm : public QThread {
    Q_OBJECT

public:
    /**
     * @brief конструктор
     * @param portName имя порта
     * @param parent родительский объект
     * сохраняет имя порта, который будет опрашиваться в потоке
     */
    explicit StatusCheckerStm(const QString &portName, QObject *parent = nullptr);

signals:
    /**
     * @brief сигнал при первом детекте порта
     */
    void connectionDetected();

    /**
     * @brief сигнал при потере порта
     */
    void connectionLost();

protected:
    /**
     * @brief проверка в цикле списка портов
     */
    void run() override;

private:
    QString portName;  ///< системное имя порта, который проверяем
};

#endif // STATUS_CHECKER_STM_H
