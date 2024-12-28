#include "menu.h"
#include "Device.h"
#include "Technician.h"
#include "MaintenanceSchedule.h"
// Khởi tạo danh sách thiết bị
void initDeviceList(DeviceList& list) {
    list.head = nullptr;
    list.tail = nullptr;
}
const char* statusToString(DeviceStatus status) {
    switch (status) {
        case DEVICE_AVAILABLE: return "Chưa sử dụng";
        case IN_USE: return "Đang sử dụng";
        case MAINTENANCE: return "Bảo trì";
        case OUT_OF_SERVICE: return "Ngừng hoạt động";
        default: return "Unknown";
    }
}

// Thêm thiết bị vào danh sách
void addDevice(DeviceList& list) {
    
    Device* newDevice = new Device;

    generateAutoID(newDevice->device_id, "DEV");
    cout << "\n\n\t\tTạo thiết bị với ID: " << newDevice->device_id << endl;
    cout << "\t\tTên thiết bị: ";
    cin.getline(newDevice->device_name, 50);
    cout << "\t\tNgày mua (dd/mm/yyyy): ";
    cin.getline(newDevice->purchase_date, 15);
    newDevice->status = DEVICE_AVAILABLE;
    cout << "\t\tVị trí: ";
    cin.getline(newDevice->location, 50);

    newDevice->next = nullptr;
    newDevice->prev = nullptr;

    if (list.head == nullptr) { // Danh sách rỗng
        list.head = list.tail = newDevice;
    } else { // Thêm vào cuối danh sách
        list.tail->next = newDevice;
        newDevice->prev = list.tail;
        list.tail = newDevice;
    }

    cout << "\t\tThiết bị đã được thêm thành công!\n";

}


// Hiển thị danh sách thiết bị
void displayDeviceDetails(const Device* device) {
    if (device == nullptr) {
        cout << "\n\t\tKhông tìm thấy thiết bị.\n";
        return;
    }
    cout << "\n\t\tID: " << device->device_id
         << "\n\t\tTên: " << device->device_name
         << "\n\t\tNgày mua: " << device->purchase_date
         << "\n\t\tTrạng thái: " << statusToString(device->status)
         << "\n\t\tVị trí: " << device->location << endl;
}

void displayDevices(const DeviceList& list) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }

    cout << "\n\n\t\tDanh sách thiết bị:\n";
    Device* current = list.head;
    while (current != nullptr) {
        displayDeviceDetails(current);
    }
}

