#include "include/QuanLySach.h"
#include "include/ThongBao.h"
#include "include/KiemTraDuLieu.h"
#include "include/XuLyChuoi.h"
#include "../include/VietnameseUtils.h"
#include "include/DauSach.h"
#include "include/ThaoTacFile.h"
#include "include/NhapLieu.h"
#include "include/Constants.h"
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <algorithm>

PTRDAUSACH dsDauSach[MAX_DAUSACH] = { nullptr };
int soLuongDauSach = 0;
bool duLieuDaThayDoi = false;

// Khoi tao seed ngau nhien
static void khoiTaoNgauNhienMotLan() {
    static bool daKhoiTao = false;
    if (!daKhoiTao) {
        srand((unsigned int)time(nullptr));
        daKhoiTao = true;
    }
}

// Tao chuoi so ngau nhien
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

// Tinh checksum EAN13
int checksumEAN13(const std::string& s12) {
    if (s12.length() != DO_DAI_ISBN_CHECKSUM) return -1;
    int tongTichLuy = 0;
    for (size_t i = 0; i < DO_DAI_ISBN_CHECKSUM; ++i) {
        int chuSo = s12[i] - '0';
        tongTichLuy += (i % 2 == 0) ? chuSo : chuSo * 3;
    }
    int chuSoKiemTra = (10 - (tongTichLuy % 10)) % 10;
    return chuSoKiemTra;
}

// Sinh ISBN-13 tu dong
std::string SinhISBN13TuDong(std::ostream& out) {
    std::string s12 = taoChuoiSoNgauNhien(DO_DAI_ISBN_CHECKSUM);
    int check = checksumEAN13(s12);
    if (check == -1) {
        thongBao(out, "Lỗi khi sinh ISBN!", LOI);
        return "";
    }
    std::string isbn = s12 + std::to_string(check);
    if (KiemTraTrungISBN(isbn, out)) {
        thongBao(out, "ISBN đã tồn tại!", LOI);
        return "";
    }
    return isbn;
}

// Sinh ma sach
std::string sinhMaSach(const std::string& chiSo, int& soThuTu, std::ostream& out) {
    std::string ma = chiSo + "-" + std::to_string(soThuTu);
    if (timDanhMucTheoMaSach(ma, dsDauSach, soLuongDauSach, out, true)) {
        thongBao(out, "Mã sách đã tồn tại: " + ma, LOI);
        return "";
    }
    return ma;
}

// Ten trang thai sach
const char* TenTrangThai(TrangThaiSach trangThai) {
    if (trangThai == CHO_MUON_DUOC) return "Cho mượn được";
    if (trangThai == DANG_MUON) return "Đang mượn";
    if (trangThai == THANH_LY) return "Thanh lý";
    return "Không rõ";
}

// Them danh muc sach
bool themDanhMucSach(PTRDMS& dms, const std::string& maSach, TrangThaiSach trangThai, const std::string& viTri) {
    if (trangThai < 0 || trangThai > 2) {
        thongBao(std::cout, "Trạng thái sách không hợp lệ!", LOI);
        return false;
    }
    if (viTri.length() > MAX_VI_TRI_KE) {
        thongBao(std::cout, "Vị trí kệ quá dài (tối đa " + std::to_string(MAX_VI_TRI_KE) + " ký tự)!", LOI);
        return false;
    }
    PTRDMS newNode = new DanhMucSach;
    newNode->maSach = maSach;
    newNode->trangThai = trangThai;
    newNode->viTri = viTri;
    newNode->next = dms;
    dms = newNode;
    return true;
}

// Cap nhat trang thai sach
void capNhatTrangThaiSach(PTRDMS dms, const std::string& maSach, TrangThaiSach trangThai) {
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
        thongBao(std::cout, "Phát hiện vòng lặp vô hạn trong danh mục sách!", LOI);
    }
}

