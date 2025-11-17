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

// chuyen chuoi trang thai thanh enum
int PhanTichTrangThaiSach(const std::string &s, std::ostream &out) {
    std::string tt = ChuyenInThuong(CatKhoangTrang(s));
    if (tt == "0" || tt == "0\r" || tt == "cho muon duoc") return CHO_MUON_DUOC;
    if (tt == "1" || tt == "1\r" || tt == "dang muon")     return DANG_MUON;
    if (tt == "2" || tt == "2\r" || tt == "thanh ly")      return THANH_LY;
    thongBao(out, "Trang thai khong hop le: " + tt, LOI);
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
    while (dms) {                                              // duyet DSLK don
        if (dms->maSach == ma) {                               // tim thay - early return
            dms->trangThai = tt;                               // cap nhat trang thai
            return true;
        }
        dms = dms->next;                                       // node tiep theo
    }
    return false;                                              // khong tim thay
}

// tim node ban sao theo ma sach 
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent) {
    //trich ISBN -> Tim dau sach O(N) -> Tim trong 1 DSLK O(M)
    std::string isbn = TachISBNTuMaSach(ma);                   // lay ISBN tu ma sach
    if (isbn.empty()) {                                         // ma sach khong hop le
        if (!silent) thongBao(out, "Ma sach khong hop le: " + ma, LOI);
        return nullptr;
    }
    
    // Tim dau sach theo ISBN (chi can tim 1 dau sach, khong can duyet tat ca)
    PTRDS dauSach = TimDauSachTheoISBN(ds, n, isbn);
    if (!dauSach) {                                             // khong tim thay dau sach
        if (!silent) thongBao(out, "Khong tim thay dau sach cho ma: " + ma + " (ISBN=" + isbn + ")", LOI);
        return nullptr;
    }
    
    // Chi duyet DSLK cua dau sach nay (thay vi duyet N dau sach)
    PTRDMS p = dauSach->dms;
    while (p) {                                                 // duyet DSLK don
        if (p->maSach == ma) return p;                         // tim thay - early return
        p = p->next;                                            // node tiep theo
    }
    
    if (!silent) {                                             // khong tim thay trong DSLK
        thongBao(out, "Khong tim thay ma sach: " + ma + " trong danh muc cua ISBN=" + isbn, LOI);
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
    node->soLuotMuon = 0;                                      // khoi tao so luot muon

    ds[n++] = node;                                            // them vao mang
    sapXepDauSachTheoTen(ds, 0, n - 1);                        // sap xep lai
    duLieuDaThayDoi = true;                                    // danh dau thay doi
    
    if (!anLang) {
        thongBao(std::cout, "Them thanh cong: " + node->tenSach, THONG_TIN);
    }
    return true;
}

// tim dau sach theo ISBN - linear search O(N)
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn) {
    for (int i = 0; i < n; ++i) {                              // duyet mang
        if (ds[i] && ds[i]->ISBN == isbn) {                    // tim thay - early return
            return ds[i];
        }
    }
    return nullptr;                                             // khong thay
}



//  NHOM 4: TIM KIEM & LIET KE


// tim kiem sach theo tu khoa - tra ve mang ket qua da sap xep
int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[]) {
    if (tk.empty() || n == 0) return 0;                        // kiem tra rong
    
    std::string tuKhoaChuan = ChuanHoaChuoiTimKiem(tk);        // chuan hoa tu khoa
    int soKetQua = 0;                                          // dem ket qua tim duoc
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        PTRDS p = ds[i];
        if (!p) continue;                                       // bo qua null
        
        // goi 1 lan ChuanHoaDuLieuSach de tranh lap lai
        auto duLieuChuan = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
        int loaiKhop = 0;                                      // xac dinh loai khop
        
        // uu tien: ten sach > tac gia > the loai > isbn
        if      (duLieuChuan.tenSach.find(tuKhoaChuan) != std::string::npos)  loaiKhop = 1;
        else if (duLieuChuan.tacGia.find(tuKhoaChuan) != std::string::npos)   loaiKhop = 2;
        else if (duLieuChuan.theLoai.find(tuKhoaChuan) != std::string::npos)  loaiKhop = 3;
        else if (duLieuChuan.isbn.find(tuKhoaChuan) != std::string::npos)     loaiKhop = 4;
        
        if (loaiKhop > 0 && soKetQua < MAX_KET_QUA_TIM_KIEM) {  // gioi han ket qua
            kq[soKetQua++] = {p, loaiKhop};
        }
        
        if (soKetQua >= MAX_KET_QUA_TIM_KIEM) break;           // dung khi du
    }
    
    sapXepKetQuaTimKiem(kq, soKetQua);                         // sap xep ket qua
    return soKetQua;
}

