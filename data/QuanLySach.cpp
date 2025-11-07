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



//  NHOM 1: SINH MA & CHUYEN DOI TRANG THAI

// sinh ma sach tu dong: ISBN-1, ISBN-2, ISBN-3, ...
std::string sinhMaSach(const std::string &chiSo, int &soThuTu) {
    std::string ma = chiSo + "-" + std::to_string(soThuTu);     // tao ma sach
    std::stringstream dummy;
    
    if (timDanhMucTheoMaSach(ma, dsDauSach, soLuongDauSach, dummy, true)) {
        return "Loi: Ma sach da ton tai: " + ma;                // ma bi trung
    }
    
    return ma;                                                  // thanh cong
}

// tra ve ten trang thai dang chuoi
const char* TenTrangThai(TrangThaiSach tt) {
    if (tt == CHO_MUON_DUOC) return "Cho muon duoc";           // 0
    if (tt == DANG_MUON)     return "Dang muon";               // 1
    if (tt == THANH_LY)      return "Thanh ly";                // 2
    return "Khong ro";                                         // khong xac dinh
}

// chuyen chuoi trang thai thanh so enum
int PhanTichTrangThaiSach(const std::string &s, std::ostream &out) {
    std::string tt = ChuyenInThuong(CatKhoangTrang(s));        // chuan hoa chuoi
    
    if (tt == "0" || tt == "0\r")       return CHO_MUON_DUOC;  // so 0
    if (tt == "1" || tt == "1\r")       return DANG_MUON;      // so 1
    if (tt == "2" || tt == "2\r")       return THANH_LY;       // so 2
    if (tt == "cho muon duoc")          return CHO_MUON_DUOC;  // chu
    if (tt == "dang muon")              return DANG_MUON;      // chu
    if (tt == "thanh ly")               return THANH_LY;       // chu
    
    thongBao(out, "Trang thai khong hop le: " + tt, LOI);     // loi
    return -1;
}



//  NHOM 2: QUAN LY DANH MUC SACH (DMS - DANH SACH LIEN KET DON)


// them ban sao vao dau danh sach lien ket
std::string themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt) {
    if (tt < 0 || tt > 2)                return "Loi: Trang thai khong hop le!";  // kiem tra trang thai
    if (vt.length() > MAX_VI_TRI_KE)     return "Loi: Vi tri qua dai!";           // kiem tra do dai
    
    PTRDMS node = new DanhMucSach{ma, tt, vt, dms};  // tao node moi
    dms = node;                                       // chen vao dau
    return "";                                        // thanh cong
}

// chen node DMS vao dau sach (dung khi doc file)
bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &ma, int tt, const std::string &vt) {
    if (!d || tt < 0 || tt > 2) return false;                  // kiem tra hop le
    
    PTRDMS node = new DanhMucSach{ma, static_cast<TrangThaiSach>(tt), vt, d->dms};
    d->dms = node;                                              // chen vao dau
    return true;
}

// cap nhat trang thai cho ban sao
bool capNhatTrangThaiSach(PTRDMS dms, const std::string &ma, TrangThaiSach tt) {
    int dem = 0;
    
    while (dms && dem < SO_VONG_LAP_DMS_MAX) {                 // duyet danh sach
        if (dms->maSach == ma) {                                // tim thay ma sach
            dms->trangThai = tt;                                // cap nhat trang thai
            return true;
        }
        dms = dms->next;                                        // chuyen node tiep
        ++dem;
    }
    
    if (dem >= SO_VONG_LAP_DMS_MAX) {                          // canh bao vo han
        thongBao(std::cout, "Vong lap vo han trong DMS!", LOI);
    }
    return false;                                               // khong tim thay
}

