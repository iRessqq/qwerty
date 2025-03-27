#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "oscilloscope_widget.h"
#include "generator_widget.h"
#include "history_window.h"
#include "generator_manager.h"
#include "status_checker_gen.h"

/**
 * @class MainWindow
 * @brief главное окно приложения
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief конструктор MainWindow
     * @param parent родительский виджет
     */
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    /**
     * @brief открывает окно истории действий
     */
    void openHistoryWindow();

    /**
     * @brief обработчик нажатия кнопки подключения
     */
    void onConnectionButtonClicked();

private:
    /**
     * @brief инициализирует главное окно
     */
    void showMainWidget();

    /**
     * @brief отображает вкладку осциллографа
     */
    void showOscilloscopeWidget();

    /**
     * @brief отображает вкладку генератора
     */
    void showGeneratorWidget();

    /**
     * @brief скрывает вкладку осциллографа
     */
    void hideOscilloscopeWidget();

    /**
     * @brief скрывает вкладку генератора
     */
    void hideGeneratorWidget();

    QVBoxLayout *mainLayout; ///< основной макет главного окна
    QLabel *typeOfConnectionLabel; ///< метка для ввода типа подключения
    QLineEdit *typeOfConnectionEdit; ///< поле ввода для типа подключения
    QPushButton *connectionButton; ///< кнопка для установления соединения
    QTabWidget *tabWidget; ///< вкладки осциллографа и генератора
    QMenuBar *menuBar; ///< меню главного окна
    HistoryWindow *historyWindow; ///< окно истории действий
    OscilloscopeWidget *oscilloscopeWidget; ///< виджет осциллографа
    GeneratorWidget *generatorWidget; ///< виджет генератора
    GeneratorManager *generatorManager; ///< объект для управления генератором
    StatusCheckerGen *statusCheckerGen; ///< поток для проверки состояния генератора
};

#endif // MAINWINDOW_H
