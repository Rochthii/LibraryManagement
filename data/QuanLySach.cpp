#include "QuanLySach.h"
#include "ThongBao.h"
#include "KiemTraDuLieu.h"
#include "XuLyChuoi.h"
#include "DauSach.h"
#include "ThaoTacFile.h"
#include "NhapLieu.h"
#include "Constants.h"
#include <iomanip>
#include <ctime>
#include <stdexcept>

// bien toan cuc

PTRDS dsDauSach[MAX_DAUSACH] = {nullptr};
int soLuongDauSach = 0;
bool duLieuDaThayDoi = false;

//////khong dung nua
// nhom 1: ham tien ich noi bo (static)
// tao chuoi so ngau nhien
/* // Ham nay hien khong duoc su dung, tam thoi vo hieu hoa
static std::string taoChuoiSoNgauNhien(size_t doDai) {
    khoiTaoNgauNhienMotLan();
    std::string ketQua;
    ketQua.reserve(doDai);
    for (size_t i = 0; i < doDai; ++i) {
        char soNgauNhien = '0' + rand() % 10;
        ketQua.push_back(soNgauNhien);
    }
    return ketQua;
}
*/
/*
static std::string taoChuoiSoNgauNhien(size_t doDai){
   // khoiTaoNgauNhienMotLan();
    std::string ketQua;
    ketQua.reserve(doDai);
    for (size_t i = 0; i < doDai; ++i){
        char soNgauNhien = '0' + rand() % 10;
        ketQua.push_back(soNgauNhien);
    }
    return ketQua;
} */

// tinh chu so kiem tra cho ma isbn 13 theo chuan ean13
int checksumEAN13(const std::string &s12) {
    if (s12.length() != DO_DAI_ISBN_CHECKSUM)
        return -1;
    int tongTichLuy = 0;
    for (size_t i = 0; i < DO_DAI_ISBN_CHECKSUM; ++i) {
        int chuSo = s12[i] - '0';
        tongTichLuy += (i % 2 == 0) ? chuSo : chuSo * 3;
    }
    int chuSoKiemTra = (10 - (tongTichLuy % 10)) % 10;
    return chuSoKiemTra;
}

// nhom 2: sinh ma & tien ich (public)

// sinh tu dong mot ma isbn 13  (khong dung nua)
/*std::string SinhISBN13TuDong(std::ostream &out){
    std::string s12 = taoChuoiSoNgauNhien(DO_DAI_ISBN_CHECKSUM);
    int check = checksumEAN13(s12);
    if (check == -1){
        thongBao(out, "Loi khi sinh ISBN!", LOI);
        return "";
    }
    std::string isbn = s12 + std::to_string(check);
    if (KiemTraTrungISBN(isbn, out)){
        thongBao(out, "ISBN da ton tai!", LOI);
        return "";
    }
    return isbn;
} */

std::string sinhMaSach(const std::string &chiSo, int &soThuTu) {
    std::string ma = chiSo + "-" + std::to_string(soThuTu);
    std::stringstream dummyStream; // Tao stream ao de truyen vao ham tim (khong dung toi)
    if (timDanhMucTheoMaSach(ma, dsDauSach, soLuongDauSach, dummyStream, true) != nullptr) {
        // Neu tim thay (ma da ton tai) -> Tra ve chuoi loi
        return "Loi: Ma sach da ton tai: " + ma;
    }

    // Neu khong tim thay -> Tra ve ma sach vua tao (thanh cong)
    return ma;
}
const char *TenTrangThai(TrangThaiSach trangThai) {
    if (trangThai == CHO_MUON_DUOC)
        return "Cho muon duoc";
    if (trangThai == DANG_MUON)
        return "Dang muon";
    if (trangThai == THANH_LY)
        return "Thanh ly";
    return "Khong ro";
}

int PhanTichTrangThaiSach(const std::string &trangThai, std::ostream &out) {
    std::string tt = ChuyenInThuong(CatKhoangTrang(trangThai)); //
    if (tt == "0" || tt == "0\r")
        return CHO_MUON_DUOC;
    if (tt == "1" || tt == "1\r")
        return DANG_MUON;
    if (tt == "2" || tt == "2\r")
        return THANH_LY;
    if (tt == "cho muon duoc")
        return CHO_MUON_DUOC;
    if (tt == "dang muon")
        return DANG_MUON;
    if (tt == "thanh ly")
        return THANH_LY;
    thongBao(out, "Trang thai sach khong hop le: " + tt, LOI);
    return -1;
}

