#ifndef OSCILLOSCOPE_WIDGET_H
#define OSCILLOSCOPE_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStackedWidget>
#include <QPlainTextEdit>
#include <QPushButton>

#include "oscilloscope_manager.h"

/**
 * @class OscilloscopeWidget
 * @brief виджет для управления осциллографом
 */
class OscilloscopeWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief конструктор OscilloscopeWidget
     * @param parent родительский виджет
     */
    explicit OscilloscopeWidget(QWidget *parent = nullptr);

    /**
     * @brief инициализирует элементы интерфейса виджета осциллографа
     */
    void showOscilloscopeWidget();

    /**
     * @brief отправляет команду на осциллограф и получает ответ
     * @param message команда для отправки
     */
    void sendMessage(QString &message);

    /**
     * @brief обработчик нажатия кнопки отправки команды
     */
    void onSendButtonClicked();

private:
    QVBoxLayout *mainLayout; ///< основной макет виджета
    QLabel *commandLabel; ///< метка для поля ввода команды
    QLineEdit *commandEdit; ///< поле ввода команды
    QPushButton *sendCommandButton; ///< кнопка для отправки команды
    QPlainTextEdit *text; ///< поле для отображения ответа от осциллографа
    OscilloscopeManager oscilloscope; ///< объект для управления осциллографом
};

#endif // OSCILLOSCOPE_WIDGET_H
