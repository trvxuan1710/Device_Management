// Struct kỹ thuật viên
#include "Technician.h"
#include "MaintenanceSchedule.h"
#include "Device.h"
#include "menu.h"

// Khởi tạo danh sách kỹ thuật viên
void initTechnicianList(TechnicianList& list) {
    list.head = nullptr;
    list.tail = nullptr;
}
const char* statusToString(TechnicianStatus status) {
    switch (status) {
        case TECH_AVAILABLE: return "Đang rảnh";
        case BUSY: return "Đang bận";
        case ON_LEAVE: return "Đang nghỉ phép";
        case RETIRED: return "Đã nghỉ việc";
        default: return "UNKNOWN";
    }
}

// Thêm kỹ thuật viên
void addTechnician(TechnicianList& list) {
    
    Technician* newTech = new Technician;
    generateAutoID(newTech->technician_id, "TECH");
    cout << "\n\n\t\tTạo kỹ thuật viên với ID: " << newTech->technician_id << endl;
    // cout << "\n\n\t\tNhập thông tin kỹ thuật viên:\n";
    // cout << "\t\tID kỹ thuật viên: "; cin >> newTech->technician_id;
    cout << "\t\tTên: "; cin.getline(newTech->name, 50);
    cout << "\t\tThông tin liên hệ: "; cin.getline(newTech->contact_info, 50);
    cout << "\t\tKỹ năng: "; cin.getline(newTech->skills, 100);
    newTech->status = TECH_AVAILABLE;

    newTech->next = nullptr;
    newTech->prev = nullptr;

    if (list.head == nullptr) {
        list.head = list.tail = newTech;
    } else {
        list.tail->next = newTech;
        newTech->prev = list.tail;
        list.tail = newTech;
    }

    cout << "\t\tKỹ thuật viên đã được thêm thành công!\n";
}

// Hiển thị danh sách kỹ thuật viên
void displayTechnicians(const TechnicianList& list) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
        return;
    }

    cout << "\n\n\t\tDanh sách kỹ thuật viên:\n";
    Technician* current = list.head;
    while (current != nullptr) {
        cout << "\n\t\tID: " << current->technician_id
             << "\n\t\tTên: " << current->name
             << "\n\t\tLiên hệ: " << current->contact_info
             << "\n\t\tKỹ năng: " << current->skills
             << "\n\t\tTrạng thái: " << statusToString(current->status) << endl;
        current = current->next;
    }
}
Technician* searchTechnician(const TechnicianList& list, const char* technician_id) {
    Technician* current = list.head;
    while (current != nullptr) {
        if (strcmp(current->technician_id, technician_id) == 0) {
            return current; // Tìm thấy
        }
        current = current->next;
    }
    return nullptr; // Không tìm thấy
}