// Tìm kiếm thiết bị theo ID
Device* searchDevice(const DeviceList& list, const char* device_id) {
    Device* current = list.head;
    while (current != nullptr) {
        if (strcmp(current->device_id, device_id) == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Xóa thiết bị theo ID
void deleteDevice(DeviceList& list, MaintenanceScheduleList& scheduleList, TechnicianList& techList) {
    if (list.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }
    char device_id[20];
    cout << "\n\n\t\tNhập ID thiết bị cần xóa: ";
    cin.getline(device_id, sizeof(device_id));
    Device* current = searchDevice(list, device_id);
    if (current != nullptr) {
        // Xử lý xóa các lịch bảo trì liên quan đến thiết bị
        MaintenanceSchedule* schedule = scheduleList.head;
        while (schedule) {
            if (strcmp(schedule->device_id, device_id) == 0) {
                // Lưu lại lịch tiếp theo trước khi xóa
                MaintenanceSchedule* nextSchedule = schedule->next;

                // Duyệt qua danh sách kỹ thuật viên của lịch bảo trì
                TechnicianNode* techNode = schedule->technicians;
                while (techNode) {
                    Technician* tech = searchTechnician(techList, techNode->technician_id);
                    if (tech) {
                        // Kiểm tra xem kỹ thuật viên còn tham gia lịch bảo trì nào khác không
                        MaintenanceSchedule* otherSchedule = scheduleList.head;
                        bool isStillBusy = false;
                        while (otherSchedule) {
                            if (otherSchedule != schedule) { // Bỏ qua lịch bảo trì hiện tại
                                TechnicianNode* otherTechNode = otherSchedule->technicians;
                                while (otherTechNode) {
                                    if (strcmp(otherTechNode->technician_id, techNode->technician_id) == 0) {
                                        isStillBusy = true;
                                        break;
                                    }
                                    otherTechNode = otherTechNode->next;
                                }
                            }
                            if (isStillBusy) break;
                            otherSchedule = otherSchedule->next;
                        }

                        // Nếu không còn lịch bảo trì nào khác, đặt trạng thái rảnh
                        if (!isStillBusy) {
                            tech->status = TECH_AVAILABLE;
                        }
                    }
                    TechnicianNode* temp = techNode;
                    techNode = techNode->next;
                    delete temp; // Xóa node kỹ thuật viên
                }

                // Xóa lịch bảo trì khỏi danh sách
                if (schedule == scheduleList.head && schedule == scheduleList.tail) { // Chỉ có một lịch bảo trì
                    scheduleList.head = scheduleList.tail = nullptr;
                } else if (schedule == scheduleList.head) { // Xóa đầu
                    scheduleList.head = schedule->next;
                    if (scheduleList.head) scheduleList.head->prev = nullptr;
                } else if (schedule == scheduleList.tail) { // Xóa cuối
                    scheduleList.tail = schedule->prev;
                    if (scheduleList.tail) scheduleList.tail->next = nullptr;
                } else { // Xóa ở giữa
                    schedule->prev->next = schedule->next;
                    schedule->next->prev = schedule->prev;
                }

                delete schedule; // Xóa lịch bảo trì
                schedule = nextSchedule; // Chuyển sang lịch tiếp theo
            } else {
                schedule = schedule->next;
            }
        }

        // Xóa thiết bị khỏi danh sách thiết bị
        if (current == list.head && current == list.tail) { // Chỉ có một phần tử
            list.head = list.tail = nullptr;
        } else if (current == list.head) { // Xóa đầu
            list.head = current->next;
            list.head->prev = nullptr;
        } else if (current == list.tail) { // Xóa cuối
            list.tail = current->prev;
            list.tail->next = nullptr;
        } else { // Xóa ở giữa
            current->prev->next = current->next;
            current->next->prev = current->prev;
        }

        delete current; // Xóa thiết bị
        cout << "\n\n\t\tĐã xóa thiết bị với ID: " << device_id << endl;
    } else {
        cout << "\n\n\t\tKhông tìm thấy thiết bị với ID: " << device_id << endl;
    }
}

bool scheduleMaintenanceForDevice(Device* device, MaintenanceScheduleList& scheduleList, TechnicianList& techList) {
    if (!device) {
        cout << "\n\t\tThiết bị không hợp lệ.\n";
        return false;
    }
    if (!techList.head) {
        cout << "\n\t\tKhông có kỹ thuật viên nào trong danh sách.\n";
        return false;
    }
    if (device->status != IN_USE) {
        cout << "\t\tThiết bị không ở trạng thái sử dụng. Không thể tạo lịch bảo trì.\n";
        return false;
    }

    MaintenanceSchedule* newSchedule = new MaintenanceSchedule();
    bool isUnique = false;

    // Tạo ID duy nhất cho lịch bảo trì
    while (!isUnique) {
        generateAutoID(newSchedule->maintenance_id, "MS");

        // Kiểm tra trùng lặp ID trong danh sách
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

    // Gán thông tin mặc định cho lịch bảo trì
    strcpy(newSchedule->device_id, device->device_id);
    cout << "\n\t\tNhập ngày dự kiến (dd/mm/yyyy): ";
    cin.getline(newSchedule->scheduled_date, sizeof(newSchedule->scheduled_date));
    strcpy(newSchedule->maintenance_notes, "Lịch bảo trì được tự động tạo.");
    newSchedule->status = PENDING;
    newSchedule->technicians = nullptr;

    // Gán kỹ thuật viên
    cout << "\n\t\tChọn kỹ thuật viên cho lịch bảo trì (nhập 'done' để kết thúc):\n";
    char techID[20];
    while (true) {
        cout << "\n\t\t\tNhập ID kỹ thuật viên: ";
        cin >> techID;
        cin.ignore();
        if (strcmp(techID, "done") == 0) break; // Thoát nếu nhập "done"

        bool techFound = false;
        Technician* tech = techList.head;
        TechnicianNode* techNode = newSchedule->technicians;

        // Kiểm tra xem kỹ thuật viên đã được thêm chưa
        bool alreadyAssigned = false;
        while (techNode) {
            if (strcmp(techNode->technician_id, techID) == 0) {
                alreadyAssigned = true;
                break;
            }
            techNode = techNode->next;
        }

        if (alreadyAssigned) {
            char stop[2];
            cout << "\t\t\tKỹ thuật viên này đã được thêm vào danh sách.\n";
            cout << "\t\t\tTiếp tục thêm kỹ thuật viên [Y/N]:"; cin >> stop; cin.ignore();
            if (strcmp(techID, "Y") == 0||strcmp(techID, "y") == 0) continue;
            else break;
        }

        // Tìm kỹ thuật viên trong danh sách kỹ thuật viên chính
        while (tech) {
            if (strcmp(tech->technician_id, techID) == 0 && (tech->status != ON_LEAVE && tech->status != RETIRED)) {
                // Thêm kỹ thuật viên vào danh sách của lịch bảo trì
                TechnicianNode* newTechNode = new TechnicianNode();
                strcpy(newTechNode->technician_id, tech->technician_id);
                newTechNode->next = newSchedule->technicians;
                newSchedule->technicians = newTechNode;

                // Cập nhật trạng thái kỹ thuật viên
                tech->status = BUSY;

                cout << "\t\t\tĐã thêm kỹ thuật viên: " << tech->name << ".\n";
                cout << "\n\t\t\tNhấn Enter để tiếp tục...";
                cin.ignore();
                techFound = true;
                break;
            }
            tech = tech->next;
        }

        if (!techFound) {
            cout << "\t\t\tKỹ thuật viên không tồn tại hoặc không sẵn sàng.\n";
            cout << "\n\t\t\tNhấn Enter để tiếp tục...";
            cin.ignore();
        }
    }

    if (!newSchedule->technicians) {
        cout << "\t\t\tKhông có kỹ thuật viên nào được chọn. Không thể tạo lịch.\n";
        cout << "\n\t\t\tNhấn Enter để quay lại ...";
        cin.ignore();
        delete newSchedule;
        return false;
    }

    // Thêm lịch bảo trì vào danh sách liên kết đôi
    newSchedule->next = nullptr;
    newSchedule->prev = scheduleList.tail;
    if (scheduleList.tail) {
        scheduleList.tail->next = newSchedule;
    } else {
        scheduleList.head = newSchedule;
    }
    scheduleList.tail = newSchedule;

    // Cập nhật trạng thái thiết bị
    device->status = MAINTENANCE;

    // In thông báo thành công
    cout << "\t\t\tLịch bảo trì đã được tạo thành công với ID: " << newSchedule->maintenance_id << ".\n";
    cout << "\t\t\tNgày dự kiến: " << newSchedule->scheduled_date << ".\n";
    cout << "\t\t\tKỹ thuật viên tham gia:\n";
    TechnicianNode* techNode = newSchedule->technicians;
    while (techNode) {
        Technician* tech = searchTechnician(techList, techNode->technician_id);
        if (tech) {
            cout << "\t\t\t- " << tech->name << " (" << techNode->technician_id << ")\n";
        } else {
            cout << "\t\t\t- Không tìm thấy thông tin kỹ thuật viên với ID: " << techNode->technician_id << "\n";
        }
        techNode = techNode->next;
    }
    return true;
}

void maintainDevice(
    DeviceList& deviceList,
    MaintenanceScheduleList& scheduleList,
    TechnicianList& techList
) {
    if(deviceList.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }
    char deviceID[20];
    cout << "\n\t\tNhập ID thiết bị cần bảo trì: ";
    cin >> deviceID;
    cin.ignore();
    // Tìm thiết bị theo ID
    Device* device = searchDevice(deviceList, deviceID);
    if (!device) {
        cout << "\t\tKhông tìm thấy thiết bị với ID: " << deviceID << ".\n";
        return;
    }

    // Kiểm tra trạng thái thiết bị
    if (device->status == MAINTENANCE) {
        cout << "\t\tThiết bị đang trong trạng thái bảo trì. Không thể tạo thêm lịch bảo trì.\n";
        return;
    }
    if (device->status == DEVICE_AVAILABLE || device->status == OUT_OF_SERVICE) {
        cout << "\t\tThiết bị không phù hợp để bảo trì (chưa sử dụng hoặc đã ngừng hoạt động).\n";
        return;
    }

    // Tạo lịch bảo trì cho thiết bị
    cout << "\t\tThiết bị đủ điều kiện để bảo trì. Tạo lịch bảo trì...\n";
    if (scheduleMaintenanceForDevice(device, scheduleList, techList)) {
        cout << "\t\tLịch bảo trì đã được tạo thành công cho thiết bị [" << device->device_name << "].\n";
    } else {
        cout << "\t\tKhông thể tạo lịch bảo trì.\n";
    }

}

void editDevice(DeviceList& list, TechnicianList& technicianList, MaintenanceScheduleList& scheduleList) {
    if(list.head == nullptr) {
        cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
        return;
    }
    char id[20];
    cout << "\n\n\t\tNhập ID thiết bị cần sửa: ";
    cin >> id;
    cin.ignore();

    Device* current = searchDevice(list, id);
    if (current != nullptr) {
        cout << "\n\n\t\tĐã tìm thấy thiết bị với ID: " << id << "\n";

        // Hiển thị thông tin cũ
        cout << "\n\t\tThông tin hiện tại:\n";
        cout << "\t\tTên thiết bị: " << current->device_name << "\n";
        cout << "\t\tNgày mua: " << current->purchase_date << "\n";
        cout << "\t\tTrạng thái: " << statusToString(current->status) << "\n";
        cout << "\t\tVị trí: " << current->location << "\n";

        // Nhập thông tin mới
        cout << "\n\t\tNhập thông tin mới (nhấn Enter để bỏ qua không thay đổi):\n";

        char newDeviceName[50];
        char newPurchaseDate[20];
        int newStatus;
        char newLocation[50];

        // Tên thiết bị
        cout << "\n\t\tTên thiết bị [" << current->device_name << "]: ";
        cin.getline(newDeviceName, sizeof(newDeviceName));
        if (strlen(newDeviceName) > 0) {
            strcpy(current->device_name, newDeviceName);
        }

        // Ngày mua
        cout << "\t\tNgày mua [" << current->purchase_date << "]: ";
        cin.getline(newPurchaseDate, sizeof(newPurchaseDate));
        if (strlen(newPurchaseDate) > 0) {
            strcpy(current->purchase_date, newPurchaseDate);
        }

        cout << "\t\tTrạng thái [" << statusToString(current->status) << "]: ";
        cout << "\t\t\t1. Đang sử dụng\n";
        cout << "\t\t\t2. Ngừng hoạt động\n";
        cout << "\t\t\tNhập trạng thái mới (hoặc 0 để giữ nguyên): ";
        cin >> newStatus;
        cin.ignore();
        switch (newStatus)
        {
        case 1: 
        {
            if(current->status == DEVICE_AVAILABLE)
            {
                //cout << "\t\t\tThiết bị đang trong trạng thái bảo trì. Không thể chuyển sang trạng thái 'Chưa sử dụng'.\n";
                current->status = IN_USE;
            }
            else if(current->status == MAINTENANCE)
            {
                cout << "\t\t\tThiết bị đang trong trạng thái bảo trì. Không thể chuyển sang trạng thái 'Đang sử dụng'.\n";
            }
            else if(current->status == OUT_OF_SERVICE)
            {
                cout << "\t\t\tThiết bị đã ngừng hoạt động.\n";
                }
            break;
        }
        case 2: 
        {
            if(current->status == MAINTENANCE)
            {
                cout << "\t\t\tThiết bị đang trong trạng thái bảo trì. Không thể chuyển sang trạng thái 'Đang sử dụng'.\n";
            }
            else current->status = OUT_OF_SERVICE; 
            break;
        }
        default:
            //cout << "\t\t\tTrạng thái không hợp lệ. Không có thay đổi nào.\n";
            break;
        }
        // Vị trí
        cout << "\t\tVị trí [" << current->location << "]: ";
        cin.getline(newLocation, sizeof(newLocation));
        if (strlen(newLocation) > 0) {
            strcpy(current->location, newLocation);
        }

        cout << "\n\t\tCập nhật thông tin thiết bị thành công!\n";
    }else {
        cout << "\n\n\t\tKhông tìm thấy thiết bị với ID: " << id << "\n";
    }
}
void menuDevice(DeviceList& deviceList, TechnicianList& technicianList, MaintenanceScheduleList& scheduleList) {

    int choice;
    char device_id[20];

    do {
        clearScreen(); // Xóa màn hình trước khi hiển thị menu
        cout << "\n\n\t\t========= QUẢN LÝ THIẾT BỊ =========\n";
        cout << "\n\t\t1. Thêm thiết bị\n";
        cout << "\t\t2. Hiển thị danh sách thiết bị\n";
        cout << "\t\t3. Sửa thông tin thiết bị\n";
        cout << "\t\t4. Tìm kiếm thiết bị\n";
        cout << "\t\t0. Quay lại chương trình chính\n";
        cout << "\t\t====================================\n";
        cout << "\t\tNhập lựa chọn của bạn: ";
        cin >> choice;
        cin.ignore(); // Xóa ký tự xuống dòng còn sót lại trong bộ đệm

        clearScreen(); // Xóa màn hình trước khi thực hiện hành động

        switch (choice) {
            case 1: {
                while (true) {
                    addDevice(deviceList);
                // Hỏi người dùng có muốn thêm thiết bị khác không
                    char choice;
                    cout << "\n\t\tTiếp tục thêm thiết bị? (y/n): ";
                    cin >> choice;
                    cin.ignore();
                    if (tolower(choice) != 'y') {
                        cout << "\n\t\tQuá trình thêm thiết bị đã kết thúc.\n";
                        break;
                    }
                }
                break;
            }
            case 2: {
                // Hiển thị danh sách thiết bị
                displayDevices(deviceList);
                break;
            }
            case 3: {
                editDevice(deviceList, technicianList, scheduleList);
                break;
            }
            case 4: {
                // Xóa thiết bị
                if(deviceList.head == nullptr) {
                    cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
                    break;
                }
                deleteDevice(deviceList, scheduleList, technicianList);
                break;
            }
            case 5: {
                // Tìm kiếm thiết bị
                if(deviceList.head == nullptr) {
                    cout << "\n\n\t\tDanh sách thiết bị rỗng.\n";
                    break;
                }
                cout << "\n\n\t\tNhập ID thiết bị cần tìm: ";
                cin.getline(device_id, sizeof(device_id));
                Device* temp = searchDevice(deviceList, device_id);
                if(temp!=NULL) {
                    displayDeviceDetails(temp);
                }
                else cout << "\n\n\t\tKhông tìm thấy thiết bị với ID: " << device_id;
                break;
            }
            case 6: {
                maintainDevice(deviceList, scheduleList, technicianList);
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
