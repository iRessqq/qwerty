# используемые модули
QT       += core gui widgets sql

# добавление widgets для Qt 5 и выше
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# поддержка C++17
CONFIG += c++17

# пути для заголовочных файлов NI-VISA
INCLUDEPATH += /usr/include/ni-visa

# библиотеки для работы с NI-VISA
LIBS += -L/usr/lib/x86_64-linux-gnu -lvisa

# исходные файлы
SOURCES += \
    main.cpp \
    mainwindow.cpp

# заголовочные файлы
HEADERS += \
    mainwindow.h

# ui файл
FORMS += \
    mainwindow.ui

# путь для установки
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
