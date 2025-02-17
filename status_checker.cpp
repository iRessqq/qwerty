#include "status_checker.h"

/**
 * @brief конструктор класса StatusChecker
 * @param parent указатель на родительский объект
 */
StatusChecker::StatusChecker(QObject *parent) : QThread(parent) {}

/**
 * @brief основной поток для проверки состояния
 */
void StatusChecker::run() {
    ViSession defaultRM, instr;
    ViStatus status;

    while (true) {
        //открытие сессии
        status = viOpenDefaultRM(&defaultRM);
        if (status == VI_SUCCESS) {
            //подключение через порт
            status = viOpen(defaultRM, "ASRL1::INSTR", VI_NULL, VI_NULL, &instr);
            if (status == VI_SUCCESS) {
                // отправка команды для проверки связи
                viPrintf(instr, "*IDN?\n");
                viFlush(instr, VI_WRITE_BUF);
                //буфер для ответа
                char buffer[256];
                ViUInt32 retCount;
                //получение ответа
                status = viRead(instr, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);
                //закрытие и отправка сигнала
                viClose(instr);
                emit statusChanged(status == VI_SUCCESS);
            } else {
                //если не удалось подключиться
                emit statusChanged(false);
            }
            viClose(defaultRM);
        } else {
            //если не удалось открыть сессию
            emit statusChanged(false);
        }
        //интервал проверки 2 сек
        QThread::sleep(2);
    }
}
