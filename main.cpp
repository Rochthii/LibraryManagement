#include <iostream>
#include <string>
#include <ctime>
#include <cctype>
#include <clocale>  
#include <locale>  
#include "include/QuanLySach.h"
#include "include/NhapLieu.h"
#include "include/KiemTraDuLieu.h"
#include "include/XuLyChuoi.h"
#include "include/ThongBao.h"
#include "include/ThaoTacFile.h"
#include "include/NgayThang.h"
#include "include/Constants.h"


using namespace std;

extern PTRDS dsDauSach[];
extern int soLuongDauSach;
extern bool duLieuDaThayDoi;


void nhapVaThemDauSachTuDong() {
    thongBao(cout, "=== Thêm đầu sách ===", THONG_TIN);

    try {
        string isbn = NhapISBNThuCong(cin, cout);
        cout << "\nNhập thông tin đầu sách:\n";
        string tenSach = NhapTen("tên sách", MIN_TEN_SACH, MAX_TEN_SACH, cin, cout);
        int soTrang = NhapSoNguyen("số trang", 1, MAX_SO_TRANG, cin, cout);
        string tacGia = NhapTen("tác giả", MIN_TAC_GIA, MAX_TAC_GIA, cin, cout);
        int namHienTai = LayNamHienTai();
        int namXB = NhapSoNguyen("năm xuất bản", NAM_XUAT_BAN_MIN, namHienTai + NAM_XUAT_BAN_OFFSET_MAX, cin, cout);
        string theLoai = NhapTen("thể loại", MIN_THE_LOAI, MAX_THE_LOAI, cin, cout);
        int soLuongBanSao = NhapSoNguyen("số lượng bản sao", 1, MAX_BAN_SAO, cin, cout);
        string viTriChung = NhapTen("vị trí ", 0, MAX_VI_TRI_KE, cin, cout);

        if (!KiemTraTongSoBanSao(isbn, soLuongBanSao, cout)) {
            return;
        }

        cout << "\nĐang thêm vào danh sách...\n";
    bool ok = themDauSach(dsDauSach, soLuongDauSach, isbn, tenSach, soTrang, tacGia, namXB, theLoai, false);
        if (!ok) {
            thongBao(cout, "Không thể thêm đầu sách!", LOI);
            return;
        }

    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        if (!dauSach) {
            thongBao(cout, "Lỗi: Không tìm thấy đầu sách vừa thêm!", LOI);
            return;
        }

        // Thêm danh mục sách (DMS) tự động
        int soThuTu = 1;
        for (int i = 0; i < soLuongBanSao; ++i) {
            string maSach = sinhMaSach(isbn, soThuTu, cout);
            if (maSach.empty()) {
                thongBao(cout, "Không thể sinh mã sách!", LOI);
                continue;
            }
            string viTri = viTriChung.empty() ? ("Kệ " + to_string((i % MAX_VI_TRI_KE) + 1)) : viTriChung;
            if (!themDanhMucSach(dauSach->dms, maSach, CHO_MUON_DUOC, viTri)) {
                thongBao(cout, "Không thể thêm mã sách: " + maSach, LOI);
            } else {
                dauSach->tongBanSao += 1;
                thongBao(cout, "Đã thêm mã sách: " + maSach, THONG_TIN);
            }
            soThuTu++;
        }

        duLieuDaThayDoi = true;
    } catch (const std::exception& e) {
        thongBao(cout, string("Lỗi khi nhập đầu sách: ") + e.what(), LOI);
    }
}

int nhapLuaChonMenu(int min, int max) {
    try {
        return NhapSoNguyen("lựa chọn", min, max, cin, cout);
    } catch (const std::exception& e) {
        thongBao(cout, string("Lỗi nhập lựa chọn: ") + e.what(), LOI);
        return -1;
    }
}

int main() {
    NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, cout);
    NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, cout);
    while (true) {
        try {
            cout << "\n=== QUẢN LÝ THƯ VIỆN ===\n";
            cout << "1. Nhập đầu sách\n";//c
            cout << "2. In danh sách đầu sách theo thể loại\n";//d
            cout << "3. Tìm kiếm sách theo tên\n";//e
            cout << "4. Nhập thẻ độc giả\n";
            cout << "0. Thoát\n";

            int luaChon = nhapLuaChonMenu(0, 3);
            if (luaChon == -1) continue;

            switch (luaChon) {
            case 0:
                if (duLieuDaThayDoi) {
                    GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, false, cout);
                    GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, false, cout);
                }
                GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
                thongBao(cout, "Đã thoát chương trình!", THONG_TIN);
                return 0;
            case 1:
                nhapVaThemDauSachTuDong();
                break;
            case 2:
                inDanhSachTheoTheLoai(dsDauSach, soLuongDauSach);
                break;
            case 3: {
                cout << "Nhập từ khóa tìm kiếm: ";
                string tuKhoa;
                getline(cin, tuKhoa);
                if (cin.eof() || cin.fail()) {
                    thongBao(cout, "Lỗi nhập từ khóa!", LOI);
                    cin.clear();
                    continue;
                }
                timDauSach(dsDauSach, soLuongDauSach, tuKhoa);
                break;
            }
            case 4: {

                break;
            }
            default:
                thongBao(cout, "Lựa chọn không hợp lệ!", LOI);
                break;
        }
    } catch (const std::exception& e) {
        thongBao(cout, std::string("Lỗi không mong muốn: ") + e.what(), LOI);
        if (cin.fail()) cin.clear();
        std::string discard;
        std::getline(cin, discard);
        continue;
    }
    }
    return 0;
}