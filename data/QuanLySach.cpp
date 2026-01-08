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
#include <sstream>

// Global variables removed - now passed as parameters

// 1. Xu ly ma sach va trang thai

std::string sinhMaSach(const std::string &chiSo, int &soThuTu, PTRDS dsDauSach[], int soLuongDauSach) {
    std::string ma = chiSo + "-" + std::to_string(soThuTu);
    std::stringstream dummy;

    if (timDanhMucTheoMaSach(ma, dsDauSach, soLuongDauSach, dummy, true)) { // check trung ma trc khi sinh
        return "Loi: Ma sach da ton tai: " + ma;
    }
    return ma;
}

const char* TenTrangThai(TrangThaiSach tt) {
    if (tt == CHO_MUON_DUOC) return "Cho muon duoc";
    if (tt == DANG_MUON)     return "Dang muon";
    if (tt == THANH_LY)      return "Thanh ly";
    return "Khong ro";
}

int PhanTichTrangThaiSach(const std::string &s, std::ostream &out) {
    std::string tt = ChuyenInThuong(CatKhoangTrang(s));
    
    if (tt == "0" || tt == "0\r" || tt == "cho muon duoc") return CHO_MUON_DUOC;
    if (tt == "1" || tt == "1\r" || tt == "dang muon")     return DANG_MUON;
    if (tt == "2" || tt == "2\r" || tt == "thanh ly")      return THANH_LY;

    thongBao(out, "Trang thai khong hop le: " + tt, LOI);
    return -1;
}


// 2. Quan ly Danh Muc Sach (DSLK)

std::string themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt) {
    if (tt < 0 || tt > 2)                return "Loi: Trang thai khong hop le!";
    if (vt.length() > MAX_VI_TRI_KE)     return "Loi: Vi tri qua dai!";

    PTRDMS node = new DanhMucSach{ma, tt, vt, dms};
    dms = node;  // chen vao dau (O(1))
    return "";
}

// tim max O(N)
int TimSoThuTuLonNhat(PTRDS dauSach) {
    if (!dauSach || !dauSach->dms) return 0;

    int soLonNhat = 0;
    PTRDMS pTemp = dauSach->dms;
    int dem = 0;

    while (pTemp && dem < MAX_BAN_SAO) { // tim max so hau to de sinh ma moi ko trung
        int soHienTai = LaySoHauToMaSach(pTemp);
        if (soHienTai > soLonNhat) {
            soLonNhat = soHienTai;
        }
        pTemp = pTemp->next;
        dem++;
    }
    return soLonNhat;
}

std::string ThemNhieuBanSao(PTRDS dauSach, int soLuong, const std::string &viTri, PTRDS dsDauSach[], int soLuongDauSach, bool &duLieuDaThayDoi) {
    if (!dauSach) return "Loi: Dau sach khong ton tai!";
    if (soLuong <= 0) return "Loi: So luong phai lon hon 0!";
    if (soLuong > MAX_BAN_SAO) return "Loi: So luong khong hop le!";

    if (dauSach->tongBanSao + soLuong > MAX_BAN_SAO) {
        return "Loi: Tong so ban sao se vuot qua gioi han!";
    }

    std::string viTriChuan = viTri;
    if (!viTri.empty()) {
        viTriChuan = ChuanHoaViTri(viTri);
        if (viTriChuan.empty()) return "Loi: Vi tri khong hop le!";
    } else {
        if (dauSach->dms && !dauSach->dms->viTri.empty()) {
            viTriChuan = dauSach->dms->viTri; // inherit vi tri cu neu ko nhap
        }
    }

    int soThuTu = TimSoThuTuLonNhat(dauSach) + 1;
    if (soThuTu > INT_MAX - soLuong) return "Loi: So thu tu qua lon!";

    int themThanhCong = 0;
    std::string loiTrongLoop = "";

    for (int i = 0; i < soLuong; ++i) { // sinh nhieu ban sao 1 luc
        std::string maSach = dauSach->ISBN + "-" + std::to_string(soThuTu);

        std::stringstream dummy;
        int baoVe = 0;
        while (timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, dummy, true)) { // skip ma trung, +1 cho den khi ko trung
            if (++baoVe > MAX_BAN_SAO) { // fail-safe tranh loop vo tan
                loiTrongLoop = "Loi: Khong sinh duoc ma duy nhat!";
                break;
            }
            ++soThuTu;
            maSach = dauSach->ISBN + "-" + std::to_string(soThuTu);
        }

        if (!loiTrongLoop.empty()) break;

        std::string loiThem = themDanhMucSach(dauSach->dms, maSach, CHO_MUON_DUOC, viTriChuan);
        if (!loiThem.empty()) {
            loiTrongLoop = loiThem;
            break;
        }

        themThanhCong++;
        soThuTu++;
    }

    dauSach->tongBanSao += themThanhCong;
    duLieuDaThayDoi = true;

    if (!loiTrongLoop.empty()) {
        if (themThanhCong > 0) return "Chi them duoc mot phan. Loi: " + loiTrongLoop;
        return loiTrongLoop;
    }
    return "";
}

bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &ma, int tt, const std::string &vt) {
    if (!d || tt < 0 || tt > 2) return false;

    PTRDMS node = new DanhMucSach{ma, static_cast<TrangThaiSach>(tt), vt, d->dms};
    d->dms = node; // chen dau O(1), dung cho load file nhanh
    return true;
}

bool capNhatTrangThaiSach(PTRDMS dms, const std::string &ma, TrangThaiSach tt) {
    while (dms) {
        if (dms->maSach == ma) {
            dms->trangThai = tt;
            return true;
        }
        dms = dms->next;
    }
    return false;
}

// tim kiem O(N) + duyet DSLK
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent) {
    std::string isbn = TachISBNTuMaSach(ma);
    if (isbn.empty()) return nullptr;

    PTRDS dauSach = TimDauSachTheoISBN(ds, n, isbn); // O(N) tim ds truoc
    if (!dauSach) {
        if (!silent) thongBao(out, "Khong tim thay dau sach (ISBN=" + isbn + ")", LOI);
        return nullptr;
    }

    PTRDMS p = dauSach->dms;
    while (p) { // duyet dslk tim ma, dung cho check trung
        if (p->maSach == ma) return p;
        p = p->next;
    }

    if (!silent) thongBao(out, "Khong tim thay ma sach: " + ma, LOI);
    return nullptr;
}


// 3. Quan ly Dau Sach

bool themDauSach(PTRDS dsDauSach[], int &soLuongDauSach, const std::string &isbn, const std::string &ten, int trang,
const std::string &tg, int nam, const std::string &tl, bool anLang, bool &duLieuDaThayDoi) {

    if (soLuongDauSach >= MAX_DAUSACH) return false;

    std::string loi, isbnChuan;

    if (!KiemTraChuoiRong(isbn, "ISBN").empty() ||
    !(loi = ChuanHoaISBNFile(isbn, isbnChuan)).empty() ||
    !(loi = KiemTraTrungISBN(dsDauSach, soLuongDauSach, isbnChuan)).empty()) {
        if (!anLang) thongBao(std::cout, loi, LOI);
        return false;
    }

    if (!(loi = KiemTraChuoiVaDodai(ten, "Ten sach", MAX_TEN_SACH)).empty() ||
    trang <= 0 || trang > MAX_SO_TRANG ||
    !(loi = KiemTraChuoiVaDodai(tg, "Tac gia", MAX_TAC_GIA)).empty() ||
    !(loi = KiemTraNamXuatBan(nam)).empty() ||
    !(loi = KiemTraChuoiVaDodai(tl, "The loai", MAX_THE_LOAI)).empty()) {
        if (!anLang) thongBao(std::cout, loi, LOI);
        return false;
    }

    PTRDS node = new DauSach;
    node->ISBN = isbnChuan;
    node->tenSach = ChuyenThanhTitleCase(ten);
    node->soTrang = trang;
    node->tacGia = ChuyenThanhTitleCase(tg);
    node->namXuatBan = nam;
    node->theLoai = ChuyenThanhTitleCase(tl);
    node->dms = nullptr;
    node->tongBanSao = 0;
    node->soLuotMuon = 0;

    dsDauSach[soLuongDauSach++] = node;

    sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1); // sxep luon O(NlogN), de tim nhanh sau
    duLieuDaThayDoi = true;

    if (!anLang) thongBao(std::cout, "Them thanh cong: " + node->tenSach, THONG_TIN);
    return true;
}