/*bool createMaintenanceForTechnician(Technician* tech, MaintenanceScheduleList& scheduleList, DeviceList& deviceList) {
    if (!tech) {
        cout << "\n\t\t\tKỹ thuật viên không hợp lệ.\n";
        return false;
    }

    MaintenanceSchedule* newSchedule = new MaintenanceSchedule();
    bool isUnique = false;

    while (!isUnique) {
        // Tự động tạo ID cho lịch bảo trì
        generateAutoID(newSchedule->maintenance_id, "MS");

        // Kiểm tra ID trong danh sách liên kết
        MaintenanceSchedule* current = scheduleList.head;
        isUnique = true; // Giả định là duy nhất
        while (current) {
            if (strcmp(current->maintenance_id, newSchedule->maintenance_id) == 0) {
                isUnique = false; // Nếu trùng, đặt lại isUnique và tạo lại ID
                break;
            }
            current = current->next;
        }
    }

    // Hiển thị danh sách các thiết bị đang sử dụng
    cout << "\n\t\t\tDanh sách thiết bị đang sử dụng:\n";
    Device* currentDevice = deviceList.head;
    bool hasAvailableDevices = false;
    while (currentDevice) {
        if (currentDevice->status == IN_USE) { // Kiểm tra trạng thái thiết bị
            hasAvailableDevices = true;
            cout << "\t\t\tID: " << currentDevice->device_id
                 << " - Tên: " << currentDevice->device_name
                 << " - Vị trí: " << currentDevice->location << endl;
        }
        currentDevice = currentDevice->next;
    }

    if (!hasAvailableDevices) {
        cout << "\t\t\tKhông có thiết bị nào đang sử dụng. Không thể tạo lịch bảo trì.\n";
        return false; // Thoát nếu không có thiết bị hợp lệ
    }

    // Nhập ID thiết bị từ danh sách thiết bị đang sử dụng
    char deviceID[20];
    bool deviceFound = false;
    do {
        cout << "\t\t\tNhập ID thiết bị từ danh sách trên: ";
        cin.getline(deviceID, sizeof(deviceID));

        currentDevice = deviceList.head;
        while (currentDevice) {
            if (strcmp(currentDevice->device_id, deviceID) == 0 && currentDevice->status == DeviceStatus::IN_USE) {
                deviceFound = true;
                break;
            }
            currentDevice = currentDevice->next;
        }

        if (!deviceFound) {
            cout << "\t\t\tID thiết bị không hợp lệ hoặc thiết bị không ở trạng thái đang sử dụng. Vui lòng nhập lại.\n";
        }
    } while (!deviceFound);

    // Gán thông tin mặc định cho lịch bảo trì
    strcpy(newSchedule->device_id, deviceID);

    cout << "\n\t\t\tNhập ngày dự kiến (dd/mm/yyyy): ";
    cin.getline(newSchedule->scheduled_date, sizeof(newSchedule->scheduled_date));

    strcpy(newSchedule->maintenance_notes, "Tự động tạo lịch bảo trì.");
    newSchedule->status = PENDING; // Trạng thái mặc định là "Đang chờ xử lý"

    // Liên kết kỹ thuật viên với lịch bảo trì
    strcpy(newSchedule->tech_id, tech->technician_id);

    // Thêm lịch bảo trì vào danh sách liên kết đôi
    newSchedule->next = nullptr;
    newSchedule->prev = scheduleList.tail;
    if (scheduleList.tail) {
        scheduleList.tail->next = newSchedule;
    } else {
        scheduleList.head = newSchedule;
    }
    scheduleList.tail = newSchedule;

    // Cập nhật trạng thái kỹ thuật viên và thiết bị
    tech->status = BUSY;
    currentDevice->status = MAINTENANCE;

    cout << "\t\t\tLịch bảo trì đã được tạo và gán cho kỹ thuật viên " << tech->name << ".\n";
    return true;
}*/
void addTechnicianToExistingSchedule(MaintenanceSchedule* schedule, Technician* technician) {
    // Kiểm tra xem kỹ thuật viên đã có trong lịch này chưa
    TechnicianNode* current = schedule->technicians;
    while (current) {
        if (strcmp(current->technician_id, technician->technician_id) == 0) {
            cout << "\n\t\t\tKỹ thuật viên đã có trong lịch bảo trì này.\n";
            return;
        }
        current = current->next;
    }

    // Thêm kỹ thuật viên mới vào lịch
    TechnicianNode* newTechNode = new TechnicianNode();
    strcpy(newTechNode->technician_id, technician->technician_id);
    newTechNode->next = schedule->technicians;
    schedule->technicians = newTechNode;

    technician->status = BUSY; // Cập nhật trạng thái kỹ thuật viên
    cout << "\n\t\t\tKỹ thuật viên đã được thêm vào lịch bảo trì với ID: " << schedule->maintenance_id << ".\n";
}

