#include <fstream>
#include <iostream>
#include <string>
#include "ThaoTacFile.h"
#include "KiemTraDuLieu.h"
#include "XuLyChuoi.h"
#include "QuanLySach.h"
#include "ThongBao.h"
#include "DauSach.h"
#include "Constants.h"

using namespace std;

static bool KiemTraSoTrangFile(int soTrang, std::ostream &out) {
    if (soTrang <= 0 || soTrang > MAX_SO_TRANG) {
        thongBao(out, "So trang phai tu 1-" + to_string(MAX_SO_TRANG) + "!", LOI);
        return false;
    }
    return true;
}

void NapDanhSachDauSach(const char *path, PTRDS dsDauSach[], int &soLuongDauSach, std::ostream &out) {
    std::ifstream inp(path);  // mo file tu duong dan
    if (!inp.is_open()) {     // kiem tra file mo dc khong
        thongBao(out, "Loi: Khong the mo file dau sach: " + std::string(path), LOI);
        return;
    }

    int lineNo = 0, bookOk = 0, bookSkip = 0;
    std::string line; // dung de luu noi dung da doc dc
    char buffer[DUNG_LUONG_CACHE_MAC_DINH]; // dung bo dem (buffer) de doc file nhanh hon

    // doc tung dong trong file cho den khi het file (eof)
    while (inp.getline(buffer, DUNG_LUONG_CACHE_MAC_DINH)) {
        line = buffer; // gan buffer vao string de xu ly
        ++lineNo;      // tang so thu tu dong (de bao loi cho dung dong)

        // mang tam luu cac truong (isbn, ten,...) sau khi tach
        std::string fields[SO_TRUONG_DAU_SACH];
        // tach dong hien tai bang ky tu '|'
        // neu so luong truong tach ra khong dung (SO_TRUONG_DAU_SACH) -> bao loi, bo qua dong
        if (!TachTruong(line, '|', fields, SO_TRUONG_DAU_SACH)) {
            thongBao(out, thongBaoLoi(lineNo, "Sai dinh dang dong: " + line, true), CANH_BAO);
            ++bookSkip;
            continue;
        }

        // khai bao bien tam de luu du lieu da kiem tra va chuan hoa
        int soTrang, namXuatBan;
        std::string isbnChuanHoa, tenChuanHoa, tacGiaChuanHoa, theLoaiChuanHoa;
        std::string loi; // bien luu loi tra ve tu ham kiem tra

        // b1: kiem tra isbn
        loi = KiemTraChuoiRong(fields[0], "ISBN");                           // kiem tra rong
        std::string loiChuanHoa = ChuanHoaISBNFile(fields[0], isbnChuanHoa); // kiem tra 10 hoac 13 so
        std::string loiTrung = KiemTraTrungISBN(isbnChuanHoa);               // kiem tra co trung khong

        if (!loi.empty() || !loiChuanHoa.empty() || !loiTrung.empty()) {     // neu co bat ky loi nao (rong, sai dinh dang, hoac trung) -> bo qua dong
            // lay loi dau tien tim thay de thong bao
            std::string loiTongHop = loi.empty() ? (loiChuanHoa.empty() ? loiTrung : loiChuanHoa) : loi;
            thongBao(out, thongBaoLoi(lineNo, loiTongHop + " ('" + fields[0] + "')", true), CANH_BAO);
            ++bookSkip;
            continue;
        }

        // b2: kiem tra ten sach
        loi = ChuanHoaTenUnicode(fields[1], MIN_TEN_SACH, MAX_TEN_SACH, tenChuanHoa); // kiem tra (rong, ky tu, do dai)
        if (!loi.empty()) {
            thongBao(out, thongBaoLoi(lineNo, loi + " ('" + fields[1] + "')", true), CANH_BAO);
            ++bookSkip;
            continue;
        }

        // b3: kiem tra so trang 
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(fields[2]), soTrang, false) || !KiemTraSoTrangFile(soTrang, out)) {  // chuyen chuoi thanh so va kiem tra gioi han (vi du: 1-10000)
            thongBao(out, thongBaoLoi(lineNo, "So trang khong hop le: " + fields[2], true), CANH_BAO);
            ++bookSkip;
            continue;
        }

        // b4: kiem tra tac gia
        loi = ChuanHoaTenUnicode(fields[3], MIN_TAC_GIA, MAX_TAC_GIA, tacGiaChuanHoa); // kiem tra (rong, ky tu, do dai)
        if (!loi.empty()) {
            thongBao(out, thongBaoLoi(lineNo, loi + " ('" + fields[3] + "')", true), CANH_BAO);
            ++bookSkip;
            continue;
        }

        //b5: kiem tra nam xuat ban
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(fields[4]), namXuatBan, false)) {  // kiem tra co phai la so khong
             thongBao(out, thongBaoLoi(lineNo, "Nam xuat ban phai la so: '" + fields[4] + "'", true), CANH_BAO);
             ++bookSkip;
             continue;
        }
        loi = KiemTraNamXuatBan(namXuatBan); // kiem tra gioi han (1800 - nam hien tai)
        if(!loi.empty()) {
             thongBao(out, thongBaoLoi(lineNo, loi + " ('" + fields[4] + "')", true), CANH_BAO);
             ++bookSkip;
             continue;
        }

        //b6: kiem tra the loai
        loi = ChuanHoaTenUnicode(fields[5], MIN_THE_LOAI, MAX_THE_LOAI, theLoaiChuanHoa); // kiem tra (rong, ky tu, do dai)
        if (!loi.empty()) {
            thongBao(out, thongBaoLoi(lineNo, loi + " ('" + fields[5] + "')", true), CANH_BAO);
            ++bookSkip;
            continue;
        }

        //b7: them vao danh sach
        // neu tat ca hop le, goi ham themDauSach
        if (themDauSach(dsDauSach, soLuongDauSach, isbnChuanHoa, tenChuanHoa, soTrang, tacGiaChuanHoa, namXuatBan, theLoaiChuanHoa, true)) {
            ++bookOk; // tang bien dem them thanh cong
        } else {
            // bao loi neu day
            thongBao(out, thongBaoLoi(lineNo, "Khong the them dau sach (da day?): " + fields[1], true), CANH_BAO);
            ++bookSkip;
        }
    } 

    // kiem tra xem file da doc het (eof) hay bi loi giua chung (fail)
    if (inp.eof()) {
        InTongKet("Doc dau sach", bookOk, lineNo, bookSkip, out);
    } else if (inp.fail()) {
        thongBao(out, "Loi: Gap su co khi doc file dau sach!", LOI);
    }
    inp.close(); // dong file
}