// tim kiem tuyen tinh O(N)
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn) {
    for (int i = 0; i < n; ++i) {
        if (ds[i] && ds[i]->ISBN == isbn) {
            return ds[i];
        }
    }
    return nullptr;
}


// 4. Tim kiem & Thong ke

// tim kiem tuyen tinh O(N)
int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[]) {
    if (tk.empty() || n == 0) return 0;

    std::string tuKhoaChuan = ChuanHoaChuoiTimKiem(tk);
    int soKetQua = 0;

    for (int i = 0; i < n; ++i) { // O(N) duyet het ds
        PTRDS p = ds[i];
        if (!p) continue;

        auto duLieuChuan = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
        int loaiKhop = 0;
        // uu tien: ten > tg > tl > isbn
        if      (duLieuChuan.tenSach.find(tuKhoaChuan) != std::string::npos)  loaiKhop = 1; 
        else if (duLieuChuan.tacGia.find(tuKhoaChuan) != std::string::npos)   loaiKhop = 2; 
        else if (duLieuChuan.theLoai.find(tuKhoaChuan) != std::string::npos)  loaiKhop = 3; 
        else if (duLieuChuan.isbn.find(tuKhoaChuan) != std::string::npos)     loaiKhop = 4; 

        if (loaiKhop > 0 && soKetQua < MAX_KET_QUA_TIM_KIEM) {
            kq[soKetQua++] = {p, loaiKhop};
        }

        if (soKetQua >= MAX_KET_QUA_TIM_KIEM) break; 
    }

    sapXepKetQuaTimKiem(kq, soKetQua); // xep theo do khop + ten A-Z
    return soKetQua;
}

int DemSoLuongBanSaoTheoTrangThai(PTRDS dauSach, TrangThaiSach trangThai) {
    if (!dauSach || !dauSach->dms) return 0;

    int dem = 0;
    PTRDMS pTemp = dauSach->dms;
    int baoVe = 0;

    while (pTemp && baoVe < MAX_BAN_SAO) {
        if (pTemp->trangThai == trangThai) {
            dem++;
        }
        pTemp = pTemp->next;
        baoVe++;
    }
    return dem;
}

ThongTinDauSach LayThongTinDauSach(PTRDS dauSach) {
    ThongTinDauSach thongTin = {0, 0, 0, 0};

    if (!dauSach || !dauSach->dms) return thongTin;

    PTRDMS pTemp = dauSach->dms;
    int baoVe = 0;

    while (pTemp && baoVe < MAX_BAN_SAO) {
        thongTin.tongBanSao++;

        if (pTemp->trangThai == CHO_MUON_DUOC) {
            thongTin.choMuonDuoc++;
        } else if (pTemp->trangThai == DANG_MUON) {
            thongTin.dangMuon++;
        } else if (pTemp->trangThai == THANH_LY) {
            thongTin.thanhLy++;
        }
        pTemp = pTemp->next;
        baoVe++;
    }
    return thongTin;
}

// tim + loai trung O(N + NlogN)
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string mangTheLoai[]) {
    if (n == 0) return 0;

    int soTheLoai = 0;

    for (int i = 0; i < n; ++i) {
        if (!ds[i]) continue;
        if (soTheLoai < MAX_DAUSACH) {
            mangTheLoai[soTheLoai++] = ds[i]->theLoai;
        }
    }

    if (soTheLoai == 0) return 0;

    SapXepTheLoaiTheoTen(mangTheLoai, soTheLoai);

    int theLoaiDuyNhat = 0;
    for (int i = 0; i < soTheLoai; ++i) {
        if (i == 0 || mangTheLoai[i] != mangTheLoai[i - 1]) { // loai trung lap sau khi sort
            mangTheLoai[theLoaiDuyNhat++] = mangTheLoai[i];
        }
    }
    return theLoaiDuyNhat;
}

// tim kiem tuyen tinh O(N)
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &theLoaiCanTim, PTRDS mangKetQua[]) {
    int soKetQua = 0;

    for (int i = 0; i < n; ++i) {
        if (ds[i] && ds[i]->theLoai == theLoaiCanTim && soKetQua < MAX_DAUSACH) {
            mangKetQua[soKetQua++] = ds[i];
        }
    }
    return soKetQua;
}

