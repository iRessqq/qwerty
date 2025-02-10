#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>
#include <QSqlTableModel>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QLineEdit *inputAction;
    QTableView *tableView;
    QSqlTableModel *model;

    bool connectToDb();
    void createTableIfNotExist();
    void setupTableModel();

private slots:
    void saveToDb();
    void showAllActions();
};

#endif // MAINWINDOW_H
