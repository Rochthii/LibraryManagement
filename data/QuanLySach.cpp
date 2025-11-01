#include "QuanLySach.h"
#include "ThongBao.h"
#include "KiemTraDuLieu.h"
#include "XuLyChuoi.h"
#include "ThaoTacFile.h"
#include "NhapLieu.h"
#include "Constants.h"
#include <iomanip>
#include <ctime>
#include <stdexcept>

//  BIEN TOAN CUC 
PTRDS dsDauSach[MAX_DAUSACH] = {nullptr};   // mang con tro dau sach
int soLuongDauSach = 0;                     // so dau sach hien co
bool duLieuDaThayDoi = false;               // danh dau du lieu bi thay doi

//  NHOM 1: SINH MA & TIEN ICH 

// sinh ma sach: ISBN-1, ISBN-2,...
std::string sinhMaSach(const std::string &chiSo, int &soThuTu) {
    std::string ma = chiSo + "-" + std::to_string(soThuTu);             // tao ma
    std::stringstream dummy;
    if (timDanhMucTheoMaSach(ma, dsDauSach, soLuongDauSach, dummy, true)) {
        return "Loi: Ma sach da ton tai: " + ma;                         // trung ma
    }
    return ma;                                                          // thanh cong
}

// tra ve ten trang thai (chuoi)
const char* TenTrangThai(TrangThaiSach tt) {
    return (tt == CHO_MUON_DUOC) ? "Cho muon duoc" :                     // 0
           (tt == DANG_MUON)     ? "Dang muon" :                        // 1
           (tt == THANH_LY)      ? "Thanh ly" : "Khong ro";             // 2
}

// chuyen chuoi trang thai -> so
int PhanTichTrangThaiSach(const std::string &s, std::ostream &out) {
    std::string tt = ChuyenInThuong(CatKhoangTrang(s));                 // chuan hoa
    if (tt == "0" || tt == "0\r") return CHO_MUON_DUOC;
    if (tt == "1" || tt == "1\r") return DANG_MUON;
    if (tt == "2" || tt == "2\r") return THANH_LY;
    if (tt == "cho muon duoc") return CHO_MUON_DUOC;
    if (tt == "dang muon")     return DANG_MUON;
    if (tt == "thanh ly")      return THANH_LY;
    thongBao(out, "Trang thai khong hop le: " + tt, LOI);               // loi
    return -1;
}

//  NHOM 2: QUAN LY DANH MUC SACH (DMS - DSLK) 

// them ban sao vao dau danh sach
std::string themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt) {
    if (tt < 0 || tt > 2) return "Loi: Trang thai khong hop le!";       // kiem tra trang thai
    if (vt.length() > MAX_VI_TRI_KE) return "Loi: Vi tri qua dai!";     // kiem tra do dai
    PTRDMS node = new DanhMucSach{ma, tt, vt, dms};                     // tao node
    dms = node;                                                         // chen vao dau
    return "";                                                          // thanh cong
}

// chen ban sao vao dau dms cua dau sach
bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &ma, int tt, const std::string &vt) {
    if (!d || tt < 0 || tt > 2) return false;                           // dau sach null hoac tt sai
    PTRDMS node = new DanhMucSach{ma, static_cast<TrangThaiSach>(tt), vt, d->dms};
    d->dms = node;                                                      // chen vao dau
    return true;
}

// cap nhat trang thai cho ban sao
void capNhatTrangThaiSach(PTRDMS dms, const std::string &ma, TrangThaiSach tt) {
    int dem = 0;
    while (dms && dem < SO_VONG_LAP_DMS_MAX) {                          // duyet dms
        if (dms->maSach == ma) { dms->trangThai = tt; return; }         // tim thay -> cap nhat
        dms = dms->next; ++dem;
    }
    if (dem >= SO_VONG_LAP_DMS_MAX) thongBao(std::cout, "Vong lap vo han trong DMS!", LOI);
}

