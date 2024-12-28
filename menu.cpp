#include "Device.h"
#include "MaintenanceLog.h"
#include "MaintenanceSchedule.h"
#include "Technician.h"
#include "menu.h"
void listTechnicianDetails(const TechnicianList& techList, 
                           const MaintenanceLogList& logList, 
                           const MaintenanceScheduleList& scheduleList, 
                           const DeviceList& deviceList) {
    if (techList.head == nullptr) {
        cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
        return;
    }

    cout << "\n\n\t\tTHÔNG TIN KỸ THUẬT VIÊN\n";
    cout << "----------------------------------------\n";

    Technician* tech = techList.head;
    while (tech != nullptr) {
        cout << "\t\tID: " << tech->technician_id << " | Tên: " << tech->name;
        cout << " | Trạng Thái: " << statusToString(tech->status) << "\n";

        // Danh sách thiết bị đã sửa xong
        cout << "\t\tThiết bị đã sửa xong:\n";
        bool hasFixedDevices = false;
        MaintenanceLog* log = logList.head;
        while (log != nullptr) {
            TechnicianNode* techNode = log->technicians;
            while (techNode != nullptr) {
                if (strcmp(techNode->technician_id, tech->technician_id) == 0 && log->status == FIXED) {
                    Device* device = searchDevice(deviceList, log->device_id);
                    if (device) {
                        cout << "\t\t\tID Thiết Bị: " << log->device_id 
                             << " | Tên: " << device->device_name 
                             << " | Vị Trí: " << device->location 
                             << " | Trạng Thái: " << statusToString(device->status) << "\n";
                    } else {
                        cout << "\t\t\tID Thiết Bị: " << log->device_id << " (Không tìm thấy thông tin)\n";
                    }
                    hasFixedDevices = true;
                }
                techNode = techNode->next;
            }
            log = log->next;
        }
        if (!hasFixedDevices) {
            cout << "\t\t\tKhông có thiết bị nào đã sửa xong.\n";
        }

        // Danh sách thiết bị đang bảo trì
        cout << "\t\tThiết Bị Đang Bảo Trì:\n";
        bool hasOngoingDevices = false;
        MaintenanceSchedule* schedule = scheduleList.head;
        while (schedule != nullptr) {
            TechnicianNode* techNode = schedule->technicians;
            while (techNode != nullptr) {
                if (strcmp(techNode->technician_id, tech->technician_id) == 0 && schedule->status == IN_PROGRESS) {
                    Device* device = searchDevice(deviceList, schedule->device_id);
                    if (device) {
                        cout << "\t\t\tID Thiết Bị: " << schedule->device_id 
                             << " | Tên: " << device->device_name 
                             << " | Vị Trí: " << device->location 
                             << " | Trạng Thái: " << statusToString(device->status) << "\n";
                    } else {
                        cout << "\t\t\tID Thiết Bị: " << schedule->device_id << " (Không tìm thấy thông tin)\n";
                    }
                    hasOngoingDevices = true;
                }
                techNode = techNode->next;
            }
            schedule = schedule->next;
        }
        if (!hasOngoingDevices) {
            cout << "\t\t\tKhông có thiết bị nào đang bảo trì.\n";
        }

        cout << "\t\t----------------------------------------\n";
        tech = tech->next;
    }
}

void generateDeviceStatistics(const MaintenanceLogList& logList, 
                              const DeviceList& deviceList, 
                              const TechnicianList& techList) {
    if (deviceList.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }

    // Duyệt qua danh sách thiết bị để thống kê
    cout << "\n\n\t\tTHỐNG KÊ THIẾT BỊ\n";
    cout << "\t\t----------------------------------------\n";

    Device* device = deviceList.head;
    while (device != nullptr) {
        int maintenanceCount = 0;  // Số lần bảo trì
        double totalCost = 0.0;    // Tổng chi phí
        char technicians[100][20]; // Lưu danh sách kỹ thuật viên (ID)
        int technicianCount = 0;   // Số lượng kỹ thuật viên đã tham gia

        // Duyệt qua nhật ký bảo trì
        MaintenanceLog* log = logList.head;
        while (log != nullptr) {
            if (strcmp(log->device_id, device->device_id) == 0) {
                maintenanceCount++;
                totalCost += log->cost;

                // Thêm kỹ thuật viên vào danh sách nếu chưa tồn tại
                TechnicianNode* techNode = log->technicians;
                while (techNode != nullptr) {
                    bool alreadyAdded = false;
                    for (int i = 0; i < technicianCount; ++i) {
                        if (strcmp(technicians[i], techNode->technician_id) == 0) {
                            alreadyAdded = true;
                            break;
                        }
                    }
                    if (!alreadyAdded && technicianCount < 100) {
                        strcpy(technicians[technicianCount], techNode->technician_id);
                        technicianCount++;
                    }
                    techNode = techNode->next;
                }
            }
            log = log->next;
        }

        // Hiển thị thông tin thiết bị
        cout << "\t\tID Thiết Bị: " << device->device_id << "\n";
        cout << "\t\tTên: " << device->device_name << "\n";
        cout << "\t\tVị Trí: " << device->location << "\n";
        cout << "\t\tTrạng Thái: " << statusToString(device->status) << "\n";
        cout << "\t\tSố Lần Bảo Trì Hoàn Thành: " << maintenanceCount << "\n";
        cout << "\t\tTổng Chi Phí Bảo Trì: " << totalCost << "\n";

        // Hiển thị danh sách kỹ thuật viên
        cout << "\t\tKỹ Thuật Viên Đã Bảo Trì:\n";
        if (technicianCount == 0) {
            cout << "\t\t\tKhông có kỹ thuật viên nào.\n";
        } else {
            for (int i = 0; i < technicianCount; ++i) {
                Technician* tech = searchTechnician(techList, technicians[i]);
                if (tech) {
                    cout << "\t\t\tID: " << technicians[i] 
                         << " | Tên: " << tech->name 
                         << " | Liên Hệ: " << tech->contact_info << "\n";
                } else {
                    cout << "\t\t\tID: " << technicians[i] << " (Không tìm thấy thông tin)\n";
                }
            }
        }

        cout << "\t\t----------------------------------------\n";
        device = device->next;
    }
}