// nhom 3: quan ly danh muc sach (dms - dslk)

std::string themDanhMucSach(PTRDMS &dms, const std::string &maSach, TrangThaiSach trangThai, const std::string &viTri) {
    if (trangThai < 0 || trangThai > 2) {
        return "Loi: Trang thai sach khong hop le!";
    }
    if (viTri.length() > MAX_VI_TRI_KE) {
        return "Loi: Vi tri ke qua dai (toi da " + std::to_string(MAX_VI_TRI_KE) + " ky tu)!";
    }
    PTRDMS newNode = new DanhMucSach;
    newNode->maSach = maSach;
    newNode->trangThai = trangThai;
    newNode->viTri = viTri;
    newNode->next = dms;
    dms = newNode;
    return "";
}

bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &maSach, int tt, const std::string &viTri) {
    if (!d)
        return false;
    if (tt < 0 || tt > 2)
        return false;

    PTRDMS node = new DanhMucSach;
    node->maSach = maSach;
    node->trangThai = static_cast<TrangThaiSach>(tt);
    node->viTri = viTri;
    node->next = d->dms;
    d->dms = node;
    return true;
}

void capNhatTrangThaiSach(PTRDMS dms, const std::string &maSach, TrangThaiSach trangThai) {
    PTRDMS p = dms;
    int dem = 0;
    while (p && dem < SO_VONG_LAP_DMS_MAX) {
        if (p->maSach == maSach) {
            p->trangThai = trangThai;
            return;
        }
        p = p->next;
        ++dem;
    }
    if (dem >= SO_VONG_LAP_DMS_MAX) {
        thongBao(std::cout, "Phat hien vong lap vo han trong danh muc sach!", LOI);
    }
}

// tim node ban sao sach theo ma, can canh bao neu vong lap qua lon de tranh loi vong lap vo han
PTRDMS timDanhMucTheoMaSach(const std::string &maSach, PTRDS ds[], int n, std::ostream &out, bool silent /*= false*/) {
    for (int i = 0; i < n; ++i) {
        if (ds[i]) {
            PTRDMS p = ds[i]->dms;
            int dem = 0;
            while (p && dem < SO_VONG_LAP_DMS_MAX) {
                if (p->maSach == maSach)
                    return p;
                p = p->next;
                ++dem;
            }
            if (dem >= SO_VONG_LAP_DMS_MAX) {
                thongBao(out, "Phat hien vong lap vo han trong danh muc sach!", LOI);
            }
        }
    }
    if (!silent) {
        // neu khong tim thay thi tach isbn tu ma sach va bao loi ro rang
        std::string isbn = TachISBNTuMaSach(maSach);
        PTRDS foundDau = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        std::string detail = "Khong tim thay ma sach: " + maSach + " (ISBN=" + isbn + ", dauSach=" + (foundDau ? "FOUND" : "MISSING") + ")";
        thongBao(out, detail, LOI);
    }
    return nullptr;
}

// nhom 4: quan ly dau sach (dstt - mang con tro)

PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn) {
    for (int chiSo = 0; chiSo < n; ++chiSo) {
        if (ds[chiSo] && ds[chiSo]->ISBN == isbn)
            return ds[chiSo];
    }
    return nullptr;
}

void InMotDauSach(PTRDS d, std::ostream &out) {
    if (!d) {
        thongBao(out, "Dau sach khong ton tai!", LOI);
        return;
    }
    out << "ISBN: " << d->ISBN << "\n";
    out << "Ten sach: " << d->tenSach << "\n";
    out << "So trang: " << d->soTrang << "\n";
    out << "Tac gia: " << d->tacGia << "\n";
    out << "Nam xuat ban: " << d->namXuatBan << "\n";
    out << "The loai: " << d->theLoai << "\n";
    out << "Tong ban sao: " << d->tongBanSao << "\n";
    out << "Danh muc sach:\n";
    PTRDMS p = d->dms;
    if (!p) {
        out << "  Chua co ban sao.\n";
    }
    else {
        int dem = 0;
        while (p && dem < SO_VONG_LAP_DMS_MAX) {
            out << "  + Ma sach: " << p->maSach << ", Trang thai: " << TenTrangThai(p->trangThai)
                << ", Vi tri: " << p->viTri << "\n";
            p = p->next;
            ++dem;
        }
        if (dem >= SO_VONG_LAP_DMS_MAX) {
            thongBao(out, "Phat hien vong lap vo han trong danh muc sach!", LOI);
        }
    }
    std::cout << std::endl;
}

