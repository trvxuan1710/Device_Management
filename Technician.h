#ifndef TECHNICIAN_H_INCLUDED
#define TECHNICIAN_H_INCLUDED
#include <iostream>
#include <cstring>
struct MaintenanceScheduleList;
struct DeviceList;
using namespace std;
// Struct kỹ thuật viên
enum TechnicianStatus {
    TECH_AVAILABLE = 1,    // Có sẵn
    BUSY = 2,         // Đang bận
    ON_LEAVE = 3,     // Đang nghỉ phép
    RETIRED = 4       // Đã nghỉ việc
};
struct Technician {
    char technician_id[20];
    char name[50];
    char contact_info[50];
    char skills[100];
    TechnicianStatus status;
    Technician* next;
    Technician* prev;
};

// Danh sách liên kết đôi quản lý kỹ thuật viên
struct TechnicianList {
    Technician* head;
    Technician* tail;
};

// Khởi tạo danh sách kỹ thuật viên
void initTechnicianList(TechnicianList& list);
const char* statusToString(TechnicianStatus status);
// Thêm kỹ thuật viên
void addTechnician(TechnicianList& list);

// Hiển thị danh sách kỹ thuật viên
void displayTechnicians(const TechnicianList& list);
Technician* searchTechnician(const TechnicianList& list, const char* technician_id);
void createMaintenanceForTechnician(Technician* technician, MaintenanceScheduleList& scheduleList, DeviceList& deviceList);
// Gọi hàm tìm kiếm
void findTechnician(TechnicianList& list);
void editTechnician(TechnicianList& list, MaintenanceScheduleList& scheduleList, DeviceList& deviceList);
void deleteTechnician(TechnicianList& list, MaintenanceScheduleList& scheduleList);
void menuTechnician(TechnicianList& techList, MaintenanceScheduleList& scheduleList, DeviceList& deviceList);

#endif // TECHNICIAN_H_INCLUDED
