#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED
#include <iostream>
#include <cstring>
using namespace std;
struct Technician;
struct MaintenanceScheduleList;
struct TechnicianList;
struct TechnicianNode;
enum DeviceStatus {
    DEVICE_AVAILABLE = 1,
    IN_USE,                // Đang sử dụng
    MAINTENANCE,   // Chờ bảo trì
    OUT_OF_SERVICE           // Ngừng hoạt động
};

struct Device {
    char device_id[20];
    char device_name[50];
    char purchase_date[15];
    DeviceStatus status;
    char location[50];
    Device* next;
    Device* prev;
};
struct DeviceList {
    Device* head;
    Device* tail;
};
void initDeviceList(DeviceList& list);
const char* statusToString(DeviceStatus status);
void addDevice(DeviceList& list) ;
void displayDevices(const DeviceList& list);
//bool createMaintenanceForDevice(Device* device, Technician* tech, MaintenanceScheduleList& scheduleList);
bool scheduleMaintenanceForDevice(Device* device, MaintenanceScheduleList& scheduleList, TechnicianList& techList);
void maintainDevice(
    DeviceList& deviceList,
    MaintenanceScheduleList& scheduleList,
    TechnicianList& techList
);
void editDevice(DeviceList& list, TechnicianList& technicianList, MaintenanceScheduleList& scheduleList);
void deleteSchedule(MaintenanceScheduleList& list, TechnicianList& techList, DeviceList& deviceList, const char* maintenance_id);
Device* searchDevice(const DeviceList& list, const char* device_id);
void menuDevice(DeviceList& deviceList,TechnicianList& technicianList, MaintenanceScheduleList& scheduleList);
#endif // DEVICE_H_INCLUDED
