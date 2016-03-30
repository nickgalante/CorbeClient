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
#include <QStringList>
#include <iterator>
#include <QTreeWidget>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    //model = new QStringListModel(this);


    QObject::connect(ui->loginButton, SIGNAL(clicked()),this, SLOT(handleLogin()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT());
    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_backToLogin_clicked()));

    on_getFileButton_clicked();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleLogin(){

    this->si = new ServerInterface();
//    ServerInterface *si = new ServerInterface();
    connect(this->si, SIGNAL(loginSignal(QString)),this,SLOT(displayMessage(QString)));


    QString enteredEmail = ui->email->text();
    QString enterPassword = ui->password->text();

    this->si->handleLogin(enteredEmail, enterPassword);

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



void MainWindow::on_getFileButton_clicked()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://localhost:8080/UserFileList?email=ceo@test.com")));

}

void MainWindow::requestReceived(QNetworkReply* reply){

    /*QString data = reply->readAll();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj = jsonResponse.object();

    QJsonObject fileObj = jsonObj["userFileMap"].toObject();
    qDebug() << fileObj;

    ui->fileList->clear();
    ui->fileList->setRowCount(0);

    ui->fileList->setColumnCount(2);

    QStringList headerValues;
    headerValues << "File Name" << "File Size";
    ui->fileList->setHorizontalHeaderLabels(headerValues);
    QJsonObject::iterator it;
    for (it = fileObj.begin(); it != fileObj.end(); it++) {
           QString key = it.key();
           qlonglong value = it.value().toVariant().toLongLong();
           ui->fileList->insertRow(ui->fileList->rowCount());
           ui->fileList->setItem(ui->fileList->rowCount()-1, 0, new QTableWidgetItem(key));
           ui->fileList->setItem(ui->fileList->rowCount()-1, 1, new QTableWidgetItem(QString::number(value)));

       }

    ui->fileList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);*/

    ui->fileList->clear();
    ui->fileList->setColumnCount(2);
    ui->fileList->setHeaderLabels(QStringList() << "File Name" << "Size");

    QString data = reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();

        QJsonObject fileObj = jsonObj["userFileMap"].toObject();
        qDebug() << fileObj;

        QJsonObject::iterator it;
        for (it = fileObj.begin(); it != fileObj.end(); it++) {
               QString key = it.key();
               qlonglong value = it.value().toVariant().toLongLong();

               QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->fileList);

                   // QTreeWidgetItem::setText(int column, const QString & text)
                   treeItem->setText(0, key);
                   treeItem->setText(1, QString::number(value));


               //ui->fileList->setItem(ui->fileList->rowCount()-1, 0, new QTableWidgetItem(key));
               //ui->fileList->setItem(ui->fileList->rowCount()-1, 1, new QTableWidgetItem(QString::number(value)));

           }
        ui->fileList->header()->setStretchLastSection(true);
         ui->fileList->setAlternatingRowColors(true);
}

void MainWindow::on_fileList_clicked(const QModelIndex &index)
{
    qDebug() <<"Setting text";
    QString val = ui->fileList->model()->data(index).toString();

    ui->fileNameLineEdit->setText(val);
}

void MainWindow::on_downloadFileButton_clicked()
{
    doDownload();
}


void MainWindow::doDownload(){
    qDebug() << "Getting file";
    ui->downloadProgress->setValue(0);
//    ServerInterface *si = new ServerInterface();
    connect(this->si, SIGNAL(progressSignal(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
    si->getFile(ui->fileNameLineEdit->text(), this->si->getToken());


}
void MainWindow::updateProgress(qint64 read, qint64 total){
    ui->downloadProgress->setMaximum(1073741824);
    ui->downloadProgress->setValue(read);
}




void MainWindow::on_fileChooseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                QDir::homePath(),
                "All files (*.*)"

                );
    //QMessageBox::information(this, tr("File Name"), fileName);

//    ServerInterface *si = new ServerInterface();

    ui->uploadFileLineEdit->setText(fileName);

}


void MainWindow::on_uploadButton_clicked()
{
    QString uploadFileName = this->ui->uploadFileLineEdit->text();
//    qDebug() << "Data: " << uploadFileName;
//    qDebug() << "From parent thread MainWindow::on_uploadButton_clicked(): "<< QThread::currentThreadId();
    this->si->uploadFile(uploadFileName);
}
