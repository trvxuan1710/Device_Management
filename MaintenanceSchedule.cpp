// Struct lịch bảo trì
#include "menu.h"
#include "MaintenanceSchedule.h"
#include "Device.h"
#include "Technician.h"
#include "MaintenanceLog.h"
// Khởi tạo danh sách lịch bảo trì
void initMaintenanceScheduleList(MaintenanceScheduleList& list) {
    list.head = nullptr;
    list.tail = nullptr;
}
const char* statusToString(Status status) {
    switch (status) {
        case PENDING: return "Chờ xử lý";
        case IN_PROGRESS: return "Đang bảo trì";
        case COMPLETED: return "Đã bảo trì";
        default: return "UNKNOWN";
    }
}
bool isTechnicianInSchedule(TechnicianNode* technicians, const char* technician_id) {
    TechnicianNode* current = technicians;
    while (current) {
        if (strcmp(current->technician_id, technician_id) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}
int countTechnicians(const TechnicianList& list) {
    int count = 0;
    Technician* current = list.head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}
// Thêm lịch bảo trì

void writeCompletedSchedulesToFile(const MaintenanceLogList& logList, 
                                   const DeviceList& deviceList, 
                                   const TechnicianList& techList, 
                                   const char* filename) {

    ofstream outFile(filename, ios::out);

    if (!outFile) {
        cerr << "\n\n\t\tKhông thể mở file để ghi.\n";
        return;
    }

    MaintenanceLog* currentLog = logList.head;
    while (currentLog != nullptr) {
        outFile << "Lịch Bảo Trì Hoàn Thành:\n";
        outFile << "----------------------------------------\n";
        outFile << "ID Lịch Bảo Trì: " << currentLog->log_id << "\n"
                << "ID Thiết Bị: " << currentLog->device_id << "\n"
                << "Ngày Hoàn Thành: " << currentLog->performed_date << "\n"
                << "Chi Phí: " << currentLog->cost << "\n"
                << "Chi Tiết: " << currentLog->details << "\n";

        // Ghi thông tin thiết bị
        Device* device = searchDevice(deviceList, currentLog->device_id);
        outFile << "Thông Tin Thiết Bị:\n";
        if (device != nullptr) {
            outFile << "\tTên: " << device->device_name 
                    << " | Vị Trí: " << device->location << "\n";
        } else {
            outFile << "\tKhông Tìm Thấy Thông Tin\n";
        }

        // Ghi danh sách kỹ thuật viên
        outFile << "Danh Sách Kỹ Thuật Viên:\n";
        TechnicianNode* techNode = currentLog->technicians;
        if (techNode == nullptr) {
            outFile << "\tKhông Có Kỹ Thuật Viên\n";
        } else {
            while (techNode != nullptr) {
                Technician* tech = searchTechnician(techList, techNode->technician_id);
                if (tech != nullptr) {
                    outFile << "\tID: " << techNode->technician_id 
                            << " | Tên: " << tech->name 
                            << " | Liên Hệ: " << tech->contact_info << "\n";
                } else {
                    outFile << "\tID: " << techNode->technician_id 
                            << " (Không Tìm Thấy Thông Tin)\n";
                }
                techNode = techNode->next;
            }
        }

        outFile << "----------------------------------------\n";
        currentLog = currentLog->next;
    }

    outFile.close();
    cout << "\n\n\t\tGhi thành công danh sách lịch bảo trì hoàn thành ra file " << filename << ".\n";
}


void addMaintenanceSchedules(MaintenanceScheduleList& list, TechnicianList& techList, DeviceList& deviceList) {
    char continueAdding;
    do {
        MaintenanceSchedule* newSchedule = new MaintenanceSchedule;
        if (!newSchedule) {
            cout << "\n\n\t\tKhông thể cấp phát bộ nhớ!\n";
            return;
        }

        cout << "\n\n\t\tNhập thông tin lịch bảo trì:\n";

        // Tạo ID lịch bảo trì tự động
        generateAutoID(newSchedule->maintenance_id, "MS");
        cout << "\t\tID lịch bảo trì (tự động): " << newSchedule->maintenance_id << endl;

        // Kiểm tra danh sách thiết bị
        char deviceID[20];
        if (deviceList.head == nullptr) {
            cout << "\n\n\t\tDanh sách thiết bị trống. Bạn cần thêm thiết bị mới.\n";
            addDevice(deviceList); // Gọi hàm thêm thiết bị
            if (deviceList.tail) {
                strcpy(deviceID, deviceList.tail->device_id); // Lấy ID của thiết bị vừa thêm
                cout << "\n\t\tThiết bị mới thêm có ID: " << deviceID << endl;
            } else {
                cout << "\n\t\tKhông thể thêm thiết bị. Hủy tạo lịch bảo trì này.\n";
                delete newSchedule;
                return;
            }
        } else {
            cout << "\t\tNhập ID thiết bị: ";
            cin >> deviceID;
            cin.ignore();
        }

        Device* device = searchDevice(deviceList, deviceID);
        if (!device) {
            cout << "\t\tThiết bị không tồn tại. Hủy tạo lịch bảo trì này. Tiếp tục tạo lịch bảo trì (Y/N): ";
            delete newSchedule;
            cin >> continueAdding; cin.ignore();
            if(continueAdding == 'y' || continueAdding == 'Y') {
                continue;
            } else {
                return;
            }
        }
        device->status = MAINTENANCE; // Đặt trạng thái thiết bị là đang bảo trì
        strcpy(newSchedule->device_id, deviceID);

        // Kiểm tra danh sách kỹ thuật viên
        // Khởi tạo danh sách kỹ thuật viên
        char addMore;
        int addedTechCount = 0;
        newSchedule->technicians = nullptr;
        do {
            char techID[20];
            if (techList.head == nullptr) {
                cout << "\n\n\t\tDanh sách kỹ thuật viên trống. Bạn cần thêm mới kỹ thuật viên.\n";
                addTechnician(techList); // Gọi hàm thêm kỹ thuật viên
                if (techList.tail) {
                    strcpy(techID, techList.tail->technician_id); // Lấy ID kỹ thuật viên vừa thêm
                    cout << "\n\t\tKỹ thuật viên mới thêm có ID: " << techList.tail->technician_id << endl;
                } else {
                    cout << "\n\t\tKhông thể thêm kỹ thuật viên. Hủy tạo lịch bảo trì này.\n";
                    delete newSchedule;
                    return;
                }
            }
            // Kiểm tra số lượng kỹ thuật viên hiện tại
            else if (addedTechCount == countTechnicians(techList)) { // Hàm đếm kỹ thuật viên
                cout << "\n\n\t\tTất cả kỹ thuật viên hiện tại đã được thêm vào lịch bảo trì.\n";
                cout << "\t\tBạn cần thêm mới kỹ thuật viên.\n";
                addTechnician(techList); // Gọi hàm thêm kỹ thuật viên mới
                if (techList.tail) {
                    strcpy(techID, techList.tail->technician_id); // Lấy ID kỹ thuật viên vừa thêm
                    cout << "\n\t\tKỹ thuật viên mới thêm có ID: " << techID << endl;
                } else {
                    cout << "\n\t\tKhông thể thêm kỹ thuật viên. Hủy tạo lịch bảo trì này.\n";
                    delete newSchedule;
                    return;
                }
            } else {
                cout << "\t\tNhập ID kỹ thuật viên: ";
                cin >> techID;
                cin.ignore();
            }

            Technician* tech = searchTechnician(techList, techID);
            if (!tech) {
                cout << "\t\tKỹ thuật viên không tồn tại. Vui lòng thử lại! Bạn có muốn thêm kỹ thuật viên khác? (y/n): ";
                cin >> addMore; cin.ignore();
                if(addMore == 'y' || addMore == 'Y') {
                    continue;
                } else {
                    break;
                }
            }

            // Kiểm tra trùng lặp kỹ thuật viên trong lịch bảo trì
            if (isTechnicianInSchedule(newSchedule->technicians, techID)) {
                cout << "\t\tKỹ thuật viên đã được thêm vào lịch bảo trì này. Vui lòng thử lại! Bạn có muốn thêm kỹ thuật viên khác? (y/n): ";
                cin >> addMore; cin.ignore();
                if(addMore == 'y' || addMore == 'Y') {
                    continue;
                } else {
                    break;
                }
            }

            // Thêm kỹ thuật viên vào danh sách liên kết trong lịch bảo trì
            TechnicianNode* newTechNode = new TechnicianNode;
            strcpy(newTechNode->technician_id, techID);
            newTechNode->next = nullptr;

            if (newSchedule->technicians == nullptr) {
                newSchedule->technicians = newTechNode;
            } else {
                TechnicianNode* temp = newSchedule->technicians;
                while (temp->next != nullptr) {
                    temp = temp->next;
                }
                temp->next = newTechNode;
            }
            if(tech->status == TECH_AVAILABLE) {
                tech->status = BUSY;
            }
            addedTechCount++;
            cout << "\t\tKỹ thuật viên đã được thêm vào danh sách bảo trì.\n";
            cout << "\t\tBạn có muốn thêm kỹ thuật viên khác? (y/n): ";
            cin >> addMore; cin.ignore();
        } while (addMore == 'y' || addMore == 'Y');

        // Nhập thông tin còn lại
        cout << "\t\tNgày dự kiến (dd/mm/yyyy): ";
        cin >> newSchedule->scheduled_date;
        cin.ignore();
        cout << "\t\tGhi chú: ";
        cin.getline(newSchedule->maintenance_notes, 100);
        newSchedule->status = PENDING;

        // Thêm vào danh sách lịch bảo trì
        newSchedule->next = nullptr;
        newSchedule->prev = nullptr;
        if (!list.head) {
            list.head = list.tail = newSchedule;
        } else {
            list.tail->next = newSchedule;
            newSchedule->prev = list.tail;
            list.tail = newSchedule;
        }

        cout << "\n\n\t\tLịch bảo trì đã được thêm thành công!\n";

        cout << "\n\n\t\tBạn có muốn tạo thêm lịch bảo trì khác? (y/n): ";
        cin >> continueAdding;
        cin.ignore();

    } while (continueAdding == 'y' || continueAdding == 'Y');
}


//Hiển thị danh sách lịch bảo trì
void displayMaintenanceSchedules(const MaintenanceSchedule* current, const DeviceList& deviceList, const TechnicianList& techList) {
    cout << "\n\t\tID lịch bảo trì: " << current->maintenance_id
             << "\n\t\tNgày dự kiến: " << current->scheduled_date
             << "\n\t\tTrạng thái: " << statusToString(current->status)
             << "\n\t\tGhi chú: " << current->maintenance_notes;

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

        // Hiển thị danh sách kỹ thuật viên
        cout << "\n\t\tDanh sách kỹ thuật viên:";
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
                    cout << "\n\t\t\tID: " << techNode->technician_id
                         << " (Không tìm thấy thông tin)";
                }
                techNode = techNode->next; // Chuyển sang kỹ thuật viên tiếp theo
            }
        }
}

