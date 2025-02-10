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

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent) {

    if (!connectToDb()) {
        QMessageBox::critical(this, "ошибка", "не подключен к бд");
        exit(1);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);

    inputAction = new QLineEdit(this);
    layout->addWidget(inputAction);

    QPushButton *saveButton = new QPushButton("сохранить", this);
    layout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveToDb);

    tableView = new QTableView(this);
    layout->addWidget(tableView);

    setupTableModel();
    showAllActions();
}

bool MainWindow::connectToDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("log.db");
    if (!db.open()) {
        qDebug() << "ошибка" << db.lastError().text();
        return false;
    }

    createTableIfNotExist();

    return true;
}

void MainWindow::createTableIfNotExist() {
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS actions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
                    "action TEXT)")) {
        qDebug() << "ошибка" << query.lastError().text();
    }
}

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

void MainWindow::showAllActions() {
    model->select();
}

void MainWindow::setupTableModel() {
    model = new QSqlTableModel(this);
    model->setTable("actions");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    tableView->setModel(model);

    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}


