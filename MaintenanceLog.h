#ifndef MAINTENANCELOG_H_INCLUDED
#define MAINTENANCELOG_H_INCLUDED
#include <iostream>
using namespace std;
#include <cstring>
// Struct nhật ký bảo trì
struct TechnicianNode;
struct TechnicianList;
struct DeviceList;
struct MaintenanceScheduleList;
struct MaintenanceSchedule;

enum MaintenanceLogStatus {
    FIXED,          // Đã sửa xong
    NOT_FIXED,      // Không sửa được
};

struct MaintenanceLog {
    char log_id[20];
    char device_id[20];
    char performed_date[15];
    TechnicianNode* technicians;
    double cost;
    char details[100];
    MaintenanceLogStatus status;
    MaintenanceLog* next;
    MaintenanceLog* prev;
};
// Danh sách liên kết đôi quản lý nhật ký bảo trì
struct MaintenanceLogList {
    MaintenanceLog* head;
    MaintenanceLog* tail;
};
void initMaintenanceLogList(MaintenanceLogList& list);
TechnicianNode* copyTechnicianList(TechnicianNode* original);
void addMaintenanceLog(MaintenanceLogList& list, MaintenanceSchedule* schedule, DeviceList& deviceList, MaintenanceScheduleList& scheduleList);
void displayMaintenanceLogs(const MaintenanceLogList& list, const TechnicianList& techList, const DeviceList& deviceList);
const char* statusToString(MaintenanceLogStatus status);
#endif // MAINTENANCELOG_H_INCLUDED