// Tim danh muc theo ma sach
PTRDMS timDanhMucTheoMaSach(const std::string& maSach, PTRDAUSACH ds[], int n, std::ostream& out, bool silent /*= false*/) {
    for (int i = 0; i < n; ++i) {
        if (ds[i]) {
            PTRDMS p = ds[i]->dms;
            int dem = 0;
            while (p && dem < SO_VONG_LAP_DMS_MAX) {
                if (p->maSach == maSach) return p;
                p = p->next;
                ++dem;
            }
            if (dem >= SO_VONG_LAP_DMS_MAX) {
                thongBao(out, "Phát hiện vòng lặp vô hạn trong danh mục sách!", LOI);
            }
        }
    }
    if (!silent) {
        std::string isbn = TachISBNTuMaSach(maSach);
        PTRDAUSACH foundDau = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        std::string detail = "Không tìm thấy mã sách: " + maSach + " (ISBN=" + isbn + ", dauSach=" + (foundDau ? "FOUND" : "MISSING") + ")";
        thongBao(out, detail, LOI);
    }
    return nullptr;
}

// Tim dau sach theo ISBN
PTRDAUSACH TimDauSachTheoISBN(PTRDAUSACH ds[], int n, const std::string& isbn) {
    for (int chiSo = 0; chiSo < n; ++chiSo) {
        if (ds[chiSo] && ds[chiSo]->ISBN == isbn) return ds[chiSo];
    }
    return nullptr;
}

void InMotDauSach(PTRDAUSACH d, std::ostream& out) {
    if (!d) {
        thongBao(out, "Đầu sách không tồn tại!", LOI);
        return;
    }
    out << "ISBN: " << d->ISBN << "\n";
    out << "Tên sách: " << d->tenSach << "\n";
    out << "Số trang: " << d->soTrang << "\n";
    out << "Tác giả: " << d->tacGia << "\n";
    out << "Năm xuất bản: " << d->namXuatBan << "\n";
    out << "Thể loại: " << d->theLoai << "\n";
    out << "Tổng bản sao: " << d->tongBanSao << "\n";
    out << "Danh mục sách:\n";
    PTRDMS p = d->dms;
    if (!p) {
        out << "  Chưa có bản sao.\n";
    } else {
        int dem = 0;
        while (p && dem < SO_VONG_LAP_DMS_MAX) {
            out << "  + Mã sách: " << p->maSach << ", Trạng thái: " << TenTrangThai(p->trangThai)
                << ", Vị trí: " << p->viTri << "\n";
            p = p->next;
            ++dem;
        }
        if (dem >= SO_VONG_LAP_DMS_MAX) {
            thongBao(out, "Phát hiện vòng lặp vô hạn trong danh mục sách!", LOI);
        }
    }
}