void createNewMaintenanceSchedule(Technician* technician, MaintenanceScheduleList& scheduleList, DeviceList& deviceList) {
    char device_id[20];
    cout << "\n\t\t\tNhập ID thiết bị cần bảo trì: ";
    cin.getline(device_id, sizeof(device_id));

    Device* device = searchDevice(deviceList, device_id);
    if (!device) {
        cout << "\n\t\t\tKhông tìm thấy thiết bị với ID: " << device_id << endl;
        return;
    }

    if (device->status != IN_USE) {
        cout << "\n\t\t\tThiết bị không ở trạng thái sử dụng, không thể bảo trì.\n";
        return;
    }

    // Tạo lịch bảo trì mới
    MaintenanceSchedule* newSchedule = new MaintenanceSchedule();
    bool isUnique = false;
    while (!isUnique) {
        generateAutoID(newSchedule->maintenance_id, "MS");
        MaintenanceSchedule* current = scheduleList.head;
        isUnique = true;
        while (current) {
            if (strcmp(current->maintenance_id, newSchedule->maintenance_id) == 0) {
                isUnique = false;
                break;
            }
            current = current->next;
        }
    }

    // Gán thông tin
    strcpy(newSchedule->device_id, device->device_id);
    cout << "\n\t\t\tNhập ngày dự kiến (dd/mm/yyyy): ";
    cin.getline(newSchedule->scheduled_date, sizeof(newSchedule->scheduled_date));

    newSchedule->technicians = nullptr;
    TechnicianNode* newTechNode = new TechnicianNode();
    strcpy(newTechNode->technician_id, technician->technician_id);
    newTechNode->next = nullptr;
    newSchedule->technicians = newTechNode;

    strcpy(newSchedule->maintenance_notes, "Lịch bảo trì được tạo mới.");
    newSchedule->status = PENDING;

    // Thêm lịch vào danh sách
    newSchedule->next = nullptr;
    newSchedule->prev = scheduleList.tail;
    if (scheduleList.tail) {
        scheduleList.tail->next = newSchedule;
    } else {
        scheduleList.head = newSchedule;
    }
    scheduleList.tail = newSchedule;

    // Cập nhật trạng thái
    technician->status = BUSY;
    device->status = MAINTENANCE;

    cout << "\n\t\t\tLịch bảo trì mới đã được tạo thành công với ID: " << newSchedule->maintenance_id << ".\n";
    cout << "\t\t\tKỹ thuật viên phụ trách: " << technician->name << ".\n";
    cout << "\t\t\tNgày dự kiến: " << newSchedule->scheduled_date << ".\n";
}
void createMaintenanceForTechnician(Technician* technician, MaintenanceScheduleList& scheduleList, DeviceList& deviceList) {
    if (!technician|| technician->status == ON_LEAVE|| technician->status == RETIRED) {
        cout << "\n\t\t\tKỹ thuật viên không hợp lệ.\n";
        return;
    }

    cout << "\n\t\t\t1. Tạo lịch bảo trì mới\n\t\t\t2. Thêm vào lịch bảo trì đã có sẵn\n";
    cout << "\t\t\tLựa chọn của bạn: ";
    int choice;
    cin >> choice;
    cin.ignore();

    switch (choice) {
        case 1: // Tạo lịch mới
            if(deviceList.head == nullptr) {
                cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
                return;
            }
            createNewMaintenanceSchedule(technician, scheduleList, deviceList);
            break;
        case 2: { // Thêm vào lịch bảo trì đã có sẵn
            if(scheduleList.head == nullptr) {
                cout << "\n\n\t\t\tDanh sách lịch bảo trì rỗng.\n";
                return;
            }
            char schedule_id[20];
            cout << "\n\t\t\tNhập ID lịch bảo trì: ";
            cin.getline(schedule_id, sizeof(schedule_id));

            MaintenanceSchedule* schedule = searchSchedule(scheduleList, schedule_id);
            if (!schedule) {
                cout << "\n\t\t\tKhông tìm thấy lịch bảo trì với ID: " << schedule_id << endl;
                return;
            }
            addTechnicianToExistingSchedule(schedule, technician);
            break;
        }
        default:
            cout << "\n\t\t\tLựa chọn không hợp lệ.\n";
            break;
    }
}
void removeTechnicianFromSchedules(MaintenanceScheduleList& scheduleList, const char* techID) {
    MaintenanceSchedule* currentSchedule = scheduleList.head;

    while (currentSchedule) {
        TechnicianNode* prevNode = nullptr;
        TechnicianNode* currentTechNode = currentSchedule->technicians;

        while (currentTechNode) {
            if (strcmp(currentTechNode->technician_id, techID) == 0) {
                // Xóa kỹ thuật viên khỏi danh sách
                if (prevNode == nullptr) {
                    currentSchedule->technicians = currentTechNode->next;
                } else {
                    prevNode->next = currentTechNode->next;
                }
                delete currentTechNode;
                cout << "\t\tĐã xóa kỹ thuật viên khỏi lịch bảo trì ID: " << currentSchedule->maintenance_id << ".\n";
                break;
            }

            prevNode = currentTechNode;
            currentTechNode = currentTechNode->next;
        }

        currentSchedule = currentSchedule->next;
    }
}