/*void updateTechnicians(TechnicianNode*& technicians, const char* newTechIDs, const TechnicianList& techList) {
    // Xóa danh sách kỹ thuật viên cũ
    while (technicians != nullptr) {
        TechnicianNode* temp = technicians;
        technicians = technicians->next;
        delete temp;
    }

    // Tách các ID kỹ thuật viên từ chuỗi newTechIDs bằng cách sử dụng strtok
    char* newTechIDsCopy = new char[strlen(newTechIDs) + 1];
    strcpy(newTechIDsCopy, newTechIDs); // Tạo một bản sao để sử dụng strtok
    char* token = strtok(newTechIDsCopy, " ");

    TechnicianNode* last = nullptr;

    while (token != nullptr) {
        // Kiểm tra xem ID kỹ thuật viên có tồn tại trong danh sách kỹ thuật viên không
        Technician* tech = searchTechnician(techList, token);
        if (tech) {
            // Tạo nút mới cho danh sách kỹ thuật viên
            TechnicianNode* newNode = new TechnicianNode;
            strcpy(newNode->technician_id, token);
            newNode->next = nullptr;

            // Thêm nút mới vào danh sách kỹ thuật viên
            if (last == nullptr) {
                technicians = newNode;
            } else {
                last->next = newNode;
            }
            last = newNode;
        } else {
            cout << "\t\tKỹ thuật viên với ID " << token << " không tồn tại. Bỏ qua ID này.\n";
        }

        // Lấy token tiếp theo
        token = strtok(nullptr, " ");
    }

    delete[] newTechIDsCopy; // Giải phóng bộ nhớ tạm

    if (technicians == nullptr) {
        cout << "\t\tDanh sách kỹ thuật viên hiện tại trống.\n";
    } else {
        cout << "\t\tDanh sách kỹ thuật viên đã được cập nhật.\n";
    }
} */
void updateTechnicians(TechnicianNode*& technicians, MaintenanceScheduleList& scheduleList, const TechnicianList& techList) {
    while (true) {
        cout << "\nChọn hành động:\n";
        cout << "1. Thêm kỹ thuật viên\n";
        cout << "2. Xóa kỹ thuật viên\n";
        cout << "3. Thoát\n";
        cout << "Lựa chọn của bạn: ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) { // Thêm kỹ thuật viên
            cout << "Nhập ID kỹ thuật viên muốn thêm: ";
            char techID[20];
            cin.getline(techID, sizeof(techID));

            Technician* tech = searchTechnician(techList, techID);
            if (!tech) {
                cout << "ID " << techID << " không tồn tại. Bỏ qua.\n";
                continue;
            }

            if (tech->status == ON_LEAVE) {
                cout << "Kỹ thuật viên ID: " << techID << " đang nghỉ phép. Không thể thêm.\n";
                continue;
            } else if (tech->status == RETIRED) {
                cout << "Kỹ thuật viên ID: " << techID << " đã nghỉ việc. Không thể thêm.\n";
                continue;
            }

            if (isTechnicianInSchedule(technicians, techID)) {
                cout << "Kỹ thuật viên ID: " << techID << " đã được gán vào lịch bảo trì này.\n";
                continue;
            }

            TechnicianNode* newNode = new TechnicianNode;
            strcpy(newNode->technician_id, techID);
            newNode->next = technicians;
            technicians = newNode;

            tech->status = BUSY;
            cout << "Thêm kỹ thuật viên ID: " << techID << " thành công.\n";

        } else if (choice == 2) { // Xóa kỹ thuật viên
            if (technicians == nullptr) {
                cout << "Danh sách hiện tại trống. Không thể xóa.\n";
                continue;
            }

            cout << "Nhập ID kỹ thuật viên muốn xóa: ";
            char deleteID[20];
            cin.getline(deleteID, sizeof(deleteID));

            TechnicianNode* prev = nullptr;
            TechnicianNode* temp = technicians;
            bool found = false;

            while (temp != nullptr) {
                if (strcmp(temp->technician_id, deleteID) == 0) {
                    found = true;
                    if (prev == nullptr) {
                        technicians = temp->next;
                    } else {
                        prev->next = temp->next;
                    }
                    delete temp;
                    cout << "Xóa kỹ thuật viên ID: " << deleteID << " thành công.\n";
                    break;
                }
                prev = temp;
                temp = temp->next;
            }

            if (!found) {
                cout << "ID " << deleteID << " không tồn tại trong danh sách hiện tại.\n";
                continue;
            }

            Technician* tech = searchTechnician(techList, deleteID);
            if (tech) {
                bool stillAssigned = false;
                MaintenanceSchedule* currentSchedule = scheduleList.head;

                while (currentSchedule) {
                    if (isTechnicianInSchedule(currentSchedule->technicians, deleteID)) {
                        stillAssigned = true;
                        break;
                    }
                    currentSchedule = currentSchedule->next;
                }

                if (!stillAssigned) {
                    tech->status = TECH_AVAILABLE;
                    cout << "Kỹ thuật viên ID: " << deleteID << " hiện không tham gia lịch bảo trì nào. Trạng thái đã cập nhật thành 'Rảnh'.\n";
                }
            }
        } else if (choice == 3) { // Thoát
            cout << "Thoát cập nhật danh sách kỹ thuật viên.\n";
            break;
        } else {
            cout << "Lựa chọn không hợp lệ. Vui lòng thử lại.\n";
        }
    }
}


