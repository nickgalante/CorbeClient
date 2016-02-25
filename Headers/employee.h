#ifndef Employee_H
#define Employee_H

#include <map>
#include <string>

using namespace std;

class Employee
{

public:
    string first_name;
    string last_name;
    string Employee_id;
    map<string, string> file_map;

    Employee();
    void addFile( string, string );
    int getFileFromServer(string);
    void upLoadFileToServer(string);
};

#endif // Employee_H
