#include <iostream>
#include <fstream>
#include "../include/DocGia.h"
#include "../include/DauSach.h"

using namespace std;

void muonSach(PTRDG root, PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    (void)root; (void)dsDauSach; (void)soLuongDauSach;
    // Triển khai mượn sách
}

void traSach(PTRDG root, PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    (void)root; (void)dsDauSach; (void)soLuongDauSach;
    // Triển khai trả sách
}

void lietKeSachDangMuon(PTRDG root, int maThe, PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    (void)root; (void)maThe; (void)dsDauSach; (void)soLuongDauSach;
    // Triển khai liệt kê sách đang mượn
}

void lietKeQuaHan(PTRDG root) {
    (void)root;
    // Triển khai liệt kê quá hạn
}

void top10SachMuonNhieu(PTRDAUSACH dsDauSach[], int soLuongDauSach, PTRDG root) {
    (void)dsDauSach; (void)soLuongDauSach; (void)root;
    // Triển khai top 10 sách
}
