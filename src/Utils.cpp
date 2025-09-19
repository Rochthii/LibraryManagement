#include <iostream>
#include <ctime>
#include "Utils.h"

using namespace std;

string sinhMaSach(string ISBN, int soThuTu) {
    return ISBN + "-" + to_string(soThuTu);
}

int sinhMaThe(DocGia* root) {
    // Triển khai sinh mã thẻ
    return 0;
}

bool kiemTraQuaHan(string ngayMuon) {
    // Triển khai kiểm tra quá hạn
    return false;
}

string layNgayHienTai() {
    // Triển khai lấy ngày hiện tại
    return "";
}

int tinhSoNgayQuaHan(string ngayMuon) {
    // Triển khai tính ngày quá hạn
    return 0;
}
