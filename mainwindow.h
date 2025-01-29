#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QLineEdit *inputAction;

    bool connectToDb();
    void createTableIfNotExist();

private slots:
    void saveToDb();
    void showAllActions();
};

#endif // MAINWINDOW_H
