#include <iostream>

using namespace std;
#include <cstring>

#include "Device.h"
#include "MaintenanceLog.h"
#include "MaintenanceSchedule.h"
#include "Technician.h"
#include "menu.h"

int main() {
    // DeviceList deviceList;
    // initDeviceList(deviceList);

    // int choice;
    // char device_id[20];

    // do {
    //     clearScreen(); // Xóa màn hình trước khi hiển thị menu
    //     menu();
    //     cin >> choice;
    //     cin.ignore(); // Xóa ký tự xuống dòng còn sót lại trong bộ đệm

    //     clearScreen(); // Xóa màn hình trước khi thực hiện hành động

    //     switch (choice) {
    //         case 1: {

    //             addDevice(deviceList);
    //             break;
    //         }
    //         case 2: {
    //             // Hiển thị danh sách thiết bị
    //             displayDevices(deviceList);
    //             break;
    //         }
    //         case 3: {

    //             editDevice(deviceList);
    //             break;
    //         }
    //         case 4: {
    //             // Xóa thiết bị
    //             cout << "Nhập ID thiết bị cần xóa: ";
    //             cin.getline(device_id, sizeof(device_id));
    //             deleteDevice(deviceList, device_id);
    //             break;
    //         }
    //         case 5: {
    //             // Tìm kiếm thiết bị
    //             cout << "Nhập ID thiết bị cần tìm: ";
    //             cin.getline(device_id, sizeof(device_id));
    //             searchDevice(deviceList, device_id);
    //             break;
    //         }
    //         case 0: {
    //             cout << "Thoát chương trình. Hẹn gặp lại!\n";
    //             break;
    //         }
    //         default:
    //             cout << "Lựa chọn không hợp lệ. Vui lòng thử lại.\n";
    //     }

    //     // Dừng màn hình để người dùng xem kết quả trước khi quay lại menu
    //     if (choice != 0) {
    //         cout << "\nNhấn Enter để quay lại menu...";
    //         cin.ignore();
    //     }
    // } while (choice != 0);
    menu();

    return 0;
}