// tim node ban sao theo ma sach
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent) {
    for (int i = 0; i < n; ++i) {                                       // duyet mang dau sach
        if (!ds[i]) continue;
        PTRDMS p = ds[i]->dms; int dem = 0;
        while (p && dem < SO_VONG_LAP_DMS_MAX) {                        // duyet dms
            if (p->maSach == ma) return p;                              // tim thay
            p = p->next; ++dem;
        }
        if (dem >= SO_VONG_LAP_DMS_MAX) thongBao(out, "Vong lap vo han trong DMS!", LOI);
    }
    if (!silent) {                                                      // khong tim thay
        std::string isbn = TachISBNTuMaSach(ma);
        thongBao(out, "Khong tim thay ma sach: " + ma + " (ISBN=" + isbn + ")", LOI);
    }
    return nullptr;
}

//  NHOM 3: QUAN LY DAU SACH (DS - MANG CON TRO) 

// them dau sach moi
bool themDauSach(PTRDS ds[], int &n, const std::string &isbn, const std::string &ten, int trang,
                 const std::string &tg, int nam, const std::string &tl, bool anLang) {
    if (n >= MAX_DAUSACH) { 
        if (!anLang) thongBao(std::cout, "Gioi han dau sach!", LOI); 
        return false; 
    }

    std::string loi, isbnChuan;
    // KIEM TRA ISBN
    if (!KiemTraChuoiRong(isbn, "ISBN").empty() ||
        !(loi = ChuanHoaISBNFile(isbn, isbnChuan)).empty() ||
        !(loi = KiemTraTrungISBN(isbnChuan)).empty()) {
        if (!anLang) thongBao(std::cout, loi, LOI);
        return false;
    }

    // KIEM TRA CAC TRUONG KHAC
    if (!(loi = KiemTraChuoiVaDodai(ten, "Ten sach", MAX_TEN_SACH)).empty() ||
        trang <= 0 || trang > MAX_SO_TRANG ||
        !(loi = KiemTraChuoiVaDodai(tg, "Tac gia", MAX_TAC_GIA)).empty() ||
        !(loi = KiemTraNamXuatBan(nam)).empty() ||
        !(loi = KiemTraChuoiVaDodai(tl, "The loai", MAX_THE_LOAI)).empty()) {
        if (!anLang) thongBao(std::cout, loi, LOI);
        return false;
    }

    // TAO DAU SACH MOI
    PTRDS node = new DauSach;
    node->ISBN = isbnChuan;
    node->tenSach = ChuyenThanhTitleCase(ten);
    node->soTrang = trang;
    node->tacGia = ChuyenThanhTitleCase(tg);
    node->namXuatBan = nam;
    node->theLoai = ChuyenThanhTitleCase(tl);
    node->dms = nullptr;
    node->tongBanSao = 0;

    ds[n++] = node;                                                     // them vao mang
    sapXepDauSachTheoTen(ds, 0, n - 1);                                 // sap xep lai
    if (!anLang) { 
        thongBao(std::cout, "Them thanh cong: " + node->tenSach, THONG_TIN); 
        duLieuDaThayDoi = true; 
    }
    return true;
}

// tim dau sach theo isbn
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn) {
    for (int i = 0; i < n; ++i)                                         // duyet mang
        if (ds[i] && ds[i]->ISBN == isbn) return ds[i];                 // tim thay
    return nullptr;                                                     // khong thay
}

// in chi tiet 1 dau sach
void InMotDauSach(PTRDS d, std::ostream &out) {
    if (!d) { thongBao(out, "Dau sach khong ton tai!", LOI); return; }  // null
    out << "ISBN: " << d->ISBN << "\n"
        << "Ten: " << d->tenSach << "\n"
        << "Trang: " << d->soTrang << "\n"
        << "Tac gia: " << d->tacGia << "\n"
        << "Nam XB: " << d->namXuatBan << "\n"
        << "The loai: " << d->theLoai << "\n"
        << "Tong ban sao: " << d->tongBanSao << "\n"
        << "DMS:\n";
    PTRDMS p = d->dms; int dem = 0;
    if (!p) out << "  Chua co ban sao.\n";
    else {
        while (p && dem < SO_VONG_LAP_DMS_MAX) {                        // in tung ban sao
            out << "  + " << p->maSach << ", " << TenTrangThai(p->trangThai) << ", " << p->viTri << "\n";
            p = p->next; ++dem;
        }
        if (dem >= SO_VONG_LAP_DMS_MAX) thongBao(out, "Vong lap vo han trong DMS!", LOI);
    }
    out << "\n";
}

