#ifndef Employee_H
#define Employee_H

#include <map>
#include <QObject>

using namespace std;

class Employee
{

public:
    QString first_name;
    QString last_name;
    QString Employee_id;
    map<QString, QString> file_map;

    Employee();
    void addFile( QString, QString );
    int getFileFromServer(QString);
    void upLoadFileToServer(QString);
};

#endif // Employee_H