// helper: binary search trong mang the loai da sap xep
static bool TimTheLoaiTrongMang(const std::string theLoaiCanTim, std::string mangTheLoai[], int soLuong) {
    int left = 0, right = soLuong - 1;                         // chi so trai/phai
    
    while (left <= right) {                                    // binary search O(log N)
        int mid = left + (right - left) / 2;                   // tranh overflow
        
        if (mangTheLoai[mid] == theLoaiCanTim)   return true;   // tim thay
        else if (mangTheLoai[mid] < theLoaiCanTim)  left = mid + 1;  // tim ben phai
        else right = mid - 1; // tim ben trai
    }
    return false;                                              // khong tim thay
}

// lay danh sach cac the loai duy nhat - sap xep va binary search de check trung
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string mangTheLoai[]) {
    int soTheLoai = 0;                                         // so the loai duy nhat
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        if (!ds[i]) continue;                                   // bo qua null
        
        std::string theLoaiHienTai = ds[i]->theLoai;
        
        // dung binary search de check trung (mang da sap xep)
        bool daTonTai = (soTheLoai > 0) && TimTheLoaiTrongMang(theLoaiHienTai, mangTheLoai, soTheLoai);
        
        if (!daTonTai && soTheLoai < MAX_DAUSACH) {            // them the loai moi
            mangTheLoai[soTheLoai++] = theLoaiHienTai;
            SapXepTheLoaiTheoTen(mangTheLoai, soTheLoai);      // sap xep lai de binary search lan sau
        }
    }
    return soTheLoai;
}

// tim sach theo the loai - tra ve mang sach cung the loai
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &theLoaiCanTim, PTRDS mangKetQua[]) {
    int soKetQua = 0;                                          // dem ket qua
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        if (ds[i] && ds[i]->theLoai == theLoaiCanTim && soKetQua < MAX_DAUSACH) {
            mangKetQua[soKetQua++] = ds[i];                    // them vao ket qua
        }
    }
    return soKetQua;
}

// lay danh sach cac vi tri duy nhat tu cac ban sao
int TimViTriDuyNhat(PTRDS ds[], int n, std::string mangViTri[]) {
    int soViTri = 0;                                           // so vi tri duy nhat
    
    for (int i = 0; i < n; ++i) {                              // duyet dau sach
        if (!ds[i]) continue;                                   // bo qua null
        
        // duyet cac ban sao de lay vi tri
        PTRDMS banSao = ds[i]->dms;
        while (banSao) {
            if (!banSao->viTri.empty()) {                      // neu co vi tri
                // kiem tra trung lap
                bool daTonTai = false;
                for (int j = 0; j < soViTri; ++j) {
                    if (mangViTri[j] == banSao->viTri) {
                        daTonTai = true;
                        break;
                    }
                }
                
                if (!daTonTai && soViTri < MAX_DAUSACH) {      // them vi tri moi
                    mangViTri[soViTri++] = banSao->viTri;
                }
            }
            banSao = banSao->next;                             // ban sao tiep theo
        }
    }
    
    // sap xep vi tri theo thu tu alphabet
    SapXepTheLoaiTheoTen(mangViTri, soViTri);                  // tai su dung ham sap xep
    return soViTri;
}


//  NHOM 5: SAP XEP


// hoan doi hai con tro dau sach
void hoanDoiDauSach(PTRDS &a, PTRDS &b) {
    PTRDS temp = a;                                           
    a = b;                                                     
    b = temp;
}