// tim node ban sao theo ma sach
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent) {
    for (int i = 0; i < n; ++i) {                              // duyet qua tung dau sach
        if (!ds[i]) continue;                                   // bo qua null
        
        PTRDMS p = ds[i]->dms;
        int dem = 0;
        
        while (p && dem < SO_VONG_LAP_DMS_MAX) {               // duyet danh muc sach
            if (p->maSach == ma) return p;                      // tim thay
            p = p->next;                                        // chuyen node tiep
            ++dem;
        }
        
        if (dem >= SO_VONG_LAP_DMS_MAX) {                      // canh bao vo han
            thongBao(out, "Vong lap vo han trong DMS!", LOI);
        }
    }
    
    if (!silent) {                                             // khong tim thay
        std::string isbn = TachISBNTuMaSach(ma);
        thongBao(out, "Khong tim thay ma sach: " + ma + " (ISBN=" + isbn + ")", LOI);
    }
    return nullptr;
}



//  NHOM 3: QUAN LY DAU SACH (MANG CON TRO)


// them dau sach moi vao mang
bool themDauSach(PTRDS ds[], int &n, const std::string &isbn, const std::string &ten, int trang,
                 const std::string &tg, int nam, const std::string &tl, bool anLang) {
    
    if (n >= MAX_DAUSACH) {                                    // kiem tra gioi han
        if (!anLang) thongBao(std::cout, "Gioi han dau sach!", LOI);
        return false;
    }

    std::string loi, isbnChuan;
    
    // kiem tra ISBN
    if (!KiemTraChuoiRong(isbn, "ISBN").empty() ||
        !(loi = ChuanHoaISBNFile(isbn, isbnChuan)).empty() ||
        !(loi = KiemTraTrungISBN(isbnChuan)).empty()) {
        if (!anLang) thongBao(std::cout, loi, LOI);
        return false;
    }

    // kiem tra cac truong khac
    if (!(loi = KiemTraChuoiVaDodai(ten, "Ten sach", MAX_TEN_SACH)).empty() ||
        trang <= 0 || trang > MAX_SO_TRANG ||
        !(loi = KiemTraChuoiVaDodai(tg, "Tac gia", MAX_TAC_GIA)).empty() ||
        !(loi = KiemTraNamXuatBan(nam)).empty() ||
        !(loi = KiemTraChuoiVaDodai(tl, "The loai", MAX_THE_LOAI)).empty()) {
        if (!anLang) thongBao(std::cout, loi, LOI);
        return false;
    }

    // tao dau sach moi
    PTRDS node = new DauSach;
    node->ISBN = isbnChuan;                                    // gan ISBN
    node->tenSach = ChuyenThanhTitleCase(ten);                 // gan ten sach
    node->soTrang = trang;                                     // gan so trang
    node->tacGia = ChuyenThanhTitleCase(tg);                   // gan tac gia
    node->namXuatBan = nam;                                    // gan nam XB
    node->theLoai = ChuyenThanhTitleCase(tl);                  // gan the loai
    node->dms = nullptr;                                       // chua co ban sao
    node->tongBanSao = 0;                                      // tong = 0

    ds[n++] = node;                                            // them vao mang
    sapXepDauSachTheoTen(ds, 0, n - 1);                        // sap xep lai
    duLieuDaThayDoi = true;                                    // danh dau thay doi
    
    if (!anLang) {
        thongBao(std::cout, "Them thanh cong: " + node->tenSach, THONG_TIN);
    }
    return true;
}

// tim dau sach theo ISBN
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn) {
    for (int i = 0; i < n; ++i) {                              // duyet mang
        if (ds[i] && ds[i]->ISBN == isbn) return ds[i];         // tim thay
    }
    return nullptr;                                             // khong thay
}