void getCurrentDate(char* currentDate) {
    time_t now = time(0);
    tm* localTime = localtime(&now);

    // Lấy từng thành phần ngày, tháng, năm
    int day = localTime->tm_mday;
    int month = localTime->tm_mon + 1;
    int year = localTime->tm_year + 1900;

    // Định dạng chuỗi `dd/mm/yyyy` bằng cách ghép thủ công
    currentDate[0] = (day / 10) + '0';     // Chữ số hàng chục của ngày
    currentDate[1] = (day % 10) + '0';     // Chữ số hàng đơn vị của ngày
    currentDate[2] = '/';
    currentDate[3] = (month / 10) + '0';   // Chữ số hàng chục của tháng
    currentDate[4] = (month % 10) + '0';   // Chữ số hàng đơn vị của tháng
    currentDate[5] = '/';
    currentDate[6] = (year / 1000) + '0';  // Hàng nghìn của năm
    currentDate[7] = ((year / 100) % 10) + '0';  // Hàng trăm của năm
    currentDate[8] = ((year / 10) % 10) + '0';   // Hàng chục của năm
    currentDate[9] = (year % 10) + '0';    // Hàng đơn vị của năm
    currentDate[10] = '\0';                // Ký tự kết thúc chuỗi
}
void notifyMaintenance(const MaintenanceScheduleList& list, const TechnicianList& techList, const DeviceList& deviceList) {
    if(list.head == nullptr) {
        cout << "\n\n\t\tDanh sách nhật ký bảo trì rỗng.\n";
        return;
    }
    char today[20];
    getCurrentDate(today); // Lấy ngày hiện tại (dd/mm/yyyy)

    cout << "\n\t\tDANH SÁCH LỊCH BẢO TRÌ CHO NGÀY HÔM NAY (" << today << "):\n";
    cout << "\t\t----------------------------------------\n";
    bool found = false;
    MaintenanceSchedule* current = list.head;
    while (current != nullptr) {
        // So sánh ngày dự kiến bảo trì với ngày hôm nay
        if (strcmp(current->scheduled_date, today) == 0&&current->status==PENDING) {
            found = true;
            cout << "\n\t\tID nhật ký: " << current->maintenance_id;
            Device* device = searchDevice(deviceList, current->device_id);
            if (device != nullptr) {
                cout << "\n\t\tThông tin thiết bị:"
                    << "\n\t\t\tTên thiết bị: " << device->device_name
                    << "\n\t\t\tVị trí: " << device->location
                    << "\n\t\t\tTrạng thái: " << statusToString(device->status);
            } else {
                cout << "\n\t\tThông tin thiết bị: Không tìm thấy";
            }
            cout << "\n\t\tDanh sách kỹ thuật viên:";
            TechnicianNode* techNode = current->technicians;
            if (techNode == nullptr) {
                cout << "\n\t\tKhông có kỹ thuật viên nào được gán!";
            } else {
                while (techNode != nullptr) {
                    Technician* technician = searchTechnician(techList, techNode->technician_id);
                    if (technician != nullptr) {
                        cout << "\n\t\t\tTên: " << technician->name
                            << " | Liên hệ: " << technician->contact_info
                            << " | Kỹ năng: " << technician->skills;
                    } else {
                        cout << "\n\t\t\tID: " << techNode->technician_id
                            << " (Không tìm thấy thông tin)";
                    }
                    techNode = techNode->next;
                }
            }
            }
        current = current->next;
    }

    if (!found) {
        cout << "\n\t\tKhông có lịch bảo trì ngày hôm nay.\n";
    }
}

