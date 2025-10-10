#include "include/NhapLieu.h"
#include "include/KiemTraDuLieu.h"
#include "../include/VietnameseUtils.h"
#include "include/XuLyChuoi.h"
#include "include/ThongBao.h"
#include "include/Constants.h"

void thongBaoSoLanThuConLai(int soLanDaThu, std::ostream& out, int maxLanThu) {
    if (soLanDaThu < maxLanThu)
        thongBao(out, "Còn " + std::to_string(maxLanThu - soLanDaThu) + " lần thử.", LOI);
}

// Kiểm tra EOF hoặc lỗi stream
static inline void kiemTraEOF(std::istream& in) {
    if (in.eof() || in.fail()) {
        throw std::invalid_argument("Lỗi: Kết thúc dữ liệu đầu vào!");
    }
}

// Ném lỗi quá số lần nhập
[[noreturn]] static inline void nemLoiQuaSoLanNhap() {
    throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
}

// Nhập tên
std::string NhapTen(const char* nhan, size_t minLength, size_t maxLength, std::istream& in, std::ostream& out) {
    std::string goc;
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhập " << (nhan ? nhan : "tên") << " (" << minLength << "-" << maxLength << " ký tự): ";
        std::getline(in, goc);
        kiemTraEOF(in);
        std::string gocCat = CatKhoangTrang(goc);
        if (gocCat.empty()) {
            thongBao(out, (nhan ? nhan : "Trường") + std::string(" không được rỗng!"), LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        std::string ketQua;
        if (!ChuanHoaTenUnicode(gocCat, minLength, maxLength, out, ketQua)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        return ketQua;
    }
    nemLoiQuaSoLanNhap();
}

int NhapSoNguyen(const char* nhan, int minVal, int maxVal, std::istream& in, std::ostream& out) {
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhập " << (nhan ? nhan : "số") << " (" << minVal << "-" << maxVal << "): ";
        std::string input;
        std::getline(in, input);
        kiemTraEOF(in);
        int ketQua;
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(input), ketQua, true) || ketQua < minVal || ketQua > maxVal) {
            thongBao(out, (nhan ? nhan : "Số") + std::string(" không hợp lệ!"), LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        return ketQua;
    }
    nemLoiQuaSoLanNhap();
}

std::string NhapPhai(std::istream& in, std::ostream& out) {
    std::string phaiNhapVao;
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhập phái (Nam/Nữ): ";
        std::getline(in, phaiNhapVao);
        std::string phaiCat = CatKhoangTrang(phaiNhapVao);
        if (phaiCat.empty()) {
            thongBao(out, "Phái không được rỗng!", LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        std::string phaiChuanHoa = ChuanHoaPhai(phaiNhapVao);
        if (!phaiChuanHoa.empty()) {
            return phaiChuanHoa;
        }
        thongBao(out, "Chỉ Nam hoặc Nữ!", LOI);
        ++soLanThu;
        thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
    }
    nemLoiQuaSoLanNhap();
}

std::string NhapISBNThuCong(std::istream& in, std::ostream& out) {
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhập ISBN (10 hoặc 13 ký tự): ";
        std::string isbnRaw;
        std::getline(in, isbnRaw);
        kiemTraEOF(in);
        std::string isbnChuan;
        if (!ChuanHoaISBNFile(isbnRaw, isbnChuan, out)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        if (KiemTraTrungISBN(isbnChuan, out)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        return isbnChuan;
    }
    nemLoiQuaSoLanNhap();
}