#include "MaintenanceLog.h"
#include "Technician.h"
#include "MaintenanceSchedule.h"
#include "Device.h"
#include "menu.h"

// Khởi tạo danh sách nhật ký bảo trì
void initMaintenanceLogList(MaintenanceLogList& list) {
    list.head = nullptr;
    list.tail = nullptr;
}
const char* statusToString(MaintenanceLogStatus status) {
    switch (status) {
        case FIXED: return "Đã sửa xong";
        case NOT_FIXED: return "Không sửa được";
        default: return "Không xác định";
    }
}
TechnicianNode* copyTechnicianList(TechnicianNode* original) {
    if (original == nullptr) return nullptr;

    TechnicianNode* newHead = nullptr;
    TechnicianNode* newTail = nullptr;

    TechnicianNode* current = original;
    while (current != nullptr) {
        TechnicianNode* newNode = new TechnicianNode;
        strcpy(newNode->technician_id, current->technician_id);
        newNode->next = nullptr;

        if (newHead == nullptr) {
            newHead = newTail = newNode;
        } else {
            newTail->next = newNode;
            newTail = newNode;
        }

        current = current->next;
    }

    return newHead;
}
// Thêm nhật ký bảo trì
void addMaintenanceLog(MaintenanceLogList& list, MaintenanceSchedule* schedule, DeviceList& deviceList, MaintenanceScheduleList& scheduleList) {
    MaintenanceLog* newLog = new MaintenanceLog();
    generateAutoID(newLog->log_id, "HIS");
    strcpy(newLog->device_id, schedule->device_id);
    // cout << "\t\tNhập ngày hoàn thành bảo trì: ";
    // cin.getline(newLog->performed_date, sizeof(newLog->performed_date));
    time_t now = time(0);
    tm* localTime = localtime(&now);
    strftime(newLog->performed_date, sizeof(newLog->performed_date), "%d/%m/%Y", localTime);
    newLog->technicians = copyTechnicianList(schedule->technicians);
    cout << "\t\tNhập chi phí bảo trì: "; cin >> newLog->cost;
    cout << "\t\tNhập chi tiết: "; cin.ignore(); cin.getline(newLog->details, sizeof(newLog->details));
    cout << "\t\tTrạng thái thiết bị sau khi bảo trì:\n";
    cout << "\t\t\t1. Đã sửa xong\n";
    cout << "\t\t\t2. Không sửa được\n";
    cout << "\t\t\tLựa chọn: ";
    int choice;
    cin >> choice;
    cin.ignore();
    switch (choice) {
        case 1:
            newLog->status = FIXED;
            break;
        case 2:
            newLog->status = NOT_FIXED;
            break;
        default:
            newLog->status = NOT_FIXED;
            break;
    }
    newLog->next = nullptr;
    newLog->prev = nullptr;

    Device* device = searchDevice(deviceList, schedule->device_id);
    if (device) {
        // Kiểm tra còn lịch bảo trì nào cho thiết bị này không
        bool hasPendingMaintenance = false;
        MaintenanceSchedule* currentSchedule = scheduleList.head;

        while (currentSchedule != nullptr) {
            if (strcmp(currentSchedule->device_id, schedule->device_id) == 0&&currentSchedule!=schedule) {
                hasPendingMaintenance = true;
                break;
            }
            currentSchedule = currentSchedule->next;
        }

        if (hasPendingMaintenance) {
            device->status = MAINTENANCE;
        } else {
            if(newLog->status == FIXED) {
                device->status = IN_USE;
            } else {
                device->status = OUT_OF_SERVICE;
            }
        }
    }


    if (list.head == nullptr) {
        list.head = list.tail = newLog;
    } else {
        list.tail->next = newLog;
        newLog->prev = list.tail;
        list.tail = newLog;
    }

    cout << "\n\t\tNhật ký bảo trì đã được thêm thành công!\n";
}