// tim + loai trung O(N*M + NlogN) voi M la so ban sao
int TimViTriDuyNhat(PTRDS ds[], int n, std::string mangViTri[]) {
    if (n == 0) return 0;

    int soViTri = 0;
    for (int i = 0; i < n; ++i) {
        if (!ds[i]) continue;
        PTRDMS banSao = ds[i]->dms;
        while (banSao) {
            if (!banSao->viTri.empty() && soViTri < MAX_DAUSACH) {
                mangViTri[soViTri++] = banSao->viTri;
            }
            banSao = banSao->next;
        }
    }

    if (soViTri == 0) return 0;

    SapXepTheLoaiTheoTen(mangViTri, soViTri);

    int viTriDuyNhat = 0;
    for (int i = 0; i < soViTri; ++i) {
        if (i == 0 || mangViTri[i] != mangViTri[i - 1]) {
            mangViTri[viTriDuyNhat++] = mangViTri[i];
        }
    }
    return viTriDuyNhat;
}


// 5. Thuat toan Sap xep

void hoanDoi(PTRDS &a, PTRDS &b) {
    PTRDS temp = a;
    a = b;
    b = temp;
}

// quicksort O(NlogN)
void sapXepDauSachTheoTen(PTRDS arr[], int left, int right) {
    if (left >= right) return;  // tranh crash khi mang rong
    int i = left, j = right;
    std::string pivot = arr[(left + right) / 2]->tenSach; // pivot giua de balance

    while (i <= j) {
        while (arr[i]->tenSach < pivot) i++;
        while (arr[j]->tenSach > pivot) j--;

        if (i <= j) {
            hoanDoi(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    if (left < j)  sapXepDauSachTheoTen(arr, left, j);
    if (i < right) sapXepDauSachTheoTen(arr, i, right);
}

// insertion sort O(N^2)
void SapXepBanSaoTheoMa(PTRDMS arr[], int n) {
    if (!arr || n <= 1) return;
    for (int i = 1; i < n; ++i) {
        PTRDMS key = arr[i];
        int soKey = LaySoHauToMaSach(key);
        int j = i - 1;
        
        while (j >= 0) {
            int soJ = LaySoHauToMaSach(arr[j]);
            
            bool canSwap = false;
            if (soKey != -1) { // so hop le uu tien hon so loi (-1)
                if (soJ == -1 || soJ > soKey) {
                    canSwap = true;
                }
            }

            if (!canSwap) break;
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// insertion sort O(N^2)
void SapXepTheLoaiTheoTen(std::string arr[], int n) {
    for (int i = 1; i < n; ++i) {
        std::string key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// insertion sort O(N^2)
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n) {

    if (n <= 1) return;

    std::string tenThuong[MAX_KET_QUA_TIM_KIEM]; // cache ten thuong de so sanh nhanh
    for (int i = 0; i < n; ++i) {
        if (arr[i].sach) {
            tenThuong[i] = ChuyenInThuong(arr[i].sach->tenSach);
        }
    }

    for (int i = 1; i < n; ++i) {
        KetQuaTimKiem keyItem = arr[i];
        std::string keyName = tenThuong[i];
        int j = i - 1;

        while (j >= 0) {
            bool canSwap = false;
            
            if (arr[j].loaiKhop > keyItem.loaiKhop) { // uu tien 1: do khop thap hon len trc
                canSwap = true;
            }
            else if (arr[j].loaiKhop == keyItem.loaiKhop && tenThuong[j] > keyName) { // uu tien 2: ten A-Z
                canSwap = true;
            }

            if (canSwap) {
                arr[j + 1] = arr[j];
                tenThuong[j + 1] = tenThuong[j]; 
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = keyItem;
        tenThuong[j + 1] = keyName;
    }
}


// 6. Quan ly Bo nho

void GiaiPhongDanhMucSach(PTRDMS &dms) {
    int dem = 0;
    while (dms && dem < SO_VONG_LAP_DMS_MAX) { // giai phong dslk, co guard tranh loop inf
        PTRDMS temp = dms;
        dms = dms->next;
        delete temp;
        dem++;
    }

    if (dms) { //  force null de tranh leak
        thongBao(std::cout, "Loi: Phat hien vong lap!", LOI);
        dms = nullptr;
    }
}

void GiaiPhongDauSach(PTRDS &d) {
    if (d) {
        GiaiPhongDanhMucSach(d->dms);
        delete d;
        d = nullptr;
    }
}

void GiaiPhongToanBoDauSach(PTRDS ds[], int &n) {
    for (int i = 0; i < n; ++i) {
        GiaiPhongDauSach(ds[i]);
    }
    n = 0;
}

int DemTongSoBanSao(PTRDS ds[], int n) {
    int tong = 0;
    for (int i = 0; i < n; ++i) {
        if (ds[i]) {
            tong += ds[i]->tongBanSao;
        }
    }
    return tong;
}

void CapNhatTongBanSao(PTRDS ds[], int n) {
    for (int i = 0; i < n; ++i) {
        if (!ds[i]) continue;

        int dem = 0;
        PTRDMS p = ds[i]->dms;

        while (p) { // dem bsao trong dslk, co guard tranh loop inf
            ++dem;
            p = p->next;

            if (dem > SO_VONG_LAP_DMS_MAX) { // fail-safe
                break;
            }
        }

        ds[i]->tongBanSao = dem;
    }
}


// 7. Xoa & Cap nhat

std::string XoaDauSach(PTRDS dsDauSach[], int &soLuongDauSach, const std::string &isbn, bool &duLieuDaThayDoi) {
    int idx = -1;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i] && dsDauSach[i]->ISBN == isbn) {
            idx = i;
            break;
        }
    }

    if (idx == -1)      return "Khong tim thay ISBN";
    if (dsDauSach[idx]->dms)   return "Loi: Con ban sao!";

    GiaiPhongDauSach(dsDauSach[idx]);

    for (int i = idx; i < soLuongDauSach - 1; ++i) {
        dsDauSach[i] = dsDauSach[i + 1];
    }

    dsDauSach[--soLuongDauSach] = nullptr;
    duLieuDaThayDoi = true;
    return "";
}

bool XoaSachTheoMaSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string &ma, std::ostream &out, bool &duLieuDaThayDoi) {
    std::string isbn = TachISBNTuMaSach(ma);
    if (isbn.empty()) return false;

    PTRDS d = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!d) return false;

    PTRDMS curr = d->dms;
    PTRDMS prev = nullptr;

    while (curr) { // duyet dslk, tim ma de xoa
        if (curr->maSach == ma) {
            if (curr->trangThai == DANG_MUON) {
                thongBao(out, "Dang muon, khong xoa duoc!", LOI);
                return false;
            }

            if (prev) prev->next = curr->next; // xoa giua/cuoi
            else d->dms = curr->next; // xoa dau

            delete curr;
            d->tongBanSao--;
            duLieuDaThayDoi = true;

            thongBao(out, "Xoa thanh cong!", THONG_TIN);
            return true;
        }

        prev = curr;
        curr = curr->next;
    }

    thongBao(out, "Khong tim thay ma sach!", LOI);
    return false;
}

std::string CapNhatDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string &isbn, const std::string &ten, 
int trang, const std::string &tg, int nam, const std::string &tl, const std::string &viTri, bool &duLieuDaThayDoi) {
    PTRDS d = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!d) return "Loi: Khong tim thay ISBN!";

    std::string loi;

    if (!(loi = KiemTraChuoiVaDodai(ten, "Ten sach", MAX_TEN_SACH)).empty() ||
    trang <= 0 || trang > MAX_SO_TRANG ||
    !(loi = KiemTraChuoiVaDodai(tg, "Tac gia", MAX_TAC_GIA)).empty() ||
    !(loi = KiemTraNamXuatBan(nam)).empty() ||
    !(loi = KiemTraChuoiVaDodai(tl, "The loai", MAX_THE_LOAI)).empty()) {
        return loi;
    }

    std::string viTriChuan = viTri;
    if (!viTri.empty()) {
        viTriChuan = ChuanHoaViTri(viTri);
        if (viTriChuan.empty()) return "Loi: Vi tri khong hop le!";
    }

    bool tenDoi = (d->tenSach != ChuyenThanhTitleCase(ten)); // check ten doi thi phai sort lai
    
    d->tenSach = ChuyenThanhTitleCase(ten);
    d->soTrang = trang;
    d->tacGia = ChuyenThanhTitleCase(tg);
    d->namXuatBan = nam;
    d->theLoai = ChuyenThanhTitleCase(tl);

    if (!viTriChuan.empty()) {
        PTRDMS p = d->dms;
        while (p) {
            p->viTri = viTriChuan;
            p = p->next;
        }
    }

    if (tenDoi) { // ten doi thi sort lai de giu thu tu
        sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1);
    }

    duLieuDaThayDoi = true;
    return "";
}

void CapNhatViTri(PTRDS dsDauSach[], int soLuongDauSach, const std::string &isbn, const std::string &viTri, bool &duLieuDaThayDoi) {
    PTRDS d = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (d && !viTri.empty()) {
        PTRDMS p = d->dms;
        while (p) {
            p->viTri = viTri;
            p = p->next;
        }
        duLieuDaThayDoi = true;
    }
}


// 8. Cac ham ho tro UI

void LayDanhSachTimKiem(PTRDS dsDauSach[], int soLuongDauSach, const std::string& tuKhoa, KetQuaTimKiem* ketQua, int& soLuongKetQua) {
    std::string tuKhoaChuan = ChuanHoaChuoiTimKiem(tuKhoa);
    
    if (tuKhoaChuan.empty()) {
        soLuongKetQua = 0;
        for (int i = 0; i < soLuongDauSach; ++i) {
            if (soLuongKetQua < MAX_KET_QUA_TIM_KIEM) {
                ketQua[soLuongKetQua++] = {dsDauSach[i], 0}; 
            }
        }
    } else {
        soLuongKetQua = timKiemLogic(dsDauSach, soLuongDauSach, tuKhoaChuan, ketQua);
    }
}

void LayDanhSachTheoTheLoai(PTRDS dsDauSach[], int soLuongDauSach, std::string* cacTheLoai, int& soTheLoai) {
    if (!cacTheLoai) {
        soTheLoai = 0;
        return;
    }
    soTheLoai = TimTheLoaiDuyNhat(dsDauSach, soLuongDauSach, cacTheLoai);
}

void LayDanhSachViTri(PTRDS dsDauSach[], int soLuongDauSach, std::string* cacViTri, int& soViTri) {
    if (!cacViTri) {
        soViTri = 0;
        return;
    }
    soViTri = TimViTriDuyNhat(dsDauSach, soLuongDauSach, cacViTri);
}

int LayDanhSachSachTheoTheLoai(PTRDS dsDauSach[], int soLuongDauSach, const std::string& theLoai, PTRDS* ketQua, int maxKetQua) {
    if (!ketQua || maxKetQua <= 0) return 0;
    return TimSachTheoTheLoai(dsDauSach, soLuongDauSach, theLoai, ketQua);
}

PTRDS TimDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn) {
    return TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
}