void displayListMaintenanceSchedules(const MaintenanceScheduleList& scheduleList, 
                                     const DeviceList& deviceList, 
                                     const TechnicianList& techList) {
    if (scheduleList.head == nullptr) {
        cout << "\n\n\t\tDanh sách lịch bảo trì rỗng.\n";
        return;
    }

    MaintenanceSchedule* current = scheduleList.head;
    while (current != nullptr) {
        // Hiển thị thông tin của một lịch bảo trì
        displayMaintenanceSchedules(current, deviceList, techList);

        // Chuyển sang lịch bảo trì tiếp theo
        current = current->next;

        // Thêm khoảng trống giữa các lịch để dễ đọc
        cout << "\n---------------------------------------------\n";
    }
}

MaintenanceSchedule* searchSchedule(MaintenanceScheduleList& list, const char* maintenance_id) {
    MaintenanceSchedule* current = list.head;
    while (current != nullptr) {
        if (strcmp(current->maintenance_id, maintenance_id) == 0) {
            return current; // Tìm thấy
        }
        current = current->next;
    }
    return nullptr; // Không tìm thấy
}

void editSchedule(MaintenanceScheduleList& list, MaintenanceLogList& logList, DeviceList& deviceList, TechnicianList& techList) {
    if(list.head == nullptr) {
        cout << "\n\n\t\tDanh sách lịch bảo trì rỗng.\n";
        return;
    }
    if(deviceList.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }
    if(techList.head == nullptr) {
        cout << "\n\n\t\tDanh sách kỹ thuật viên rỗng.\n";
        return;
    }
    char id[20];
    cout << "\n\n\t\tNhập ID lịch bảo trì cần sửa: ";
    cin >> id;
    cin.ignore();

    MaintenanceSchedule* schedule = searchSchedule(list, id);
    if (schedule != nullptr) {
        cout << "\n\n\t\tĐã tìm thấy lịch bảo trì với ID: " << id << "\n";

        // Hiển thị thông tin cũ
        cout << "\t\tThông tin hiện tại:\n";
        cout << "\t\tID thiết bị: " << schedule->device_id << "\n";
        cout << "\t\tNgày dự kiến: " << schedule->scheduled_date << "\n";
        cout << "\t\tTrạng thái: " << statusToString(schedule->status) << "\n";
        cout << "\t\tGhi chú: " << schedule->maintenance_notes << "\n";

        // Hiển thị danh sách kỹ thuật viên
        cout << "\t\tDanh sách kỹ thuật viên:\n";
        TechnicianNode* techNode = schedule->technicians;
        if (techNode == nullptr) {
            cout << "\t\t  Không có kỹ thuật viên nào được gán.\n";
        } else {
            while (techNode != nullptr) {
                Technician* tech = searchTechnician(techList, techNode->technician_id);
                if (tech) {
                    cout << "\t\t  - ID: " << tech->technician_id 
                         << " | Tên: " << tech->name 
                         << " | Liên hệ: " << tech->contact_info << "\n";
                } else {
                    cout << "\t\t  - ID: " << techNode->technician_id << " (Không tìm thấy thông tin)\n";
                }
                techNode = techNode->next;
            }
        }

        // Nhập thông tin mới
        cout << "\t\tNhập thông tin mới (nhấn Enter để bỏ qua không thay đổi):\n";

        // Ngày dự kiến
        char newScheduleDate[15];
        cout << "\t\tNgày dự kiến [" << schedule->scheduled_date << "]: ";
        cin.getline(newScheduleDate, sizeof(newScheduleDate));
        if (strlen(newScheduleDate) > 0) {
            strcpy(schedule->scheduled_date, newScheduleDate);
        }

        cout << "Danh sách kỹ thuật viên hiện tại:\n";
        TechnicianNode* temp = schedule->technicians;
        while (temp != nullptr) {
            cout <<"\n\t\t\t" << temp->technician_id << endl;
            temp = temp->next;
        }

        if (temp==nullptr) {
            cout << "(Danh sách hiện tại đang trống)\n";
        }

        /*// Chọn hành động
        cout << "\nChọn hành động:\n";
        cout << "1. Thêm kỹ thuật viên\n";
        cout << "2. Xóa kỹ thuật viên\n";
        cout << "3. Thoát\n";
        
        while (true)
        {
            cout << "Lựa chọn của bạn: ";
            int choice;
            cin >> choice;
            cin.ignore(); // Xóa ký tự thừa trong input buffer

            if (choice == 1) {
                // Thêm kỹ thuật viên mới
                cout << "Nhập ID kỹ thuật viên muốn thêm: ";
                char techID[20];
                cin >> techID; cin.ignore();

                Technician* tech = searchTechnician(techList, techID);
                if (tech) {
                    // Kiểm tra trạng thái của kỹ thuật viên
                    if (tech->status == ON_LEAVE) {
                        cout << "Kỹ thuật viên ID: " << techID << " đang nghỉ phép. Không thể thêm.\n";
                        continue;
                    } else if (tech->status == RETIRED) {
                        cout << "Kỹ thuật viên ID: " << techID << " đã nghỉ việc. Không thể thêm.\n";
                        continue;
                    }
                         // Thoát sớm nếu trạng thái không hợp lệ

                    // Kiểm tra kỹ thuật viên đã có trong lịch bảo trì chưa
                    TechnicianNode* temp = schedule->technicians;
                    while (temp != nullptr) {
                        if (strcmp(temp->technician_id, techID) == 0) {
                            cout << "Kỹ thuật viên ID: " << techID << " đã được gán vào lịch bảo trì này.\n";
                            continue; // Thoát sớm nếu đã tồn tại
                        }
                        temp = temp->next;
                    }

                    // Thêm kỹ thuật viên vào danh sách
                    TechnicianNode* newNode = new TechnicianNode;
                    strcpy(newNode->technician_id, techID);
                    newNode->next = schedule->technicians;
                    schedule->technicians = newNode;
                    tech->status = BUSY; // Đặt trạng thái kỹ thuật viên thành "Đang bận"
                    cout << "Thêm kỹ thuật viên ID: " << techID << " thành công.\n";
                } else {
                    cout << "ID " << techID << " không tồn tại. Bỏ qua.\n";
                }
            }
            else if (choice == 2) {
                // Xóa kỹ thuật viên
                if (schedule->technicians == nullptr) {
                    cout << "Danh sách hiện tại trống. Không thể xóa.\n";
                    continue;
                }

                cout << "Nhập ID kỹ thuật viên muốn xóa: ";
                char deleteID[20];
                cin >> deleteID; cin.ignore();

                TechnicianNode* prev = nullptr;
                TechnicianNode* temp = schedule->technicians;
                bool found = false;

                while (temp != nullptr) {
                    if (strcmp(temp->technician_id, deleteID) == 0) {
                        // Tìm thấy ID cần xóa
                        found = true;
                        if (prev == nullptr) {
                            // Xóa phần tử đầu danh sách
                            schedule->technicians = temp->next;
                        } else {
                            prev->next = temp->next;
                        }
                        delete temp;
                        cout << "Xóa kỹ thuật viên ID: " << deleteID << " thành công.\n";
                        break;
                    }
                    prev = temp;
                    temp = temp->next;
                }

                if (!found) {
                    cout << "ID " << deleteID << " không tồn tại trong danh sách hiện tại.\n";
                } else {
                    // Kiểm tra trạng thái của kỹ thuật viên
                    Technician* tech = searchTechnician(techList, deleteID);
                    if (tech) {
                        // Duyệt qua tất cả các lịch bảo trì để xem kỹ thuật viên này còn tham gia không
                        bool stillAssigned = false;
                        MaintenanceSchedule* scheduleNode = list.head;
                        while (scheduleNode != nullptr) {
                            TechnicianNode* techNode = scheduleNode->technicians;
                            while (techNode != nullptr) {
                                if (strcmp(techNode->technician_id, deleteID) == 0) {
                                    stillAssigned = true;
                                    break;
                                }
                                techNode = techNode->next;
                            }
                            if (stillAssigned) {
                                break;
                            }
                            scheduleNode = scheduleNode->next;
                        }

                        if (!stillAssigned) {
                            // Nếu không còn trong lịch bảo trì nào, đặt trạng thái thành "AVAILABLE"
                            tech->status = TECH_AVAILABLE;
                            cout << "Kỹ thuật viên ID: " << deleteID << " hiện không tham gia lịch bảo trì nào. Trạng thái đã cập nhật thành 'Rảnh'.\n";
                        }
                    } else {
                        cout << "Không tìm thấy thông tin kỹ thuật viên ID: " << deleteID << ".\n";
                    }
                }
            }
            else if (choice == 3) {
                cout << "Thoát cập nhật danh sách kỹ thuật viên.\n";
                break;
            } else {
                cout << "Lựa chọn không hợp lệ. Vui lòng thử lại.\n";
            }
        }*/
        // Trạng thái
        updateTechnicians(schedule->technicians, list, techList);
        int newScheduleStatus;
        cout << "\t\tTrạng thái [" << statusToString(schedule->status) << "]: ";
        cout << "\n\t\t\t1. Chờ xử lý\n";
        cout << "\t\t\t2. Đang bảo trì\n";
        cout << "\t\t\t3. Đã bảo trì\n";
        cout << "\t\t\tNhập trạng thái mới (hoặc 0 để giữ nguyên): ";
        cin >> newScheduleStatus;
        cin.ignore();

        if (newScheduleStatus >= 1 && newScheduleStatus <= 3) {
            if (newScheduleStatus == 2 && schedule->status == PENDING) { // Đang bảo trì
                if (schedule->technicians == nullptr) {
                    cout << "\t\tDanh sách kỹ thuật viên rỗng. Thêm kỹ thuật viên để tiếp tục (y/n).\n";
                    char newTechIDs;
                    cin >> newTechIDs; cin.ignore();
                    if (newTechIDs == 'y' || newTechIDs == 'Y') {
                        updateTechnicians(schedule->technicians, list, techList);
                        schedule->status = IN_PROGRESS;
                    }
                    if (schedule->technicians == nullptr) {
                        cout << "\t\tKhông thể chuyển sang trạng thái Đang bảo trì vì không có kỹ thuật viên nào được thêm.\n";
                        //return;
                    }
                }   else {
                    schedule->status = IN_PROGRESS;
                }
            } else if (newScheduleStatus == 3 && schedule->status == IN_PROGRESS) {
                schedule->status = COMPLETED;
                addMaintenanceLog(logList, schedule, deviceList, list);
                deleteSchedule(list, techList, deviceList, schedule->maintenance_id);
                writeCompletedSchedulesToFile(logList, deviceList, techList, "CompletedSchedules.txt");
                cout << "\t\tLịch bảo trì đã chuyển vào nhật ký và trạng thái được cập nhật.\n";
                return;
            } else {
                cout << "\t\tTrạng thái mới của lịch bảo trì không hợp lệ. Giữ nguyên trạng thái cũ.\n";
            }
        }
        // Ghi chú
        char newScheduleNote[100];
        cout << "\t\tGhi chú [" << schedule->maintenance_notes << "]: ";
        cin.getline(newScheduleNote, sizeof(newScheduleNote));
        if (strlen(newScheduleNote) > 0) {
            strcpy(schedule->maintenance_notes, newScheduleNote);
        }

        cout << "\t\tLịch bảo trì đã được cập nhật.\n";
    } else {
        cout << "\n\n\t\tKhông tìm thấy lịch bảo trì với ID: " << id << endl;
    }
}



