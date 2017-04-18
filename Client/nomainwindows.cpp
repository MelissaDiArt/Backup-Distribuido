#include "nomainwindows.h"
#include "ui_nomainwindows.h"

NoMainWindows::NoMainWindows(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NoMainWindows)
{
    ui->setupUi(this);
}

NoMainWindows::~NoMainWindows()
{
    delete ui;
}
