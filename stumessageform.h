#ifndef STUMESSAGEFORM_H
#define STUMESSAGEFORM_H

#include <QWidget>

namespace Ui {
class StuMessageForm;
}

class StuMessageForm : public QWidget
{
    Q_OBJECT

public:
    explicit StuMessageForm(QWidget *parent = nullptr);
    ~StuMessageForm();

private:
    Ui::StuMessageForm *ui;
};

#endif // STUMESSAGEFORM_H