ThongTinDauSach LayThongTinDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn) {
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach) {
        return {0, 0, 0, 0};
    }
    return LayThongTinDauSach(dauSach);
}


// 9. Ham thao tac nguoi dung

std::string ThemDauSachMoi(PTRDS dsDauSach[], int &soLuongDauSach, const std::string& isbn, const std::string& ten, int trang,
const std::string& tg, int nam, const std::string& tl, 
int soLuongBanSao, const std::string& viTri, bool &duLieuDaThayDoi) {
    
    if (soLuongBanSao <= 0) return "Loi: So luong phai > 0";
    if (soLuongBanSao > MAX_BAN_SAO) return "Loi: So luong qua nhieu";

    bool ok = themDauSach(dsDauSach, soLuongDauSach, isbn, ten, trang, tg, nam, tl, true, duLieuDaThayDoi);
    if (!ok) return "Loi: Khong them duoc dau sach!";

    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach) return "Loi: Khong tim thay";

    std::string viTriCanDung = viTri;
    if (viTriCanDung.empty() && dauSach->dms && !dauSach->dms->viTri.empty()) {
        viTriCanDung = dauSach->dms->viTri;
    }

    std::string loiThem = ThemNhieuBanSao(dauSach, soLuongBanSao, viTriCanDung, dsDauSach, soLuongDauSach, duLieuDaThayDoi);
    if (!loiThem.empty()) return loiThem;

    duLieuDaThayDoi = true;

    return "";
}

