#ifndef STM_WIDGET_H
#define STM_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLabel>

#include "stm_manager.h"

/**
 * @brief класс интерфейса работы с платой
 */
class StmWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief виджет для работы с платой
     */
    explicit StmWidget(QWidget *parent = nullptr);

private slots:
    /**
     * @brief слот при нажатии кнопки отправки
     */
    void onSendClicked();

private:
    QVBoxLayout *layout; ///< вертикальный layout
    QLabel *label1, *label2, *label3; ///< метки для операции, объекта и данных
    QLineEdit *operationEdit, *objectEdit, *dataEdit; ///< ввод операции, объекта и данных
    QPushButton *sendButton; ///< кнопка отправки
    QPlainTextEdit *log; ///< поле для вывода лога
    StmManager manager;
};

#endif // STM_WIDGET_H
