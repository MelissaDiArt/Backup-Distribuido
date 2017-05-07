#include "nomainwindows.h"
#include "ui_nomainwindows.h"

NoMainWindows::NoMainWindows(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NoMainWindows)
{
    ui->setupUi(this);
    ActualSize = 0;
    TotalSize = 0;
    Name = "";
    ui->FinishPushButton->setEnabled(false);
}

NoMainWindows::~NoMainWindows()
{
    delete ui;
}

void NoMainWindows::setName(QString name)
{
    Name = name;
    ui->DNameLabel->setText(Name);
}

void NoMainWindows::setTotalSize(qint64 size)
{
    TotalSize = size;
    ui->progressBar->setRange(0,TotalSize);
}

void NoMainWindows::setActualSize(qint64 size)
{
    ActualSize = size;
    ui->progressBar->setValue(ActualSize);
    if(ActualSize == TotalSize) {
        ui->FinishPushButton->setEnabled(true);
    }
}

void NoMainWindows::on_FinishPushButton_clicked()
{
    this->close();
    emit ready();
}