std::string ThemBanSaoMoi(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn, int soLuong, const std::string& viTri, bool &duLieuDaThayDoi) {
    if (soLuong <= 0) return "Loi: So luong phai > 0";
    if (soLuong > MAX_BAN_SAO) return "Loi: So luong qua nhieu";

    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach) return "Loi: Khong tim thay dau sach";

    std::string viTriCanDung = viTri;
    if (viTriCanDung.empty() && dauSach->dms && !dauSach->dms->viTri.empty()) {
        viTriCanDung = dauSach->dms->viTri;
    }

    std::string loiThem = ThemNhieuBanSao(dauSach, soLuong, viTriCanDung, dsDauSach, soLuongDauSach, duLieuDaThayDoi);
    if (!loiThem.empty()) return loiThem;

    duLieuDaThayDoi = true;
    return "";
}

std::string CapNhatThongTinDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn, const std::string& ten, int trang,
const std::string& tg, int nam, const std::string& tl, 
const std::string& viTri, bool &duLieuDaThayDoi) {
    
    std::string loi = CapNhatDauSach(dsDauSach, soLuongDauSach, isbn, ten, trang, tg, nam, tl, viTri, duLieuDaThayDoi);

    if (loi.empty()) {
        duLieuDaThayDoi = true;
    }

    return loi;
}

