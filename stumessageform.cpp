#include "stumessageform.h"
#include "ui_stumessageform.h"

StuMessageForm::StuMessageForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StuMessageForm)
{
    ui->setupUi(this);
}

StuMessageForm::~StuMessageForm()
{
    delete ui;
}
