#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :  QMainWindow(parent),  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton, SIGNAL(toggled(bool)), this, SLOT(handlePushButton()));
    ui->pushButton->setCheckable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handlePushButton(){
    LoginWindow *firstLoginWindow = new LoginWindow(this);
    firstLoginWindow->open();
    ui->pushButton->setText(ui->pushButton->isChecked() ? "NotClicked" : "Clicked");
}
