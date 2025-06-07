#ifndef STATUS_CHECKER_OSC_H
#define STATUS_CHECKER_OSC_H

#include <QThread>
#include <QTimer>

/**
 * @class StatusCheckerOsc
 * @brief класс для проверки состояния соединения с осциллографом
 */
class StatusCheckerOsc : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief конструктор StatusCheckerOsc
     * @param ip айпи осциллографа
     * @param parent родительский объект
     */
    StatusCheckerOsc(const QString &ip, QObject *parent = nullptr);

    /**
     * @brief основной поток проверки состояния
     */
    void run() override;

private:
    QString oscilloscopeIP;  ///< айпи осциллографа

signals:
    /**
     * @brief сигнал при потере соединения с осциллографом
     */
    void connectionLost();

    /**
     * @brief сигнал при успешном подключении к осциллографу
     */
    void connectionDetected();
};

#endif // STATUS_CHECKER_OSC_H
