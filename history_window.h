#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDir>
#include <QApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QStandardItem>
#include <QScrollBar>
#include <QWidget>
#include <QSqlDatabase>
#include <QStandardItemModel>

class QLineEdit;
class QPushButton;
class QTableView;

/**
 * @brief окно истории команд и ответов
 */
class HistoryWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief конструктор
     * @param parent родительский виджет
     */
    explicit HistoryWindow(QWidget *parent = nullptr);
    /**
     * @brief Деструктор
     */
    ~HistoryWindow() override;

    /**
     * @brief лог команд, ответа и прибора в бд
     * @param cmd команда
     * @param response ответ
     * @param device прибор
     */
    static void logCommand(const QString &cmd,
                           const QString &response,
                           const QString &device);

private slots:
    /**
     * @brief сохраняет введённую вручную команду
     */
    void saveToDb();

    /**
     * @brief обрабатка удаления записи по нажатию
     */
    void handleDeleteButton();

private:
    /**
     * @brief соединение с бд
     * @return true - удалось, else false
     */
    bool connectToDb();

    /**
     * @brief создание бд, если её нет
     */
    void createTableIfNotExist();

    /**
     * @brief настраивает модель данных и вид таблицы
     */
    void setupModelAndView();

    /**
     * @brief обновляет содержимое таблицы из бд
     */
    void refreshView();

    /**
     * @brief подгон ширины окна под содержимое таблицы
     */
    void adjustWindowSize();

    QSqlDatabase       db;          ///< соединение с бд
    QStandardItemModel *model;      ///< модель для таблицы
    QLineEdit          *inputAction;///< поле ввода команды
    QPushButton        *saveButton; ///< кнопка сохранения
    QTableView         *tableView;  ///< таблица истории

    /**
     * @brief общее соединение с бд для логирования
     */
    static QSqlDatabase& sharedDb();
};

#endif // HISTORY_WINDOW_H
