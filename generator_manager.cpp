/**
 * @file visa_controller.cpp
 * @brief реализация visa_controller
 */

#include "visa_controller.h"
#include <visa.h>

/**
 * @brief VisaController управление генератором
 * @param command команда генератору
 * @return ответ генератора
 */
QString VisaController::sendCommand(const QString &command) {
    ViSession defaultRM, instr;
    ViStatus status;
    QString response;

    // открытие VISA
    status = viOpenDefaultRM(&defaultRM);
    if (status != VI_SUCCESS) {
        return "Не удалось установить соединение с VISA(проверить драйвер)";
    }

    // подключение через порт
    status = viOpen(defaultRM, "ASRL1::INSTR", VI_NULL, VI_NULL, &instr);
    if (status != VI_SUCCESS) {
        viClose(defaultRM);
        return "Не удалось подключиться к генератору";
    }

    // отправка команды на генератор
    viPrintf(instr, "%s\n", command.toUtf8().constData());
    viFlush(instr, VI_WRITE_BUF);

    /**
     * @brief если содержит "?" ожидается ответ
     */
    if (command.contains("?")) {
        char buffer[256]; ///< буфер для хранения ответа
        ViUInt32 retCount; ///< количество считанных байт

        // чтение ответа
        status = viRead(instr, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);
        if (status == VI_SUCCESS) {
            buffer[retCount] = '\0'; ///< завершение строки
            response = buffer; ///< сохранение ответа
        }
    }

    // закрытие
    viClose(instr);
    viClose(defaultRM);
    return response;
}
