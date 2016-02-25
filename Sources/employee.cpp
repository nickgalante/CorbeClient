#include "employee.h"
#include "serverinterface.h"


Employee::Employee()
{
    this->first_name = "Cody";
    this->last_name = "Herbst";
    this->Employee_id = "ENG0000000000000001";
}

void Employee::addFile(string file_name, string fileLocation){
    file_map[file_name] = fileLocation;
}

/*void Employee::upLoadFileToServer(string file_name){
    string file_location(file_map[file_name]);
    ServerInterface::postFile(Employee_id, file_name, file_location);
}*/

int Employee::getFileFromServer(string file_name){
    ServerInterface::getFile(Employee_id, file_name);
    return 0;
}
