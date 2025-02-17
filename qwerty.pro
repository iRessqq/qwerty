# используемые модули
QT       += core gui widgets sql

# добавление widgets для Qt 5 и выше
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# поддержка C++17
CONFIG += c++17

# пути для заголовочных файлов NI-VISA
INCLUDEPATH += $$PWD/ni-visa/include/ni-visa

# библиотеки для работы с NI-VISA
LIBS += -L$$PWD/ni-visa/lib -lvisa

# исходные файлы
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    status_checker.cpp \
    visa_controller.cpp

# заголовочные файлы
HEADERS += \
    mainwindow.h \
    status_checker.h \
    visa_controller.h

# ui файл
FORMS += \
    mainwindow.ui

