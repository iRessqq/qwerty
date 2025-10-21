#include "history_window.h"

/**
 * @brief класс для окна истории команд
 * @param parent родительский виджет
 */
HistoryWindow::HistoryWindow(QWidget *parent)
    : QWidget(parent),
    model(nullptr)
{
    // попытка соединения с бд
    if (!connectToDb()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("не удалось подключиться к бд"));
        return;
    }

    // вертикальный layout
    auto *layout = new QVBoxLayout(this);

    // поле ввода
    inputAction = new QLineEdit(this);
    inputAction->setPlaceholderText(tr("введите команду..."));
    layout->addWidget(inputAction);

    // кнопка сохранения
    saveButton = new QPushButton(tr("сохранить вручную"), this);
    layout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &HistoryWindow::saveToDb);

    // таблица истории
    tableView = new QTableView(this);
    layout->addWidget(tableView);

    setupModelAndView();  // модель и колонки
    refreshView();        // заполнение из бд
    adjustWindowSize();   // подгон размера окна
}

/**
 * @brief деструктор
 */
HistoryWindow::~HistoryWindow()
{
    if (db.isOpen())
        db.close();
}

/**
 * @brief подключаемся к бд
 * @return true - удалось, else false
 */
bool HistoryWindow::connectToDb()
{
    //создаем соединение и log.db
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::current().filePath("log.db"));
    if (!db.open())
        return false;

    //при первом запуске создание
    createTableIfNotExist();
    return true;
}

/**
 * @brief создание бд, если её нет
 */
void HistoryWindow::createTableIfNotExist()
{
    QSqlQuery q(db);
    q.exec(
        "CREATE TABLE IF NOT EXISTS actions ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        " command TEXT,"
        " response TEXT,"
        " device TEXT)"
        );
}

/**
 * @brief настраивает модель данных и вид таблицы
 */
void HistoryWindow::setupModelAndView()
{
    // модель с 4 столбцами
    model = new QStandardItemModel(0, 4, this);

    //скрытие заголовка первой(там помойка)
    model->setHeaderData(0, Qt::Horizontal, QString());
    model->setHeaderData(1, Qt::Horizontal, tr("команда"));
    model->setHeaderData(2, Qt::Horizontal, tr("ответ"));
    model->setHeaderData(3, Qt::Horizontal, tr("устройство"));

    tableView->setModel(model);
    //tableView->verticalHeader()->hide();

    auto *h = tableView->horizontalHeader();
    //фиксированная ширина для стобца с помойкой
    h->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView->setColumnWidth(0, 30);
    //остальные ресайз под содержимое
    h->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    h->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    h->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}

/**
 * @brief обновляет содержимое таблицы из бд
 */
void HistoryWindow::refreshView()
{
    // очистка старые строки
    model->removeRows(0, model->rowCount());

    QSqlQuery q(db);
    q.exec("SELECT id, command, response, device FROM actions ORDER BY id");

    // картинка помойки для удаления
    QIcon icon;
    QString iconPath = QApplication::applicationDirPath() + "/icons/trash2.png";
    if (QFile::exists(iconPath))
        icon = QIcon(iconPath);

    int row = 0;
    while (q.next()) {
        int id       = q.value(0).toInt();
        QString cmd  = q.value(1).toString();
        QString resp = q.value(2).toString();
        QString dev  = q.value(3).toString();

        // пустой элемент для колонки удаления, хранит id
        QStandardItem *del = new QStandardItem;
        del->setData(id, Qt::UserRole);
        del->setText(QString());

        // создаём элементы для остальных колонок
        QStandardItem *c1 = new QStandardItem(cmd);
        QStandardItem *c2 = new QStandardItem(resp);
        QStandardItem *c3 = new QStandardItem(dev);

        model->insertRow(row, {del, c1, c2, c3});

        // создаём кнопку удаления и встраиваем в ячейку
        QModelIndex idx = model->index(row, 0);
        auto *btn = new QPushButton(tableView);
        btn->setIcon(icon);
        btn->setFlat(true);
        btn->setFixedSize(24,24);
        btn->setProperty("id", id);

        // контейнер и горизонтальный layout для кнопки
        QWidget *ctr = new QWidget(tableView);
        auto *hl = new QHBoxLayout(ctr);
        hl->setContentsMargins(0,0,0,0); //без отступов
        hl->addWidget(btn, 0, Qt::AlignCenter); //выравнивание по центру

        //контейнер вместо айди
        tableView->setIndexWidget(idx, ctr);
        //фиксированная высота(чтобы всегда помещалось)
        tableView->setRowHeight(row, 24);

        //нажатие к слоту удаления
        connect(btn, &QPushButton::clicked, this, &HistoryWindow::handleDeleteButton);
        ++row;
    }
}

