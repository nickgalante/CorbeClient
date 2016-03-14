#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "serverinterface.h"

LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(handleLogin()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(handleCancel()));
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
}

void LoginWindow::cancelLogin()
{

}