// them mot dau sach moi vao mang, kiem tra va chuan hoa du lieu truoc khi them

bool themDauSach(PTRDS dsDauSach[], int &soLuongDauSach, const std::string &ISBN, const std::string &tenSach, int soTrang, const std::string &tacGia, int namXuatBan, const std::string &theLoai, bool anLang) {

    // kiem tra gioi han so luong dau sach
    if (soLuongDauSach >= MAX_DAUSACH) {
        if (!anLang) {
            thongBao(std::cout, "Da dat gioi han toi da " + std::to_string(MAX_DAUSACH) + " dau sach!", LOI);
        }
        return false;
    }

    std::string loi;
    std::string isbnChuan;

    //  B1: Kiem tra ISBN 
    loi = KiemTraChuoiRong(ISBN, "ISBN"); //
    if (!loi.empty()) { // Kiem tra neu chuoi loi KHONG rong
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    // goi ham chuanhoaisbnfile moi (tra ve string loi)
    loi = ChuanHoaISBNFile(ISBN, isbnChuan); //
    if (!loi.empty()) {
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    // goi ham kiemtratrungisbn moi (tra ve string loi)
    loi = KiemTraTrungISBN(isbnChuan); //
    if (!loi.empty()) {
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    //  B2: Kiem tra Ten Sach 
    loi = KiemTraChuoiVaDodai(tenSach, "Ten sach", MAX_TEN_SACH); //
    if (!loi.empty()) { 
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    //  B3: Kiem tra So Trang 
    if (soTrang <= 0 || soTrang > MAX_SO_TRANG) {
        loi = "So trang phai tu 1-" + std::to_string(MAX_SO_TRANG) + "!";
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    //  B4: Kiem tra Tac Gia 
    loi = KiemTraChuoiVaDodai(tacGia, "Tac gia", MAX_TAC_GIA); //
    if (!loi.empty()) {
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    //  B5: Kiem tra Nam Xuat Ban 
    loi = KiemTraNamXuatBan(namXuatBan); //
    if (!loi.empty()) {
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    //  B6: Kiem tra The Loai 
    loi = KiemTraChuoiVaDodai(theLoai, "The loai", MAX_THE_LOAI); //
    if (!loi.empty()) {
        if (!anLang)
            thongBao(std::cout, loi, LOI);
        return false;
    }

    //  B7: Tat ca hop le -> Tao Dau Sach moi 
    PTRDS newBook = new DauSach;
    newBook->ISBN = isbnChuan;
    newBook->tenSach = ChuyenThanhTitleCase(tenSach); //
    newBook->soTrang = soTrang;
    newBook->tacGia = ChuyenThanhTitleCase(tacGia); //
    newBook->namXuatBan = namXuatBan;
    newBook->theLoai = ChuyenThanhTitleCase(theLoai); //
    newBook->dms = nullptr;
    newBook->tongBanSao = 0;

    dsDauSach[soLuongDauSach++] = newBook;

    sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1); //

    if (!anLang) {
        thongBao(std::cout, "Da them dau sach: " + newBook->tenSach, THONG_TIN);
        duLieuDaThayDoi = true;
    }
    return true;
}

// nhom 5: ham sap xep

void hoanDoiDauSach(PTRDS &a, PTRDS &b) {
    PTRDS temp = a;
    a = b;
    b = temp;
}

void insertionSort(PTRDS arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH)
        return;
    for (int i = low + 1; i <= high; ++i) {
        PTRDS key = arr[i];
        if (!key)
            continue;
        int j = i - 1;
        while (j >= low && arr[j] && arr[j]->tenSach > key->tenSach) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

int partition(PTRDS arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH)
        return low;
    PTRDS pivot = arr[high];
    if (!pivot)
        return low;
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (arr[j] && arr[j]->tenSach <= pivot->tenSach) {
            ++i;
            hoanDoiDauSach(arr[i], arr[j]);
        }
    }
    hoanDoiDauSach(arr[i + 1], arr[high]);
    return i + 1;
}


// Ham sap xep mang con tro Ban Sao (PTRDMS) bang Insertion Sort
// Sap xep theo so hau to cua Ma Sach tang dan
void SapXepBanSaoTheoMa(PTRDMS arr[], int n) {
    if (!arr || n <= 1) return; // khong can sap xep

    PTRDMS key; // node hien tai dang xet
    int i, j;
    int soCuoiKey, soCuoiJ; // so hau to

    for (i = 1; i < n; i++) {
        key = arr[i];          // lay node thu i
        soCuoiKey = LaySoHauToMaSach(key); // lay so hau to cua no
        j = i - 1;           // bat dau so sanh voi node ben trai no

        // tiep tuc dich phai KHI j van hop le VA arr[j] "lon hon" key
        while (j >= 0) {
            soCuoiJ = LaySoHauToMaSach(arr[j]); // lay so hau to cua node j

            // dieu kien de DICH PHAI phan tu arr[j]
            bool shouldShift = false;
            if (soCuoiKey == -1) {
                // Neu key khong hop le (-1), no luon nho hon hoac bang moi node khac
                // => Khong bao gio can dich phai node j => break
                break;
            } else if (soCuoiJ == -1) {
                // Neu j khong hop le (-1) ma key hop le => j nho hon key
                // => Khong can dich phai node j => break
                 break; // Sua: Khong can dich phai khi j la -1
            } else if (soCuoiJ > soCuoiKey) {
                // Ca hai deu hop le va j > key => Can dich phai node j
                shouldShift = true;
            } else {
                 // Truong hop soCuoiJ <= soCuoiKey (ca hai hop le)
                 // => Khong can dich phai node j => break
                 break;
            }

            // Thuc hien dich phai neu can
            if (shouldShift) {
                 arr[j + 1] = arr[j];
                 j = j - 1;
            }
            // else da break roi
        } // ket thuc while

        arr[j + 1] = key; // chen key vao vi tri dung cua no
    } // ket thuc for
}

void SapXepTheLoaiTheoTen(std::string arr[], int n) { // Da xoa static
    if (!arr || n <= 1)
        return; // Khong can sap xep

    std::string key; // The loai hien tai dang xet
    int i, j;

    for (i = 1; i < n; i++) {
        key = arr[i]; // Lay the loai thu i
        j = i - 1;    // Bat dau so sanh voi ben trai

        // Dich chuyen cac phan tu arr[0..i-1] lon hon key sang phai
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j]; // Dich phan tu j sang phai
            j = j - 1;           // Lui j ve trai
        }
        arr[j + 1] = key; // Chen key vao vi tri dung
    }
}
// sap xep mang dau sach theo ten, ket hop quicksort va insertion sort de toi uu
void sapXepDauSachTheoTen(PTRDS arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH || low >= high)
        return;
    try {
        if (high - low > 10) {
            int pi = partition(arr, low, high);
            sapXepDauSachTheoTen(arr, low, pi - 1);
            sapXepDauSachTheoTen(arr, pi + 1, high);
        }
        else {
            insertionSort(arr, low, high);
        }
    }
    catch (const std::exception &e) {
        insertionSort(arr, low, high);
    }
}

void hoanDoiKetQua(KetQuaTimKiem &a, KetQuaTimKiem &b) {
    KetQuaTimKiem temp = a;
    a = b;
    b = temp;
}

void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            bool canHoanDoi = false;
            if (arr[i].loaiKhop > arr[j].loaiKhop) {
                canHoanDoi = true;
            }
            else if (arr[i].loaiKhop == arr[j].loaiKhop) {
                if (arr[i].sach->tenSach > arr[j].sach->tenSach) {
                    canHoanDoi = true;
                }
            }
            if (canHoanDoi) {
                hoanDoiKetQua(arr[i], arr[j]);
            }
        }
    }
}

