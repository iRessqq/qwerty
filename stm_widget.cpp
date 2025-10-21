#include "stm_widget.h"

#include <QMessageBox>

/**
 * @brief виджет для работы с платой
 */
StmWidget::StmWidget(QWidget *parent)
    : QWidget(parent)
{
    // вертикальный layout
    layout = new QVBoxLayout(this);

    // лейбл для поля операции
    label1 = new QLabel("Операция:");
    // поле ввода операции в шестнадцатеричном формате
    operationEdit = new QLineEdit();

    // лейбл для поля объекта
    label2 = new QLabel("Объект:");
    // поле для ввода объекта
    objectEdit = new QLineEdit();

    // лейбл для поля данных
    label3 = new QLabel("Данные:");
    // поле ввода последовательности байт через пробел в шестнадцатеричном формате
    dataEdit = new QLineEdit();

    // кнопка отправки команды
    sendButton = new QPushButton("Отправить");
    // поле для лога
    log = new QPlainTextEdit();
    log->setReadOnly(true);

    // добавление виджетов в layout
    layout->addWidget(label1);
    layout->addWidget(operationEdit);
    layout->addWidget(label2);
    layout->addWidget(objectEdit);
    layout->addWidget(label3);
    layout->addWidget(dataEdit);
    layout->addWidget(sendButton);
    layout->addWidget(log);

    // сигнал нажатия кнопки к слоту обработки отправки
    connect(sendButton, &QPushButton::clicked, this, &StmWidget::onSendClicked);
}

/**
 * @brief слот при нажатии кнопки отправки
 */
void StmWidget::onSendClicked()
{
    // на всякий подкление к /dev/ttyUSB0
    if (!manager.isConnected()) {
        if (!manager.connectToPort("/dev/ttyUSB0")) {
            log->appendPlainText("ошибка подключения к плате");
            return;
        }
    }

    // флаги успешного преобразования из строки в число
    bool ok1, ok2;
    // операцию из шестнадцатеричной строки в uint8
    quint8 op = operationEdit->text().toUInt(&ok1, 16);
    // объект из шестнадцатеричной строки в uint32
    quint32 obj = objectEdit->text().toUInt(&ok2, 16);
    // оба преобразования успешны и поля не пустые
    if (!ok1 || !ok2 || operationEdit->text().isEmpty() || objectEdit->text().isEmpty()) {
        QMessageBox::warning(this, "ошибка", "некорректные данные");
        return;
    }

    // формирование содержимого поля данных
    QByteArray data;
    // разбиваем строку данных по пробелам и преобразуем каждый байт из hex
    for (const QString &byteStr : dataEdit->text().simplified().split(" ")) {
        if (!byteStr.isEmpty()) {
            bool ok;
            // конверт элементов в uint8
            quint8 b = byteStr.toUInt(&ok, 16);
            if (!ok) {
                // если преобразование не удалось
                QMessageBox::warning(this, "ошибка", "некорректные данные");
                return;
            }
            // добавляем байт в массив
            data.append(char(b));
        }
    }

    // отправляем команду и записываем ответ
    QByteArray response = manager.sendCommand(op, obj, data);
    // вывод в лог отправленного пакета
    log->appendPlainText("-> send: " + manager.buildPacket(op, obj, data).toHex(' ').toUpper());
    // если ответ пустой - нет ответа, else полученные данные
    log->appendPlainText(response.isEmpty() ? "нет ответа от stm" : "<- resp: " + response.toHex(' ').toUpper());
}
