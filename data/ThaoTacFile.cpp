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

// --- Hàm hỗ trợ nội bộ ---

// Kiem tra tinh hop le cua so trang doc tu file
// (Hàm này trả về bool và tự thông báo lỗi, có thể giữ nguyên cho việc đọc file)
static bool KiemTraSoTrangFile(int soTrang, std::ostream &out)
{
    if (soTrang <= 0 || soTrang > MAX_SO_TRANG)
    {
        thongBao(out, "So trang phai tu 1-" + to_string(MAX_SO_TRANG) + "!", LOI);
        return false;
    }
    return true;
}

// --- Hàm đọc file ---

void NapDanhSachDauSach(const char *path, PTRDS dsDauSach[], int &soLuongDauSach, std::ostream &out)
{
    std::ifstream inp(path);
    if (!inp.is_open())
    {
        thongBao(out, "Loi: Khong the mo file dau sach: " + std::string(path), LOI);
        return;
    }

    int lineNo = 0;   // So thu tu dong dang doc
    int bookOk = 0;   // So luong dau sach nap thanh cong
    int bookSkip = 0; // So luong dau sach bi bo qua do loi
    std::string line; // Chuoi chua noi dung cua mot dong
    char buffer[DUNG_LUONG_CACHE_MAC_DINH]; // Bo dem doc file

    // Doc tung dong trong file dau sach
    while (inp.getline(buffer, DUNG_LUONG_CACHE_MAC_DINH))
    {
        line = buffer;
        ++lineNo;

        // Tach dong thanh cac truong du lieu, ngan cach boi dau '|'
        std::string fields[SO_TRUONG_DAU_SACH];
        if (!TachTruong(line, '|', fields, SO_TRUONG_DAU_SACH)) //
        {
            thongBao(out, thongBaoLoi(lineNo, "Sai dinh dang dong (thieu truong hoac thua dau '|'): " + line, true), CANH_BAO); //
            ++bookSkip;
            continue; // Bo qua dong loi, doc dong tiep theo
        }

        // Bien tam de luu du lieu sau khi chuan hoa va kiem tra
        int soTrang, namXuatBan;
        std::string isbnChuanHoa, tenChuanHoa, tacGiaChuanHoa, theLoaiChuanHoa;
        std::string loi; // Bien luu loi tu cac ham KiemTra...

        // --- Kiem tra va Chuan hoa tung truong ---

        // 1. Kiem tra ISBN
        loi = KiemTraChuoiRong(fields[0], "ISBN"); //
        // Kiem tra loi rong HOAC loi dinh dang HOAC bi trung
        if (!loi.empty() || !ChuanHoaISBNFile(fields[0], isbnChuanHoa, out) || KiemTraTrungISBN(isbnChuanHoa, out)) //
        {
            thongBao(out, thongBaoLoi(lineNo, "ISBN khong hop le hoac bi trung: '" + fields[0] + "'", true), CANH_BAO); //
            ++bookSkip;
            continue;
        }
        // 2. Kiem tra Ten Sach
        // ChuanHoaTenUnicode kiem tra ca rong, do dai va ky tu
        if (!ChuanHoaTenUnicode(fields[1], MIN_TEN_SACH, MAX_TEN_SACH, out, tenChuanHoa)) //
        {
            thongBao(out, thongBaoLoi(lineNo, "Ten sach khong hop le: '" + fields[1] + "'", true), CANH_BAO); //
            ++bookSkip;
            continue;
        }
        // 3. Kiem tra So Trang
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(fields[2]), soTrang, false) || !KiemTraSoTrangFile(soTrang, out)) //
        {
            thongBao(out, thongBaoLoi(lineNo, "So trang khong hop le: '" + fields[2] + "'", true), CANH_BAO); //
            ++bookSkip;
            continue;
        }
        // 4. Kiem tra Tac Gia
        if (!ChuanHoaTenUnicode(fields[3], MIN_TAC_GIA, MAX_TAC_GIA, out, tacGiaChuanHoa)) //
        {
            thongBao(out, thongBaoLoi(lineNo, "Tac gia khong hop le: '" + fields[3] + "'", true), CANH_BAO); //
            ++bookSkip;
            continue;
        }
        // 5. Kiem tra Nam Xuat Ban
        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(fields[4]), namXuatBan, false)) {
            thongBao(out, thongBaoLoi(lineNo, "Nam xuat ban phai la so: '" + fields[4] + "'", true), CANH_BAO); //
            ++bookSkip;
            continue;
        }
        loi = KiemTraNamXuatBan(namXuatBan); //
        if(!loi.empty()) // Kiem tra neu co loi
        {
             thongBao(out, thongBaoLoi(lineNo, loi + " ('" + fields[4] + "')", true), CANH_BAO); //
             ++bookSkip;
             continue;
        }


        // 6. Kiem tra The Loai
        if (!ChuanHoaTenUnicode(fields[5], MIN_THE_LOAI, MAX_THE_LOAI, out, theLoaiChuanHoa)) {
            thongBao(out, thongBaoLoi(lineNo, "The loai khong hop le: '" + fields[5] + "'", true), CANH_BAO); //
            ++bookSkip;
            continue;
        }

        // --- Them vao danh sach neu tat ca hop le ---
        // Goi ham themDauSach voi anLang = true de khong in thong bao thanh cong ra console
        if (themDauSach(dsDauSach, soLuongDauSach, isbnChuanHoa, tenChuanHoa, soTrang, tacGiaChuanHoa, namXuatBan, theLoaiChuanHoa, true)) {
            ++bookOk;
        }
        else
        {
            // Loi nay chi xay ra neu mang dsDauSach da day
            thongBao(out, thongBaoLoi(lineNo, "Khong the them dau sach (da day?): " + fields[1], true), CANH_BAO); //
            ++bookSkip;
        }
    } // Ket thuc vong while doc file

    // Kiem tra trang thai ket thuc doc file
    if (inp.eof()) // Doc den cuoi file thanh cong
    {
        InTongKet("Doc dau sach", bookOk, lineNo, bookSkip, out); //
    }
    else if (inp.fail()) // Gap loi khac trong qua trinh doc (vd: loi o dia)
    {
        thongBao(out, "Loi: Gap su co khi doc file dau sach!", LOI);
    }
    inp.close(); // Dong file
}

