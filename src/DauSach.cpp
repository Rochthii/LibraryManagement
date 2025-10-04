// Chuc nang dau sach: them, in theo the loai, tim kiem, doc/ghi file.
// Dung InputUtils de kiem tra du lieu, chuan hoa chuoi va doc/ghi dung chung.
#include <iostream>
#include <algorithm>
#include "../include/DauSach.h"
#include "../include/InputUtils.h"

using namespace std;

// Mảng con trỏ tĩnh cho đầu sách
PTRDAUSACH dsDauSach[MAX_DAUSACH] = { nullptr };
int soLuongDauSach = 0;

// Biến theo dõi thay đổi dữ liệu
bool duLieuDaThayDoi = false;

// Them dau sach moi vao mang, kiem tra hop le, sap xep theo ten tang dan
bool themDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach, const string& ISBN, const string& tenSach, int soTrang, const string& tacGia, int namXuatBan, const string& theLoai, bool anLang) {
    using namespace InputUtils;
    
    // Kiem tra tham so dau vao
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách")) {
        return false;
    }
    
    // Kiem tra gioi han he thong
    if (soLuongDauSach >= MAX_DAUSACH) {
        thongBao(cout, "=== LỖI HỆ THỐNG ===", LOI);
        thongBao(cout, "Đã đạt giới hạn tối đa " + to_string(MAX_DAUSACH) + " đầu sách!", LOI);
        thongBao(cout, "Không thể thêm đầu sách mới.", THONG_TIN);
        return false;
    }
    
    // Validation du lieu dau vao
    bool coLoi = false;
    string chiTietLoi = "";
    
    // Kiem tra ISBN
    string isbnKiemTra;
    if (!KiemTraChuoiRong(ISBN, "ISBN") || !ChuanHoaISBNFile(ISBN, isbnKiemTra, cout)) {
        coLoi = true;
        chiTietLoi += "- ISBN không đúng định dạng\n";
    }
    
    // Kiem tra ten sach
    if (!KiemTraChuoiVaDodai(tenSach, "Tên sách", 500)) {
        coLoi = true;
        chiTietLoi += "- Tên sách không hợp lệ\n";
    }
    
    // Kiem tra so trang
    if (soTrang <= 0 || soTrang > 50000) {
        coLoi = true;
        chiTietLoi += "- Số trang phải từ 1-50,000 (hiện tại: " + to_string(soTrang) + ")\n";
    }
    
    // Kiem tra tac gia
    if (!KiemTraChuoiVaDodai(tacGia, "Tác giả", 300)) {
        coLoi = true;
        chiTietLoi += "- Tác giả không hợp lệ\n";
    }
    
    // Kiem tra nam xuat ban
    if (!KiemTraNamXuatBan(namXuatBan)) {
        coLoi = true;
        chiTietLoi += "- Năm xuất bản không hợp lệ\n";
    }
    
    // Kiem tra the loai
    if (!KiemTraChuoiVaDodai(theLoai, "Thể loại", 200)) {
        coLoi = true;
        chiTietLoi += "- Thể loại không hợp lệ\n";
    }
    
    // Bao cao loi tong hop neu co
    if (coLoi) {
        thongBao(cout, "\n=== LỖI DỮ LIỆU ĐẦU VÀO ===", LOI);
        thongBao(cout, "Phát hiện lỗi:", LOI);
        cout << chiTietLoi;
        thongBao(cout, "Vui lòng kiểm tra và nhập lại thông tin chính xác.", THONG_TIN);
        return false;
    }
    
    // Kiem tra trung lap
    PTRDAUSACH dauSachTonTai = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN);
    if (dauSachTonTai) {
        thongBao(cout, "\n=== LỖI TRÙNG LẶP ===", LOI);
        thongBao(cout, "ISBN " + ISBN + " đã tồn tại trong hệ thống!", LOI);
        thongBao(cout, "Thông tin sách hiện có:", THONG_TIN);
        InMotDauSach(dauSachTonTai);
        return false;
    }
    
    // Chuan hoa du lieu
    string tenSachSauChuanHoa, tacGiaSauChuanHoa, theLoaiSauChuanHoa;
    
    try {
        if (!ChuanHoaTenUnicode(tenSach, 1, 500, cout, tenSachSauChuanHoa) ||
            !ChuanHoaTenUnicode(tacGia, 1, 300, cout, tacGiaSauChuanHoa) ||
            !ChuanHoaTenUnicode(theLoai, 1, 200, cout, theLoaiSauChuanHoa)) {
            thongBao(cout, "Lỗi chuẩn hóa dữ liệu!", LOI);
            return false;
        }
    } catch (const exception& e) {
        thongBao(cout, "Lỗi trong quá trình chuẩn hóa dữ liệu: " + string(e.what()), LOI);
        return false;
    }
    
    // Tao va chen dau sach
    PTRDAUSACH dauSachMoi = TaoDauSachMoi(ISBN, tenSachSauChuanHoa, soTrang, tacGiaSauChuanHoa, namXuatBan, theLoaiSauChuanHoa, cout);
    if (!dauSachMoi) {
        return false;
    }
    
    // Tim vi tri chen
    int viTriChen = TimViTriChenDauSach(dsDauSach, soLuongDauSach, tenSachSauChuanHoa, cout);
    if (viTriChen < 0) {
        delete dauSachMoi;
        return false;
    }
    
    // Chen dau sach vao vi tri tim duoc
    if (!ChenDauSachVaoViTri(dsDauSach, soLuongDauSach, dauSachMoi, viTriChen, cout)) {
        delete dauSachMoi;
        return false;
    }
    
    // Xac nhan thanh cong
    if (!anLang) {
        thongBao(cout, "\n=== THÊM THÀNH CÔNG ===", THONG_TIN);
        thongBao(cout, "Đã thêm đầu sách vào vị trí " + to_string(viTriChen + 1) + "/" + to_string(soLuongDauSach), THONG_TIN);
        thongBao(cout, "Tên: " + tenSachSauChuanHoa, THONG_TIN);
        thongBao(cout, "Tác giả: " + tacGiaSauChuanHoa, THONG_TIN);
        thongBao(cout, "ISBN: " + ISBN, THONG_TIN);
    }
    
    return true;
}