//  NHOM 4: LOGIC TIM KIEM & LIET KE 

// tim kiem sach theo tu khoa
int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[], int max) {
    if (tk.empty() || n == 0) return 0;                                 // rong
    std::string tkChuan = BoDauVaThuong(ChuanHoaKhoangTrang(tk));       // chuan hoa tu khoa
    int dem = 0;
    for (int i = 0; i < n && dem < max; ++i) {                          // duyet dau sach
        PTRDS p = ds[i]; if (!p) continue;
        auto chuan = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
        int loai = 0;
        if (chuan.tenSach.find(tkChuan) != std::string::npos) loai = 1;  // uu tien ten
        else if (chuan.tacGia.find(tkChuan) != std::string::npos) loai = 2;
        else if (chuan.theLoai.find(tkChuan) != std::string::npos) loai = 3;
        else if (chuan.isbn.find(tkChuan) != std::string::npos) loai = 4;
        if (loai > 0) { kq[dem++] = {p, loai}; }                        // them ket qua
    }
    sapXepKetQuaTimKiem(kq, dem);                                       // sap xep ket qua
    return dem;
}

// lay danh sach the loai duy nhat
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string tl[], int max) {
    int dem = 0;
    for (int i = 0; i < n && dem < max; ++i) {                          // duyet dau sach
        if (!ds[i]) continue;
        bool tonTai = false;
        for (int j = 0; j < dem; ++j) if (tl[j] == ds[i]->theLoai) { tonTai = true; break; }
        if (!tonTai) tl[dem++] = ds[i]->theLoai;                        // them neu chua co
    }
    return dem;
}

// tim sach theo the loai
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &tl, PTRDS kq[], int max) {
    int dem = 0;
    for (int i = 0; i < n && dem < max; ++i)                             // duyet mang
        if (ds[i] && ds[i]->theLoai == tl) kq[dem++] = ds[i];           // trung the loai
    return dem;
}

//  NHOM 6: HAM SAP XEP 

// hoan doi hai con tro dau sach
void hoanDoiDauSach(PTRDS &a, PTRDS &b) { PTRDS t = a; a = b; b = t; }

// hoan doi ket qua tim kiem
void hoanDoiKetQua(KetQuaTimKiem &a, KetQuaTimKiem &b) {
    KetQuaTimKiem t = a; a = b; b = t;
}

// insertion sort cho dau sach
void insertionSort(PTRDS a[], int l, int h) {
    for (int i = l + 1; i <= h; ++i) {                                  // duyet tu l+1
        PTRDS k = a[i]; if (!k) continue;                               // lay phan tu
        int j = i - 1;
        while (j >= l && a[j] && a[j]->tenSach > k->tenSach) { a[j + 1] = a[j]; --j; } // dich phai
        a[j + 1] = k;                                                   // chen vao
    }
}

// partition cho quicksort
int partition(PTRDS a[], int l, int h) {
    PTRDS p = a[h]; if (!p) return l;                                   // chon pivot
    int i = l - 1;
    for (int j = l; j < h; ++j)
        if (a[j] && a[j]->tenSach <= p->tenSach) hoanDoiDauSach(a[++i], a[j]); // nho hon -> ben trai
    hoanDoiDauSach(a[i + 1], a[h]);                                     // dat pivot vao giua
    return i + 1;
}