// nhom 6: logic tim kiem & liet ke

// tim kiem sach theo tu khoa, uu tien theo ten, tac gia, the loai, isbn
int timKiemLogic(PTRDS dsDauSach[], int soLuongDauSach, const std::string &tuKhoa,
                 KetQuaTimKiem ketQua[], int maxKetQua) {

    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "DS dau sach", std::cout))
        return 0;
    if (tuKhoa.empty())
        return 0;

    // chuan hoa tu khoa de so sanh khong phan biet dau va viet hoa
    std::string tuKhoaChuanHoa = BoDauVaThuong(tuKhoa);
    tuKhoaChuanHoa = ChuanHoaKhoangTrang(tuKhoaChuanHoa);
    int soKetQua = 0;

    for (int i = 0; i < soLuongDauSach && soKetQua < maxKetQua; ++i) {
        PTRDS p = dsDauSach[i];
        if (!p || p->tenSach.empty())
            continue;

        // chuan hoa du lieu sach de so sanh
        ThongTinSachChuanHoa sachChuanHoa = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
        int loaiKhop = 0;

        if (sachChuanHoa.tenSach.find(tuKhoaChuanHoa) != std::string::npos)
            loaiKhop = 1;
        else if (sachChuanHoa.tacGia.find(tuKhoaChuanHoa) != std::string::npos)
            loaiKhop = 2;
        else if (sachChuanHoa.theLoai.find(tuKhoaChuanHoa) != std::string::npos)
            loaiKhop = 3;
        else if (sachChuanHoa.isbn.find(tuKhoaChuanHoa) != std::string::npos)
            loaiKhop = 4;
        if (loaiKhop > 0) {
            ketQua[soKetQua].sach = p;
            ketQua[soKetQua].loaiKhop = loaiKhop;
            soKetQua++;
        }
    }

    // sap xep ket qua theo do uu tien va ten sach
    sapXepKetQuaTimKiem(ketQua, soKetQua);
    return soKetQua;
}