// In danh sach dau sach theo tung the loai, sap xep ten sach tang dan
void inDanhSachTheoTheLoai(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    using namespace InputUtils;
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách")) {
        return;
    }
    
    if (soLuongDauSach == 0) {
        thongBao(cout, "Không có đầu sách nào để hiển thị.", THONG_TIN);
        return;
    }
    
    string theLoaiList[MAX_DAUSACH];
    int soTheLoai = 0;
    
    for (int i = 0; i < soLuongDauSach; ++i) {
        PTRDAUSACH p = dsDauSach[i];
        if (!p || p->theLoai.empty()) continue;
        
        if (soTheLoai >= MAX_DAUSACH) {
            thongBao(cout, "Quá nhiều thể loại khác nhau!", CANH_BAO);
            break;
        }
        
        bool daTonTai = false;
        for (int j = 0; j < soTheLoai; ++j) {
            if (theLoaiList[j] == p->theLoai) {
                daTonTai = true;
                break;
            }
        }
        
        if (!daTonTai) {
            theLoaiList[soTheLoai] = p->theLoai;
            soTheLoai++;
        }
    }
    
    for (int i = 0; i < soTheLoai - 1; ++i) {
        for (int j = i + 1; j < soTheLoai; ++j) {
            if (theLoaiList[i] > theLoaiList[j]) {
                string temp = theLoaiList[i];
                theLoaiList[i] = theLoaiList[j];
                theLoaiList[j] = temp;
            }
        }
    }
    
    for (int tl = 0; tl < soTheLoai; ++tl) {
        cout << "\n===== Thể loại: " << theLoaiList[tl] << " =====\n";
        
        PTRDAUSACH sachTheLoai[MAX_DAUSACH];
        int soSachTheLoai = 0;
        
        for (int i = 0; i < soLuongDauSach; ++i) {
            PTRDAUSACH s = dsDauSach[i];
            if (s && s->theLoai == theLoaiList[tl]) {
                sachTheLoai[soSachTheLoai] = s;
                soSachTheLoai++;
            }
        }
        
        if (soSachTheLoai > MAX_DAUSACH) {
            thongBao(cout, "Quá nhiều sách trong thể loại!", CANH_BAO);
            soSachTheLoai = MAX_DAUSACH;
        }
        
        if (soSachTheLoai > 1) {
            sapXepDauSachTheoTen(sachTheLoai, 0, soSachTheLoai - 1);
        }
        
        for (int i = 0; i < soSachTheLoai; ++i) {
            InMotDauSach(sachTheLoai[i]);
        }
    }
}