void editTechnician(TechnicianList& list, MaintenanceScheduleList& scheduleList, DeviceList& deviceList) {
    if(list.head == nullptr) {
        cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
        return;
    }
    char id[20];
    cout << "\n\n\t\tNhập ID kỹ thuật viên cần sửa: ";
    cin >> id; cin.ignore();

    Technician* tech = searchTechnician(list, id);
    if (tech != nullptr) {
        cout << "\n\n\t\tĐã tìm thấy thiết bị với ID: " << id << "\n";

        // Hiển thị thông tin cũ
        cout << "\n\t\tThông tin hiện tại:\n";
        cout << "\t\tTên: " << tech->name << "\n";
        cout << "\t\tThông tin liên hệ: " << tech->contact_info << "\n";
        cout << "\t\tKỹ năng: " << tech->skills << "\n";
        cout << "\t\tTrạng thái: " << statusToString(tech->status) << "\n";

        // Nhập thông tin mới
        cout << "\n\n\t\tNhập thông tin mới (nhấn Enter để bỏ qua không thay đổi):\n";
        char newTechName[50];
        char newTechContact[50];
        char newTechSkill[100];
        int newTechStatus;

        // Tên nhân viên
        cout << "\t\tTên nhân viên [" << tech->name << "]: ";
        cin.getline(newTechName, sizeof(newTechName));
        if (strlen(newTechName) > 0) {
            strcpy(tech->name, newTechName);
        }

        // Thông tin liên hệ
        cout << "\t\tThông tin liên hệ [" << tech->contact_info << "]: ";
        cin.getline(newTechContact, sizeof(newTechContact));
        if (strlen(newTechContact) > 0) {
            strcpy(tech->contact_info, newTechContact);
        }

        // Kỹ năng
        cout << "\t\tKỹ năng [" << tech->skills << "]: ";
        cin.getline(newTechSkill, sizeof(newTechSkill));
        if (strlen(newTechSkill) > 0) {
            strcpy(tech->skills, newTechSkill);
        }
        //TechnicianStatus oldStatus = tech->status;
        // Trạng thái
        cout << "\t\tTrạng thái [" << statusToString(tech->status) << "]: ";
        cout << "\t\t\t1. Đang rảnh\n";
        cout << "\t\t\t2. Đang nghỉ phép\n";
        cout << "\t\t\t3. Đã nghỉ việc\n";
        cout << "\t\t\tNhập trạng thái mới (hoặc 0 để giữ nguyên): ";
        cin >> newTechStatus;
        cin.ignore();
        if(newTechStatus != 0)
        {
            if(tech->status == BUSY)
            {
                removeTechnicianFromSchedules(scheduleList, tech->technician_id);
            }
        }
        switch (newTechStatus)
        {
            case 1: tech->status = TECH_AVAILABLE; break;
            case 2: tech->status = ON_LEAVE; break;
            case 3: tech->status = RETIRED; break;
            default:
                break;
        }
        cout << "\n\n\t\tKỹ thuật viên đã được cập nhật.\n";}
        // Kiểm tra nếu người dùng nhập trạng thái hợp lệ
    //     if (newTechStatus >= 1 && newTechStatus <= 4) {
    //         tech->status = (TechnicianStatus)newTechStatus;
    //         if (tech->status == BUSY && oldStatus != BUSY) {
    //             cout << "\n\t\t\tKỹ thuật viên được chuyển sang trạng thái BUSY. Tạo lịch bảo trì...\n";
    //             if(!createMaintenanceForTechnician(tech, scheduleList, deviceList))
    //             {
    //                 cout << "\t\t\tKhông thể tạo lịch bảo trì. Trạng thái không thay đổi.\n";
    //                 tech->status = oldStatus;
    //             }
    //         }
    //         cout << "\t\t\tTrạng thái đã được cập nhật.\n";
    //     } else if (newTechStatus == 0) {
    //         cout << "\t\t\tTrạng thái giữ nguyên.\n";
    //     } else {
    //         cout << "\t\t\tTrạng thái không hợp lệ. Không có thay đổi nào.\n";
    //     }
    //     cin.ignore();
    //     cout << "\n\n\t\tKỹ thuật viên đã được cập nhật.\n";
    // } else {
    //     cout << "\n\n\t\tKhông tìm thấy kỹ thuật viên với ID: " << id << endl;
    // }
        
}
void deleteTechnician(TechnicianList& list, MaintenanceScheduleList& scheduleList) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
        return;
    }
    char technician_id[20];
    cout << "\n\n\t\tNhập ID thiết bị cần xóa: ";
    cin.getline(technician_id, sizeof(technician_id));
    Technician* tech = searchTechnician(list, technician_id);
    if (tech == nullptr) {
        cout << "\n\n\t\tKhông tìm thấy kỹ thuật viên với ID: " << technician_id << endl;
        return;
    }
    // Xóa kỹ thuật viên khỏi các lịch bảo trì
    MaintenanceSchedule* schedule = scheduleList.head;
    while (schedule) {
        TechnicianNode* prevNode = nullptr;
        TechnicianNode* currentNode = schedule->technicians;

        while (currentNode) {
            if (strcmp(currentNode->technician_id, technician_id) == 0) {
                // Xóa kỹ thuật viên khỏi danh sách kỹ thuật viên của lịch bảo trì
                if (prevNode) {
                    prevNode->next = currentNode->next;
                } else {
                    schedule->technicians = currentNode->next;
                }
                TechnicianNode* temp = currentNode;
                currentNode = currentNode->next;
                delete temp;
            } else {
                prevNode = currentNode;
                currentNode = currentNode->next;
            }
        }

        // Kiểm tra nếu lịch bảo trì không còn kỹ thuật viên nào, chuyển trạng thái sang chờ xử lý
        if (schedule->technicians == nullptr) {
            schedule->status = PENDING;
        }

        schedule = schedule->next;
    }

    // Xóa kỹ thuật viên khỏi danh sách kỹ thuật viên
    if (tech->prev != nullptr) {
        tech->prev->next = tech->next;
    } else {
        list.head = tech->next;
    }

    if (tech->next != nullptr) {
        tech->next->prev = tech->prev;
    } else {
        list.tail = tech->prev;
    }

    delete tech;
    cout << "\n\n\t\tKỹ thuật viên đã được xóa.\n";
}
void menuTechnician(TechnicianList& techList, MaintenanceScheduleList& scheduleList, DeviceList& deviceList) {

    int choice;
    char tech_id[20];

    do {
        clearScreen(); // Xóa màn hình trước khi hiển thị menu
        cout << "\n\n\t\t========= QUẢN LÝ KỸ THUẬT VIÊN =========\n";
        cout << "\n\t\t1. Thêm kỹ thuật viên\n";
        cout << "\t\t2. Hiển thị danh sách kỹ thuật viên\n";
        cout << "\t\t3. Sửa thông tin kỹ thuật viên\n";
        //cout << "\t\t4. Xóa kỹ thuật viên\n";
        cout << "\t\t5. Tìm kiếm kỹ thuật viên\n";
        //cout << "\t\t6. Tạo lịch bảo trì cho kỹ thuật viên\n";
        cout << "\t\t0. Thoát chương trình\n";
        cout << "\t\t====================================\n";
        cout << "\t\tNhập lựa chọn của bạn: ";
        cin >> choice;
        cin.ignore(); // Xóa ký tự xuống dòng còn sót lại trong bộ đệm

        clearScreen(); // Xóa màn hình trước khi thực hiện hành động

        switch (choice) {
            case 1: {
                while (true) {
                    addTechnician(techList);
                    // Hỏi người dùng có muốn tiếp tục thêm kỹ thuật viên không
                    char choice;
                    cout << "\n\t\tBạn có muốn thêm kỹ thuật viên khác không? (y/n): ";
                    cin >> choice;
                    cin.ignore();
                    if (tolower(choice) != 'y') {
                        cout << "\n\t\tHoàn thành việc thêm kỹ thuật viên.\n";
                        break;
                    }
                }
                break;
            }
            case 2: {
                // Hiển thị danh sách thiết bị
                displayTechnicians(techList);
                break;
            }
            case 3: {

                editTechnician(techList, scheduleList, deviceList);
                break;
            }
            case 4: {
                // Xóa thiết bị
                if(techList.head == nullptr) {
                    cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
                    break;
                }
                deleteTechnician(techList, scheduleList);
                break;
            }
            case 5: {
                if(techList.head == nullptr) {
                    cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
                    break;
                }
                // Tìm kiếm thiết bị
                cout << "\n\n\t\tNhập ID thiết bị cần tìm: ";
                cin.getline(tech_id, sizeof(tech_id));
                Technician* result = searchTechnician(techList, tech_id);
                if (result != nullptr) {
                    cout << "\n\n\t\tTìm thấy kỹ thuật viên:\n";
                    cout << "\n\t\tID: " << result->technician_id
                                         << "\n\t\tTên: " << result->name
                                         << "\n\t\tLiên hệ: " << result->contact_info
                                         << "\n\t\tKỹ năng: " << result->skills
                                         << "\n\t\tTrạng thái: " << statusToString(result->status) << endl;
                } else {
                    cout << "\n\n\t\tKhông tìm thấy kỹ thuật viên với ID: " << tech_id << endl;
                }
                break;
            }
            // case 6: {
            //     // Tạo lịch bảo trì cho kỹ thuật viên
            //     if(techList.head == nullptr) {
            //         cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
            //         break;
            //     }
            //     cout << "\n\n\t\tNhập ID kỹ thuật viên cần tạo lịch bảo trì: ";
            //     cin.getline(tech_id, sizeof(tech_id));
            //     Technician* tech = searchTechnician(techList, tech_id);
            //     if (tech) {
            //         createMaintenanceForTechnician(tech, scheduleList, deviceList);
            //     } else {
            //         cout << "\n\n\t\tKhông tìm thấy kỹ thuật viên với ID: " << tech_id << endl;
            //     }
            //     break;
            // }
            case 0: {
                cout << "\n\n\t\tThoát chương trình. Hẹn gặp lại!\n";
                break;
            }
            default:
                cout << "\n\n\t\tLựa chọn không hợp lệ. Vui lòng thử lại.\n";
        }

        // Dừng màn hình để người dùng xem kết quả trước khi quay lại menu
        if (choice != 0) {
            cout << "\n\n\t\tNhấn Enter để quay lại menu...";
            cin.ignore();
        }
    } while (choice != 0);
}
