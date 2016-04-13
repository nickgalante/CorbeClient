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
    this->si = new ServerInterface();
    ui->setupUi(this);

    //model = new QStringListModel(this);


    QObject::connect(ui->loginButton, SIGNAL(clicked()),this, SLOT(handleLogin()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT());
    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_backToLogin_clicked()));

    //on_getFileButton_clicked();

    connect(this->si, SIGNAL(getSubordiantesSignal(QString)),this,SLOT(fillDropdown(QString)));
    connect(this->si, SIGNAL(loginSignal(QString)),this,SLOT(displayMessage(QString)));
    connect(this->si, SIGNAL(userFileListSignal(QString)),this,SLOT(fillFileList(QString)));
    connect(this->si, SIGNAL(downloadStatusSignal(QString)),this,SLOT(displayDownloadStatus(QString)));



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleLogin(){

    if(! this->si->isServerContactable()){
        displayServerIsNotContactable();
    } else{

        QString enteredEmail = ui->email->text();
        QString enterPassword = ui->password->text();

        this->si->handleLogin(enteredEmail, enterPassword);
    }
}

void MainWindow::displayMessage(QString msg){
    if(msg.contains("success", Qt::CaseInsensitive)){
        ui->statusLabel->setText(msg);
        ui->stackedWidget->setCurrentIndex(1);
        ui->tabWidget->setCurrentIndex(0);
        ui->tabStatusLabel->setText(msg);

        this->si->getSubordiantes();
        this->si->getUserFileList(si->userEmail);
    }
    else{
        ui->statusLabel->setText("Invalid email or password");

    }
}

void MainWindow::displayServerIsNotContactable(){
    this->ui->stackedWidget->setCurrentIndex(0);
    ui->statusLabel->setText("Server is currently unreachable.");
}

void MainWindow::on_backToLogin_clicked()
{
    if(ui->tabWidget->currentIndex()==4){

            ui->stackedWidget->setCurrentIndex(0);
        }
}



void MainWindow::on_getFileListButton_clicked()
{
    si->getUserFileList(ui->userList->currentText());
}

void MainWindow::fillFileList(QString msg){

    ui->fileList->clear();
    ui->fileList->setColumnCount(2);
    ui->fileList->setHeaderLabels(QStringList() << "File Name" << "Size");

    QJsonDocument jsonResponse = QJsonDocument::fromJson(msg.toUtf8());
    QJsonObject jsonObj = jsonResponse.object();

    QJsonObject fileObj = jsonObj["userFileMap"].toObject();
    //qDebug() << fileObj;

    QJsonObject::iterator it;
    for (it = fileObj.begin(); it != fileObj.end(); it++) {
        QString key = it.key();
        qlonglong value = it.value().toVariant().toLongLong();

        QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->fileList);

        treeItem->setText(0, key);
        treeItem->setText(1, QString::number(value));

    }

    ui->fileList->header()->setStretchLastSection(true);
    ui->fileList->setAlternatingRowColors(true);
}


void MainWindow::on_downloadFileButton_clicked()
{
    ui->downloadStatus->clear();
    doDownload();
}


void MainWindow::doDownload(){
    qDebug() << "Getting file";
    ui->downloadProgress->setValue(0);
    connect(this->si, SIGNAL(progressSignal(qint64, qint64)), this, SLOT(updateProgress(qint64)));
    si->getFile(ui->fileNameLineEdit->text(), this->si->getToken());


}
void MainWindow::updateProgress(qint64 read){

    ui->downloadProgress->setMaximum(100);
    qint64 value = (read/(si->fileSize.toDouble())*100);
    QString percent = QString::number(value);
    ui->downloadProgress->setValue(value);
    qDebug() << "percent" << percent;
    ui->progressPrecent->setText(percent + "%");
}

void MainWindow::displayDownloadStatus(QString status){
    qDebug() <<"setting downloadStatus text" << status;
     ui->downloadStatus->setText(status);
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

    connect(this->si, SIGNAL(uploadProgressSignal(qint64, qint64)), this, SLOT(updateUploadProgress(qint64)));
    this->si->uploadFile(uploadFileName);
}

void MainWindow::on_fileList_itemClicked(QTreeWidgetItem *item)
{
    qDebug() <<"Setting fileName" << item->text(0);
    ui->fileNameLineEdit->setText(item->text(0));
    qDebug() << "Setting FileSize: " << item->text(1).toLongLong();
    //ui->downloadProgress->setMaximum(item->text(1).toLongLong());
    si->fileSize = item->text(1);
}

void MainWindow::fillDropdown(QString msg){
    ui->userList->clear();
    ui->userList->addItem(si->getUserEmail());
    ui->newUserSuperior->addItem(si->getUserEmail());
    ui->userToRemove->addItem(si->getUserEmail());

    QJsonDocument jsonResponse = QJsonDocument::fromJson(msg.toUtf8());
    QJsonArray jsonArray = jsonResponse.array();

    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();
        ui->userList->addItem(obj["email"].toString());
        ui->newUserSuperior->addItem(obj["email"].toString());
        ui->userToRemove->addItem(obj["email"].toString());
        //ui->ListOfSubordinates->addItem(obj["email"].toString());
    }



}
void MainWindow::updateUploadProgress(qint64 read){
    ui->uploadProgress->setMaximum(100);
    qint64 value = (read/(si->fileSize.toDouble())*100);
    QString percent = QString::number(value);
    ui->uploadProgress->setValue(value);
    qDebug() << "percent" << percent;
    //ui->progressPrecent->setText(percent + "%");
}

void MainWindow::on_deleteFileButton_clicked()
{
    si->deleteFile(ui->fileNameLineEdit->text(), this->si->getToken());
}


void MainWindow::on_insertUserButton_clicked()
{
    QString email = ui->newUserEmail->text();
    QString firstName = ui->newUserFirstName->text();
    QString lastName = ui->newUserLastName->text();
    QString department = ui->newUserDepartment->text();
    QString superior = ui->newUserSuperior->currentText();
    QString password = ui->newUserPassword->text();

    si->insertNewUser(email, firstName, lastName, department, superior, password);
}

void MainWindow::on_removeUserButton_clicked()
{
    QString email = ui->userToRemove->currentText();

    si->removeUser(email);
}
