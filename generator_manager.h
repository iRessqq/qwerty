/**
 * @file visa_controller.h
 * @brief заголовочный файл для работы с VISA
 */

#ifndef VISA_CONTROLLER_H
#define VISA_CONTROLLER_H

#include <QString>

/**
 * @class VisaController
 * @brief управление генератором
 */
class VisaController {
public:
    /**
     * @brief отправка команды генератору
     */
    QString sendCommand(const QString &command);
};

#endif // VISA_CONTROLLER_H
