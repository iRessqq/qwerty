#include "generator_widget.h"
#include "history_window.h"

/**
 * @brief конструктор GeneratorWidget.
 * @param parent родительский виджет.
 */
GeneratorWidget::GeneratorWidget(QWidget *parent)
    : QWidget{parent}
{
    showGeneratorWidget();

    // коннект сигналов кнопки и поля ввода
    connect(sendCommandButton, &QPushButton::clicked, this, &GeneratorWidget::onSendButtonClicked);
    connect(commandEdit, &QLineEdit::returnPressed, this, &GeneratorWidget::onSendButtonClicked);
}

/**
 * @brief инициализирует элементы интерфейса виджета генератора.
 */
void GeneratorWidget::showGeneratorWidget()
{
    mainLayout = new QVBoxLayout(this);

    commandLabel = new QLabel("Команда", this);
    commandEdit = new QLineEdit(this);
    sendCommandButton = new QPushButton("Отправить", this);
    text = new QPlainTextEdit(this);
    text->setReadOnly(true);

    mainLayout->addWidget(commandLabel);
    mainLayout->addWidget(commandEdit);
    mainLayout->addWidget(sendCommandButton);
    mainLayout->addWidget(text);
}

/**
 * @brief обработчик нажатия кнопки отправки команды.
 */
void GeneratorWidget::onSendButtonClicked()
{
    QString command = commandEdit->text().trimmed(); // убираем лишние пробелы
    if (command.isEmpty()) {
        command = "*IDN?"; // если пусто — отправляем idn
    }

    QString response = generatorManager.sendCommand(command); // отправляем команду
    text->appendPlainText(response); // отображаем ответ от генератора

    //лог управления в бд
    HistoryWindow::logCommand(
                command,
                response,
                QStringLiteral("Generator")
                );
}
