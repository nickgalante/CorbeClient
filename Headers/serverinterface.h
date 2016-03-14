#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

using namespace std;

class ServerInterface
{

public:
  static bool getFile(QString userid, QString name);
  static int postFile(QString userid, QString name, QString file_location);
  static void handleLogin(QString email, QString password);
  // .. lots of great stuff
};
