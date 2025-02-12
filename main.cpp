/**
 * @file main.cpp
 * @brief основной файл
 */

#include <QApplication>
#include "mainwindow.h"

/**
 * @brief главная ф-я программы
 * @param argc количество аргументов строки
 * @param argv массив аргументов строки
 * @return код завершения приложения
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show(); //главное окно

    return app.exec();
}
