#ifndef GENERATOR_WIDGET_H
#define GENERATOR_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

#include "generator_manager.h"

/**
 * @class GeneratorWidget
 * @brief виджет для управления генератором
 * класс графического интерфейса для отправки команд генератору и получения ответов
 */
class GeneratorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief конструктор GeneratorWidget
     * @param parent родительский виджет
     */
    explicit GeneratorWidget(QWidget *parent = nullptr);

private slots:
    /**
     * @brief обработчик кнопки "отправить"
     * читает команду из поля и отправляет генератору
     */
    void onSendButtonClicked();

private:
    /**
     * @brief инициализирует элементы интерфейса
     */
    void showGeneratorWidget();

    QVBoxLayout *mainLayout; ///< основной макет
    QLabel *commandLabel; ///< надпись "Команда"
    QLineEdit *commandEdit; ///< поле ввода команды
    QPushButton *sendCommandButton; ///< кнопка "Отправить"
    QPlainTextEdit *text; ///< поле вывода ответов генератора

    GeneratorManager generatorManager; ///< объект для управления генератором.
};

#endif // GENERATOR_WIDGET_H
