/**
 * @file mainwindow.cpp
 * @brief реализация класса MainWindow
 */

#include "mainwindow.h"

#include <QVBoxLayout>
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

/**
 * @brief конструктор MainWindow, создает интерфейс
 * @param parent родительский виджет
 */
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent) {

    // проверка подключения к бд
    if (!connectToDb()) {
        QMessageBox::critical(this, "ошибка", "не подключен к бд");
        exit(1);
    }

    // вертикальный layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // поле для ввода
    inputAction = new QLineEdit(this);
    layout->addWidget(inputAction);

    // кнопка сохранения
    QPushButton *saveButton = new QPushButton("сохранить", this);
    layout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveToDb);

    // таблица для отображения данных
    tableView = new QTableView(this);
    layout->addWidget(tableView);

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
    model->select();
}

/**
 * @brief настраивает модель таблицы
 */
void MainWindow::setupTableModel() {
    model = new QSqlTableModel(this);
    model->setTable("actions");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    tableView->setModel(model);

    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}