// in chi tiet mot dau sach
void InMotDauSach(PTRDS d, std::ostream &out) {
    if (!d) {                                                  // kiem tra null
        thongBao(out, "Dau sach khong ton tai!", LOI);
        return;
    }
    
    out << "ISBN: " << d->ISBN << "\n"
        << "Ten: " << d->tenSach << "\n"
        << "Trang: " << d->soTrang << "\n"
        << "Tac gia: " << d->tacGia << "\n"
        << "Nam XB: " << d->namXuatBan << "\n"
        << "The loai: " << d->theLoai << "\n"
        << "Tong ban sao: " << d->tongBanSao << "\n"
        << "DMS:\n";
    
    PTRDMS p = d->dms;
    int dem = 0;
    
    if (!p) {
        out << "  Chua co ban sao.\n";
    } else {
        while (p && dem < SO_VONG_LAP_DMS_MAX) {               // duyet ban sao
            out << "  + " << p->maSach 
                << ", " << TenTrangThai(p->trangThai) 
                << ", " << p->viTri << "\n";
            p = p->next;                                        // chuyen node tiep
            ++dem;
        }
        
        if (dem >= SO_VONG_LAP_DMS_MAX) {                      // canh bao vo han
            thongBao(out, "Vong lap vo han trong DMS!", LOI);
        }
    }
    out << "\n";
}



//  NHOM 4: TIM KIEM & LIET KE


// tim kiem sach theo tu khoa
int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[]) {
    if (tk.empty() || n == 0) return 0;                        // kiem tra rong
    
    std::string tkChuan = BoDauVaThuong(ChuanHoaKhoangTrang(tk));  // chuan hoa
    int dem = 0;
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        PTRDS p = ds[i];
        if (!p) continue;                                       // bo qua null
        
        auto chuan = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
        int loai = 0;
        
        // xac dinh loai khop (uu tien: ten > tac gia > the loai > isbn)
        if      (chuan.tenSach.find(tkChuan) != std::string::npos)  loai = 1;  // khop ten
        else if (chuan.tacGia.find(tkChuan) != std::string::npos)   loai = 2;  // khop tac gia
        else if (chuan.theLoai.find(tkChuan) != std::string::npos)  loai = 3;  // khop the loai
        else if (chuan.isbn.find(tkChuan) != std::string::npos)     loai = 4;  // khop isbn
        
        if (loai > 0 && dem < MAX_DAUSACH) {                   // them vao ket qua
            kq[dem++] = {p, loai};
        }
    }
    
    sapXepKetQuaTimKiem(kq, dem);                              // sap xep ket qua
    return dem;
}

// lay danh sach cac the loai duy nhat
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string tl[]) {
    int dem = 0;
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        if (!ds[i]) continue;                                   // bo qua null
        
        bool tonTai = false;                                    // kiem tra trung
        for (int j = 0; j < dem; ++j) {                        // duyet the loai da co
            if (tl[j] == ds[i]->theLoai) {                     // trung the loai
                tonTai = true;
                break;
            }
        }
        
        if (!tonTai && dem < MAX_DAUSACH) {                    // them the loai moi
            tl[dem++] = ds[i]->theLoai;
        }
    }
    return dem;
}

// tim sach theo the loai
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &tl, PTRDS kq[]) {
    int dem = 0;
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        if (ds[i] && ds[i]->theLoai == tl && dem < MAX_DAUSACH) {  // khop the loai
            kq[dem++] = ds[i];                                  // them vao ket qua
        }
    }
    return dem;
}


//  NHOM 5: SAP XEP


// hoan doi hai con tro dau sach
void hoanDoiDauSach(PTRDS &a, PTRDS &b) {
    PTRDS temp = a;                                            // luu tam
    a = b;                                                      // hoan doi
    b = temp;
}

// hoan doi ket qua tim kiem
void hoanDoiKetQua(KetQuaTimKiem &a, KetQuaTimKiem &b) {
    KetQuaTimKiem temp = a;                                    // luu tam
    a = b;                                                      // hoan doi
    b = temp;
}

