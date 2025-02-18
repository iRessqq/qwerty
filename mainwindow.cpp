/**
 * @file mainwindow.cpp
 * @brief реализация класса MainWindow
 */

#include "mainwindow.h"
#include "status_checker.h"
#include "visa_controller.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QSqlTableModel>
#include <QHeaderView>

/**
 * @brief конструктор MainWindow, создает интерфейс
 * @param parent родительский виджет
 */
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), tableView(nullptr), model(nullptr) {

    // проверка подключения к бд
    if (!connectToDb()) {
        QMessageBox::critical(this, "ошибка", "не подключен к бд");
        exit(1);
    }

    // горизонтальный(основной) layout
    QHBoxLayout *mainLayout = new QHBoxLayout();

    // вертикальный(левый) layout
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // поле для ввода комманды
    commandInput = new QLineEdit(this);
    leftLayout->addWidget(commandInput);

    // кнопка отправки команды
    QPushButton *generatorButton = new QPushButton("Генератор", this);
    leftLayout->addWidget(generatorButton);
    connect(generatorButton, &QPushButton::clicked, this, &MainWindow::sendCommandToGenerator);

    // поле для ответа генератора
    responseOutput = new QPlainTextEdit(this);
    responseOutput->setReadOnly(true);
    leftLayout->addWidget(responseOutput);

    // вертикальный(правый) layout
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // поле для ввода действия
    inputAction = new QLineEdit(this);
    rightLayout->addWidget(inputAction);

    // кнопка сохранения
    QPushButton *saveButton = new QPushButton("Сохранить", this);
    rightLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveToDb);

    // таблица для отображения данных
    tableView = new QTableView(this);
    rightLayout->addWidget(tableView);

    // layout для статуса генератора
    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLabel = new QLabel("Проверка...", this);
    statusLayout->addStretch(); // сдвиг
    statusLayout->addWidget(statusLabel);

    // вертикальный layout со всем
    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(this);
    mainVerticalLayout->addLayout(mainLayout);
    mainVerticalLayout->addLayout(statusLayout);

    // левый и правый layout в основной layout
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);

    // настройка модели и заполнение таблицы
    setupTableModel();
    showAllActions();

    // создание объекта проверки соединения
    StatusChecker *checker = new StatusChecker(this);

    // подключение сигнала от StatusChecker к QLabel
    connect(checker, &StatusChecker::statusChanged, this, [this](bool connected) {
        statusLabel->setText(connected ? "Подключен" : "Отключен");
    });

    // запуск потока
    checker->start();
}

/**
 * @brief подключение к бд
 * @return true - успешно, false - error
 */
bool MainWindow::connectToDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("log.db");
    if (!db.open()) {
        qDebug() << "ошибка" << db.lastError().text();
        return false;
    }

    //создание таблицы, если ее нет
    createTableIfNotExist();

    return true;
}

/**
* @brief создание таблицы, если ее не существует
 */
void MainWindow::createTableIfNotExist() {
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS actions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
                    "action TEXT)")) {
        qDebug() << "ошибка" << query.lastError().text();
    }
}

/**
 * @brief сохранение действия в бд
 */
void MainWindow::saveToDb() {
    QString actionText = inputAction->text();

    if (actionText.isEmpty()) {
        QMessageBox::warning(this, "ошибка", "пустое поле");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO actions (action) VALUES (:action)");
    query.bindValue(":action", actionText);

    if (query.exec()) {
        inputAction->clear();
        model->select();
    } else {
        QMessageBox::critical(this, "ошибка", "не сохранилось" + query.lastError().text());
    }
}


/**
 * @brief настраивает модель таблицы
 */
void MainWindow::setupTableModel() {
    if (!model) {
        model = new QSqlTableModel(this);
        model->setTable("actions");
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
    }

    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


/**
 * @brief отправка команды генератору и отображание ответа
 */
void MainWindow::sendCommandToGenerator() {
    responseOutput->clear();
    VisaController visa;

    QString command = commandInput->text().isEmpty() ? "*IDN?" : commandInput->text();
    QString response = visa.sendCommand(command);

    if (response.isEmpty() && !command.contains("?")) {
        responseOutput->setPlainText("Команда отправлена:)");
    } else if (response.isEmpty()) {
        responseOutput->setPlainText("Генератор не отвечает");
    } else {
        responseOutput->setPlainText("Ответ: " + response);
    }
}

/**
 * @brief обновление данных в таблице
 */
void MainWindow::showAllActions() {
    model->select();
}
