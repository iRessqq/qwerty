#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

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

    QPushButton *viewButton = new QPushButton("показать все записи", this);
    layout->addWidget(viewButton);
    connect(viewButton, &QPushButton::clicked, this, &MainWindow::showAllActions);
}

bool MainWindow::connectToDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("log.db");

    if (!db.open()) {
        qDebug() << "ошибка" << db.lastError().text();
        return false;
    }
    return true;
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
    } else {
        QMessageBox::critical(this, "ошибка", "не сохранилось" + query.lastError().text());
    }
}

void MainWindow::showAllActions() {
    QSqlQuery query("SELECT * FROM actions");

    if (query.exec()) {
        QString allActions;
        while (query.next()) {
            int id = query.value(0).toInt();
            QString timestamp = query.value(1).toString();
            QString action = query.value(2).toString();
            allActions += QString("ID: %1, время: %2, действие: %3\n").arg(id).arg(timestamp).arg(action);
        }
        if (allActions.isEmpty()) {
            allActions = "нет записей в базе данных.";
        }
        QMessageBox::information(this, "все действия", allActions);
    } else {
        QMessageBox::critical(this, "ошибка", "нет записей" + query.lastError().text());
    }
}