// insertion sort cho dau sach
void insertionSort(PTRDS a[], int l, int h) {
    for (int i = l + 1; i <= h; ++i) {                         // duyet tu l+1
        PTRDS key = a[i];                                       // lay phan tu hien tai
        if (!key) continue;                                     // bo qua null
        
        int j = i - 1;                                          // vi tri so sanh
        while (j >= l && a[j] && a[j]->tenSach > key->tenSach) {  // dich phai
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;                                         // chen vao vi tri dung
    }
}

// partition cho quicksort
int partition(PTRDS a[], int l, int h) {
    PTRDS pivot = a[h];                                        // chon pivot cuoi
    if (!pivot) return l;
    
    int i = l - 1;                                              // chi so phan hoach
    for (int j = l; j < h; ++j) {                              // duyet mang
        if (a[j] && a[j]->tenSach <= pivot->tenSach) {         // nho hon pivot
            hoanDoiDauSach(a[++i], a[j]);                      // doi cho
        }
    }
    hoanDoiDauSach(a[i + 1], a[h]);                            // dat pivot vao giua
    return i + 1;
}

// sap xep dau sach theo ten (quicksort + insertion sort)
void sapXepDauSachTheoTen(PTRDS a[], int l, int h) {
    if (l >= h || l < 0 || h >= MAX_DAUSACH) return;           // dieu kien dung
    
    try {
        if (h - l > 10) {                                       // mang lon
            int p = partition(a, l, h);                         // phan hoach
            sapXepDauSachTheoTen(a, l, p - 1);                 // de quy trai
            sapXepDauSachTheoTen(a, p + 1, h);                 // de quy phai
        } else {                                                // mang nho
            insertionSort(a, l, h);                            // insertion sort
        }
    } catch (...) {                                            // bat loi
        insertionSort(a, l, h);                                // fallback
    }
}

// sap xep ban sao theo ma sach (bubble sort)
void SapXepBanSaoTheoMa(PTRDMS arr[], int n) {
    if (!arr || n <= 1) return;                                // mang rong

    for (int i = 0; i < n - 1; ++i) {                         // duyet lan
        bool swapped = false;                                  // co doi cho
        
        for (int j = 0; j < n - i - 1; ++j) {                 // duyet phan tu
            int soA = LaySoHauToMaSach(arr[j]);               // lay so a
            int soB = LaySoHauToMaSach(arr[j + 1]);           // lay so b
            bool canSwap = false;                              // co can doi
            
            // sap xep: so hop le tang dan, loi (-1) o cuoi
            if      (soA == -1 && soB != -1)                canSwap = true;   // a loi, b hop le
            else if (soA != -1 && soB == -1)                canSwap = false;  // a hop le, b loi
            else if (soA != -1 && soB != -1 && soA > soB)   canSwap = true;   // ca 2 hop le
            
            if (canSwap) {                                     // doi cho
                PTRDMS temp = arr[j];                          // luu tam
                arr[j] = arr[j + 1];                           // gan a = b
                arr[j + 1] = temp;                             // gan b = a
                swapped = true;                                // danh dau doi
            }
        }
        
        if (!swapped) break;                                   // da xong
    }
}

// sap xep mang the loai (insertion sort)
void SapXepTheLoaiTheoTen(std::string a[], int n) {
    for (int i = 1; i < n; ++i) {                             // duyet tu 1
        std::string key = a[i];                                // lay phan tu hien tai
        int j = i - 1;                                         // vi tri so sanh
        
        while (j >= 0 && a[j] > key) {                        // dich phai
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;                                        // chen vao vi tri dung
    }
}

// sap xep ket qua tim kiem (bubble sort)
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n) {
    for (int i = 0; i < n - 1; ++i) {                         // duyet phan tu
        for (int j = i + 1; j < n; ++j) {                     // duyet phan tu sau
            bool swap = false;                                 // co doi cho
            
            if (arr[i].loaiKhop > arr[j].loaiKhop) {          // uu tien loai khop
                swap = true;
            } else if (arr[i].loaiKhop == arr[j].loaiKhop) {  // cung loai
                std::string tenA = ChuyenInThuong(arr[i].sach->tenSach);  // ten a
                std::string tenB = ChuyenInThuong(arr[j].sach->tenSach);  // ten b
                if (tenA > tenB) swap = true;                 // so sanh ten
            }
            
            if (swap) hoanDoiKetQua(arr[i], arr[j]);          // doi cho
        }
    }
}



//  NHOM 6: QUAN LY BO NHO


// giai phong danh muc sach (danh sach lien ket)
void GiaiPhongDanhMucSach(PTRDMS &dms) {
    while (dms) {                                              // duyet het danh sach
        PTRDMS temp = dms;                                     // luu node hien tai
        dms = dms->next;                                       // chuyen sang node sau
        delete temp;                                           // xoa node cu
    }
    dms = nullptr;                                             // dat null
}

// giai phong mot dau sach
void GiaiPhongDauSach(PTRDS &d) {
    if (d) {                                                   // neu ton tai
        GiaiPhongDanhMucSach(d->dms);                         // xoa danh sach ban sao
        delete d;                                              // xoa dau sach
        d = nullptr;                                           // dat null
    }
}

// giai phong toan bo dau sach
void GiaiPhongToanBoDauSach(PTRDS ds[], int &n) {
    for (int i = 0; i < n; ++i) {                             // duyet mang
        GiaiPhongDauSach(ds[i]);                              // xoa tung dau sach
    }
    n = 0;                                                     // reset so luong
}

// dem tong so ban sao
int DemTongSoBanSao(PTRDS ds[], int n) {
    int tong = 0;                                              // khoi tao tong
    
    for (int i = 0; i < n; ++i) {                             // duyet dau sach
        if (!ds[i]) continue;                                  // bo qua null
        
        PTRDMS p = ds[i]->dms;                                // lay danh sach ban sao
        int dem = 0;                                           // dem vong lap
        
        while (p && dem < SO_VONG_LAP_DMS_MAX) {              // duyet danh sach
            ++tong;                                            // tang tong
            p = p->next;                                       // node tiep theo
            ++dem;                                             // tang dem
        }
        
        if (dem >= SO_VONG_LAP_DMS_MAX) {                     // kiem tra vong lap vo han
            thongBao(std::cout, "Vong lap vo han DMS!", LOI);
        }
    }
    return tong;                                               // tra ve tong
}

// cap nhat tong ban sao cho tat ca dau sach
void CapNhatTongBanSao(PTRDS ds[], int n) {
    for (int i = 0; i < n; ++i) {                             // duyet dau sach
        if (!ds[i]) continue;                                  // bo qua null
        
        int dem = 0;                                           // khoi tao dem
        PTRDMS p = ds[i]->dms;                                // lay danh sach ban sao
        int vongLap = 0;                                       // bien kiem tra vong lap
        
        while (p && vongLap < SO_VONG_LAP_DMS_MAX) {          // duyet danh sach
            ++dem;                                             // tang dem
            p = p->next;                                       // node tiep theo
            ++vongLap;                                         // tang vong lap
        }
        
        if (vongLap >= SO_VONG_LAP_DMS_MAX) {                 // kiem tra vong lap vo han
            thongBao(std::cout, "Vong lap vo han!", LOI);
        }
        
        ds[i]->tongBanSao = dem;                              // cap nhat tong
    }
}



//  NHOM 7: XOA & CAP NHAT


// xoa dau sach theo ISBN
std::string XoaDauSachTheoISBN(PTRDS ds[], int &n, const std::string &isbn) {
    int idx = -1;                                              // vi tri dau sach
    for (int i = 0; i < n; ++i) {                             // duyet mang
        if (ds[i] && ds[i]->ISBN == isbn) {                   // tim thay
            idx = i;
            break;
        }
    }
    
    if (idx == -1)      return "Loi: Khong tim thay ISBN: " + isbn;  // khong tim thay
    if (ds[idx]->dms)   return "Loi: Con ban sao, khong xoa duoc!";  // con ban sao
    
    GiaiPhongDauSach(ds[idx]);                                // giai phong bo nho
    
    for (int i = idx; i < n - 1; ++i) {                       // dich mang sang trai
        ds[i] = ds[i + 1];
    }
    
    ds[--n] = nullptr;                                         // giam so luong
    duLieuDaThayDoi = true;                                    // danh dau thay doi
    return "";                                                 // thanh cong
}

// xoa sach theo ma sach
bool XoaSachTheoMaSach(PTRDS ds[], int n, const std::string &ma, std::ostream &out) {
    std::string isbn = TachISBNTuMaSach(ma);                  // lay ISBN tu ma sach
    if (isbn.empty()) {                                        // ma khong hop le
        thongBao(out, "Ma sach khong hop le!", LOI);
        return false;
    }
    
    PTRDS d = TimDauSachTheoISBN(ds, n, isbn);                // tim dau sach
    if (!d) {                                                  // khong tim thay
        thongBao(out, "Khong tim thay dau sach!", LOI);
        return false;
    }

    PTRDMS curr = d->dms;                                      // node hien tai
    PTRDMS prev = nullptr;                                     // node truoc
    int dem = 0;                                               // dem vong lap
    
    while (curr && dem < SO_VONG_LAP_DMS_MAX) {               // duyet danh sach
        if (curr->maSach == ma) {                             // tim thay ma sach
            // chi khong cho xoa sach DANG_MUON
            // con CHO_MUON_DUOC va THANH_LY thi duoc phep xoa
            if (curr->trangThai == DANG_MUON) {               // dang muon
                thongBao(out, "Khong xoa duoc sach dang muon!", LOI);
                return false;
            }
            
            if (prev)   prev->next = curr->next;              // xoa node giua
            else        d->dms = curr->next;                  // xoa node dau
            
            delete curr;                                       // giai phong bo nho
            d->tongBanSao--;                                   // giam so luong
            duLieuDaThayDoi = true;                            // danh dau thay doi
            thongBao(out, "Xoa thanh cong!", THONG_TIN);
            return true;
        }
        
        prev = curr;                                           // luu node truoc
        curr = curr->next;                                     // chuyen node sau
        ++dem;                                                 // tang dem
    }
    
    if (dem >= SO_VONG_LAP_DMS_MAX) {                         // vong lap vo han
        thongBao(out, "Vong lap vo han!", LOI);
    }
    
    thongBao(out, "Khong tim thay ma sach!", LOI);
    return false;
}

// cap nhat dau sach
std::string CapNhatDauSach(PTRDS ds[], int n, const std::string &isbn, const std::string &ten, 
                          int trang, const std::string &tg, int nam, const std::string &tl) {
    PTRDS d = TimDauSachTheoISBN(ds, n, isbn);                // tim dau sach
    if (!d) return "Loi: Khong tim thay ISBN!";               // khong tim thay
    
    std::string loi;                                           // bien luu loi
    
    // kiem tra validation
    if (!(loi = KiemTraChuoiVaDodai(ten, "Ten sach", MAX_TEN_SACH)).empty() ||        // kiem tra ten
        trang <= 0 || trang > MAX_SO_TRANG ||                                         // kiem tra trang
        !(loi = KiemTraChuoiVaDodai(tg, "Tac gia", MAX_TAC_GIA)).empty() ||          // kiem tra tac gia
        !(loi = KiemTraNamXuatBan(nam)).empty() ||                                    // kiem tra nam
        !(loi = KiemTraChuoiVaDodai(tl, "The loai", MAX_THE_LOAI)).empty()) {        // kiem tra the loai
        return loi;                                            // tra ve loi
    }

    bool tenDoi = (d->tenSach != ChuyenThanhTitleCase(ten));  // kiem tra ten doi
    d->tenSach = ChuyenThanhTitleCase(ten);                   // cap nhat ten
    d->soTrang = trang;                                        // cap nhat trang
    d->tacGia = ChuyenThanhTitleCase(tg);                     // cap nhat tac gia
    d->namXuatBan = nam;                                       // cap nhat nam
    d->theLoai = ChuyenThanhTitleCase(tl);                    // cap nhat the loai
    
    if (tenDoi) {                                              // neu ten doi
        sapXepDauSachTheoTen(ds, 0, n - 1);                   // sap xep lai
    }
    
    duLieuDaThayDoi = true;                                    // danh dau thay doi
    return "";                                                 // thanh cong
}