#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QFile>
#include <QString>
#include <QNetworkReply>
#include <QThread>

class DownloadWorker : public QObject{
    Q_OBJECT

public:
    DownloadWorker(QString token, QString filename, QString downloadToDirectory,
                   QNetworkReply* reply, QSslConfiguration *sslConfig);

private:
    QString token;
    QString filename;
    QString downloadToDirectory;
    QFile* fileToWriteTo;
    QNetworkReply* reply;
    long totalWrote;


private slots:
    void onHttpReadyRead();
    void downloadFinished(QNetworkReply *rep);

signals:
    void invalidTokenSignal(QString msg);

};


#endif // DOWNLOADWORKER_H