// sap xep dau sach theo ten (quicksort + insertion)
void sapXepDauSachTheoTen(PTRDS a[], int l, int h) {
    if (l >= h || l < 0 || h >= MAX_DAUSACH) return;                    // dieu kien dung
    try {
        if (h - l > 10) {                                               // mang lon -> quicksort
            int p = partition(a, l, h);
            sapXepDauSachTheoTen(a, l, p - 1);
            sapXepDauSachTheoTen(a, p + 1, h);
        } else insertionSort(a, l, h);                                  // mang nho -> insertion
    } catch (...) { insertionSort(a, l, h); }                           // loi -> insertion
}

// sap xep ban sao theo so hau to (tang dan)
void SapXepBanSaoTheoMa(PTRDMS arr[], int n) {
    if (!arr || n <= 1) return;                                         // khong can sap xep

    for (int i = 0; i < n - 1; ++i) {                                   // bubble sort
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            int a = LaySoHauToMaSach(arr[j]);                           // so cua arr[j]
            int b = LaySoHauToMaSach(arr[j + 1]);                       // so cua arr[j+1]

            // CHI SWAP NEU SAI THU TU (a > b)
            if (a == -1 && b != -1) continue;                           // a > b -> dung thu tu
            if (a != -1 && b == -1) {                                   // a < b -> sai thu tu -> swap
                PTRDMS t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
                swapped = true;
            }
            else if (a != -1 && b != -1 && a > b) {                     // a > b -> swap
                PTRDMS t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
                swapped = true;
            }
        }
        if (!swapped) break;                                            // toi uu
    }
}

// sap xep mang the loai (insertion sort)
void SapXepTheLoaiTheoTen(std::string a[], int n) {
    for (int i = 1; i < n; ++i) {                                       // duyet tu 1
        std::string k = a[i]; int j = i - 1;                            // lay phan tu
        while (j >= 0 && a[j] > k) { a[j + 1] = a[j]; --j; }             // dich phai
        a[j + 1] = k;                                                   // chen vao
    }
}

// sap xep ket qua tim kiem (bubble sort)
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n) {
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j) {
            bool swap = false;
            if (arr[i].loaiKhop > arr[j].loaiKhop) swap = true;         // uu tien loai khop
            else if (arr[i].loaiKhop == arr[j].loaiKhop && arr[i].sach->tenSach > arr[j].sach->tenSach) swap = true;
            if (swap) hoanDoiKetQua(arr[i], arr[j]);                    // hoan doi
        }
}

//  NHOM 7: QUAN LY BO NHO 

// giai phong danh muc sach
void GiaiPhongDanhMucSach(PTRDMS &dms) {
    while (dms) { PTRDMS t = dms; dms = dms->next; delete t; }          // xoa tung node
    dms = nullptr;
}

// giai phong 1 dau sach
void GiaiPhongDauSach(PTRDS &d) {
    if (d) { GiaiPhongDanhMucSach(d->dms); delete d; d = nullptr; }     // xoa dms truoc
}

// giai phong toan bo dau sach
void GiaiPhongToanBoDauSach(PTRDS ds[], int &n) {
    for (int i = 0; i < n; ++i) GiaiPhongDauSach(ds[i]);                 // xoa tung dau sach
    n = 0;
}

// dem tong so ban sao
int DemTongSoBanSao(PTRDS ds[], int n) {
    int tong = 0;
    for (int i = 0; i < n; ++i) {                                       // duyet dau sach
        if (!ds[i]) continue;
        PTRDMS p = ds[i]->dms; int dem = 0;
        while (p && dem < SO_VONG_LAP_DMS_MAX) { ++tong; p = p->next; ++dem; }
        if (dem >= SO_VONG_LAP_DMS_MAX) thongBao(std::cout, "Vong lap vo han DMS!", LOI);
    }
    return tong;
}

