#ifndef STATUS_CHECKER_GEN_H
#define STATUS_CHECKER_GEN_H

#include <QThread>
#include <visa.h>

/**
 * @class StatusCheckerGen
 * @brief класс для проверки состояния соединения с генератором
 */
class StatusCheckerGen : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief конструктор StatusCheckerGen
     * @param parent родительский объект
     */
    explicit StatusCheckerGen(QObject *parent = nullptr);

    /**
     * @brief деструктор StatusCheckerGen
     */
    ~StatusCheckerGen();

signals:
    /**
     * @brief сигнал при успешном подключении генератора
     */
    void connectionDetected();

    /**
     * @brief сигнал при потере соединения с генератором
     */
    void connectionLost();

protected:
    /**
     * @brief основной поток проверки состояния
     */
    void run() override;

private:
    /**
     * @brief проверяет подключение к генератору
     * @return true если генератор подключён, false в противном случае
     */
    bool isConnected();
};

#endif // STATUS_CHECKER_GEN_H