void deleteSchedule(MaintenanceScheduleList& list, TechnicianList& techList, DeviceList& deviceList, const char* maintenance_id) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách lịch bảo trì rỗng.\n";
        return;
    }
    
    // Tìm lịch bảo trì cần xóa
    MaintenanceSchedule* schedule = searchSchedule(list, maintenance_id);
    if (schedule == nullptr) {
        cout << "\n\n\t\tKhông tìm thấy lịch bảo trì với ID: " << maintenance_id << endl;
        return;
    }

    // Cập nhật trạng thái các kỹ thuật viên trong lịch bảo trì
    TechnicianNode* techNode = schedule->technicians;
    while (techNode) {
        Technician* tech = searchTechnician(techList, techNode->technician_id);
        if (tech) {
            // Kiểm tra xem kỹ thuật viên còn lịch bảo trì nào khác không
            bool isBusy = false;
            MaintenanceSchedule* currentSchedule = list.head;
            while (currentSchedule) {
                if (currentSchedule != schedule) { // Bỏ qua lịch đang xóa
                    TechnicianNode* currentTechNode = currentSchedule->technicians;
                    while (currentTechNode) {
                        if (strcmp(currentTechNode->technician_id, techNode->technician_id) == 0) {
                            isBusy = true;
                            break;
                        }
                        currentTechNode = currentTechNode->next;
                    }
                }
                if (isBusy) break;
                currentSchedule = currentSchedule->next;
            }

            // Nếu không còn lịch bảo trì nào khác, đặt trạng thái là rảnh
            if (!isBusy) {
                tech->status = TECH_AVAILABLE;
            }
        }
        techNode = techNode->next;
    }
    // Device* device = searchDevice(deviceList, schedule->device_id);
    // if (device&&device->status==MAINTENANCE) {
    //     cout << "\n\n\t\tLịch bảo trì liên quan đến thiết bị có ID: " << device->device_id << ".\n";
    //     cout << "\t\tTrạng thái hiện tại của thiết bị: " << (device->status == MAINTENANCE ? "Bảo trì" : "Khác") << ".\n";
    //     cout << "\t\tCập nhật trạng thái mới cho thiết bị:\n";
    //     cout << "\t\t1. Sử dụng \n";
    //     cout << "\t\t2. Ngừng hoạt động \n";
    //     int newStatus;
    //     cout << "\t\tLựa chọn: ";
    //     cin >> newStatus;
    //     cin.ignore();

    //     switch (newStatus) {
    //         case 1:
    //             device->status = IN_USE;
    //             break;
    //         case 2:
    //             device->status = OUT_OF_SERVICE;
    //             break;
    //         default:
    //             cout << "\n\n\t\tLựa chọn không hợp lệ. Trạng thái thiết bị có trạng thái mặc định là đang sử dụng.\n";
    //             device->status = IN_USE;
    //             break;
    //     }
    // } else {
    //     cout << "\n\n\t\tKhông tìm thấy thiết bị liên quan đến lịch bảo trì này.\n";
    // }

    // Loại bỏ lịch bảo trì khỏi danh sách liên kết đôi
    if (schedule->prev != nullptr) {
        schedule->prev->next = schedule->next;
    } else {
        list.head = schedule->next;
    }

    if (schedule->next != nullptr) {
        schedule->next->prev = schedule->prev;
    } else {
        list.tail = schedule->prev;
    }

    // Giải phóng bộ nhớ danh sách kỹ thuật viên trong lịch bảo trì
    techNode = schedule->technicians;
    while (techNode) {
        TechnicianNode* temp = techNode;
        techNode = techNode->next;
        delete temp;
    }

    // Giải phóng bộ nhớ lịch bảo trì
    delete schedule;
    cout << "\n\n\t\tLịch bảo trì đã được xóa.\n";
}

