#ifndef STATUS_CHECKER_H
#define STATUS_CHECKER_H

#include <QObject>
#include <QThread>
#include <visa.h>

/**
 * @brief проверка связи с генератором в отдельном потоке
 */
class StatusChecker : public QThread {
    Q_OBJECT

public:
    explicit StatusChecker(QObject *parent = nullptr);
    void run() override; ///< запуск потока

signals:
    void statusChanged(bool isConnected); ///< сигнал при изменении статуса
};

#endif // STATUS_CHECKER_H