// Them dau sach moi
bool themDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach, const std::string& ISBN, const std::string& tenSach, int soTrang, const std::string& tacGia, int namXuatBan, const std::string& theLoai, bool anLang) {
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách", std::cout)) {
        return false;
    }
    if (soLuongDauSach >= MAX_DAUSACH) {
        if (!anLang) {
            thongBao(std::cout, "Đã đạt giới hạn tối đa " + std::to_string(MAX_DAUSACH) + " đầu sách!", LOI);
            thongBao(std::cout, "Không thể thêm đầu sách mới.", THONG_TIN);
        }
        return false;
    }
    
    bool coLoi = false;
    std::string chiTietLoi = "";
    
    // Kiem tra ISBN
    std::string isbnKiemTra;
    if (!KiemTraChuoiRong(ISBN, "ISBN", std::cout) || !ChuanHoaISBNFile(ISBN, isbnKiemTra, std::cout) || KiemTraTrungISBN(isbnKiemTra, std::cout)) {
        coLoi = true;
        chiTietLoi += "- ISBN không đúng định dạng hoặc đã tồn tại\n";
    }
    
    // Kiem tra ten sach
    if (!KiemTraChuoiVaDodai(tenSach, "Tên sách", MAX_TEN_SACH, std::cout)) {
        coLoi = true;
        chiTietLoi += "- Tên sách không hợp lệ\n";
    }
    
    // Kiem tra so trang
    if (soTrang <= 0 || soTrang > MAX_SO_TRANG) {
        coLoi = true;
        chiTietLoi += "- Số trang phải từ 1-" + std::to_string(MAX_SO_TRANG) + " (hiện tại: " + std::to_string(soTrang) + ")\n";
    }
    
    // Kiem tra tac gia
    if (!KiemTraChuoiVaDodai(tacGia, "Tác giả", MAX_TAC_GIA, std::cout)) {
        coLoi = true;
        chiTietLoi += "- Tác giả không hợp lệ\n";
    }
    
    // Kiem tra nam xuat ban
    if (!KiemTraNamXuatBan(namXuatBan, std::cout)) {
        coLoi = true;
        chiTietLoi += "- Năm xuất bản không hợp lệ\n";
    }
    
    // Kiem tra the loai
    if (!KiemTraChuoiVaDodai(theLoai, "Thể loại", MAX_THE_LOAI, std::cout)) {
        coLoi = true;
        chiTietLoi += "- Thể loại không hợp lệ\n";
    }
    
    // Bao cao loi neu co
    if (coLoi) {
        if (!anLang) {
            thongBao(std::cout, "=== LỖI DỮ LIỆU ĐẦU VÀO ===", LOI);
            thongBao(std::cout, "Phát hiện lỗi:\n" + chiTietLoi, LOI);
        }
        return false;
    }
    
    // Them dau sach moi
    PTRDAUSACH newBook = new DauSach;
    newBook->ISBN = isbnKiemTra;
    newBook->tenSach = VietnameseUtils::toTitleCase(tenSach);
    newBook->soTrang = soTrang;
    newBook->tacGia = VietnameseUtils::toTitleCase(tacGia);
    newBook->namXuatBan = namXuatBan;
    newBook->theLoai = VietnameseUtils::toTitleCase(theLoai);
    newBook->dms = nullptr;
    newBook->tongBanSao = 0;
    dsDauSach[soLuongDauSach++] = newBook;
    
    sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1);
    
    if (!anLang) {
        thongBao(std::cout, "Đã thêm đầu sách: " + newBook->tenSach, THONG_TIN);
    }
    if (!anLang) {
        duLieuDaThayDoi = true;
    }
    return true;
}

void inDanhSachTheoTheLoai(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách", std::cout)) {
        return;
    }
    
    if (soLuongDauSach == 0) {
        thongBao(std::cout, "Danh sách đầu sách rỗng!", THONG_TIN);
        return;
    }
    
    std::string theLoaiDaIn[MAX_DAUSACH];
    int soTheLoai = 0;
    
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (dsDauSach[i]) {
            bool daTonTai = false;
            for (int j = 0; j < soTheLoai; ++j) {
                if (theLoaiDaIn[j] == dsDauSach[i]->theLoai) {
                    daTonTai = true;
                    break;
                }
            }
            if (!daTonTai) {
                theLoaiDaIn[soTheLoai++] = dsDauSach[i]->theLoai;
            }
        }
    }
    
    // In tung the loai
    for (int i = 0; i < soTheLoai; ++i) {
        thongBao(std::cout, "Thể loại: " + theLoaiDaIn[i], THONG_TIN);
        for (int j = 0; j < soLuongDauSach; ++j) {
            if (dsDauSach[j] && dsDauSach[j]->theLoai == theLoaiDaIn[i]) {
                InMotDauSach(dsDauSach[j], std::cout);
            }
        }
    }
}

