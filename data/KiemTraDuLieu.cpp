#include "KiemTraDuLieu.h"
#include "ThongBao.h"
#include "XuLyChuoi.h"
#include "QuanLySach.h"
#include "NgayThang.h"
#include "DauSach.h"

extern PTRDS dsDauSach[];
extern int soLuongDauSach;

bool KiemTraChuoiRong(const std::string& chuoi, const std::string& tenTruong, std::ostream& out){
    if(chuoi.empty()) { thongBao(out, tenTruong + " không được rỗng!", LOI); return false; }
    if(laChuoiRongHoacChiKhoangTrang(chuoi)) { thongBao(out, tenTruong + " chỉ khoảng trắng!", LOI); return false; }
    return true;
}

bool KiemTraTrangThaiThe(int trangThai, std::ostream& out) {
    if (trangThai == 0 || trangThai == 1) return true;
    thongBao(out, "Trạng thái thẻ phải 0 hoặc 1!", LOI);
    return false;
}

bool KiemTraMaThe(const std::string& maThe, std::ostream& out) {
    int giaTri = 0;
    if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(maThe), giaTri, true)) { 
        thongBao(out, "Mã thẻ không hợp lệ!", LOI); 
        return false; 
    }
    return true;
}

bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out) {
    bool coDauSach = (TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN) != nullptr);
    if(coDauSach) thongBao(out, "Trùng ISBN!", LOI);
    return coDauSach;
}

bool KiemTraISBN(const std::string& ISBN, std::ostream& out) {
    std::string isbnChuan;
    return ChuanHoaISBNCore(ISBN, isbnChuan, false, false, false, out);
}

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

bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out) {
    bool coMaSach = (timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, out, true) != nullptr);
    if(coMaSach) thongBao(out, "Trùng mã sách!", LOI);
    return coMaSach;
}

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

bool KiemTraNamXuatBan(int nam, std::ostream& out) {
    int namHienTai = LayNamHienTai();
    if (nam < NAM_XUAT_BAN_MIN || 
        nam > namHienTai + NAM_XUAT_BAN_OFFSET_MAX) {
        thongBao(out, "Năm xuất bản không hợp lý!", LOI);
        return false;
    }
    return true;
}