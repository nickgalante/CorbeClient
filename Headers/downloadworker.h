#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QtCore>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QProgressBar>
#include <QFile>

class DownloadWorker : public QThread{


    Q_OBJECT
public:
    DownloadWorker(QString token, QString fileName, QNetworkReply *rep); //requesting your own file
    DownloadWorker(QString token, QString email, QString fileName, QNetworkAccessManager *manager, QNetworkRequest req); //requesting someone elses.

    QNetworkReply *rep;
    QNetworkAccessManager *manager;

public slots:
//   void replyFinished(QNetworkReply*);
   void downloadFinished(QNetworkReply*);
   void updateDownloadProgress(qint64 read, qint64 total);
   void httpReadyRead();

signals:
//   void loginSignal(QString msg);
   void progressSignal(qint64 read, qint64 total);

private:
    QString token;
    QString fileName;
    QString email;
    QNetworkRequest req;
    QFile *file;
    long totalWrote;

    void run();
};

#endif // DOWNLOADWORKER_H