// Tim sach theo tu khoa
int timDauSach(PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& tuKhoa) {
    
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách")) {
        return -1;
    }
    
    if (soLuongDauSach == 0) {
        thongBao(std::cout, "Không có đầu sách nào để tìm kiếm.", THONG_TIN);
        return 0;
    }
    
    if (!KiemTraChuoiRong(tuKhoa, "Từ khóa tìm kiếm", std::cout) || tuKhoa.length() > 1000) {
        return -1;
    }
    
    try {
        // Chuan hoa tu khoa
    std::string tuKhoaChuanHoa = VietnameseUtils::removeAccents(tuKhoa);
    tuKhoaChuanHoa = ChuanHoaKhoangTrang(tuKhoaChuanHoa);
        
    // Mang luu ket qua
        struct KetQuaTimKiem {
            PTRDAUSACH sach;
            int loaiKhop;  // 1=ten sach, 2=the loai, 3=tac gia, 4=ISBN
        };
        
        const int MAX_KET_QUA = MAX_KET_QUA_TIM_KIEM;
        KetQuaTimKiem ketQua[MAX_KET_QUA_TIM_KIEM];
        int soKetQua = 0;
        
    // Tim kiem trong tat ca dau sach
        for (int i = 0; i < soLuongDauSach && soKetQua < MAX_KET_QUA; ++i) {
            PTRDAUSACH p = dsDauSach[i];
            if (!p || p->tenSach.empty()) continue;
            
            try {
                ThongTinSachChuanHoa sachChuanHoa = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
                
                // Uu tien ten sach truoc
                int loaiKhop = 0;  // 0=khong khop, 1=ten sach, 2=tac gia, 3=the loai, 4=ISBN

                // Uu tien 1: ten sach
                if (sachChuanHoa.tenSach.find(tuKhoaChuanHoa) != std::string::npos) {
                    loaiKhop = 1;
                }
                // Uu tien 2: tac gia
                else if (sachChuanHoa.tacGia.find(tuKhoaChuanHoa) != std::string::npos) {
                    loaiKhop = 2;
                }
                // Uu tien 3: the loai
                else if (sachChuanHoa.theLoai.find(tuKhoaChuanHoa) != std::string::npos) {
                    loaiKhop = 3;
                }
                // Uu tien 4: ISBN
                else if (sachChuanHoa.isbn.find(tuKhoaChuanHoa) != std::string::npos) {
                    loaiKhop = 4;
                }
                // Luu ket qua neu co
                if (loaiKhop > 0) {
                    ketQua[soKetQua].sach = p;
                    ketQua[soKetQua].loaiKhop = loaiKhop;
                    soKetQua++;
                }
                
            } catch (const std::exception& e) {
                continue; // Bo qua loi va tiep tuc tim kiem
            }
        }
        
    // Sap xep ket qua: uu tien loaiKhop, neu bang thi ten sach tang dan
        for (int i = 0; i < soKetQua - 1; i++) {
            for (int j = i + 1; j < soKetQua; j++) {
                bool canHoanDoi = false;
                
                // Uu tien loaiKhop nho hon
                if (ketQua[i].loaiKhop > ketQua[j].loaiKhop) {
                    canHoanDoi = true;
                } else if (ketQua[i].loaiKhop == ketQua[j].loaiKhop) {
                    // Cung uu tien thi sap xep theo ten sach A-Z
                    if (ketQua[i].sach->tenSach > ketQua[j].sach->tenSach) {
                        canHoanDoi = true;
                    }
                }
                
                if (canHoanDoi) {
                    KetQuaTimKiem temp = ketQua[i];
                    ketQua[i] = ketQua[j];
                    ketQua[j] = temp;
                }
            }
        }
        
    // Hien thi ket qua
        if (soKetQua > 0) {
            std::cout << "\nKet qua tim kiem: \"" << tuKhoa << "\" (" << soKetQua << " ket qua)\n\n";
            
            for (int i = 0; i < soKetQua; i++) {
                std::cout << (i+1) << ". ";
                InMotDauSach(ketQua[i].sach, std::cout);
            }
            
            thongBao(std::cout, "\n=== HOAN TAC: Tim thay " + std::to_string(soKetQua) + " ket qua ===", THONG_TIN);
        } else {
            std::cout << "\nKhông tìm thấy kết quả phù hợp với từ khóa: \"" << tuKhoa << "\"\n";
            std::cout << "Gợi ý: Thử từ khóa ngắn hơn hoặc kiểm tra chính tả.\n\n";
        }
        
        return soKetQua;
        
    } catch (const std::exception& e) {
        thongBao(std::cout, std::string("Lỗi trong quá trình tìm kiếm: ") + e.what(), LOI);
        return -1;
    }
}