// Hiển thị danh sách nhật ký bảo trì
void displayMaintenanceLogs(const MaintenanceLogList& list, const TechnicianList& techList, const DeviceList& deviceList) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách nhật ký bảo trì rỗng.\n";
        return;
    }
    // Hiển thị thông tin thiết bị
    cout << "\n\n\t\tDanh sách nhật ký bảo trì:\n";
    MaintenanceLog* current = list.head;
    while (current != nullptr) {
        cout << "\n\t\tID lịch sử bảo trì: " << current->log_id;
        // Hiển thị thông tin thiết bị
        Device* device = searchDevice(deviceList, current->device_id);
        if (device != nullptr) {
            cout << "\n\t\tThông tin thiết bị:"
                 << "\n\t\t\tTên thiết bị: " << device->device_name
                 << "\n\t\t\tVị trí: " << device->location
                 << "\n\t\t\tTrạng thái: " << statusToString(device->status);
        } else {
            cout << "\n\t\tThông tin thiết bị: Không tìm thấy";
        }
        TechnicianNode* techNode = current->technicians;
        if (techNode == nullptr) {
            cout << "\n\t\t  Không có kỹ thuật viên nào được gán!";
        } else {
            while (techNode != nullptr) {
                Technician* technician = searchTechnician(techList, techNode->technician_id);
                if (technician != nullptr) {
                    cout << "\n\t\t\tTên: " << technician->name
                         << "\n\t\t\tLiên hệ: " << technician->contact_info
                         << "\n\t\t\tKỹ năng: " << technician->skills;
                } else {
                    cout << "\n(Không tìm thấy thông tin)";
                }
                techNode = techNode->next;
            }
        }
        cout << "\n\t\tNgày hoàn thành: " << current->performed_date;
        cout << "\n\t\tChi tiết bảo trì: " << current->details << endl;
        cout << "\n\t\tChi phí: " << current->cost << endl;
        cout << "\n\t\tTrạng thái: " << statusToString(current->status) << endl;
        current = current->next;
    }
}
/*MaintenanceLog* searchLog(MaintenanceLogList& list, const char* log_id) {
    MaintenanceLog* current = list.head;
    while (current != nullptr) {
        if (strcmp(current->log_id, log_id) == 0) {
            return current; // Tìm thấy
        }
        current = current->next;
    }
    return nullptr; // Không tìm thấy
}*/
/*void editMaintenanceLog(MaintenanceLogList& list) {
    char id[20];
    cout << "\n\n\t\tNhập ID bản ghi bảo trì cần sửa: ";
    cin >> id;

    MaintenanceLog* current = searchLog(list, id);
    if (current != nullptr) {
        cout << "\n\t\tĐã tìm thấy bản ghi bảo trì với log_id: " << id << "\n";

        // Hiển thị thông tin cũ
        cout << "\t\tThông tin hiện tại:\n";
        cout << "\t\tMaintenance ID: " << current->maintenance_id << "\n";
        cout << "\t\tTechnician ID: " << current->technician_id << "\n";
        cout << "\t\tNgày bảo trì: " << current->performed_date << "\n";
        cout << "\t\tChi phí: " << current->cost << "\n";

        // Nhập thông tin mới
        cout << "\n\n\t\tNhập thông tin mới (nhấn Enter để bỏ qua không thay đổi):\n";

        char newMaintenanceID[50];
        char newTechnicianID[50];
        char newPerformedDate[20];
        double newCost = current->cost;

        // Maintenance ID
        cout << "\t\tMaintenance ID [" << current->maintenance_id << "]: ";
        cin.getline(newMaintenanceID, sizeof(newMaintenanceID));
        if (strlen(newMaintenanceID) > 0) {
            strcpy(current->maintenance_id, newMaintenanceID);
        }

        // Technician ID
        cout << "\t\tTechnician ID [" << current->technician_id << "]: ";
        cin.getline(newTechnicianID, sizeof(newTechnicianID));
        if (strlen(newTechnicianID) > 0) {
            strcpy(current->technician_id, newTechnicianID);
        }

        // Performed Date
        cout << "\t\tNgày bảo trì [" << current->performed_date << "]: ";
        cin.getline(newPerformedDate, sizeof(newPerformedDate));
        if (strlen(newPerformedDate) > 0) {
            strcpy(current->performed_date, newPerformedDate);
        }

        // Cost
        cout << "\t\tChi phí [" << current->cost << "]: ";
        string costInput;
        getline(cin, costInput); // Sử dụng getline cho nhập liệu số
        if (!costInput.empty()) {
            newCost = stod(costInput); // Chuyển từ string sang double
        }

        cout << "\n\t\tCập nhật thông tin bản ghi thành công!\n";
    }else {
        cout << "\n\n\t\tKhông tìm thấy bản ghi với log_id: " << id << "\n";
    }
}*/
/*void deleteLog(MaintenanceLogList& list, const char* log_id) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }

    MaintenanceLog* log = searchLog(list, log_id);

    if (log == nullptr) {
        cout << "\n\n\t\tKhông tìm thấy nhật ký với ID: " << log_id << endl;
        return;
    }

    if (log->prev != nullptr) {
        log->prev->next = log->next;
    } else {
        list.head = log->next;
    }

    if (log->next != nullptr) {
        log->next->prev = log->prev;
    } else {
        list.tail = log->prev;
    }

    delete log;
    cout << "\n\n\t\tNhật ký bảo trì đã được xóa.\n";
}*/

