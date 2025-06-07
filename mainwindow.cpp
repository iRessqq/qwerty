#include "mainwindow.h"
#include "oscilloscope_manager.h"
#include "status_checker_osc.h"
#include "generator_manager.h"
#include "status_checker_gen.h"
#include <QDebug>

/**
 * @brief Конструктор MainWindow
 * @param parent Родительский виджет
 *
 * инициализирует главный виджет, настраивает и подключает сигналы для кнопки подключения и поля ввода типа подключения
 */
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
    historyWindow(nullptr),
    generatorManager(nullptr),
    statusCheckerGen(nullptr)
{
    showMainWidget();
    connect(connectionButton, &QPushButton::clicked, this, &MainWindow::onConnectionButtonClicked);
    connect(typeOfConnectionEdit, &QLineEdit::returnPressed, this, &MainWindow::onConnectionButtonClicked);
}

/**
 * @brief инициализация главного виджета
 */
void MainWindow::showMainWidget()
{
    mainLayout = new QVBoxLayout(this);

    menuBar = new QMenuBar(this);
    QMenu *historyMenu = new QMenu("Меню", this);
    QAction *openHistoryAction = new QAction("История", this);
    connect(openHistoryAction, &QAction::triggered, this, &MainWindow::openHistoryWindow);
    historyMenu->addAction(openHistoryAction);
    menuBar->addMenu(historyMenu);
    mainLayout->setMenuBar(menuBar);

    typeOfConnectionLabel = new QLabel("Тип подключения", this);
    typeOfConnectionEdit = new QLineEdit(this);
    connectionButton = new QPushButton("Подключиться", this);

    oscilloscopeWidget = new OscilloscopeWidget();
    generatorWidget = new GeneratorWidget();

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(generatorWidget, "Генератор");
    tabWidget->addTab(oscilloscopeWidget, "Осциллограф");

    mainLayout->addWidget(typeOfConnectionLabel);
    mainLayout->addWidget(typeOfConnectionEdit);
    mainLayout->addWidget(connectionButton);
    mainLayout->addWidget(tabWidget);

    tabWidget->setTabEnabled(1, false);
    tabWidget->setTabEnabled(0, false);
}

/**
 * @brief скрытие виджета осциллографа
 */
void MainWindow::hideOscilloscopeWidget()
{
    tabWidget->setTabEnabled(1, false);
}

/**
 * @brief скрытие виджета генератора
 */
void MainWindow::hideGeneratorWidget()
{
    tabWidget->setTabEnabled(0, false);
}

/**
 * @brief отображение виджета генератора
 */
void MainWindow::showGeneratorWidget()
{
    tabWidget->setCurrentIndex(0);
    tabWidget->setTabEnabled(0, true);
}

/**
 * @brief отображение виджета осциллографа
 */
void MainWindow::showOscilloscopeWidget()
{
    tabWidget->setCurrentIndex(1);
    tabWidget->setTabEnabled(1, true);
}

/**
 * @brief обработчик нажатия кнопки подключения, проверка типа подключения и запуск статус чекера
 */
void MainWindow::onConnectionButtonClicked()
{
    QString str_typeOfConnection = typeOfConnectionEdit->text();
    OscilloscopeManager oscManager;
    QString oscilloscopeIP = oscManager.getIp();

    // подключение осциллографа
    if (oscilloscopeIP == str_typeOfConnection)
    {
        StatusCheckerOsc *statusCheckerOsc = new StatusCheckerOsc(str_typeOfConnection);
        statusCheckerOsc->start();
        connect(statusCheckerOsc, &StatusCheckerOsc::connectionDetected, this, &MainWindow::showOscilloscopeWidget);
        connect(statusCheckerOsc, &StatusCheckerOsc::connectionLost, this, &MainWindow::hideOscilloscopeWidget);
    }
    // подключение генератора
    else if (str_typeOfConnection == "ASRL1::INSTR")
    {
        if (!generatorManager) {
            generatorManager = new GeneratorManager();
        }

        QString response = generatorManager->sendCommand("*IDN?");
        if (!response.contains("Не удалось")) { // проверка успешного ответа
            showGeneratorWidget();

            if (!statusCheckerGen) {
                statusCheckerGen = new StatusCheckerGen();
                connect(statusCheckerGen, &StatusCheckerGen::connectionDetected, this, &MainWindow::showGeneratorWidget);
                connect(statusCheckerGen, &StatusCheckerGen::connectionLost, this, &MainWindow::hideGeneratorWidget);
                statusCheckerGen->start();
            }
        } else {
            qDebug() << "Ошибка: " << response;
        }
    }
}

/**
 * @brief открытие окна истории
 */
void MainWindow::openHistoryWindow()
{
    if (!historyWindow) {
        historyWindow = new HistoryWindow();
        historyWindow->setWindowTitle("История");
        historyWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(historyWindow, &HistoryWindow::destroyed, this, [this]() { historyWindow = nullptr; });
    }
    historyWindow->show();
}