// Sap xep dau sach theo ten
void sapXepDauSachTheoTen(PTRDAUSACH arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH || low >= high) return;
    
    try {
        if (high - low > 10) { // Chuyển sang insertion sort nếu kích thước nhỏ
            int pi = partition(arr, low, high);
            sapXepDauSachTheoTen(arr, low, pi - 1);
            sapXepDauSachTheoTen(arr, pi + 1, high);
        } else {
            insertionSort(arr, low, high);
        }
    } catch (const std::exception& e) {
        insertionSort(arr, low, high);
    }
}

// Ham phu cho quick sort
int partition(PTRDAUSACH arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH) return low;
    
    PTRDAUSACH pivot = arr[high];
    if (!pivot) return low;
    
    int i = low - 1;
    
    for (int j = low; j < high; ++j) {
        if (arr[j] && arr[j]->tenSach <= pivot->tenSach) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Insertion sort cho dau sach
void insertionSort(PTRDAUSACH arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH) return;
    
    for (int i = low + 1; i <= high; ++i) {
        PTRDAUSACH key = arr[i];
        if (!key) continue;
        
        int j = i - 1;
        while (j >= low && arr[j] && arr[j]->tenSach > key->tenSach) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// Phan tich trang thai sach
int PhanTichTrangThaiSach(const std::string& trangThai, std::ostream& out) {
    std::string tt = VietnameseUtils::trimAndNormalize(trangThai);
    if (tt == "0" || tt == "0\r") return CHO_MUON_DUOC;
    if (tt == "1" || tt == "1\r") return DANG_MUON;
    if (tt == "2" || tt == "2\r") return THANH_LY;
    if (tt == "cho muon duoc" || tt == "cho mượn được" || tt == "cho muon được") return CHO_MUON_DUOC;
    if (tt == "dang muon" || tt == "đang mượn") return DANG_MUON;
    if (tt == "thanh ly" || tt == "thanh lý") return THANH_LY;
    thongBao(out, "Trạng thái sách không hợp lệ: " + tt, LOI);
    return -1;
}

// Giai phong danh muc sach
void GiaiPhongDanhMucSach(PTRDMS& dms) {
    while (dms) {
        PTRDMS temp = dms;
        dms = dms->next;
        delete temp;
    }
    dms = nullptr;
}

// Giai phong dau sach
void GiaiPhongDauSach(PTRDAUSACH& dauSach) {
    if (dauSach) {
        GiaiPhongDanhMucSach(dauSach->dms);
        delete dauSach;
        dauSach = nullptr;
    }
}

// Giai phong toan bo dau sach
void GiaiPhongToanBoDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach) {
    if (!dsDauSach) return;
    for (int i = 0; i < soLuongDauSach; ++i) {
        GiaiPhongDauSach(dsDauSach[i]);
    }
    soLuongDauSach = 0;
}

int DemTongSoBanSao(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    int tong = 0;
    if (!dsDauSach) return 0;
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
                thongBao(std::cout, "Phát hiện vòng lặp vô hạn trong danh mục sách của đầu sách: " + dsDauSach[i]->ISBN, LOI);
            }
        }
    }
    return tong;
}

bool ChenNodeDMSVaoDauSach(PTRDAUSACH d, const std::string& maSach, int tt, const std::string& viTri) {
    if (!d) return false;
    if (tt < 0 || tt > 2) return false;

    PTRDMS node = new DanhMucSach;
    node->maSach = maSach;
    node->trangThai = static_cast<TrangThaiSach>(tt);
    node->viTri = viTri;
    node->next = d->dms;
    d->dms = node;
    return true;
}

void CapNhatTongBanSao(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    if (!dsDauSach) return;
    for (int i = 0; i < soLuongDauSach; ++i) {
        if (!dsDauSach[i]) continue;
        int dem = 0;
        PTRDMS p = dsDauSach[i]->dms;
        int vong = 0;
        while (p && vong < SO_VONG_LAP_DMS_MAX) {
            ++dem;
            p = p->next;
            ++vong;
        }
        if (vong >= SO_VONG_LAP_DMS_MAX) {
            thongBao(std::cout, "Phát hiện vòng lặp vô hạn khi cập nhật tổng bản sao: " + dsDauSach[i]->ISBN, LOI);
        }
        dsDauSach[i]->tongBanSao = dem;
    }
}