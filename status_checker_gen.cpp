#include "status_checker_gen.h"
#include <QThread>

/**
 * @brief конструктор StatusCheckerGen
 * @param parent родительский объект
 */
StatusCheckerGen::StatusCheckerGen(QObject *parent) : QThread(parent) {}

/**
 * @brief деструктор StatusCheckerGen
 */
StatusCheckerGen::~StatusCheckerGen() {
    requestInterruption();  // запрос завершения потока
    quit();
    wait();  // ожидание завершения потока
}

/**
 * @brief запуск фонового потока проверки состояния генератора
 */
void StatusCheckerGen::run() {
    bool wasConnected = false;  // хранит предыдущее состояние подключения

    while (!isInterruptionRequested()) {  // пока не запросили остановку
        bool connected = isConnected();  // проверка подключение

        if (connected && !wasConnected) {
            emit connectionDetected();  // сигнал если подключился впервые
        } else if (!connected && wasConnected) {
            emit connectionLost();  // сигнал если потеряли связь
        }

        wasConnected = connected;  // обновляет состояние
        QThread::sleep(2);  // каждые 2 сек
    }
}

/**
 * @brief проверка соединения с генератором
 * @return true, если соединение установлено, иначе false
 */
bool StatusCheckerGen::isConnected() {
    ViSession defaultRM, instr;
    ViStatus status;

    // открытие сессии VISA
    status = viOpenDefaultRM(&defaultRM);
    if (status != VI_SUCCESS) {
        return false;  // не удалось открыть сессию
    }

    // подключение через порт
    status = viOpen(defaultRM, "ASRL1::INSTR", VI_NULL, VI_NULL, &instr);
    if (status != VI_SUCCESS) {
        viClose(defaultRM);
        return false;  // не удалось подключиться к генератору
    }

    // отправка idn для проверки связи
    viPrintf(instr, "*IDN?\n");
    viFlush(instr, VI_WRITE_BUF);

    // буфер для ответа
    char buffer[256];
    ViUInt32 retCount;
    status = viRead(instr, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);

    // закрытие сессии
    viClose(instr);
    viClose(defaultRM);

    return status == VI_SUCCESS;  // если успешно выполнена — подключение есть
}