// hoan doi ket qua tim kiem
void hoanDoiKetQua(KetQuaTimKiem &a, KetQuaTimKiem &b) {
    KetQuaTimKiem temp = a;                                    
    a = b;                                                      
    b = temp;
}

// insertion sort cho dau sach - hieu qua voi mang nho
void insertionSort(PTRDS arr[], int left, int right) {
    for (int i = left + 1; i <= right; ++i) {                  // duyet tu left+1
        PTRDS key = arr[i];                                     // lay phan tu hien tai
        if (!key) continue;                                     // bo qua null
        
        int j = i - 1;                                          // vi tri so sanh
        while (j >= left && arr[j] && arr[j]->tenSach > key->tenSach) {
            arr[j + 1] = arr[j];                                // dich phai
            --j;
        }
        arr[j + 1] = key;                                       // chen vao vi tri dung
    }
}

// partition cho quicksort (Lomuto scheme)
int partition(PTRDS arr[], int left, int right) {
    if (left < 0 || right >= MAX_DAUSACH || left > right) {   // kiem tra bounds
        return left;                                           // tra ve gia tri an toan
    }
    
    PTRDS pivot = arr[right];                                  // chon pivot = phan tu cuoi
    if (!pivot) return left;                                   // tranh loi null
    
    int i = left - 1;                                          // chi so phan tu < pivot
    for (int j = left; j < right; ++j) {                       // duyet tu left den right-1
        if (!arr[j]) continue;                                 // bo qua null
        if (arr[j]->tenSach <= pivot->tenSach) {               // nho hon hoac bang pivot
            hoanDoiDauSach(arr[++i], arr[j]);                  // doi vao phan ben trai
        }
    }
    hoanDoiDauSach(arr[i + 1], arr[right]);                    // dat pivot vao vi tri dung
    return i + 1;                                              // tra ve vi tri pivot
}

// hybrid sort: quicksort cho mang lon (>10), insertion cho mang nho
void sapXepDauSachTheoTen(PTRDS arr[], int left, int right) {
    if (left >= right || left < 0 || right >= MAX_DAUSACH) return;  // dieu kien dung
    
    try {
        const int NGUONG_CHUYEN_INSERTION = 10;                // nguong chuyen sang insertion sort
        
        if (right - left > NGUONG_CHUYEN_INSERTION) {          // mang > 10 phan tu
            int p = partition(arr, left, right);               // phan hoach O(N)
            sapXepDauSachTheoTen(arr, left, p - 1);           // de quy trai
            sapXepDauSachTheoTen(arr, p + 1, right);          // de quy phai
        } else {                                               // mang <= 10 phan tu
            insertionSort(arr, left, right);                   // insertion nhanh hon voi N nho
        }
    } catch (...) {                                            // bat loi stack overflow
        insertionSort(arr, left, right);                       // fallback an toan
    }
}

