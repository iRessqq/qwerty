#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>

/**
 * @class HistoryWindow
 * @brief окно истории действий оператора
 */
class HistoryWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief конструктор HistoryWindow
     * @param parent родительский виджет
     */
    explicit HistoryWindow(QWidget *parent = nullptr);

    /**
     * @brief деструктор HistoryWindow
     */
    ~HistoryWindow();

private slots:
    /**
     * @brief сохраняет действие в бд
     */
    void saveToDb();

private:
    /**
     * @brief соединение с бд
     * @return true, если соединение успешно установлено, иначе false
     */
    bool connectToDb();

    /**
     * @brief создание таблицы, если ее нет
     */
    void createTableIfNotExist();

    /**
     * @brief настраивает модель таблицы
     */
    void setupTableModel();

    /**
     * @brief отображает все записи
     */
    void showAllActions();

    QSqlDatabase db; ///< объект базы данных
    QSqlTableModel *model; ///< модель для работы с таблицей
    QTableView *tableView; ///< виджет для отображения истории действий
    QLineEdit *inputAction; ///< поле для ввода нового действия
    QPushButton *saveButton; ///< кнопка для сохранения
};

#endif // HISTORY_WINDOW_H