/*void menuMaintenanceLog(MaintenanceLogList& ListLog) {

    int choice;
    char log_id[20];

    do {
        clearScreen(); // Xóa màn hình trước khi hiển thị menu

        cout << "\n\n\t\t========= QUẢN LÝ LỊCH SỬ BẢO TRÌ =========\n";
        cout << "\n\t\t1. Thêm thiết bị\n";
        cout << "\t\t2. Hiển thị danh sách thiết bị\n";
        cout << "\t\t3. Sửa thông tin thiết bị\n";
        cout << "\t\t4. Xóa thiết bị\n";
        cout << "\t\t5. Tìm kiếm thiết bị\n";
        cout << "\t\t0. Thoát chương trình\n";
        cout << "\t\t====================================\n";
        cout << "\t\tNhập lựa chọn của bạn: ";

        cin >> choice;
        cin.ignore(); // Xóa ký tự xuống dòng còn sót lại trong bộ đệm

        clearScreen(); // Xóa màn hình trước khi thực hiện hành động

        switch (choice) {
            case 1: {

                addMaintenanceLog(ListLog);
                break;
            }
            case 2: {
                // Hiển thị danh sách thiết bị
                displayMaintenanceLogs(ListLog);
                break;
            }
            case 3: {

                editMaintenanceLog(ListLog);
                break;
            }
            case 4: {
                // Xóa thiết bị
                cout << "Nhập ID thiết bị cần xóa: ";
                cin.getline(log_id, sizeof(log_id));
                deleteLog(ListLog, log_id);
                break;
            }
            case 5: {
                // Tìm kiếm thiết bị
                cout << "Nhập ID thiết bị cần tìm: ";
                cin.getline(log_id, sizeof(log_id));
                MaintenanceLog* temp = searchLog(ListLog, log_id);
                if(temp!=NULL) {
                    cout << "\n\n\t\tĐã tìm thấy thiết bị với ID: " << log_id;
                    cout << "\n\t\tThông tin thiết bị:";
                    cout << "\n\t\tTên thiết bị: " << temp->maintenance_id;
                    cout << "\n\t\tNgày mua: " << temp->technician_id;
                    cout << "\n\t\tTrạng thái: " << temp->performed_date;
                    cout << "\n\t\tVị trí: " << temp->cost;
                }
                else cout << "\n\n\t\tKhông tìm thấy thiết bị với ID: " << log_id;
                break;
            }
            case 0: {
                cout << "Thoát chương trình. Hẹn gặp lại!\n";
                break;
            }
            default:
                cout << "Lựa chọn không hợp lệ. Vui lòng thử lại.\n";
        }

        // Dừng màn hình để người dùng xem kết quả trước khi quay lại menu
        if (choice != 0) {
            cout << "\nNhấn Enter để quay lại menu...";
            cin.ignore();
        }
    } while (choice != 0);
}*/
