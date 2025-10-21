# используемые модули
QT       += core gui widgets sql serialport

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
    generator_manager.cpp \
    generator_widget.cpp \
    history_window.cpp \
    main.cpp \
    mainwindow.cpp \
    oscilloscope_manager.cpp \
    oscilloscope_widget.cpp \
    status_checker_gen.cpp \
    status_checker_osc.cpp \
    status_checker_stm.cpp \
    stm_manager.cpp \
    stm_widget.cpp

# заголовочные файлы
HEADERS += \
    generator_manager.h \
    generator_widget.h \
    history_window.h \
    mainwindow.h \
    oscilloscope_manager.h \
    oscilloscope_widget.h \
    status_checker_gen.h \
    status_checker_osc.h \
    status_checker_stm.h \
    stm_manager.h \
    stm_widget.h

