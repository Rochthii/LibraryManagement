#include "include/ThongBao.h"

void thongBao(std::ostream& out, const std::string& msg, LoaiThongBao loai) {
    const char* tienTo[] = {"Lỗi: ", "Thông tin: ", "Cảnh báo: "};
    out << tienTo[loai] << msg << "\n";
}

std::string thongBaoLoi(int soDong, const std::string& chiTiet, bool canhBao) {
    std::string prefix = canhBao ? std::string("Cảnh báo: ") : std::string("Lỗi: ");
    return prefix + "Dòng " + std::to_string(soDong) + ": " + chiTiet;
}

std::string thongBaoLoiDMS(int soDong, const std::string& chiTiet, bool canhBao) {
    std::string prefix = canhBao ? std::string("Cảnh báo: ") : std::string("Lỗi: ");
    return prefix + "Dòng DMS " + std::to_string(soDong) + ": " + chiTiet;
}

void thongBaoSoLanThuConLai(int soLanDaThu, std::ostream& out, int maxLanThu) {
    if (soLanDaThu < maxLanThu) {
        out << "Bạn còn " << (maxLanThu - soLanDaThu) << "/" << maxLanThu << " lần thử." << std::endl;
    }
}