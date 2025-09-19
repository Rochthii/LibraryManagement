#include <iostream>
#include <fstream>
#include "../include/DauSach.h"

using namespace std;

void themDauSach(DauSach* dsDauSach[], int& soLuongDauSach) {
    if (soLuongDauSach >= MAX_DAUSACH) {
        cout << "Danh sach dau sach day!\n";
        return;
    }
    DauSach* newDS = new DauSach;
    cout << "Nhap ISBN: "; getline(cin, newDS->ISBN);
    cout << "Nhap ten sach: "; getline(cin, newDS->tenSach);
    cout << "Nhap so trang: "; cin >> newDS->soTrang;
    cin.ignore();
    cout << "Nhap tac gia: "; getline(cin, newDS->tacGia);
    cout << "Nhap nam xuat ban: "; cin >> newDS->namXuatBan;
    cin.ignore();
    cout << "Nhap the loai: "; getline(cin, newDS->theLoai);
    newDS->dms = nullptr;

    int soSach;
    cout << "Nhap so luong sach: "; cin >> soSach;
    cin.ignore();
    themDanhMucSach(newDS->dms, newDS->ISBN, soSach);

    // Insertion sort theo tenSach
    int i = soLuongDauSach - 1;
    while (i >= 0 && dsDauSach[i]->tenSach > newDS->tenSach) {
        dsDauSach[i + 1] = dsDauSach[i];
        i--;
    }
    dsDauSach[i + 1] = newDS;
    soLuongDauSach++;
}

void inDauSachTheoTheLoai(DauSach* dsDauSach[], int soLuongDauSach) {
    // Triển khai in theo thể loại
}

void timSachTheoTen(DauSach* dsDauSach[], int soLuongDauSach, string tenSach) {
    // Triển khai tìm sách
}

void docDauSachTuFile(DauSach* dsDauSach[], int& soLuongDauSach) {
    // Đọc từ dausach.txt
}

void ghiDauSachVaoFile(DauSach* dsDauSach[], int soLuongDauSach) {
    // Ghi vào dausach.txt
}