std::string XoaDauSachTheoISBN(PTRDS dsDauSach[], int &soLuongDauSach, const std::string& isbn, bool &duLieuDaThayDoi) {
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach) return "Loi: Khong tim thay";

    if (dauSach->dms != nullptr) {
        return "Loi: Con ban sao, hay xoa ban sao truoc.";
    }

   std::string loi = XoaDauSach(dsDauSach, soLuongDauSach, isbn, duLieuDaThayDoi);

    if (loi.empty()) {
        duLieuDaThayDoi = true;
    }

    return loi;
}

std::string ThanhLyBanSaoTheoMa(PTRDS dsDauSach[], int soLuongDauSach, const std::string& maSach, bool &duLieuDaThayDoi) {
    std::string isbn = TachISBNTuMaSach(maSach);
    if (isbn.empty()) return "Loi: Ma sach loi";

    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach) return "Loi: Khong tim thay dau sach";

    bool thanhCong = capNhatTrangThaiSach(dauSach->dms, maSach, THANH_LY);
    if (!thanhCong) return "Loi: Khong tim thay ban sao!";

    duLieuDaThayDoi = true;
    return "";
}

std::string XoaBanSaoTheoMa(PTRDS dsDauSach[], int soLuongDauSach, const std::string& maSach, bool &duLieuDaThayDoi) {
    std::stringstream dummy;
    PTRDMS banSao = timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, dummy, true);

    if (!banSao) return "Loi: Khong tim thay ban sao!";

    if (banSao->trangThai == DANG_MUON) {
        return "Loi: Sach dang muon, khong xoa duoc";
    }

    bool thanhCong = XoaSachTheoMaSach(dsDauSach, soLuongDauSach, maSach, dummy, duLieuDaThayDoi);
    if (!thanhCong) return "Loi: Khong xoa duoc!";

    duLieuDaThayDoi = true;

    return "";
}


// 10. Tien ich kha

void DanhDauDuLieuThayDoi(bool &duLieuDaThayDoi) {
    duLieuDaThayDoi = true;
}

int LayDanhSachBanSaoSapXep(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn, PTRDMS* mangKetQua, int maxKetQua) {
    if (!mangKetQua || maxKetQua <= 0) return 0;

    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach) return 0;

    int count = 0;
    PTRDMS p = dauSach->dms;
    
    while (p != nullptr && count < maxKetQua) {
        mangKetQua[count++] = p;
        p = p->next;
    }

    SapXepBanSaoTheoMa(mangKetQua, count);

    return count;
}

std::string LayViTriDauTienCuaDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn) {
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSach || !dauSach->dms) return "";
    return dauSach->dms->viTri;
}

bool CoTheBanSao(PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn) {
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    return (dauSach && dauSach->dms != nullptr);
}