// Tim dau sach 
int timDauSach(PTRDAUSACH dsDauSach[], int soLuongDauSach, const string& tuKhoa) {
    using namespace InputUtils;
    
    // Kiểm tra tính hợp lệ của đầu vào
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách")) {
        return -1;
    }
    
    if (soLuongDauSach == 0) {
        thongBao(cout, "Không có đầu sách nào để tìm kiếm.", THONG_TIN);
        return 0;
    }
    
    if (!KiemTraChuoiRong(tuKhoa, "Từ khóa tìm kiếm") || tuKhoa.length() > 1000) {
        return -1;
    }
    
    try {
        // 1. Chuan hoa tu khoa tim kiem
        string tuKhoaChuanHoa = BoDauVaThuong(tuKhoa);
        tuKhoaChuanHoa = ChuanHoaKhoangTrang(tuKhoaChuanHoa);
        
        // Mang luu ket qua tim kiem
        struct KetQuaTimKiem {
            PTRDAUSACH sach;
            int loaiKhop;  // 1=ten sach, 2=the loai, 3=tac gia, 4=ISBN
        };
        
        const int MAX_KET_QUA = 100;
        KetQuaTimKiem ketQua[MAX_KET_QUA];
        int soKetQua = 0;
        
        // 2. Tim kiem trong tat ca dau sach
        for (int i = 0; i < soLuongDauSach && soKetQua < MAX_KET_QUA; ++i) {
            PTRDAUSACH p = dsDauSach[i];
            if (!p || p->tenSach.empty()) continue;
            
            try {
                ThongTinSachChuanHoa sachChuanHoa = ChuanHoaDuLieuSach(p->tenSach, p->tacGia, p->theLoai, p->ISBN);
                
                // Uu tien ten sach truoc
                int loaiKhop = 0;  // 0=khong khop, 1=ten sach, 2=the loai, 3=tac gia, 4=ISBN
                
                // Uu tien cao nhat: Ten sach
                if (sachChuanHoa.tenSach.find(tuKhoaChuanHoa) != string::npos) {
                    loaiKhop = 1;
                }
                // Uu tien thu 2: The loai
                else if (sachChuanHoa.theLoai.find(tuKhoaChuanHoa) != string::npos) {
                    loaiKhop = 2;
                }
                // Uu tien thu 3: Tac gia
                else if (sachChuanHoa.tacGia.find(tuKhoaChuanHoa) != string::npos) {
                    loaiKhop = 3;
                }
                // Uu tien thap nhat: ISBN
                else if (sachChuanHoa.isbn.find(tuKhoaChuanHoa) != string::npos) {
                    loaiKhop = 4;
                }
                
                // Luu ket qua neu co khop
                if (loaiKhop > 0) {
                    ketQua[soKetQua].sach = p;
                    ketQua[soKetQua].loaiKhop = loaiKhop;
                    soKetQua++;
                }
                
            } catch (const std::exception& e) {
                continue; // Bo qua loi va tiep tuc tim kiem
            }
        }
        
        // Sap xep theo uu tien: ten sach truoc, the loai, tac gia, ISBN
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
            cout << "\nKet qua tim kiem: \"" << tuKhoa << "\" (" << soKetQua << " ket qua)\n\n";
            
            for (int i = 0; i < soKetQua; i++) {
                cout << (i+1) << ". ";
                InMotDauSach(ketQua[i].sach);
            }
            
            thongBao(cout, "\n=== HOÀN TẤT: Tìm thấy " + to_string(soKetQua) + " kết quả ===", THONG_TIN);
        } else {
            cout << "\nKhông tìm thấy kết quả phù hợp với từ khóa: \"" << tuKhoa << "\"\n";
            cout << "Gợi ý: Thử từ khóa ngắn hơn hoặc kiểm tra chính tả.\n\n";
        }
        
        return soKetQua;
        
    } catch (const std::exception& e) {
        thongBao(cout, string("Lỗi trong quá trình tìm kiếm: ") + e.what(), LOI);
        return -1;
    }
}

// Doc du lieu dau sach va danh muc sach tu file
void docDauSachTuFile(PTRDAUSACH dsDauSach[], int& soLuongDauSach) {
    using namespace InputUtils;
    if (!KiemTraDanhSachHopLe(dsDauSach, 0, MAX_DAUSACH, "Danh sách đầu sách")) {
        return;
    }
    
    GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
    
    try {
        NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach);
        NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach);
    } catch (const exception& e) {
        thongBao(cout, string("Lỗi đọc file: ") + e.what(), LOI);
        soLuongDauSach = 0;
    }
}

// Ghi du lieu dau sach va danh muc sach ra file
void ghiDauSachVaoFile(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    using namespace InputUtils;
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách")) {
        return;
    }
    
    try {
        bool okBooks = GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach);
        bool okDms   = GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach);
        
        if(okBooks && okDms) {
            thongBao(std::cout, "Đã lưu dữ liệu đầu sách và danh mục sách.", THONG_TIN);
        } else {
            thongBao(std::cout, "Có lỗi khi lưu dữ liệu!", LOI);
        }
    } catch (const exception& e) {
        thongBao(cout, string("Lỗi ghi file: ") + e.what(), LOI);
    }
}

// Sap xep mang dau sach theo ten bang quick sort
void sapXepDauSachTheoTen(PTRDAUSACH arr[], int low, int high) {
    if (!arr || low < 0 || high >= MAX_DAUSACH || low >= high) return;
    
    try {
        if (low < high) {
            int pi = partition(arr, low, high);
            
            // Su dung insertion sort cho mang lon de tranh stack overflow
            if (high - low > 1000) {
                insertionSort(arr, low, high);
            } else {
                sapXepDauSachTheoTen(arr, low, pi - 1);
                sapXepDauSachTheoTen(arr, pi + 1, high);
            }
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
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Sap xep mang dau sach theo ten bang insertion sort
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


