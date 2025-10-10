#include <fstream>
#include <iostream>
#include <string>
#include "include/ThaoTacFile.h"
#include "include/KiemTraDuLieu.h"
#include "include/XuLyChuoi.h"
#include "include/QuanLySach.h"
#include "include/ThongBao.h"
#include "include/DauSach.h"
#include "include/Constants.h"
#include "../include/VietnameseUtils.h"

using namespace std;

// Kiểm tra số trang
static bool KiemTraSoTrang(int soTrang, std::ostream& out) {
    if (soTrang <= 0 || soTrang > MAX_SO_TRANG) {
        thongBao(out, "Số trang phải từ 1-" + to_string(MAX_SO_TRANG) + "!", LOI);
        return false;
    }
    return true;
}

// Nạp danh sách đầu sách
void NapDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int& soLuongDauSach, std::ostream& out) {
    std::ifstream inp(path);
    if (!inp.is_open()) {
        thongBao(out, "Không thể mở file đầu sách: " + std::string(path), LOI);
        return;
    }
    int lineNo = 0, bookOk = 0, bookSkip = 0;
    std::string line;
    char buffer[DUNG_LUONG_CACHE_MAC_DINH];
    while (inp.getline(buffer, DUNG_LUONG_CACHE_MAC_DINH)) {
        line = buffer;
        ++lineNo;
        std::string f[SO_TRUONG_DAU_SACH];
        if (!TachTruong(line, '|', f, SO_TRUONG_DAU_SACH)) {
            thongBao(out, thongBaoLoi(lineNo, "Sai định dạng dòng: " + line, true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        int soTrang, namXuatBan;
        std::string isbnChuanHoa, tenChuanHoa, tacGiaChuanHoa, theLoaiChuanHoa;
        if (!KiemTraChuoiRong(f[0], "ISBN", out) || !ChuanHoaISBNFile(f[0], isbnChuanHoa, out) || KiemTraTrungISBN(isbnChuanHoa, out)) {
            thongBao(out, thongBaoLoi(lineNo, "Sai định dạng ISBN hoặc ISBN bị trùng: " + f[0], true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        if (!ChuanHoaTenUnicode(f[1], MIN_TEN_SACH, MAX_TEN_SACH, out, tenChuanHoa)) {
            thongBao(out, thongBaoLoi(lineNo, "Sai định dạng tên sách: " + f[1], true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(f[2]), soTrang, false) || !KiemTraSoTrang(soTrang, out)) {
            thongBao(out, thongBaoLoi(lineNo, "Số trang không hợp lệ: " + f[2], true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        if (!ChuanHoaTenUnicode(f[3], MIN_TAC_GIA, MAX_TAC_GIA, out, tacGiaChuanHoa)) {
            thongBao(out, thongBaoLoi(lineNo, "Sai định dạng tác giả: " + f[3], true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(f[4]), namXuatBan, false) || !KiemTraNamXuatBan(namXuatBan, out)) {
            thongBao(out, thongBaoLoi(lineNo, "Năm xuất bản không hợp lệ: " + f[4], true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        if (!ChuanHoaTenUnicode(f[5], MIN_THE_LOAI, MAX_THE_LOAI, out, theLoaiChuanHoa)) {
            thongBao(out, thongBaoLoi(lineNo, "Sai định dạng thể loại: " + f[5], true), CANH_BAO);
            ++bookSkip;
            continue;
        }
        if (themDauSach(dsDauSach, soLuongDauSach, isbnChuanHoa, tenChuanHoa, soTrang, tacGiaChuanHoa, namXuatBan, theLoaiChuanHoa, true)) {
            ++bookOk;
        } else {
            thongBao(out, thongBaoLoi(lineNo, "Không thể thêm đầu sách: " + f[1], true), CANH_BAO);
            ++bookSkip;
        }
    }
    if (inp.eof()) {
        InTongKet("Đọc đầu sách", bookOk, lineNo, bookSkip, out);
    } else if (inp.fail()) {
        thongBao(out, "Lỗi đọc file đầu sách!", LOI);
    }
    inp.close();
}

// Nạp danh mục sách
void NapDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, std::ostream& out) {
    std::ifstream inp(path);
    if (!inp.is_open()) {
        thongBao(out, "Không thể mở file danh mục sách: " + std::string(path), LOI);
        return;
    }
    int soThuTuDong = 0, soLuongOk = 0, soLuongBoQua = 0;
    std::string line;
    char buffer[DUNG_LUONG_CACHE_MAC_DINH];
    while (inp.getline(buffer, DUNG_LUONG_CACHE_MAC_DINH)) {
        line = buffer;
        ++soThuTuDong;
        std::string f[3];
        if (!TachTruong(line, '|', f, 3)) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Sai định dạng dòng: " + line, true), CANH_BAO);
            ++soLuongBoQua;
            continue;
        }
        std::string maSach = CatKhoangTrang(f[0]);
        int trangThai = PhanTichTrangThaiSach(f[1], out);
        std::string viTri = ChuanHoaViTri(f[2]);
        if (trangThai == -1 || viTri.empty() || viTri.length() > MAX_VI_TRI_KE) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Trạng thái hoặc vị trí không hợp lệ: " + line, true), CANH_BAO);
            ++soLuongBoQua;
            continue;
        }
        std::string isbn = TachISBNTuMaSach(maSach);
        PTRDAUSACH dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        if (!dauSach) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Không tìm thấy đầu sách cho mã sách: " + maSach, true), CANH_BAO);
            ++soLuongBoQua;
            continue;
        }
        if (KiemTraTrungmaSach(maSach, out)) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Mã sách đã tồn tại: " + maSach, true), CANH_BAO);
            ++soLuongBoQua;
            continue;
        }
        if (!themDanhMucSach(dauSach->dms, maSach, static_cast<TrangThaiSach>(trangThai), viTri)) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Không thể thêm mã sách: " + maSach, true), CANH_BAO);
            ++soLuongBoQua;
            continue;
        }
        if (dauSach) {
            if (dauSach->tongBanSao < 0) dauSach->tongBanSao = 0;
            dauSach->tongBanSao += 1;
        }
        ++soLuongOk;
    }
    if (inp.eof()) {
        InTongKet("Đọc danh mục sách", soLuongOk, soThuTuDong, soLuongBoQua, out);
    } else if (inp.fail()) {
        thongBao(out, "Lỗi đọc file danh mục sách!", LOI);
    }
    inp.close();
    // Sau khi nạp xong danh mục sách, cập nhật lại trường tongBanSao cho toàn bộ đầu sách
    CapNhatTongBanSao(dsDauSach, soLuongDauSach);
}

// Ghi danh sách đầu sách
bool GhiDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, bool silent, std::ostream& out) {
    std::ofstream outf(path);
    if (!outf.is_open()) {
        if (!silent) thongBao(out, "Không thể mở file để ghi đầu sách: " + std::string(path), LOI);
        return false;
    }
    int dem = 0;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) {
            outf << dsDauSach[i]->ISBN << "|"
                 << dsDauSach[i]->tenSach << "|"
                 << dsDauSach[i]->soTrang << "|"
                 << dsDauSach[i]->tacGia << "|"
                 << dsDauSach[i]->namXuatBan << "|"
                 << dsDauSach[i]->theLoai << "\n";
            ++dem;
        }
    }
    outf.close();
    if (!silent) InTongKet("Ghi đầu sách", dem, soLuongDauSach, soLuongDauSach - dem, out);
    return true;
}

// Ghi danh mục sách
bool GhiDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, bool silent, std::ostream& out) {
    std::ofstream outf(path);
    if (!outf.is_open()) {
        if (!silent) thongBao(out, "Không thể mở file để ghi danh mục sách: " + std::string(path), LOI);
        return false;
    }
    int dem = 0;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) {
            PTRDMS dms = dsDauSach[i]->dms;
            int demVong = 0;
            while (dms && demVong < SO_VONG_LAP_DMS_MAX) {
                outf << dms->maSach << "|" << dms->trangThai << "|" << dms->viTri << "\n";
                ++dem;
                dms = dms->next;
                ++demVong;
            }
            if (demVong >= SO_VONG_LAP_DMS_MAX) {
                if (!silent) thongBao(out, "Phát hiện vòng lặp vô hạn trong danh mục sách của đầu sách: " + dsDauSach[i]->ISBN, LOI);
                outf.close();
                return false;
            }
        }
    }
    outf.close();
    if (!silent) InTongKet("Ghi danh mục sách", dem, dem, 0, out);
    return true;
}

// In tổng kết
void InTongKet(const std::string& hanhDong, int thanhCong, int tongSo, int boQua, std::ostream& out) {
    thongBao(out, hanhDong + ": " + std::to_string(thanhCong) + "/" + std::to_string(tongSo) +
             " thành công, " + std::to_string(boQua) + " bị bỏ qua.", THONG_TIN);
}