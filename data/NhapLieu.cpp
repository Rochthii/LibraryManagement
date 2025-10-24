#include "NhapLieu.h"
#include "KiemTraDuLieu.h"
// #include "VietnameseUtils.h" // Da xoa
#include "XuLyChuoi.h"
#include "ThongBao.h"
#include "Constants.h"

// Kiểm tra EOF (Ctrl+Z o Win, Ctrl+D o Linux) hoac loi stream
static inline void kiemTraEOF(std::istream& in) {
    if (in.eof() || in.fail()) {
        throw std::invalid_argument("Loi: Ket thuc du lieu dau vao!");
    }
}

// Nem loi khi nguoi dung nhap sai qua so lan quy dinh
[[noreturn]] static inline void nemLoiQuaSoLanNhap() {
    throw std::invalid_argument("Loi: Qua so lan nhap sai cho phep!");
}

std::string NhapTen(const char* nhan, size_t minLength, size_t maxLength, std::istream& in, std::ostream& out) {
    std::string goc;
    int soLanThu = 0;
    // Vong lap cho phep thu lai toi da SO_LAN_THU_TOI_DA lan
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhap " << (nhan ? nhan : "ten") << " (" << minLength << "-" << maxLength << " ky tu): ";
        std::getline(in, goc);
        kiemTraEOF(in);
        std::string gocCat = CatKhoangTrang(goc);
        if (gocCat.empty()) {
            thongBao(out, (nhan ? nhan : "Truong") + std::string(" khong duoc rong!"), LOI);
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
        return ketQua; // Tra ve neu hop le
    }
    nemLoiQuaSoLanNhap(); // Nem loi neu het so lan thu
}

int NhapSoNguyen(const char* nhan, int minVal, int maxVal, std::istream& in, std::ostream& out) {
    int soLanThu = 0;
    // Vong lap cho phep thu lai
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhap " << (nhan ? nhan : "so") << " (" << minVal << "-" << maxVal << "): ";
        std::string input;
        std::getline(in, input);
        kiemTraEOF(in);
        int ketQua;
        // Kiem tra xem chuoi co phai la so nguyen hop le va nam trong khoang min-max
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(input), ketQua, true) || ketQua < minVal || ketQua > maxVal) {
            thongBao(out, (nhan ? nhan : "So") + std::string(" khong hop le!"), LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        return ketQua; // Tra ve neu hop le
    }
    nemLoiQuaSoLanNhap();
}


std::string NhapPhai(std::istream& in, std::ostream& out) {
    std::string phaiNhapVao;
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhap phai (Nam/Nu): ";
        std::getline(in, phaiNhapVao);
        std::string phaiCat = CatKhoangTrang(phaiNhapVao);
        if (phaiCat.empty()) {
            thongBao(out, "Phai khong duoc rong!", LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        std::string phaiChuanHoa = ChuanHoaPhai(phaiNhapVao);
        if (!phaiChuanHoa.empty()) {
            return phaiChuanHoa; // Tra ve "Nam" hoac "Nu"
        }
        thongBao(out, "Chi Nam hoac Nu!", LOI);
        ++soLanThu;
        thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
    }
    nemLoiQuaSoLanNhap();
}

std::string NhapISBNThuCong(std::istream& in, std::ostream& out) {
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhap ISBN (10 hoac 13 ky tu): ";
        std::string isbnRaw;
        std::getline(in, isbnRaw);
        kiemTraEOF(in);
        std::string isbnChuan;
        // Kiem tra dinh dang
        if (!ChuanHoaISBNFile(isbnRaw, isbnChuan, out)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        // Kiem tra trung lap
        if (KiemTraTrungISBN(isbnChuan, out)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out, SO_LAN_THU_TOI_DA);
            continue;
        }
        return isbnChuan; // Tra ve neu hop le
    }
    nemLoiQuaSoLanNhap();
}