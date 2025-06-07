/**
 * @file generat_manager.h
 * @brief заголовочный файл для работы с VISA
 */

#ifndef GENERATOR_MANAGER_H
#define GENERATOR_MANAGER_H

#include <QString>

/**
 * @class GeneratorManager
 * @brief управление генератором
 */
class GeneratorManager {
public:
    /**
     * @brief отправка команды генератору
     */
    QString sendCommand(const QString &command);
};

#endif // GENERATOR_MANAGER_H