int TimTheLoaiDuyNhat(PTRDS dsDauSach[], int soLuongDauSach, std::string mangTheLoai[], int maxTheLoai) {
    int soTheLoai = 0;
    for (int i = 0; i < soLuongDauSach && soTheLoai < maxTheLoai; ++i) {
        if (dsDauSach[i]) {
            bool daTonTai = false;
            for (int j = 0; j < soTheLoai; ++j) {
                if (mangTheLoai[j] == dsDauSach[i]->theLoai) {
                    daTonTai = true;
                    break;
                }
            }
            if (!daTonTai) {
                mangTheLoai[soTheLoai++] = dsDauSach[i]->theLoai;
            }
        }
    }
    return soTheLoai;
}

int TimSachTheoTheLoai(PTRDS dsDauSach[], int soLuongDauSach, const std::string &theLoai, PTRDS ketQua[], int maxSach) {
    int soSach = 0;
    for (int j = 0; j < soLuongDauSach && soSach < maxSach; ++j) {
        if (dsDauSach[j] && dsDauSach[j]->theLoai == theLoai) {
            ketQua[soSach++] = dsDauSach[j];
        }
    }
    return soSach;
}

// Ham Console (Goc - Da Refactor)   ///KHONG DUNG VI IN THONG BAO RA MAN HINH CONSOLE

int timDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string &tuKhoa) {

    KetQuaTimKiem ketQua[MAX_KET_QUA_TIM_KIEM];
    int soKetQua = timKiemLogic(dsDauSach, soLuongDauSach, tuKhoa, ketQua, MAX_KET_QUA_TIM_KIEM);

    if (soKetQua > 0) {
        std::cout << "\n Ket qua tim kiem: \"" << tuKhoa << "\" (" << soKetQua << " ket qua)\n\n";
        for (int i = 0; i < soKetQua; i++) {
            std::cout << (i + 1) << ". ";
            InMotDauSach(ketQua[i].sach, std::cout);
        }
        thongBao(std::cout, "\n HOAN TAT ", THONG_TIN);
    }
    else {
        std::cout << "\nKhong tim thay ket qua phu hop voi tu khoa: \"" << tuKhoa << "\"\n";
    }
    return soKetQua;
}

void inDanhSachTheoTheLoai(PTRDS dsDauSach[], int soLuongDauSach) {
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sach dau sach", std::cout)) {
        return;
    }
    if (soLuongDauSach == 0) {
        thongBao(std::cout, "Danh sach dau sach rong!", THONG_TIN);
        return;
    }

    std::string theLoaiDaIn[MAX_DAUSACH];
    int soTheLoai = TimTheLoaiDuyNhat(dsDauSach, soLuongDauSach, theLoaiDaIn, MAX_DAUSACH);

    for (int i = 0; i < soTheLoai; ++i) {
        thongBao(std::cout, "The loai: " + theLoaiDaIn[i], THONG_TIN);
        std::cout << std::endl;

        PTRDS sachCungTheLoai[MAX_DAUSACH];
        int soSach = TimSachTheoTheLoai(dsDauSach, soLuongDauSach, theLoaiDaIn[i], sachCungTheLoai, MAX_DAUSACH);

        for (int j = 0; j < soSach; ++j) {
            InMotDauSach(sachCungTheLoai[j], std::cout);
        }
        std::cout << std::endl;
    }
}

