#include <iostream>

#include <iostream>
#include <new>
#include "../include/DauSach.h"
#include "../include/InputUtils.h"

using namespace std;

const int DO_DAI_MA_SACH_MAX = 50;
const int SO_VONG_LAP_DMS_MAX = 1000;

// Thêm danh mục sách mới vào đầu sách
void themDanhMucSach(PTRDMS& dms, PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& ISBN, int soLuong) {
    using namespace InputUtils;
    if (!KiemTraTongSoBanSao(ISBN, soLuong)) return;
    if (soLuong == 0) return;
    PTRDAUSACH dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN);
    if (!dauSach) {
        thongBao(cout, "Không tìm thấy đầu sách với ISBN này!", LOI);
        return;
    }
    extern bool duLieuDaThayDoi;
    int daTao = 0;
    for (int i = 1; i <= soLuong; ++i) {
        std::string ma = sinhMaSach(ISBN, i, cout);
        if (ma.empty()) {
            thongBao(cout, "Không thể sinh mã sách!", LOI);
            break;
        }
        cout << "✓ Bản sao " << i << " (Mã: " << ma << ")" << endl;
        int trangThai = -1;
        int soLanThu = 0;
        while (soLanThu < SO_LAN_THU_TOI_DA) {
            trangThai = NhapSoNguyenNhan("Trạng thái bản sao (0: CHO_MUON_DUOC, 1: DANG_MUON, 2: THANH_LY)", 0, 2);
            if (trangThai < 0 || trangThai > 2) {
                thongBao(cout, "Lỗi: Trạng thái phải là 0, 1 hoặc 2!", LOI);
                ++soLanThu;
            } else {
                break;
            }
        }
        if (trangThai < 0 || trangThai > 2) {
            thongBao(cout, "Quá số lần nhập sai trạng thái, bỏ qua bản sao này!", LOI);
            continue;
        }
        std::string viTri = "";
        if (ChenNodeDMSVaoDauSach(dauSach, ma, trangThai, viTri)) {
            ++daTao;
            duLieuDaThayDoi = true;
        } else {
            thongBao(cout, "Không thể thêm bản sao vào đầu sách!", LOI);
        }
    }
    InTongKet("Tạo bản sao", daTao, soLuong, soLuong - daTao);
}

// Cập nhật trạng thái sách theo mã sách
void capNhatTrangThaiSach(PTRDMS dms, const std::string& maSach, TrangThaiSach trangThai) {
    using namespace InputUtils;
    if (!KiemTraDanhSachHopLe(dms, 1, SO_VONG_LAP_DMS_MAX, "Danh mục sách")) return;
    if (!KiemTraChuoiVaDodai(maSach, "Mã sách", DO_DAI_MA_SACH_MAX)) return;
    if (trangThai < CHO_MUON_DUOC || trangThai > THANH_LY) {
        thongBao(cout, "Trạng thái sách không hợp lệ (0-2)!", LOI);
        return;
    }
    PTRDMS current = dms;
    int dem = 0;
    while (current && dem < SO_VONG_LAP_DMS_MAX) {
        if (current->maSach == maSach) {
            TrangThaiSach cu = current->trangThai;
            current->trangThai = trangThai;
            thongBao(cout, "Cập nhật mã sách " + maSach + " từ " + TenTrangThai(cu) + " thành " + TenTrangThai(trangThai), THONG_TIN);
            return;
        }
        current = current->next;
        ++dem;
    }
    if (dem >= SO_VONG_LAP_DMS_MAX) {
        thongBao(cout, "Phát hiện vòng lặp vô hạn trong danh sách!", LOI);
    } else {
        thongBao(cout, "Không tìm thấy mã sách: " + maSach, LOI);
    }
}