void NapDanhMucSach(const char *path, PTRDS dsDauSach[], int soLuongDauSach, std::ostream &out) {
    std::ifstream inp(path);
    if (!inp.is_open()) {
        thongBao(out, "Loi: Khong the mo file danh muc sach: " + std::string(path), LOI);
        return;
    }

    int soThuTuDong = 0;  // Dong hien tai trong file DMS
    int soLuongOk = 0;    // So ban sao nap thanh cong
    int soLuongBoQua = 0; // So ban sao bi bo qua
    std::string line;
    char buffer[DUNG_LUONG_CACHE_MAC_DINH];

    // doc tung dong trong file danh muc sach
    while (inp.getline(buffer, DUNG_LUONG_CACHE_MAC_DINH)) {
        line = buffer;
        ++soThuTuDong;

        // tach truong (masach|trangthai|vitri)
        std::string fields[3];
        if (!TachTruong(line, '|', fields, 3)) { //
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Sai dinh dang dong: " + line, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        // lay va chuan hoa du lieu tung truong
        std::string maSach = CatKhoangTrang(fields[0]);        //
        int trangThai = PhanTichTrangThaiSach(fields[1], out); //
        std::string viTri = ChuanHoaViTri(fields[2]);          //

        if (trangThai == -1 || viTri.empty() || viTri.length() > MAX_VI_TRI_KE) { // Kiem tra trang thai va vi tri co hop le khong
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Trang thai hoac vi tri khong hop le: " + line, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        std::string isbn = TachISBNTuMaSach(maSach);                         //// Tim DauSach cha dua vao phan ISBN cua MaSach
        PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn); //
        if (!dauSach) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Khong tim thay dau sach cho ma sach: " + maSach, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        // kiem tra xem ma sach nay da ton tai trong he thong chua
        if (KiemTraTrungmaSach(maSach, out)) {
            // ham kiemtratrungmasach da tu thong bao loi
            ++soLuongBoQua;
            continue;
        }

        // them ban sao vao danh sach lien ket (dms) cua dausach tim duoc
        std::string loiThem = themDanhMucSach(dauSach->dms, maSach, static_cast<TrangThaiSach>(trangThai), viTri); // Luu ket qua string
        if (loiThem != "") { // Kiem tra neu string khong rong (co loi)
            // hien thi loi chi tiet tra ve tu ham themdanhmucsach
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Khong the them ma sach: " + maSach + ". (" + loiThem + ")", true), CANH_BAO);
            ++soLuongBoQua;
            continue;
        }
        // tam thoi tang so luong ban sao cua dau sach (se cap nhat lai sau)
        // if (dauSach->tongBanSao < 0) dauSach->tongBanSao = 0; // Khong can thiet neu khoi tao dung
        // dauSach->tongBanSao++;

        ++soLuongOk;
    } // Ket thuc vong while doc file

    if (inp.eof()) {
        InTongKet("Doc danh muc sach", soLuongOk, soThuTuDong, soLuongBoQua, out); //
    } else if (inp.fail()) {
        thongBao(out, "Loi: Gap su co khi doc file danh muc sach!", LOI);
    }
    inp.close();
    CapNhatTongBanSao(dsDauSach, soLuongDauSach); //// cap nhat lai chinh xac tong so ban sao cho tat ca dau sach sau khi da nap xong DMS
}

//
bool GhiDanhSachDauSach(const char *path, PTRDS dsDauSach[], int soLuongDauSach, bool silent, std::ostream &out) {
    std::ofstream outf(path);
    if (!outf.is_open()) {
        if (!silent)
            thongBao(out, "Loi: Khong the mo file de ghi dau sach: " + std::string(path), LOI);
        return false;
    }

    int dem = 0; // Dem so luong dau sach da ghi
    for (int i = 0; i < soLuongDauSach; ++i) { // Duyet qua mang dau sach
        if (dsDauSach[i]) { // Chi ghi neu con tro khong null
            // ghi cac truong, ngan cach boi dau '|'
            outf << dsDauSach[i]->ISBN << "|"
                 << dsDauSach[i]->tenSach << "|"
                 << dsDauSach[i]->soTrang << "|"
                 << dsDauSach[i]->tacGia << "|"
                 << dsDauSach[i]->namXuatBan << "|"
                 << dsDauSach[i]->theLoai << "\n";
            ++dem;
        }
    }
    outf.close(); // Dong file

    if (!silent)
        InTongKet("Ghi dau sach", dem, soLuongDauSach, soLuongDauSach - dem, out); //
    return true;
}

bool GhiDanhMucSach(const char *path, PTRDS dsDauSach[], int soLuongDauSach, bool silent, std::ostream &out) {
    std::ofstream outf(path);
    if (!outf.is_open()) {
        if (!silent)
            thongBao(out, "Loi: Khong the mo file de ghi danh muc sach: " + std::string(path), LOI);
        return false;
    }

    int dem = 0; // Dem tong so ban sao da ghi
    // duyet qua tung dau sach
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) {
            PTRDMS dms = dsDauSach[i]->dms; // Lay con tro dau DSLK cua dau sach thu i
            int demVong = 0;                // dung bien dem de phat hien vong lap vo han

            // duyet qua danh sach lien ket cac ban sao cua dau sach hien tai
            while (dms && demVong < SO_VONG_LAP_DMS_MAX) {
                // ghi thong tin ban sao: masach|trangthai(so)|vitri
                outf << dms->maSach << "|" << dms->trangThai << "|" << dms->viTri << "\n";
                ++dem;
                dms = dms->next; // Chuyen qua ban sao tiep theo
                ++demVong;
            }

            // kiem tra neu vong lap while chay qua gioi han -> co the co vong lap vo han
            if (demVong >= SO_VONG_LAP_DMS_MAX) {
                if (!silent)
                    thongBao(out, "Loi: Phat hien vong lap vo han trong DMS cua ISBN: " + dsDauSach[i]->ISBN, LOI);
                outf.close(); // Dong file ngay neu loop vo han
                return false; // Tra ve loi
            }
        }
    }
    outf.close(); // Dong file

    int tongSoBanSaoThucTe = DemTongSoBanSao(dsDauSach, soLuongDauSach); //
    if (!silent)
        InTongKet("Ghi danh muc sach", dem, tongSoBanSaoThucTe, tongSoBanSaoThucTe - dem, out); //
    return true;
}

void InTongKet(const std::string &hanhDong, int thanhCong, int tongSo, int boQua, std::ostream &out) {
    thongBao(out, hanhDong + ": " + std::to_string(thanhCong) + "/" + std::to_string(tongSo) + " thanh cong, " + std::to_string(boQua) + " bi bo qua.", THONG_TIN);
}