// sap xep ban sao theo ma sach (bubble sort) - N nho nen OK
void SapXepBanSaoTheoMa(PTRDMS arr[], int n) {
    if (!arr || n <= 1) return;                                // mang rong hoac 1 phan tu

    for (int i = 0; i < n - 1; ++i) {                          // duyet n-1 lan
        bool swapped = false;                                  // co doi cho trong lan nay
        
        for (int j = 0; j < n - i - 1; ++j) {                  // duyet phan tu chua sap xep
            int soA = LaySoHauToMaSach(arr[j]);                // tach so tu "ISBN-12" -> 12
            int soB = LaySoHauToMaSach(arr[j + 1]);            // tach so tu "ISBN-13" -> 13
            bool canSwap = false;                              // co can doi vi tri
            
            // logic: so hop le (>0) tang dan, so loi (-1) day xuong cuoi
            if      (soA == -1 && soB != -1)                            canSwap = true;   // a loi, b OK
            else if (soA != -1 && soB == -1)                            canSwap = false;  // a OK, b loi
            else if (soA != -1 && soB != -1 && soA > soB)               canSwap = true;   // ca 2 OK, a>b
            
            if (canSwap) {                                     // doi cho
                PTRDMS temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        
        if (!swapped) break;                                   // da xong
    }
}

// sap xep mang the loai (insertion sort) - N nho nen hieu qua
void SapXepTheLoaiTheoTen(std::string arr[], int n) {
    for (int i = 1; i < n; ++i) {                              // duyet tu 1
        std::string key = arr[i];                              // lay phan tu hien tai
        int j = i - 1;                                         // vi tri so sanh
        
        while (j >= 0 && arr[j] > key) {                       // dich phai
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;                                      // chen vao vi tri dung
    }
}

// phan hoach mang ket qua theo loai khop va ten sach
static int phanHoachKetQua(KetQuaTimKiem arr[], std::string tenThuong[], int left, int right) {
    if (left < 0 || right >= MAX_KET_QUA_TIM_KIEM || left > right) {  // kiem tra bounds
        return left;                                           // tra ve gia tri an toan
    }
    
    int loaiKhopPivot = arr[right].loaiKhop;                   // pivot = loai khop cuoi
    std::string tenPivot = tenThuong[right];                   // ten pivot
    int i = left - 1;                                          // chi so phan hoach
    
    for (int j = left; j < right; ++j) {
        bool canSwap = false;                                  // can doi vi tri
        
        // so sanh: loai khop truoc, neu bang thi so sanh ten
        if (arr[j].loaiKhop < loaiKhopPivot) {
            canSwap = true;                                    // loai khop nho hon
        } else if (arr[j].loaiKhop == loaiKhopPivot && tenThuong[j] < tenPivot) {
            canSwap = true;                                    // cung loai, ten nho hon
        }
        
        if (canSwap) {
            ++i;
            hoanDoiKetQua(arr[i], arr[j]);                     // doi ket qua
            // doi ca cache
            std::string temp = tenThuong[i];
            tenThuong[i] = tenThuong[j];
            tenThuong[j] = temp;
        }
    }
    
    hoanDoiKetQua(arr[i + 1], arr[right]);                     // dat pivot dung vi tri
    std::string temp = tenThuong[i + 1];
    tenThuong[i + 1] = tenThuong[right];
    tenThuong[right] = temp;
    
    return i + 1;                                              // tra ve vi tri pivot
}

// sap xep nhanh ket qua tim kiem - O(N log N)
static void sapXepNhanhKetQua(KetQuaTimKiem arr[], std::string tenThuong[], int left, int right) {
    if (left < right && left >= 0 && right < MAX_KET_QUA_TIM_KIEM) {
        int p = phanHoachKetQua(arr, tenThuong, left, right);        // phan hoach
        sapXepNhanhKetQua(arr, tenThuong, left, p - 1);              // de quy trai
        sapXepNhanhKetQua(arr, tenThuong, p + 1, right);             // de quy phai
    }
}

// sap xep ket qua tim kiem - quicksort O(N log N)
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n) {
    if (n <= 1) return;                                        // khong can sap xep
    
    // cache chuoi da chuyen thuong truoc khi sap xep
    std::string tenThuong[MAX_KET_QUA_TIM_KIEM];               // mang cache ten da chuyen thuong
    
    for (int i = 0; i < n; ++i) {                              // chuan bi cache
        if (arr[i].sach) {
            tenThuong[i] = ChuyenInThuong(arr[i].sach->tenSach);
        }
    }
    
    sapXepNhanhKetQua(arr, tenThuong, 0, n - 1);               // goi quicksort
}



//  NHOM 6: QUAN LY BO NHO


// giai phong danh muc sach (danh sach lien ket)
void GiaiPhongDanhMucSach(PTRDMS &dms) {
    int dem = 0;                                               // dem de bao ve vong lap
    while (dms && dem < SO_VONG_LAP_DMS_MAX) {                // duyet het danh sach
        PTRDMS temp = dms;                                     // luu node hien tai
        dms = dms->next;                                       // chuyen sang node sau
        delete temp;                                           // xoa node cu
        dem++;                                                 // tang dem
    }
    
    if (dms) {                                                 // con node sau SO_VONG_LAP_DMS_MAX
        thongBao(std::cout, "Phat hien vong lap trong DSLK! Cat chuoi de tranh leak.", LOI);
        dms = nullptr;                                         // cat chuoi, chap nhan mat vai node thay vi crash
    }
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
        if (ds[i]) {                                           // neu ton tai
            tong += ds[i]->tongBanSao;                         // cong truc tiep field
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
        
        while (p) {                                            // duyet DSLK don
            ++dem;                                             // tang dem
            p = p->next;                                       // node tiep theo
            
            // phong vong lap vo han
            if (dem > SO_VONG_LAP_DMS_MAX) {
                thongBao(std::cout, "Canh bao: Vong lap vo han DMS cho ISBN " + ds[i]->ISBN + "!", CANH_BAO);
                break;
            }
        }
        
        ds[i]->tongBanSao = dem;                              // cap nhat tong
        
        // Canh bao neu vuot qua MAX_BAN_SAO
        if (dem > MAX_BAN_SAO) {
            thongBao(std::cout, "Canh bao: Dau sach ISBN " + ds[i]->ISBN + " co " + std::to_string(dem) + " ban sao, vuot qua gioi han " + std::to_string(MAX_BAN_SAO) + "!", CANH_BAO);
        }
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
    
    while (curr) {                                             // duyet DSLK don
        if (curr->maSach == ma) {                             // tim thay ma sach
            // chi khong cho xoa sach DANG_MUON
            // con CHO_MUON_DUOC va THANH_LY thi duoc phep xoa
            if (curr->trangThai == DANG_MUON) {               // dang muon
                thongBao(out, "Khong xoa duoc sach dang muon!", LOI);
                return false;
            }
            
            if (prev)   prev->next = curr->next;              // xoa node giua/cuoi
            else        d->dms = curr->next;                  // xoa node dau
            
            delete curr;                                       // giai phong bo nho
            d->tongBanSao--;                                   // giam so luong
            duLieuDaThayDoi = true;                            // danh dau thay doi
            
            //Neu xoa het ban sao, canh bao nen xoa dau sach
            if (d->tongBanSao == 0) {
                thongBao(out, "Xoa thanh cong! Luu y: Da xoa het ban sao, nen xoa dau sach!", THONG_TIN);
            } else {
                thongBao(out, "Xoa thanh cong!", THONG_TIN);
            }
            return true;
        }
        
        prev = curr;                                           // luu node truoc
        curr = curr->next;                                     // node tiep theo
    }
    
    thongBao(out, "Khong tim thay ma sach!", LOI);
    return false;
}

// cap nhat dau sach
std::string CapNhatDauSach(PTRDS ds[], int n, const std::string &isbn, const std::string &ten, 
                          int trang, const std::string &tg, int nam, const std::string &tl, const std::string &viTri) {
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
    
    // kiem tra va chuan hoa vi tri (neu co)
    std::string viTriChuan = viTri;
    if (!viTri.empty()) {
        viTriChuan = ChuanHoaViTri(viTri);
        if (viTriChuan.empty()) {
            return "Loi: Vi tri khong hop le! Vd: A-12, B5, Ke C-3";
        }
    }

    bool tenDoi = (d->tenSach != ChuyenThanhTitleCase(ten));  // kiem tra ten doi
    d->tenSach = ChuyenThanhTitleCase(ten);                   // cap nhat ten
    d->soTrang = trang;                                        // cap nhat trang
    d->tacGia = ChuyenThanhTitleCase(tg);                     // cap nhat tac gia
    d->namXuatBan = nam;                                       // cap nhat nam
    d->theLoai = ChuyenThanhTitleCase(tl);                    // cap nhat the loai
    
    // cap nhat vi tri cho tat ca ban sao (neu co)
    if (!viTriChuan.empty()) {
        PTRDMS p = d->dms;
        while (p) {
            p->viTri = viTriChuan;                             // cap nhat vi tri
            p = p->next;
        }
    }
    
    if (tenDoi) {                                              // neu ten doi
        sapXepDauSachTheoTen(ds, 0, n - 1);                   // sap xep lai
    }
    
    duLieuDaThayDoi = true;                                    // danh dau thay doi
    return "";                                                 // thanh cong
}