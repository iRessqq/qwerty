#include "oscilloscope_widget.h"
#include "oscilloscope_manager.h"
#include "history_window.h"

/**
 * @brief конструктор OscilloscopeWidget
 * @param parent родительский виджет
 */
OscilloscopeWidget::OscilloscopeWidget(QWidget *parent)
    : QWidget{parent}
{
    showOscilloscopeWidget();
    connect(sendCommandButton, &QPushButton::clicked, this, &OscilloscopeWidget::onSendButtonClicked);
    connect(commandEdit, &QLineEdit::returnPressed, this, &OscilloscopeWidget::onSendButtonClicked);
}

/**
 * @brief инициализирует элементы интерфейса виджета осциллографа
 */
void OscilloscopeWidget::showOscilloscopeWidget()
{
    mainLayout = new QVBoxLayout(this);
    commandLabel = new QLabel("Команда");
    commandEdit = new QLineEdit();
    sendCommandButton = new QPushButton("Отправить");
    text = new QPlainTextEdit();
    text->setReadOnly(true);

    mainLayout->addWidget(commandLabel);
    mainLayout->addWidget(commandEdit);
    mainLayout->addWidget(sendCommandButton);
    mainLayout->addWidget(text);
}

/**
 * @brief обработчик нажатия кнопки отправки команды
 */
void OscilloscopeWidget::onSendButtonClicked()
{
    QString message = commandEdit->text();
    sendMessage(message);
}

/**
 * @brief отправляет команду на осциллограф и получает ответ
 * @param message команда для отправки
 */
void OscilloscopeWidget::sendMessage(QString &message)
{
    const int MAX_ST = 512;
    char inbuf[MAX_ST] = {0};
    int sockfd;

    sockfd = oscilloscope.connectToScope(oscilloscope.getIp().toUtf8().data(), MAX_TCP_CONNECT);
    if (sockfd > 0)
    {
        oscilloscope.writeData(sockfd, message.toUtf8().data());
        oscilloscope.readData(sockfd, inbuf, sizeof(inbuf), MAX_TCP_READ);

        QString reply = QString::fromLatin1(inbuf); //подумать над этим
        if (strlen(inbuf) != 0)
        {
            QString replyStr = QString::fromLatin1(inbuf).trimmed(); //осциллограф отвечает с пробелом в конце
            text->appendPlainText(replyStr);
        }

        HistoryWindow::logCommand(
                    message,
                    reply,
                    QStringLiteral("Oscolloscope")
                    );
        oscilloscope.disconnectFromScope(sockfd);
    }
}