void NapDanhMucSach(const char *path, PTRDS dsDauSach[], int soLuongDauSach, std::ostream &out)
{
    std::ifstream inp(path);
    if (!inp.is_open())
    {
        thongBao(out, "Loi: Khong the mo file danh muc sach: " + std::string(path), LOI);
        return;
    }

    int soThuTuDong = 0;   // Dong hien tai trong file DMS
    int soLuongOk = 0;     // So ban sao nap thanh cong
    int soLuongBoQua = 0;  // So ban sao bi bo qua
    std::string line;
    char buffer[DUNG_LUONG_CACHE_MAC_DINH];

    // Doc tung dong trong file danh muc sach
    while (inp.getline(buffer, DUNG_LUONG_CACHE_MAC_DINH)) {
        line = buffer;
        ++soThuTuDong;

        // Tach truong (MaSach|TrangThai|ViTri)
        std::string fields[3];
        if (!TachTruong(line, '|', fields, 3)) //
        {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Sai dinh dang dong: " + line, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        // Lay va chuan hoa du lieu tung truong
        std::string maSach = CatKhoangTrang(fields[0]); //
        int trangThai = PhanTichTrangThaiSach(fields[1], out); //
        std::string viTri = ChuanHoaViTri(fields[2]); //

        // Kiem tra trang thai va vi tri co hop le khong
        if (trangThai == -1 || viTri.empty() || viTri.length() > MAX_VI_TRI_KE) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Trang thai hoac vi tri khong hop le: " + line, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        // Tim DauSach cha dua vao phan ISBN cua MaSach
        std::string isbn = TachISBNTuMaSach(maSach); //
        PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn); //
        if (!dauSach) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Khong tim thay dau sach cho ma sach: " + maSach, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        // Kiem tra xem ma sach nay da ton tai trong he thong chua
        if (KiemTraTrungmaSach(maSach, out)) {
            // Ham KiemTraTrungmaSach da tu thong bao loi
            ++soLuongBoQua;
            continue;
        }

        // Them ban sao vao danh sach lien ket (DMS) cua DauSach tim duoc
        if (!themDanhMucSach(dauSach->dms, maSach, static_cast<TrangThaiSach>(trangThai), viTri)) {
            thongBao(out, thongBaoLoiDMS(soThuTuDong, "Khong the them ma sach: " + maSach, true), CANH_BAO); //
            ++soLuongBoQua;
            continue;
        }

        // Tam thoi tang so luong ban sao cua dau sach (se cap nhat lai sau)
        // if (dauSach->tongBanSao < 0) dauSach->tongBanSao = 0; // Khong can thiet neu khoi tao dung
        // dauSach->tongBanSao++;

        ++soLuongOk;
    } // Ket thuc vong while doc file

    if (inp.eof()) {
        InTongKet("Doc danh muc sach", soLuongOk, soThuTuDong, soLuongBoQua, out); //
    }
    else if (inp.fail())
    {
        thongBao(out, "Loi: Gap su co khi doc file danh muc sach!", LOI);
    }
    inp.close();

    // Cap nhat lai chinh xac tong so ban sao cho tat ca dau sach sau khi da nap xong DMS
    CapNhatTongBanSao(dsDauSach, soLuongDauSach); //
}

