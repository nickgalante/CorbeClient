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

    model = new QStringListModel(this);


    QObject::connect(ui->loginButton, SIGNAL(clicked()),this, SLOT(handleLogin()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT());
    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_backToLogin_clicked()));
    QObject::connect(ui->getFile, SIGNAL(clicked()), this, SLOT(doDownload()));


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
        ui->tabWidget->setCurrentIndex(0);
        ui->tabStatusLabel->setText(msg);
    }
    else{
        ui->statusLabel->setText("Invalid email or password");
    }
}

void MainWindow::on_backToLogin_clicked()
{
    if(ui->tabWidget->currentIndex()==3){

            ui->stackedWidget->setCurrentIndex(0);
        }
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                QDir::homePath(),
                "All files (*.*)"

                );
    QMessageBox::information(this, tr("File Name"), fileName);

    ServerInterface *si = new ServerInterface();

    si->sendFile(fileName);


        /*QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) {

        }

        QTextStream in(&file);
        ui->textBrowser->setText(in.readAll());*/
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

    QStringList List;
    for(int i=0; i< jsonArray.size(); i++){
        qDebug() << "files:" << jsonArray[i].toString();
        List.append(jsonArray[i].toString());
    }

    model->setStringList(List);
    ui->fileList->setModel(model);

}
void MainWindow::doDownload(){
    qDebug() << "Getting file";
    ui->downloadProgress->setValue(0);
    ServerInterface *si = new ServerInterface();
    connect(si, SIGNAL(progressSignal(qint64, qint64)),this,SLOT(updateProgress(qint64, qint64)));

    si->getFile();

}
void MainWindow::updateProgress(qint64 read, qint64 total){
    ui->downloadProgress->setMaximum(1073741824);
    ui->downloadProgress->setValue(read);
}



void MainWindow::on_fileList_clicked(const QModelIndex &index)
{
    qDebug() <<"Setting text";
    QString val = ui->fileList->model()->data(index).toString();
    ui->fileName->setText(val);

}
