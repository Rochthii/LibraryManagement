#include "Utils.h"
#include <random>

time_t layNgayHienTai() {
    return time(nullptr);
}

bool quaHan(time_t NgayMuon) {
    return difftime(layNgayHienTai(), NgayMuon) > 7 * 86400;
}

int sinhMaTheNgauNhien() {
    static std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> dis(1000, 9999);
    return dis(gen);  // TODO: Kiểm tra trùng trong BST
}