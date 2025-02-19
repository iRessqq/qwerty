/**
 * @file mainwindow.h
 * @brief заголовочный файл
 */

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
#include <QPlainTextEdit>

/**
 * @class MainWindow
 * @brief главное окно приложения
 *
 * интерфейс для взаимодействия с бд и отображения данных в таблице
 */
class MainWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief конструктор класса MainWindow
     * @param parent родительский виджет
     */
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QLineEdit *inputAction; ///< ввод действия
    QTableView *tableView; ///< отображение таблицы
    QSqlTableModel *model; ///< модель данных для таблицы
    QLineEdit *commandInput;    ///< поле для ввода команды
    QPlainTextEdit *responseOutput;  ///< поле для ответа генератора
    QLabel *statusLabel; ///< статус соединения

    /**
     * @brief подключение к бд
     * @return true - успешно, false - error
     */
    bool connectToDb();

    /**
     * @brief создание таблицы, если ее не существует
     */
    void createTableIfNotExist();

    /**
     * @brief настройка модели таблицы
     */
    void setupTableModel();

private slots:
    /**
     * @brief сохранение действия в бд
     */
    void saveToDb();
    /**
     * @brief обновление данных в таблице
     */
    void showAllActions();
    /**
     * @brief отправляет команду генератору
     */
    void sendCommandToGenerator();
};

#endif // MAINWINDOW_H
