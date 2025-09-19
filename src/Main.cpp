#include "DauSach.h"
#include "DocGia.h"
#include "Utils.h"
#include <iostream>
#include <fstream>

void menu() {
    while (true) {
        std::cout << "1. Nhap the doc gia\n2. In danh sach doc gia\n"
                  << "3. Nhap dau sach\n4. In danh sach dau sach\n"
                  << "5. Tim sach\n6. Muon sach\n7. Tra sach\n"
                  << "8. Liet ke sach dang muon\n9. Doc gia qua han\n"
                  << "10. Top 10 sach muon nhieu\n0. Thoat\nChon: ";
        int choice;
        std::cin >> choice;
        switch (choice) {
            case 0:
                {
                    std::ofstream file("data/thuvien.dat", std::ios::binary);
                    saveDauSach(file);
                    saveDocGia(file);
                    file.close();
                    return;
                }
            // TODO: Thêm case cho các chức năng
        }
    }
}

int main() {
    std::ifstream file("data/thuvien.dat", std::ios::binary);
    if (file.is_open()) {
        loadDauSach(file);
        loadDocGia(file);
        file.close();
    }
    
    menu();
    return 0;
}