// cap nhat tong ban sao cho tat ca dau sach
void CapNhatTongBanSao(PTRDS ds[], int n) {
    for (int i = 0; i < n; ++i) {                                       // duyet dau sach
        if (!ds[i]) continue;
        int dem = 0; PTRDMS p = ds[i]->dms; int v = 0;
        while (p && v < SO_VONG_LAP_DMS_MAX) { ++dem; p = p->next; ++v; }
        if (v >= SO_VONG_LAP_DMS_MAX) thongBao(std::cout, "Vong lap vo han!", LOI);
        ds[i]->tongBanSao = dem;                                        // cap nhat
    }
}

//  NHOM 8: XOA & CAP NHAT 

// xoa dau sach theo isbn
std::string XoaDauSachTheoISBN(PTRDS ds[], int &n, const std::string &isbn) {
    int idx = -1;
    for (int i = 0; i < n; ++i) if (ds[i] && ds[i]->ISBN == isbn) { idx = i; break; }
    if (idx == -1) return "Loi: Khong tim thay ISBN: " + isbn;
    if (ds[idx]->dms) return "Loi: Con ban sao, khong xoa duoc!";
    GiaiPhongDauSach(ds[idx]);                                          // giai phong
    for (int i = idx; i < n - 1; ++i) ds[i] = ds[i + 1];                 // dich mang
    ds[--n] = nullptr; duLieuDaThayDoi = true;
    return "";
}

// xoa ban sao theo ma sach
bool XoaSachTheoMaSach(PTRDS ds[], int n, const std::string &ma, std::ostream &out) {
    std::string isbn = TachISBNTuMaSach(ma);
    if (isbn.empty()) { thongBao(out, "Ma sach khong hop le!", LOI); return false; }
    PTRDS d = TimDauSachTheoISBN(ds, n, isbn);
    if (!d) { thongBao(out, "Khong tim thay dau sach!", LOI); return false; }

    PTRDMS curr = d->dms, prev = nullptr; int dem = 0;
    while (curr && dem < SO_VONG_LAP_DMS_MAX) {
        if (curr->maSach == ma) {
            if (curr->trangThai != CHO_MUON_DUOC) { thongBao(out, "Khong xoa duoc sach dang muon!", LOI); return false; }
            if (prev) prev->next = curr->next; else d->dms = curr->next;
            delete curr; d->tongBanSao--; duLieuDaThayDoi = true;
            thongBao(out, "Xoa thanh cong!", THONG_TIN); return true;
        }
        prev = curr; curr = curr->next; ++dem;
    }
    if (dem >= SO_VONG_LAP_DMS_MAX) thongBao(out, "Vong lap vo han!", LOI);
    thongBao(out, "Khong tim thay ma sach!", LOI); return false;
}

// cap nhat dau sach
std::string CapNhatDauSach(PTRDS ds[], int n, const std::string &isbn,
                           const std::string &ten, int trang, const std::string &tg,
                           int nam, const std::string &tl) {
    PTRDS d = TimDauSachTheoISBN(ds, n, isbn);
    if (!d) return "Loi: Khong tim thay ISBN!";
    std::string loi;
    if (!(loi = KiemTraChuoiVaDodai(ten, "Ten sach", MAX_TEN_SACH)).empty() ||
        trang <= 0 || trang > MAX_SO_TRANG ||
        !(loi = KiemTraChuoiVaDodai(tg, "Tac gia", MAX_TAC_GIA)).empty() ||
        !(loi = KiemTraNamXuatBan(nam)).empty() ||
        !(loi = KiemTraChuoiVaDodai(tl, "The loai", MAX_THE_LOAI)).empty()) return loi;

    bool tenDoi = (d->tenSach != ChuyenThanhTitleCase(ten));
    d->tenSach = ChuyenThanhTitleCase(ten);
    d->soTrang = trang;
    d->tacGia = ChuyenThanhTitleCase(tg);
    d->namXuatBan = nam;
    d->theLoai = ChuyenThanhTitleCase(tl);
    if (tenDoi) sapXepDauSachTheoTen(ds, 0, n - 1);                     // sap xep neu ten thay doi
    duLieuDaThayDoi = true;
    return "";
}