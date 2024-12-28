#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED
#include <iostream>
#include <ctime>
#include <cstring>
using namespace std;
struct MaintenanceScheduleList;
struct TechnicianList;
struct DeviceList;
struct MaintenanceLogList;

void listTechnicianDetails(const TechnicianList& techList, 
                           const MaintenanceLogList& logList, 
                           const MaintenanceScheduleList& scheduleList, 
                           const DeviceList& deviceList);
void generateDeviceStatistics(const MaintenanceLogList& logList, 
                              const DeviceList& deviceList, 
                              const TechnicianList& techList);
void generateAutoID(char* idBuffer, const char* prefix);
void menu();
void clearScreen();
void getCurrentDate(char* currentDate);
void notifyMaintenance(const MaintenanceScheduleList& list, const TechnicianList& techList, const DeviceList& deviceList);
#endif // MENU_H_INCLUDED
