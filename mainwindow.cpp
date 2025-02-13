/**
 * @file mainwindow.cpp
 * @brief реализация класса MainWindow
 */

#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QTableView>
#include <QSqlTableModel>
#include <QHeaderView>

#include <visa.h>

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
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // вертикальный(левый) layout
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // поле для ввода комманды
    commandInput = new QLineEdit(this);
    leftLayout->addWidget(commandInput);

    // кнопка отправки команды
    QPushButton *generatorButton = new QPushButton("генератор", this);
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
    QPushButton *saveButton = new QPushButton("сохранить", this);
    rightLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveToDb);

    // таблица для отображения данных
    tableView = new QTableView(this);
    rightLayout->addWidget(tableView);

    // левый и правый layout в основной layout
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);

    // настройка модели и заполнение таблицы
    setupTableModel();
    showAllActions();
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
 * @brief обновление данных в таблице
 */
void MainWindow::showAllActions() {
    if (model) {
        model->select();
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

    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}

/**
 * @brief отправка команды генератору и отображание ответа
 */
void MainWindow::sendCommandToGenerator() {
    responseOutput->clear();

    ViSession defaultRM, instr;
    ViStatus status;

    // открытие VISA
    status = viOpenDefaultRM(&defaultRM);
    if (status != VI_SUCCESS) {
        responseOutput->setPlainText("не удалось открыть VISA");
        return;
    }

    // подключение через порт
    status = viOpen(defaultRM, "ASRL1::INSTR", VI_NULL, VI_NULL, &instr);
    if (status != VI_SUCCESS) {
        responseOutput->setPlainText("не удалось подключиться к генератору");
        viClose(defaultRM);
        return;
    }

    // получение команды(isempty *IDN?)
    QString command = commandInput->text().isEmpty() ? "*IDN?" : commandInput->text();
    viPrintf(instr, "%s\n", command.toUtf8().constData());
    viFlush(instr, VI_WRITE_BUF);

    // чтение ответа
    char buffer[256];
    ViUInt32 retCount;
    status = viRead(instr, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);

    if (status != VI_SUCCESS) {
        responseOutput->setPlainText("тайм-аут или нет ответа от генератора");
    } else {
        buffer[retCount] = '\0';  // завершение строки
        responseOutput->setPlainText(QString("Ответ: ") + buffer);
    }

    // закрытие
    viClose(instr);
    viClose(defaultRM);
}

