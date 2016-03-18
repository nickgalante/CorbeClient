#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinterface.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QDebug>
#include <QStringRef>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    QObject::connect(ui->loginButton, SIGNAL(clicked()),this, SLOT(handleLogin()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT());
    QObject::connect(ui->backToLogin, SIGNAL(clicked()),this, SLOT(on_backToLogin_clicked()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleLogin(){

    ServerInterface *si = new ServerInterface();
    connect(si, SIGNAL(loginSignal(QString)),this,SLOT(displayMessage(QString)));


    QString enteredEmail = ui->email->text();
    QString enterPassword = ui->password->text();

    si->handleLogin(enteredEmail, enterPassword);

}

void MainWindow::displayMessage(QString msg){
    if(msg.contains("success", Qt::CaseInsensitive)){
        ui->statusLabel->setText(msg);
        ui->stackedWidget->setCurrentIndex(1);
        ui->tabStatusLabel->setText(msg);
    }
    else{
        ui->statusLabel->setText("Invalid email or password");
    }
}

void MainWindow::on_backToLogin_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                QDir::homePath(),
                "All files (*.*)"

                );
    QMessageBox::information(this, tr("File Name"), filename);


        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) {

        }

        QTextStream in(&file);
        ui->textBrowser->setText(in.readAll());
}

void MainWindow::on_getFileButton_clicked()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://localhost:8080/UserFileList?userId=0000000001")));

}

void MainWindow::requestReceived(QNetworkReply* reply){

    QString data = reply->readAll();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj = jsonResponse.object();
    QJsonArray jsonArray = jsonObj["actualList"].toArray();


    for(int i=0; i< jsonArray.size(); i++){
    qDebug() << "files:" << jsonArray[i].toString();
    ui->textBrowser->append(jsonArray[i].toString());
    }


}