// --- Hàm ghi file ---
bool GhiDanhSachDauSach(const char *path, PTRDS dsDauSach[], int soLuongDauSach, bool silent, std::ostream &out)
{
    std::ofstream outf(path);
    if (!outf.is_open()) {
        if (!silent)
            thongBao(out, "Loi: Khong the mo file de ghi dau sach: " + std::string(path), LOI);
        return false;
    }

    int dem = 0; // Dem so luong dau sach da ghi
    // Duyet qua mang dau sach
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) // Chi ghi neu con tro khong null
        {
            // Ghi cac truong, ngan cach boi dau '|'
            outf << dsDauSach[i]->ISBN << "|"
                 << dsDauSach[i]->tenSach << "|"
                 << dsDauSach[i]->soTrang << "|"
                 << dsDauSach[i]->tacGia << "|"
                 << dsDauSach[i]->namXuatBan << "|"
                 << dsDauSach[i]->theLoai << "\n";
            ++dem;
        }
    }
    outf.close(); // Dong file sau khi ghi xong

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
    // Duyet qua tung dau sach
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) {
            PTRDMS dms = dsDauSach[i]->dms; // Lay con tro dau DSLK cua dau sach thu i
            int demVong = 0; // dung bien dem de phat hien vong lap vo han

            // Duyet qua danh sach lien ket cac ban sao cua dau sach hien tai
            while (dms && demVong < SO_VONG_LAP_DMS_MAX) {
                // Ghi thong tin ban sao: MaSach|TrangThai(so)|ViTri
                outf << dms->maSach << "|" << dms->trangThai << "|" << dms->viTri << "\n";
                ++dem;
                dms = dms->next; // Chuyen qua ban sao tiep theo
                ++demVong;
            }

            // Kiem tra neu vong lap while chay qua gioi han -> co the co vong lap vo han
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

// --- Hàm in tổng kết ---
void InTongKet(const std::string &hanhDong, int thanhCong, int tongSo, int boQua, std::ostream &out)
{
    thongBao(out, hanhDong + ": " + std::to_string(thanhCong) + "/" + std::to_string(tongSo) +
             " thanh cong, " + std::to_string(boQua) + " bi bo qua.", THONG_TIN);
}