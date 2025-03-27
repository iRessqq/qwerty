#include "history_window.h"

#include <QDebug>
#include <QDir>

/**
 * @brief конструктор HistoryWindow
 * @param parent родительский виджет
 */
HistoryWindow::HistoryWindow(QWidget *parent)
    : QWidget(parent), model(nullptr)
{
    if (!connectToDb()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
        return;
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    inputAction = new QLineEdit(this);
    mainLayout->addWidget(inputAction);

    saveButton = new QPushButton("Сохранить", this);
    mainLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &HistoryWindow::saveToDb);

    tableView = new QTableView(this);
    mainLayout->addWidget(tableView);

    setupTableModel();
    showAllActions();
}

/**
 * @brief деструктор HistoryWindow
 */
HistoryWindow::~HistoryWindow()
{
    db.close();
}

/**
 * @brief подключение к базе данных
 * @return true если подключение успешно, иначе false
 */
bool HistoryWindow::connectToDb()
{
    QDir dir = QDir::current();
    QString dbPath = dir.filePath("log.db");
    qDebug() << "Путь к бд: " << dbPath;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Ошибка при подключении к базе данных: " << db.lastError().text();
        return false;
    }

    createTableIfNotExist();
    return true;
}

/**
 * @brief создание таблицы в базе данных, если ее нет
 */
void HistoryWindow::createTableIfNotExist()
{
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS actions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
                    "action TEXT)")) {
        qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
    }
}

/**
 * @brief сохранение действия в бд
 */
void HistoryWindow::saveToDb()
{
    QString actionText = inputAction->text();

    if (actionText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле ввода пустое");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO actions (action) VALUES (:action)");
    query.bindValue(":action", actionText);

    if (query.exec()) {
        inputAction->clear();
        model->select();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить: " + query.lastError().text());
    }
}

/**
 * @brief настройка модели для отображения данных в qtableview
 */
void HistoryWindow::setupTableModel()
{
    model = new QSqlTableModel(this);
    model->setTable("actions");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/**
 * @brief отображение всех действий из базы данных
 */
void HistoryWindow::showAllActions()
{
    model->select();
}