// Nhom 7: Quan Ly Bo Nho

void GiaiPhongDanhMucSach(PTRDMS &dms) {
    while (dms) {
        PTRDMS temp = dms;
        dms = dms->next;
        delete temp;
    }
    dms = nullptr;
}

void GiaiPhongDauSach(PTRDS &dauSach) {
    if (dauSach) {
        GiaiPhongDanhMucSach(dauSach->dms);
        delete dauSach;
        dauSach = nullptr;
    }
}

void GiaiPhongToanBoDauSach(PTRDS dsDauSach[], int &soLuongDauSach) {
    if (!dsDauSach)
        return;
    for (int i = 0; i < soLuongDauSach; ++i) {
        GiaiPhongDauSach(dsDauSach[i]);
    }
    soLuongDauSach = 0;
}

int DemTongSoBanSao(PTRDS dsDauSach[], int soLuongDauSach) {
    int tong = 0;
    if (!dsDauSach)
        return 0;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) {
            PTRDMS dms = dsDauSach[i]->dms;
            int dem = 0;
            while (dms && dem < SO_VONG_LAP_DMS_MAX) {
                ++tong;
                dms = dms->next;
                ++dem;
            }
            if (dem >= SO_VONG_LAP_DMS_MAX) {
                thongBao(std::cout, "Phat hien vong lap vo han trong danh muc sach cua dau sach: " + dsDauSach[i]->ISBN, LOI);
            }
        }
    }
    return tong;
}

void CapNhatTongBanSao(PTRDS dsDauSach[], int soLuongDauSach) {
    if (!dsDauSach)
        return;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (!dsDauSach[i])
            continue;
        int dem = 0;
        PTRDMS p = dsDauSach[i]->dms;
        int vong = 0;
        while (p && vong < SO_VONG_LAP_DMS_MAX) {
            ++dem;
            p = p->next;
            ++vong;
        }
        if (vong >= SO_VONG_LAP_DMS_MAX) {
            thongBao(std::cout, "Phat hien vong lap vo han khi cap nhat tong ban sao: " + dsDauSach[i]->ISBN, LOI);
        }
        dsDauSach[i]->tongBanSao = dem;
    }
}

// nhom 8: xoa sach

// xoa mot dau sach theo isbn, chi xoa khi khong con ban sao nao
std::string XoaDauSachTheoISBN(PTRDS dsDauSach[], int &soLuongDauSach, const std::string &isbn) {
    int indexToDelete = -1;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i] && dsDauSach[i]->ISBN == isbn) {
            indexToDelete = i;
            break;
        }
    }
    if (indexToDelete == -1) {
        // thongBao(out, "Loi: Khong tim thay dau sach voi ISBN: " + isbn, LOI);
        return "Loi: Khong tim thay dau sach voi ISBN: " + isbn;
    }
    if (dsDauSach[indexToDelete]->dms != nullptr) {
        // thongBao(out, "Loi: Khong the xoa dau sach '" + dsDauSach[indexToDelete]->tenSach + "' vi van con ban sao.", LOI);
        return "Loi: Khong the xoa dau sach '" + dsDauSach[indexToDelete]->tenSach + "' vi van con ban sao.";
    }

    // thongBao(out, "Dang xoa dau sach: " + dsDauSach[indexToDelete]->tenSach, THONG_TIN); // Co the bo qua thong tin nay
    PTRDS dauSachCanXoa = dsDauSach[indexToDelete];
    GiaiPhongDauSach(dauSachCanXoa);
    for (int i = indexToDelete; i < soLuongDauSach - 1; ++i) {
        dsDauSach[i] = dsDauSach[i + 1];
    }
    dsDauSach[soLuongDauSach - 1] = nullptr;
    soLuongDauSach--;
    duLieuDaThayDoi = true;
    // thongBao(out, "Xoa dau sach thanh cong!", THONG_TIN);
    return ""; // Thanh cong
}

