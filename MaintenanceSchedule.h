#ifndef MAINTENANCESCHEDULE_H_INCLUDED
#define MAINTENANCESCHEDULE_H_INCLUDED
#include <iostream>
#include <cstring>
#include <fstream>
struct TechnicianList;
struct DeviceList;
struct MaintenanceLogList;
using namespace std;
enum Status {
    PENDING = 1,
    IN_PROGRESS,
    COMPLETED
};
struct TechnicianNode {
    char technician_id[20];
    TechnicianNode* next;
};
// Struct lịch bảo trì
struct MaintenanceSchedule {
    char maintenance_id[20];
    char device_id[20];
    TechnicianNode* technicians;
    char scheduled_date[15];
    Status status;
    char maintenance_notes[100];
    MaintenanceSchedule* next;
    MaintenanceSchedule* prev;
};

// Danh sách liên kết đôi quản lý lịch bảo trì
struct MaintenanceScheduleList {
    MaintenanceSchedule* head;
    MaintenanceSchedule* tail;
};

// Khởi tạo danh sách lịch bảo trì
void initMaintenanceScheduleList(MaintenanceScheduleList& list);
const char* statusToString(Status status);
// Thêm lịch bảo trì
bool isTechnicianInSchedule(TechnicianNode* technicians, const char* technician_id);
int countTechnicians(const TechnicianList& list);
void writeCompletedSchedulesToFile(const MaintenanceLogList& logList, const DeviceList& deviceList, const TechnicianList& techList, const char* filename);
void addMaintenanceSchedules(MaintenanceScheduleList& list, TechnicianList& techList, DeviceList& deviceList);
// Hiển thị danh sách lịch bảo trì
//void displayMaintenanceSchedules(const MaintenanceScheduleList& list);
void displayMaintenanceSchedules(const MaintenanceSchedule* current, const DeviceList& deviceList, const TechnicianList& techList);
void displayListMaintenanceSchedules(const MaintenanceScheduleList& scheduleList,
                                 const DeviceList& deviceList,
                                 const TechnicianList& techList);
MaintenanceSchedule* searchSchedule(MaintenanceScheduleList& list, const char* maintenance_id);
void updateTechnicians(TechnicianNode*& technicians, const char* newTechIDs, const TechnicianList& techList) ;
void editSchedule(MaintenanceScheduleList& list, MaintenanceLogList& logList, DeviceList& deviceList, TechnicianList& techList);
void deleteSchedule(MaintenanceScheduleList& list, TechnicianList& techList, DeviceList& deviceList, const char* maintenance_id);
void menuMaintenanceSchedule(MaintenanceScheduleList& ScheduleList, DeviceList& deviceList, TechnicianList& techList, MaintenanceLogList& MaintenanceLogList);
#endif // MAINTENANCESCHEDULE_H_INCLUDED
