#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "serverinterface.h"

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(handleLogin()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelLogin()));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::handleLogin()
{
    QString enteredEmail = ui->email->toPlainText();
    QString enterPassword = ui->password->toPlainText();
    ServerInterface::handleLogin(enteredEmail, enterPassword);
    this->accept();

}

void LoginWindow::cancelLogin(){
    this->reject();
}

