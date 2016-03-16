#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QDebug>
#include <QStringRef>


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
    //ui->label->setText(ui->email->toPlainText());

    QNetworkAccessManager *manager = new QNetworkAccessManager();

     QNetworkReply *reply;

        QUrl url("http://localhost:8080/signin");
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    //     QUrlQuery params;
    //     params.addQueryItem("email", "testEmail");
    //     params.addQueryItem("md5Password", "testPassword");
    //     QString POSTDATA("email=blahEmail&md5Password=blahPass");
        QByteArray postData;
        postData.append("email=" + ui->email->text());
        postData.append("&password=" + ui->password->text());
        //QByteArray POSTDATABYTES("email=ceo@test.com&password=5f4dcc3b5aa765d61d8327deb882cf99");
    //     POSTDATABYTES.append(POSTDATA);

        manager->post(request, postData);
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));



            //ui->label->setText((reply->readAll()));

    //    QCoreApplication a(argc, argv);
    //    return a.exec();

}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    qDebug() << "reply finished";
    QString data = reply->readAll();
    if(data.contains("success", Qt::CaseInsensitive)){
        ui->statusLabel->setText(data);
        ui->stackedWidget->setCurrentIndex(1);
        ui->tabStatusLabel->setText(data);
    }
    else{
        ui->statusLabel->setText("Invalid email or password");
    }


    // document.setContent(reply);
}



void MainWindow::on_backToLogin_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}

void MainWindow::on_loginButtonBox_accepted()
{

}
