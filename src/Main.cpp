#include <iostream>
#include "../include/DocGia.h"
#include "../include/DauSach.h"

using namespace std;

// Global variables definition
DauSach* dsDauSach[MAX_DAUSACH];
int soLuongDauSach = 0;

int main() {
    DocGia* cayDocGia = nullptr;

    // Đọc dữ liệu từ file
    docDauSachTuFile(dsDauSach, soLuongDauSach);
    docDocGiaTuFile(cayDocGia);

    int choice;
    do {
        cout << "\n=== QUAN LY THU VIEN ===\n";
        cout << "1. Nhap the doc gia\n";
        cout << "2. In danh sach doc gia\n";
        cout << "3. Nhap dau sach\n";
        cout << "4. In danh sach dau sach theo the loai\n";
        cout << "5. Tim sach theo ten\n";
        cout << "6. Muon sach\n";
        cout << "7. Tra sach\n";
        cout << "8. Liet ke sach dang muon\n";
        cout << "9. Liet ke doc gia qua han\n";
        cout << "10. Top 10 sach muon nhieu nhat\n";
        cout << "0. Thoat\n";
        cout << "Chon: "; cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: themDocGia(cayDocGia); break;
            case 2: {
                cout << "1. Theo ten | 2. Theo ma: "; int subChoice; cin >> subChoice;
                if (subChoice == 1) inDocGiaTheoTen(cayDocGia);
                else inDocGiaTheoMa(cayDocGia);
                break;
            }
            case 3: themDauSach(dsDauSach, soLuongDauSach); break;
            case 4: inDauSachTheoTheLoai(dsDauSach, soLuongDauSach); break;
            case 5: {
                string tenSach; cout << "Nhap ten sach: "; getline(cin, tenSach);
                timSachTheoTen(dsDauSach, soLuongDauSach, tenSach); break;
            }
            case 6: muonSach(cayDocGia, dsDauSach, soLuongDauSach); break;
            case 7: traSach(cayDocGia, dsDauSach, soLuongDauSach); break;
            case 8: {
                int maThe; cout << "Nhap ma the: "; cin >> maThe;
                lietKeSachDangMuon(cayDocGia, maThe, dsDauSach, soLuongDauSach); break;
            }
            case 9: lietKeQuaHan(cayDocGia); break;
            case 10: top10SachMuonNhieu(dsDauSach, soLuongDauSach, cayDocGia); break;
            case 0: break;
            default: cout << "Lua chon khong hop le!\n";
        }
    } while (choice != 0);

    // Ghi dữ liệu trước khi thoát
    ghiDauSachVaoFile(dsDauSach, soLuongDauSach);
    ghiDocGiaVaoFile(cayDocGia);

    // Giải phóng bộ nhớ
    // (Thêm hàm giải phóng cho dsDauSach, dms, cayDocGia, dsMuonTra)

    return 0;
}
