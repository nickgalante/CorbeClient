#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>

class ServerInterface : public QObject
{
    Q_OBJECT
public:
    explicit ServerInterface(QObject *parent = 0);

    static bool getFile(QString userid, QString name);
    static int postFile(QString userid, QString name, QString file_location);
    void handleLogin(QString email, QString password);

public slots:
   void replyFinished(QNetworkReply*);

signals:
   void loginSignal(QString msg);

};

#endif // SERVERINTERFACE_H