/**
 * @brief обрабатка удаления записи по нажатию
 */
void HistoryWindow::handleDeleteButton()
{
    //получаем указатель на кнопку
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return; // на всякий проверочка

    //айди читаем
    int id = btn->property("id").toInt();

    // подтверждение
    if (QMessageBox::question(this, tr("удалить запись"),
                              tr("удалить запись id=%1?").arg(id),
                              QMessageBox::Yes|QMessageBox::No)
        != QMessageBox::Yes)
        return;

    // удаляем из бд и обновляем вид
    QSqlQuery q(db);
    q.prepare("DELETE FROM actions WHERE id = :id");
    q.bindValue(":id", id);
    q.exec();

    //обновление таблицы и окна
    refreshView();
    adjustWindowSize();
}

/**
 * @brief сохраняет введённую вручную команду
 */
void HistoryWindow::saveToDb()
{
    const QString txt = inputAction->text().trimmed();
    //если пустое поле
    if (txt.isEmpty()) {
        QMessageBox::warning(this, tr("ошибка"), tr("поле пустое"));
        return;
    }

    // вставляем новую запись
    QSqlQuery q(db);
    q.prepare("INSERT INTO actions(command,response,device) VALUES(:c,'',:d)");
    q.bindValue(":c", txt);
    q.bindValue(":d", "Manual");
    q.exec();

    //сброс поля и обновление таблицы и окна
    inputAction->clear();
    refreshView();
    adjustWindowSize();
}

/**
 * @brief лог команд, ответа и прибора в бд
 * @param cmd команда
 * @param response ответ
 * @param device прибор
 */
void HistoryWindow::logCommand(const QString &cmd,
                               const QString &response,
                               const QString &device)
{
    //вставка записи
    QSqlQuery q(sharedDb());
    q.prepare("INSERT INTO actions(command,response,device) VALUES(:c,:r,:d)");
    q.bindValue(":c", cmd);
    q.bindValue(":r", response);
    q.bindValue(":d", device);
    q.exec();

    // пробегаем по всем открытым окнам и обновляем
    for (auto *w : qApp->topLevelWidgets()) {
        if (auto *hw = qobject_cast<HistoryWindow*>(w)) {
            hw->refreshView();
            hw->adjustWindowSize();
            break;
        }
    }
}

/**
 * @brief подгон ширины окна под содержимое таблицы
 */
void HistoryWindow::adjustWindowSize()
{
    int columnsWidth = tableView->horizontalHeader()->length();
    int scrollbarWidth = tableView->verticalScrollBar()->isVisible()
                             ? tableView->verticalScrollBar()->width() : 0;
    int frameWidth = tableView->frameWidth() * 2;
    int totalWidth = columnsWidth + scrollbarWidth + frameWidth;

    //минимальная и фактическая ширина
    setMinimumWidth(totalWidth + 20);
    resize(totalWidth + 20, height());
}

/**
 * @brief общее соединение с бд для логирования
 */
QSqlDatabase& HistoryWindow::sharedDb()
{
    static QSqlDatabase s_db = [](){
        QSqlDatabase x = QSqlDatabase::addDatabase("QSQLITE", "shared");
        x.setDatabaseName(QDir::current().filePath("log.db"));
        x.open();
        QSqlQuery q(x);
        q.exec(
            "CREATE TABLE IF NOT EXISTS actions("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "command TEXT,"
            "response TEXT,"
            "device TEXT)"
            );
        return x;
    }();
    return s_db;
}