// xoa mot ban sao sach theo ma sach, chi xoa neu trang thai la cho muon duoc
bool XoaSachTheoMaSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string &maSach, std::ostream &out) {
    // tach isbn tu ma sach de tim dau sach cha
    std::string isbn = TachISBNTuMaSach(maSach);
    if (isbn.empty()) {
        thongBao(out, "Loi: Ma sach khong hop le: " + maSach, LOI);
        return false;
    }
    // tim dau sach cha
    PTRDS dauSachCha = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!dauSachCha) {
        thongBao(out, "Loi: Khong tim thay dau sach tuong ung voi ma sach: " + maSach, LOI);
        return false;
    }

    // duyet danh sach lien ket de tim va xoa node ban sao
    PTRDMS current = dauSachCha->dms;
    PTRDMS prev = nullptr;
    int demVong = 0;

    while (current && demVong < SO_VONG_LAP_DMS_MAX) {
        if (current->maSach == maSach) {
            if (current->trangThai != CHO_MUON_DUOC) { // chi xoa neu trang thai la cho muon duoc
                thongBao(out, "Loi: Khong the xoa sach dang duoc muon hoac da thanh ly!", LOI);
                return false;
            }

            thongBao(out, "Dang xoa ban sao: " + maSach, THONG_TIN);

            if (prev == nullptr) {
                dauSachCha->dms = current->next;
            }
            else {
                prev->next = current->next;
            }

            delete current;
            dauSachCha->tongBanSao--;
            duLieuDaThayDoi = true;
            thongBao(out, "Xoa ban sao thanh cong!", THONG_TIN);
            return true;
        }

        prev = current;
        current = current->next;
        demVong++;
    }

    if (demVong >= SO_VONG_LAP_DMS_MAX) {
        thongBao(out, "Loi: Phat hien vong lap vo han trong DMS khi tim ma sach: " + maSach, LOI);
        return false;
    }

    thongBao(out, "Loi: Khong tim thay ban sao voi ma sach: " + maSach, LOI);
    return false;
}

std::string CapNhatDauSach(
    PTRDS dsDauSach[], int soLuongDauSach, const std::string &isbn,
    const std::string &tenSachMoi, int soTrangMoi, const std::string &tacGiaMoi,
    int namXuatBanMoi, const std::string &theLoaiMoi) {
    //  B1: Tim kiem dau sach 
    PTRDS sachCanSua = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    if (!sachCanSua) {
        return "Loi: Khong tim thay dau sach voi ISBN '" + isbn + "' de cap nhat.";
    }

    //  B2: Kiem tra du lieu dau vao moi 
    std::string loi;

    // Kiem tra Ten sach
    loi = KiemTraChuoiVaDodai(tenSachMoi, "Ten sach", MAX_TEN_SACH);
    if (!loi.empty()) {
        return loi;
    }

    // Kiem tra So trang
    if (soTrangMoi <= 0 || soTrangMoi > MAX_SO_TRANG) {
        return "Loi: So trang phai tu 1-" + std::to_string(MAX_SO_TRANG) + "!";
    }

    // Kiem tra Tac gia
    loi = KiemTraChuoiVaDodai(tacGiaMoi, "Tac gia", MAX_TAC_GIA);
    if (!loi.empty()) {
        return loi;
    }

    // Kiem tra Nam xuat ban
    loi = KiemTraNamXuatBan(namXuatBanMoi);
    if (!loi.empty()) {
        return loi;
    }

    // Kiem tra The loai
    loi = KiemTraChuoiVaDodai(theLoaiMoi, "The loai", MAX_THE_LOAI);
    if (!loi.empty()) {
        return loi;
    }

    // B3: Cap nhat du lieu
    bool tenSachThayDoi = (sachCanSua->tenSach != ChuyenThanhTitleCase(tenSachMoi));

    sachCanSua->tenSach = ChuyenThanhTitleCase(tenSachMoi);
    sachCanSua->soTrang = soTrangMoi;
    sachCanSua->tacGia = ChuyenThanhTitleCase(tacGiaMoi);
    sachCanSua->namXuatBan = namXuatBanMoi;
    sachCanSua->theLoai = ChuyenThanhTitleCase(theLoaiMoi);

    //  B4: Sap xep lai neu ten thay doi 
    // Vi mang luon phai duoc sap xep theo ten
    if (tenSachThayDoi) {
        sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1);
    }

    //  B5: Danh dau thay doi va tra ve thanh cong 
    duLieuDaThayDoi = true;
    return ""; // Tra ve chuoi rong la thanh cong
}


