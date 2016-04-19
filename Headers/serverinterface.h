#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QString>
#include <QProgressBar>
#include <QFile>

class ServerInterface : public QObject
{
    Q_OBJECT
public:
    explicit ServerInterface(QObject *parent = 0);

    bool getFile();
    bool getFile(QString fileName, QString token);
    //void sendFile(QString userid, QString name, QString file_location);
    void sendFile(QString name);
    void uploadFile(QString fileNameAndDirectory); //the one we use
    void handleLogin(QString email, QString password);
    void startDownload();
    QString getToken();
    QString getUserEmail();
    bool isServerContactable();
    void getSubordiantes();
    void deleteFile(QString fileName, QString token);
    void getUserFileList(QString user);
    void insertNewUser(QString email, QString firstName, QString lastName, QString department, QString superior, QString password);
    void removeUser(QString email);
    void signout();

public slots:
   void replyFinished(QNetworkReply*);
   void uploadReplyFinished(); //when the upload is complete do this
   //void downloadFinished(QNetworkReply*);
   void updateDownloadProgress(qint64 read, qint64 total);
    void updateUploadProgress(qint64 read, qint64 total);
   void getSubordiantesFinished(QNetworkReply*);
   //void httpReadyRead();
   void deleteFinished(QNetworkReply *rep);
   void userFileListResponse(QNetworkReply *reply);
   void downloadStatus(QString msg);
   void uploadStatus(QString msg);
   void insertFinished(QNetworkReply* reply);
   void removeFinished(QNetworkReply* reply);
   void signoutFinished(QNetworkReply* reply);

signals:
   void loginSignal(QString msg);
   void signoutSignal(QString msg);
   void progressSignal(qint64 read, qint64 total);
   void uploadProgressSignal(qint64 read, qint64 total);
   void getSubordiantesSignal(QString msg);
   void userFileListSignal(QString msg);
   void invalidDownloadStatus(QString msg);
   void invalidStatusCodeSignal(QString msg);

private:
   QString token;
   QString fileName;
   QNetworkReply *reply;
   QFile *file;
   QSslConfiguration *sslConfig;
   void establishSslConfig();


public:
   QString fileSize;
   QString userEmail;

};

#endif // SERVERINTERFACE_H

