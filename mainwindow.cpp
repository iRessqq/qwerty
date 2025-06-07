#include "mainwindow.h"
#include "oscilloscope_manager.h"
#include "status_checker_osc.h"
#include "generator_manager.h"
#include "status_checker_gen.h"

#include <QDebug>

/**
 * @brief конструктор MainWindow
 * @param parent родительский виджет
 */
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
    historyWindow(nullptr),
    generatorManager(nullptr),
    statusCheckerGen(nullptr),
    statusCheckerStm(nullptr)
{
    showMainWidget();
    connect(connectionButton, &QPushButton::clicked, this, &MainWindow::onConnectionButtonClicked);
    connect(typeOfConnectionEdit, &QLineEdit::returnPressed, this, &MainWindow::onConnectionButtonClicked);
    resize(301, 600); //жижа
    //adjustSize();
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
    stmWidget = new StmWidget();

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(generatorWidget, "Генератор");
    tabWidget->addTab(oscilloscopeWidget, "Осциллограф");
    tabWidget->addTab(stmWidget, "Плата");

    mainLayout->addWidget(typeOfConnectionLabel);
    mainLayout->addWidget(typeOfConnectionEdit);
    mainLayout->addWidget(connectionButton);
    mainLayout->addWidget(tabWidget);

    tabWidget->setTabEnabled(0, false); // генератор
    tabWidget->setTabEnabled(1, false); // осциллограф
    tabWidget->setTabEnabled(2, false); // плата
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
 * @brief скрытие виджета платы
 */
void MainWindow::hideStmWidget()
{
    tabWidget->setTabEnabled(2, false);
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
 * @brief отображение виджета платы
 */
void MainWindow::showStmWidget()
{
    tabWidget->setTabEnabled(2, true);
    tabWidget->setCurrentIndex(2);
}

/**
 * @brief обработчик нажатия кнопки подключения, проверка типа подключения и запуск статус чекера
 */
void MainWindow::onConnectionButtonClicked()
{
    QString connType = typeOfConnectionEdit->text().trimmed();
    OscilloscopeManager oscManager;
    QString oscilloscopeIP = oscManager.getIp();

    if (connType == oscilloscopeIP)
    {
        auto *statusCheckerOsc = new StatusCheckerOsc(connType);
        connect(statusCheckerOsc, &StatusCheckerOsc::connectionDetected, this, &MainWindow::showOscilloscopeWidget);
        connect(statusCheckerOsc, &StatusCheckerOsc::connectionLost, this, &MainWindow::hideOscilloscopeWidget);
        statusCheckerOsc->start();
    }
    else if (connType == "ASRL33::INSTR")
    {
        if (!generatorManager)
            generatorManager = new GeneratorManager();

        QString response = generatorManager->sendCommand("*IDN?");
        if (!response.contains("Не удалось"))
        {
            showGeneratorWidget();
            if (!statusCheckerGen)
            {
                statusCheckerGen = new StatusCheckerGen();
                connect(statusCheckerGen, &StatusCheckerGen::connectionDetected, this, &MainWindow::showGeneratorWidget);
                connect(statusCheckerGen, &StatusCheckerGen::connectionLost, this, &MainWindow::hideGeneratorWidget);
                statusCheckerGen->start();
            }
        }
        else
        {
            qDebug() << "Ошибка генератора: " << response;
        }
    }

    else if (connType == "/dev/ttyUSB0") {
        if (!statusCheckerStm) {
            statusCheckerStm = new StatusCheckerStm("/dev/ttyUSB0", this);
            connect(statusCheckerStm, &StatusCheckerStm::connectionDetected,
                    this, &MainWindow::showStmWidget);
            connect(statusCheckerStm, &StatusCheckerStm::connectionLost,
                    this, &MainWindow::hideStmWidget);
            statusCheckerStm->start();
        }
    }

    else
    {
        qDebug() << "Неизвестный тип подключения: " << connType;
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
