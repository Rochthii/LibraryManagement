#include "include/KiemTraDuLieu.h"
#include "include/ThongBao.h"
#include "include/XuLyChuoi.h"
#include "include/QuanLySach.h"
#include "include/NgayThang.h"
#include "include/DauSach.h"

extern PTRDS dsDauSach[];
extern int soLuongDauSach;

// Kiểm tra chuỗi rỗng
bool KiemTraChuoiRong(const std::string& chuoi, const std::string& tenTruong, std::ostream& out){
    if(chuoi.empty()) { thongBao(out, tenTruong + " không được rỗng!", LOI); return false; }
    if(laChuoiRongHoacChiKhoangTrang(chuoi)) { thongBao(out, tenTruong + " chỉ khoảng trắng!", LOI); return false; }
    return true;
}

// Kiểm tra trạng thái thẻ
bool KiemTraTrangThaiThe(int trangThai, std::ostream& out) {
    if (trangThai == 0 || trangThai == 1) return true;
    thongBao(out, "Trạng thái thẻ phải 0 hoặc 1!", LOI);
    return false;
}

// Kiểm tra mã thẻ
bool KiemTraMaThe(const std::string& maThe, std::ostream& out) {
    int giaTri = 0;
    if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(maThe), giaTri, true)) { 
        thongBao(out, "Mã thẻ không hợp lệ!", LOI); 
        return false; 
    }
    return true;
}

// Kiểm tra trùng ISBN
bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out) {
    bool coDauSach = (TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN) != nullptr);
    if(coDauSach) thongBao(out, "Trùng ISBN!", LOI);
    return coDauSach;
}

// Kiểm tra ISBN
bool KiemTraISBN(const std::string& ISBN, std::ostream& out) {
    std::string isbnChuan;
    return ChuanHoaISBNCore(ISBN, isbnChuan, false, false, false, out);
}

// Kiểm tra tổng số bản sao
bool KiemTraTongSoBanSao(const std::string& isbn, int soLuongThem, std::ostream& out){
    int soBanSaoHienTai = 0;
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if(dauSach) {
        for(DanhMucSach* p = dauSach->dms; p != nullptr; p = p->next) {
            soBanSaoHienTai++;
        }
    }
    int tongSoSauKhiThem = soBanSaoHienTai + soLuongThem;
    if(tongSoSauKhiThem > MAX_BAN_SAO) { 
        thongBao(out, "Tổng số bản sao > " + std::to_string(MAX_BAN_SAO) + "!", LOI);
        return false; 
    }
    return true;
}

// Kiểm tra trùng mã sách
bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out) {
    bool coMaSach = (timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, out, true) != nullptr);
    if(coMaSach) thongBao(out, "Trùng mã sách!", LOI);
    return coMaSach;
}
// Kiểm tra danh sách hợp lệ
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out) {
    if (!danhSach) {
        thongBao(out, tenDS + " không hợp lệ (null pointer)!", LOI);
        return false;
    }
    if (soLuong < 0) {
        thongBao(out, "Số lượng " + tenDS + " không thể âm (" + std::to_string(soLuong) + ")!", LOI);
        return false;
    }
    if (soLuong > maxSize) {
        thongBao(out, tenDS + " vượt quá giới hạn (" + std::to_string(soLuong) + "/" + std::to_string(maxSize) + ")!", LOI);
        return false;
    }
    return true;
}

// Kiểm tra chuỗi và độ dài
bool KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen, std::ostream& out) {
    if (!KiemTraChuoiRong(str, fieldName, out)) {
        return false;
    }
    if (str.length() > static_cast<size_t>(maxLen)) {
        thongBao(out, fieldName + " quá dài (tối đa " + std::to_string(maxLen) + " ký tự)!", LOI);
        return false;
    }
    return true;
}

// Kiểm tra năm xuất bản
bool KiemTraNamXuatBan(int nam, std::ostream& out) {
    int namHienTai = LayNamHienTai();
    if (nam < NAM_XUAT_BAN_MIN || 
        nam > namHienTai + NAM_XUAT_BAN_OFFSET_MAX) {
        thongBao(out, "Năm xuất bản không hợp lý!", LOI);
        return false;
    }
    return true;
}