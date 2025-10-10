#include "include/ThongBao.h"

// -------------------------
// Hiển thị / tạo chuỗi thông báo
// -------------------------
// Hiển thị thông báo
void thongBao(std::ostream& out, const std::string& msg, LoaiThongBao loai) {
    const char* tienTo[] = {"Lỗi: ", "Thông tin: ", "Cảnh báo: "};
    out << tienTo[loai] << msg << "\n";
}

// Thông báo lỗi với số dòng
std::string thongBaoLoi(int soDong, const std::string& chiTiet, bool canhBao) {
    std::string prefix = canhBao ? std::string("Cảnh báo: ") : std::string("Lỗi: ");
    return prefix + "Dòng " + std::to_string(soDong) + ": " + chiTiet;
}

// Thông báo lỗi danh mục sách
std::string thongBaoLoiDMS(int soDong, const std::string& chiTiet, bool canhBao) {
    std::string prefix = canhBao ? std::string("Cảnh báo: ") : std::string("Lỗi: ");
    return prefix + "Dòng DMS " + std::to_string(soDong) + ": " + chiTiet;
}