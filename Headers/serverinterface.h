#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

using namespace std;

class ServerInterface
{

public:
  static bool getFile(string userid, string name);
  static int postFile(string userid, string name, string file_location);

  // .. lots of great stuff
};