void menuMaintenanceSchedule(MaintenanceScheduleList& ScheduleList, DeviceList& deviceList, TechnicianList& techList, MaintenanceLogList& MaintenanceLogList) {

    int choice;
    char schedule_id[20];

    do {
        clearScreen(); // Xóa màn hình trước khi hiển thị menu
        cout << "\n\n\t\t========= QUẢN LÝ LỊCH BẢO TRÌ =========\n";
        cout << "\n\t\t1. Thêm lịch bảo trì\n";
        cout << "\t\t2. Hiển thị danh sách lịch bảo trì\n";
        cout << "\t\t3. Sửa thông tin lịch bảo trì\n";
        //cout << "\t\t4. Xóa lịch bảo trì\n";
        cout << "\t\t5. Tìm kiếm lịch bảo trì\n";
        cout << "\t\t0. Thoát chương trình\n";
        cout << "\t\t====================================\n";
        cout << "\t\tNhập lựa chọn của bạn: ";
        cin >> choice;
        cin.ignore(); // Xóa ký tự xuống dòng còn sót lại trong bộ đệm

        clearScreen(); // Xóa màn hình trước khi thực hiện hành động

        switch (choice) {
            case 1: {

                addMaintenanceSchedules(ScheduleList, techList, deviceList);
                break;
            }
            case 2: {
                // Hiển thị danh sách thiết bị
                displayListMaintenanceSchedules(ScheduleList,deviceList,techList);
                break;
            }
            case 3: {

                editSchedule(ScheduleList, MaintenanceLogList, deviceList, techList);
                break;
            }
            case 4: {
                //Xóa thiết bị
                if(ScheduleList.head == nullptr) {
                    cout << "\n\n\t\tDanh sách lịch bảo trì rỗng.\n";
                    break;
                }
                cout << "\n\n\t\tNhập ID lịch bảo trì cần xóa: ";
                cin.getline(schedule_id, sizeof(schedule_id));
                deleteSchedule(ScheduleList, techList, deviceList, schedule_id);
                break;
            }
            case 5: {
                // Tìm kiếm thiết bị
                if(ScheduleList.head == nullptr) {
                    cout << "\n\n\t\tDanh sách lịch bảo trì rỗng.\n";
                    break;
                }
                cout << "\n\n\t\tNhập ID lịch bảo trì cần tìm: ";
                cin.getline(schedule_id, sizeof(schedule_id));
                MaintenanceSchedule* temp = searchSchedule(ScheduleList, schedule_id);
                displayMaintenanceSchedules(temp, deviceList, techList);
                break;
            }
            case 0: {
                cout << "\n\n\t\tTrở về chương trình chính";
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