void generateAutoID(char* idBuffer, const char* prefix) {
    // Biến đếm tĩnh cho từng loại mã
    static int deviceCounter = 1;
    static int technicianCounter = 1;
    static int maintenanceCounter = 1;
    static int historyCounter = 1;

    int temp;
    if (strcmp(prefix, "DEV") == 0) {
        temp = deviceCounter++;
    } else if (strcmp(prefix, "TECH") == 0) {
        temp = technicianCounter++;
    } else if (strcmp(prefix, "MS") == 0) {
        temp = maintenanceCounter++;
    } else if (strcmp(prefix, "HIS") == 0) {
        temp = historyCounter++;
    } else {
        cout << "Prefix không hợp lệ.\n";
        return;
    }

    char numberPart[10]; // Chuỗi để lưu phần số
    int i = 0;

    // Xử lý phần số của ID
    do {
        numberPart[i++] = '0' + (temp % 10); // Lấy chữ số cuối
        temp /= 10;                          // Bỏ chữ số cuối
    } while (temp > 0);

    // Thêm các số 0 vào đầu để đủ 5 chữ số
    while (i < 5) {
        numberPart[i++] = '0';
    }

    // Đảo ngược chuỗi số
    for (int j = 0; j < i / 2; j++) {
        char tmp = numberPart[j];
        numberPart[j] = numberPart[i - 1 - j];
        numberPart[i - 1 - j] = tmp;
    }
    numberPart[i] = '\0'; // Kết thúc chuỗi

    // Ghép tiền tố và phần số vào idBuffer
    int k = 0;
    for (int j = 0; prefix[j] != '\0'; j++) {
        idBuffer[k++] = prefix[j];
    }
    for (int j = 0; numberPart[j] != '\0'; j++) {
        idBuffer[k++] = numberPart[j];
    }
    idBuffer[k] = '\0'; // Kết thúc chuỗi ID
}

void menu() {
    DeviceList deviceList;
    initDeviceList(deviceList);
    MaintenanceScheduleList ScheduleList;
    initMaintenanceScheduleList(ScheduleList);
    TechnicianList techList;
    initTechnicianList(techList);
    MaintenanceLogList ListLog;
    initMaintenanceLogList(ListLog);
    int choice;
    do {
        clearScreen();
        cout << "\n\n\t\t========= HỆ THỐNG QUẢN LÝ THIẾT BỊ =========\n";
        cout << "\n\t\t1. Thiết bị\n";
        cout << "\t\t2. Kỹ thuật viên\n";
        cout << "\t\t3. Lịch bảo trì\n";
        cout << "\t\t4. Lịch sử bảo trì\n";
        cout << "\t\t5. Thông báo lịch bảo trì sắp tới\n";
        cout << "\t\t6. Thống kê thiết bị\n";
        cout << "\t\t7. Thống kê Kỹ thuật viên\n";
        cout << "\t\t0. Thoát chương trình\n";
        cout << "\t\t=============================================\n";
        cout << "\t\tNhập lựa chọn của bạn: ";
        cin >> choice;
        cin.ignore();
        clearScreen(); // Xóa màn hình trước khi thực hiện hành động

        switch (choice) {
            case 1: {
                menuDevice(deviceList,techList, ScheduleList);
                break;
            }
            case 2: {
                menuTechnician(techList,ScheduleList,deviceList);
                break;
            }
            case 3: {
                menuMaintenanceSchedule(ScheduleList,deviceList,techList,ListLog);
                break;
            }
            case 4: {
                displayMaintenanceLogs(ListLog,techList,deviceList);
                break;
            }
            case 5: {
                notifyMaintenance(ScheduleList,techList,deviceList);
                break;
            }
            case 6: {
                generateDeviceStatistics(ListLog,deviceList,techList);
                break;
            }
            case 7: {
                listTechnicianDetails(techList, ListLog, ScheduleList, deviceList);
                break;
            }
            case 0: {
                cout << "\n\n\t\tThoát chương trình. Hẹn gặp lại!\n";
                break;
            }
            default:
                cout << "\n\n\t\tLựa chọn không hợp lệ. Vui lòng thử lại.\n";
        }
        // Dừng màn hình để người dùng xem kết quả trước khi quay lại menu
        if (choice != 0) {
            cout << "\n\t\tNhấn Enter để quay lại menu...";
            cin.ignore();
        }
    } while (choice != 0);
}
void clearScreen() {
    #ifdef _WIN32
        system("cls"); // Windows
    #else
        system("clear"); // Linux/MacOS
    #endif
}
