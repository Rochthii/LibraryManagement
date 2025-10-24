#include "KiemTraDuLieu.h"
#include "ThongBao.h"
#include "XuLyChuoi.h"
#include "QuanLySach.h"
#include "NgayThang.h"
#include "DauSach.h"
#include "Constants.h"

extern PTRDS dsDauSach[];
extern int soLuongDauSach;

// Tra ve chuoi loi hoac ""
std::string KiemTraChuoiRong(const std::string& chuoi, const std::string& tenTruong){
    if(chuoi.empty()) { return tenTruong + " khong duoc rong!"; }
    if(laChuoiRongHoacChiKhoangTrang(chuoi)) { return tenTruong + " chi chua khoang trang!"; } //
    return ""; // Hop le
}

bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out) {
    bool coDauSach = (TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN) != nullptr); //
    if(coDauSach) thongBao(out, "Trung ISBN!", LOI); //
    return coDauSach;
}

bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out) {
    bool coMaSach = (timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, out, true) != nullptr); //
    if(coMaSach) thongBao(out, "Trung ma sach!", LOI); //
    return coMaSach;
}

// Tra ve chuoi loi hoac ""
std::string KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen) {
    std::string loiRong = KiemTraChuoiRong(str, fieldName);
    if (!loiRong.empty()) {
        return loiRong;
    }
    // Su dung static_cast de tranh canh bao sign-compare
    if (str.length() > static_cast<size_t>(maxLen)) {
        return fieldName + " qua dai (toi da " + std::to_string(maxLen) + " ky tu)!";
    }
    return ""; // Hop le
}

// Tra ve chuoi loi hoac ""
std::string KiemTraNamXuatBan(int nam) {
    int namHienTai = LayNamHienTai(); //
    if (nam < NAM_XUAT_BAN_MIN ||
        nam > namHienTai + NAM_XUAT_BAN_OFFSET_MAX) {
        return "Nam xuat ban phai tu " + std::to_string(NAM_XUAT_BAN_MIN) + " den " + std::to_string(namHienTai + NAM_XUAT_BAN_OFFSET_MAX) + "!";
    }
    return ""; // Hop le
}

// Ham KiemTraISBN
bool KiemTraISBN(const std::string& ISBN, std::ostream& out) {
    std::string isbnChuan;
    return ChuanHoaISBNCore(ISBN, isbnChuan, false, false, false, out); //
}

// Ham KiemTraDanhSachHopLe
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out) {
    if (!danhSach) {
        thongBao(out, tenDS + " khong hop le (null pointer)!", LOI); //
        return false;
    }
    if (soLuong < 0) {
        thongBao(out, "So luong " + tenDS + " khong the am (" + std::to_string(soLuong) + ")!", LOI); //
        return false;
    }
    if (soLuong > maxSize) {
        thongBao(out, tenDS + " vuot qua gioi han (" + std::to_string(soLuong) + "/" + std::to_string(maxSize) + ")!", LOI); //
        return false;
    }
    return true;
}

bool KiemTraTongSoBanSao(const std::string& isbn, int soLuongThem, std::ostream& out){
    int soBanSaoHienTai = 0;
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn); //
    if(dauSach) {
        for(DanhMucSach* p = dauSach->dms; p != nullptr; p = p->next) {
            soBanSaoHienTai++;
        }
    }
    int tongSoSauKhiThem = soBanSaoHienTai + soLuongThem;
    if(tongSoSauKhiThem > MAX_BAN_SAO) {
        thongBao(out, "Tong so ban sao se vuot qua " + std::to_string(MAX_BAN_SAO) + "!", LOI); //
        return false;
    }
    return true;
}