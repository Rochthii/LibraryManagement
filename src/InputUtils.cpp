
#include "../include/InputUtils.h"
#include "../include/VietnameseUtils.h"  // Them include cho ho tro tieng Viet
#include <stdexcept>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cstdio>
#include <cstdint>
#include <fstream>

using namespace std;

namespace InputUtils {


const char* FILE_DAUSACH = "data/dausach.txt";
const char* FILE_DANHMUCSACH = "data/danhmucsach.txt";

static const int SO_LAN_THU_TOI_DA = 5;

static bool g_choPhepSuaFile = true;      // Tự động sửa check digit khi đọc file
static bool g_canhBaoKhiSua = false;      // Hiển thị cảnh báo khi tự sửa

static inline void seedNgauNhienMotLan();
static std::string taoChuoiSoNgauNhien(size_t length);

inline bool laKyTuSo(char c)           { return c >= '0' && c <= '9'; }
inline bool laKyTuChuHoa(char c)       { return c >= 'A' && c <= 'Z'; }  
inline bool laKyTuChuThuong(char c)    { return c >= 'a' && c <= 'z'; }
inline bool laKyTuChuCai(char c)       { return laKyTuChuHoa(c) || laKyTuChuThuong(c); }
inline bool laKyTuKhoangTrang(char c)  { return c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v'; }

// In thong bao ra man hinh
void thongBao(std::ostream& out, const std::string& msg, LoaiThongBao loai) {
    const char* tienTo[] = {"Lỗi: ", "Thông tin: ", "Cảnh báo: "};
    out << tienTo[loai] << msg << "\n";
}

// Tao chuoi thong bao loi voi so dong
inline std::string thongBaoLoi(int soDong, const std::string& noiDung) {
    return "Dòng " + std::to_string(soDong) + ": " + noiDung;
}

// Tao chuoi thong bao loi cho danh muc sach
inline std::string thongBaoLoiDMS(int soDong, const std::string& noiDung) {
    return "Dòng DMS " + std::to_string(soDong) + ": " + noiDung;
}

// In so lan thu con lai
static inline void thongBaoSoLanThuConLai(int soLanDaThu, std::ostream& out){
    if(soLanDaThu < SO_LAN_THU_TOI_DA)
        thongBao(out, "Còn " + std::to_string(SO_LAN_THU_TOI_DA - soLanDaThu) + " lần thử.", THONG_TIN);
}

// In thong bao loi ngoai pham vi
static inline bool thongBaoLoiNgoaiPhamVi(std::ostream& out, const std::string& tenTruong, long long giaTriMin, long long giaTriMax){
    thongBao(out, tenTruong + " phải trong [" + std::to_string(giaTriMin) + ", " + std::to_string(giaTriMax) + "]!", LOI);
    return false;
}


// Chuyen doi ky tu thanh hoa hoac thuong
inline char chuyenDoiKyTu(char c, bool thanhHoa) { 
    return thanhHoa ? (laKyTuChuThuong(c) ? char(c - 'a' + 'A') : c)
                   : (laKyTuChuHoa(c) ? char(c - 'A' + 'a') : c);
}

// Chuyen ky tu thanh hoa
inline char chuyenThanhHoa(char c)    { return chuyenDoiKyTu(c, true); }

// Chuyen ky tu thanh thuong
inline char chuyenThanhThuong(char c) { return chuyenDoiKyTu(c, false); }


// Loai bo khoang trang dau va cuoi chuoi
static std::string loaiBoKhoangTrangDauCuoi(const std::string& s){
    size_t trai = 0, phai = s.size();
    while(trai < phai && laKyTuKhoangTrang(s[trai])) ++trai;
    while(phai > trai && laKyTuKhoangTrang(s[phai-1])) --phai;
    return s.substr(trai, phai - trai);
}

// Kiem tra chuoi rong hoac chi khoang trang
static bool laChuoiRongHoacChiKhoangTrang(const std::string& s){
    for(char c: s) {
        if(!laKyTuKhoangTrang(c)) return false;
    }
    return true;
}

// So sanh do dai chuoi voi min va max
static int soSanhDoDaiChuoi(const std::string& s, size_t minL, size_t maxL){
    if(s.length() < minL) return -1;  // Quá ngắn
    if(s.length() > maxL) return 1;   // Quá dài  
    return 0;                         // Hợp lệ
}

// Lay chi cac ky tu so tu chuoi
static std::string layChiCacKyTuSo(const std::string& s){
    std::string ketQua; 
    ketQua.reserve(s.size());
    for(char c: s) {
        if(laKyTuSo(c)) ketQua.push_back(c);
    }
    return ketQua;
}


// Loc chu cai Unicode
static std::string locChuCaiUnicodeKhoang1(const std::string& s){
    std::string out; out.reserve(s.size());
    bool truocSpace = true; // để tránh space đầu
    size_t i = 0;
    while(i < s.size()){
        size_t start = i;
        uint32_t cp = VietnameseUtils::readCodePoint(s, i);
        if(cp <= 0x7F){
            char c = static_cast<char>(cp);
            if(laKyTuChuCai(c)){
                out.push_back(c); truocSpace = false;
            } else if(c == ' ' && !truocSpace){
                out.push_back(' '); truocSpace = true;
            } else if(isdigit(c)) {
                // Giữ số
                out.push_back(c); truocSpace = false;
            } else if(c == '+' || c == '#' || c == '*' || c == '-' || c == '_' || 
                     c == '.' || c == '&' || c == '@' || c == '$' || c == '%' ||
                     c == '!' || c == '?' || c == ':' || c == ';' || c == ',' ||
                     c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
                     c == '\'' || c == '"' || c == '/' || c == '\\') {
                // Giữ các ký tự đặc biệt thường gặp trong tên sách
                out.push_back(c); truocSpace = false;
            }
            // else bỏ qua ký tự khác
        } else {
            if(VietnameseUtils::isVietnameseLetter(cp)){
                out.append(s, start, i - start); // giữ nguyên byte unicode
                truocSpace = false;
            } else {
                // Ký tự unicode không thuộc Latin: bỏ qua; nếu muốn giữ các dấu kết hợp, có thể mở rộng ở đây
            }
        }
    }
    return CatKhoangTrang(out);
}


// Chuan hoa phai cho so sanh
static std::string chuanHoaPhaiForCompare(const std::string& s){
    std::string out; out.reserve(s.size());
    size_t i = 0;
    while(i < s.size()){
        uint32_t cp = VietnameseUtils::readCodePoint(s, i);
        if(cp <= 0x7F){
            char c = static_cast<char>(cp);
            if(!laKyTuKhoangTrang(c)){
                c = chuyenThanhThuong(c);
                out.push_back(c);
            }
            continue;
        }
        // Map tất cả biến thể 'Ư'/'ư' (có/không dấu) -> 'u'
        if(cp == 0x01AF || cp == 0x01B0 ||
           cp == 0x1EE8 || cp == 0x1EE9 || cp == 0x1EEA || cp == 0x1EEB ||
           cp == 0x1EEC || cp == 0x1EED || cp == 0x1EEE || cp == 0x1EEF ||
           cp == 0x1EF0 || cp == 0x1EF1){ out.push_back('u'); continue; }
    }
    return out;
}

// Kiem tra hop le theo do dai
static bool hopLeTheoDoDai(const std::string& s, size_t minL, size_t maxL, std::ostream& out, bool themTienToSauLoc){
    int cmp = soSanhDoDaiChuoi(s, minL, maxL);
    if(cmp == 0) return true;
    const char* tienTo = themTienToSauLoc ? "Sau lọc " : "";
    if(cmp < 0) thongBao(out, std::string(tienTo) + "quá ngắn (>= " + to_string(minL) + ")!", LOI);
    else        thongBao(out, std::string(tienTo) + "quá dài (<= " + to_string(maxL) + ")!", LOI);
    return false;
}

// Tim danh muc sach theo ma sach
static DanhMucSach* timDanhMucTheoMaSach(const std::string& maSach){
    std::string ms = loaiBoKhoangTrangDauCuoi(maSach);
    for(int i=0;i<soLuongDauSach;++i){
        if(dsDauSach[i]){
            for(DanhMucSach* s = dsDauSach[i]->dms; s; s = s->next){
                if(s->maSach == ms) return s;
            }
        }
    }
    return nullptr;
}

// CHUYEN DOI CHUOI THANH SO

// Chuyen chuoi thanh so nguyen
static bool chuyenChuoiThanhSoNguyen(const std::string& chuoi, int& ketQua, bool chiChoPhepSoDuong = false){
    if(chuoi.empty()) return false;
    
    long long dau = 1;
    size_t viTri = 0;
    
    if(chuoi[0] == '-'){
        if(chiChoPhepSoDuong) return false; 
        dau = -1; 
        viTri = 1;
        if(chuoi.size() == 1) return false; 
    }
    
    long long soTichLuy = 0;
    for(; viTri < chuoi.size(); ++viTri){
        if(!laKyTuSo(chuoi[viTri])) return false;
        soTichLuy = soTichLuy * 10 + (chuoi[viTri] - '0');
        if(soTichLuy * dau > INT_MAX || soTichLuy * dau < INT_MIN) return false;
    }
    
    ketQua = int(soTichLuy * dau);
    return true;
}

// Wrapper functions cho dễ sử dụng
static bool chuyenChuoiThanhSoBatKy(const std::string& chuoi, int& ketQua){
    return chuyenChuoiThanhSoNguyen(chuoi, ketQua, false);
}

static bool chuyenChuoiThanhSoDuong(const std::string& chuoi, int& ketQua){
    return chuyenChuoiThanhSoNguyen(chuoi, ketQua, true);
}

// Phan tich trang thai sach
static bool PhanTichTrangThaiSach(const std::string& s, int& out){
    std::string t = loaiBoKhoangTrangDauCuoi(s);
    if(t.empty()) return false;
    int v = -1;
    if(chuyenChuoiThanhSoBatKy(t, v)){
        out = v; return true;
    }
    std::string norm = BoDauVaThuong(t);
    if(norm == "cho muon duoc" || norm == "cho muon duoc (0)" || norm == "0"){ out = 0; return true; }
    if(norm == "dang muon" || norm == "1"){ out = 1; return true; }
    if(norm == "thanh ly" || norm == "2"){ out = 2; return true; }
    return false;
}

// Phan tich ngay tu chuoi
static bool phanTichNgay(const std::string& s, int& ngay, int& thang, int& nam){
    // Hỗ trợ: D/M/YY hoặc DD/MM/YYYY; chấp nhận '-' '.' và tự quy về '/'
    int thanhPhan[3] = {0, 0, 0};
    int chiSo = 0;
    std::string phan;

    // Tách chuỗi theo '/'
    for(char c : s){
        if(c=='-' || c=='.') c = '/';
        if(c == '/'){
            if(phan.empty() || chiSo >= 3) return false;
            int v = 0;
            if(!chuyenChuoiThanhSoDuong(phan, v)) return false;
            thanhPhan[chiSo++] = v;
            phan.clear();
        } else {
            if(!laKyTuSo(c)) return false;
            phan.push_back(c);
        }
    }

    // Phải có đúng 2 dấu '/', còn lại là phần thứ 3
    if(phan.empty() || chiSo != 2) return false;
    int v = 0;
    if(!chuyenChuoiThanhSoDuong(phan, v)) return false;
    thanhPhan[chiSo] = v;

    ngay  = thanhPhan[0];
    thang = thanhPhan[1];
    nam   = thanhPhan[2];

    // Chuẩn hóa năm 2 chữ số
    if(nam < 100) nam = (nam <= 30 ? nam + 2000 : nam + 1900);

    // Kiểm tra phạm vi cơ bản
    if(thang < 1 || thang > 12) return false;
    if(ngay  < 1 || ngay  > 31) return false;
    if(nam   < 1900 || nam > 2100) return false;

    // Kiểm tra số ngày hợp lệ theo tháng, có xét năm nhuận
    int soNgayThang[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    bool namNhuan = (nam % 400 == 0) || (nam % 4 == 0 && nam % 100 != 0);
    if(namNhuan) soNgayThang[2] = 29;
    if(ngay > soNgayThang[thang]) return false;

    return true;
}

// Chuan hoa chuoi ngay
bool ChuanHoaNgay(std::string& s, std::ostream& out){
    (void)out; // không dùng tham số out trong hàm này
    s = loaiBoKhoangTrangDauCuoi(s);
    if(s.empty()) return false;
    int d=0,m=0,y=0;
    if(!phanTichNgay(s, d, m, y)) return false;
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", d, m, y);
    s.assign(buf);
    return true;
}

// HÀM PHỤ TRỢ CÔNG KHAI
// Trien khai cac wrapper cong khai

// HÀM XỬ LÝ KHOẢNG TRẮNG TỔNG HỢP
std::string CatKhoangTrang(const std::string& s){
    return loaiBoKhoangTrangDauCuoi(s);
}

// Chuan hoa khoang trang
std::string ChuanHoaKhoangTrang(const std::string& s){
    std::string out; out.reserve(s.size());
    bool prevSpace = false;
    for(char c: s){
        // Tái sử dụng hàm laKhoangTrang thay vì liệt kê lại
        char x = laKyTuKhoangTrang(c) ? ' ' : c;
        if(x==' '){
            if(!prevSpace){ out.push_back(' '); prevSpace = true; }
        } else {
            out.push_back(x); prevSpace = false;
        }
    }
    // Tái sử dụng catKhoangTrang để trim thay vì viết lại logic
    return loaiBoKhoangTrangDauCuoi(out);
}

bool ChuyenNgaySangTimeT(const std::string& sNgay, time_t& outTime){
    std::string t = CatKhoangTrang(sNgay);
    if(t.empty()) return false;
    int d=0,m=0,y=0; if(!phanTichNgay(t, d, m, y)) return false;
    tm tg{}; tg.tm_mday=d; tg.tm_mon=m-1; tg.tm_year=y-1900;
    time_t tt = mktime(&tg); if(tt == -1) return false; outTime = tt; return true;
}

std::string ChuanHoaPhai(const std::string& raw){
    std::string loc = chuanHoaPhaiForCompare(raw);
    if(loc == "nam") return "Nam";
    if(loc == "nu") return "Nữ";
    return std::string();
}

// Forward declaration
static bool ChuanHoaISBNCore(const std::string& raw, std::string& outISBN, bool lenient, bool warnOnFix, bool quiet, std::ostream& out);

// Tach va tim kiem
bool TachTruong(const std::string& line, char sep, std::string out[], int expected){
    int idx = 0; std::string cur;
    for(char c: line){
        if(c == sep){ if(idx < expected) out[idx++] = cur; cur.clear(); }
        else cur.push_back(c);
    }
    if(idx < expected) out[idx++] = cur;
    return idx == expected;
}

int TachTokenKhoangTrang(const std::string& s, std::string tokens[], int maxTok){
    int n = 0; std::string t;
    for(char c: s){
        if(laKyTuKhoangTrang(c)){ if(!t.empty()){ if(n<maxTok) tokens[n++] = t; t.clear(); } }
        else t.push_back(c);
    }
    if(!t.empty() && n<maxTok) tokens[n++] = t;
    return n;
}

bool ChuaTatCaTuKhoa(const std::string& haystack, const std::string tokens[], int count){
    for(int i=0;i<count;++i){ if(haystack.find(tokens[i]) == std::string::npos) return false; }
    return true;
}

// ISBN va ma sach
std::string TachISBNTuMaSach(const std::string& maSach){
    size_t pos = maSach.find('-');
    return (pos == std::string::npos) ? maSach : maSach.substr(0, pos);
}

const char* TenTrangThai(TrangThaiSach tt){
    switch(tt){
        case CHO_MUON_DUOC: return "Cho mượn được";
        case DANG_MUON:     return "Đang mượn";
        case THANH_LY:      return "Thanh lý";
        default:            return "Không rõ";
    }
}

PTRDAUSACH TimDauSachTheoISBN(PTRDAUSACH ds[], int n, const std::string& isbn){
    for(int i=0;i<n;++i){ PTRDAUSACH d = ds[i]; if(d && d->ISBN == isbn) return d; }
    return nullptr;
}

bool ChenNodeDMSVaoDauSach(PTRDAUSACH d, const std::string& maSach, int tt, const std::string& viTri){
    PTRDMS node = new (std::nothrow) DanhMucSach; if(!node) return false;
    node->maSach = maSach;
    node->trangThai = (TrangThaiSach)tt;
    node->viTri = ChuanHoaKhoangTrang(viTri);
    node->next = nullptr;
    if(!d->dms) d->dms = node; else { PTRDMS t = d->dms; while(t->next) t = t->next; t->next = node; }
    return true;
}

void InTongKet(const char* nhan, int ok, int total, int skip) {
    if (total > 0) {
        std::string msg = std::string(nhan) + ": " + std::to_string(ok) + "/" + std::to_string(total);
        if (skip > 0) {
            msg += " (bỏ qua: " + std::to_string(skip) + ")";
        }
        thongBao(std::cout, msg, THONG_TIN);
    }
}

// Hien thi va nhom cho Dau sach

// Tao doi tuong dau sach moi
PTRDAUSACH TaoDauSachMoi(const std::string& ISBN, const std::string& tenSach, int soTrang, 
                         const std::string& tacGia, int namXuatBan, const std::string& theLoai, std::ostream& out) {
    try {
        // Cấp phát bộ nhớ an toàn
        PTRDAUSACH dauSachMoi = new (std::nothrow) DauSach;
        if (!dauSachMoi) {
            thongBao(out, "Không đủ bộ nhớ để tạo đầu sách mới!", LOI);
            return nullptr;
        }
        
        // Khởi tạo dữ liệu
        dauSachMoi->ISBN = ISBN;
        dauSachMoi->tenSach = tenSach;
        dauSachMoi->soTrang = soTrang;
        dauSachMoi->tacGia = tacGia;
        dauSachMoi->namXuatBan = namXuatBan;
        dauSachMoi->theLoai = theLoai;
        dauSachMoi->dms = nullptr;
        
        return dauSachMoi;
        
    } catch (const std::exception& e) {
        thongBao(out, string("Lỗi khi tạo đầu sách: ") + e.what(), LOI);
        return nullptr;
    }
}

// Tim vi tri chen theo ten
int TimViTriChenDauSach(PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& tenSach, std::ostream& out) {
    if (!dsDauSach) {
        thongBao(out, "Danh sách đầu sách không hợp lệ!", LOI);
        return -1;
    }
    
    int viTriChen = 0;
    try {
        while (viTriChen < soLuongDauSach) {
            if (!dsDauSach[viTriChen]) {
                thongBao(out, "Cảnh báo: Phát hiện phần tử null tại vị trí " + to_string(viTriChen), CANH_BAO);
                break;
            }
            
            if (dsDauSach[viTriChen]->tenSach > tenSach) {
                break;
            }
            
            viTriChen++;
        }
        
        return viTriChen;
        
    } catch (const std::exception& e) {
        thongBao(out, string("Lỗi khi tìm vị trí chèn: ") + e.what(), LOI);
        return -1;
    }
}

// Chen dau sach vao vi tri
bool ChenDauSachVaoViTri(PTRDAUSACH dsDauSach[], int& soLuongDauSach, PTRDAUSACH dauSachMoi, int viTri, std::ostream& out) {
    if (!dsDauSach || !dauSachMoi || viTri < 0 || viTri > soLuongDauSach || soLuongDauSach >= MAX_DAUSACH) {
        thongBao(out, "Tham số chèn không hợp lệ!", LOI);
        
        return false;
    }
    
    try {
        // Dịch chuyển các phần tử để tạo chỗ trống
        for (int i = soLuongDauSach; i > viTri; i--) {
            if (i >= MAX_DAUSACH || (i-1) < 0) {
                thongBao(out, "Lỗi overflow trong quá trình dịch chuyển!", LOI);
                
                return false;
            }
            dsDauSach[i] = dsDauSach[i - 1];
        }
        
        // Chèn đầu sách mới
        dsDauSach[viTri] = dauSachMoi;
        soLuongDauSach++;
        
        return true;
        
    } catch (const std::exception& e) {
        thongBao(out, string("Lỗi trong quá trình chèn: ") + e.what(), LOI);
        
        return false;
    }
}

void InMotDauSach(PTRDAUSACH s) {
    if (!s) return;
    
    std::cout << "\nISBN: " << s->ISBN << "\n";
    std::cout << "Tên sách: " << s->tenSach << "\n";
    std::cout << "Tác giả: " << s->tacGia << "\n";
    std::cout << "Năm XB: " << s->namXuatBan << "\n";
    std::cout << "Số trang: " << s->soTrang << "\n";
    std::cout << "Thể loại: " << s->theLoai << "\n";
    
    // Danh sách bản sao
    if (s->dms) {
        std::cout << "\nDANH SÁCH BẢN SAO:\n";
        
        int count = 1;
        for (PTRDMS q = s->dms; q; q = q->next) {
            std::string trangThai = TenTrangThai(q->trangThai);
            std::string viTri = q->viTri.empty() ? "Chưa xác định" : q->viTri;
            
            std::cout << "  " << count << ". " << q->maSach 
                      << " | " << trangThai << " | Vị trí: " << viTri << "\n";
            count++;
        }
    } else {
        std::cout << "\nChưa có bản sao nào\n";
    }
    
    std::cout << "\n";
}

bool KhopTieuDeKhongDau(const std::string& titleNorm, const std::string& kwNorm,
                          const std::string tokens[], int tokenCount){
    if(!kwNorm.empty() && titleNorm.find(kwNorm) != std::string::npos) return true;
    return ChuaTatCaTuKhoa(titleNorm, tokens, tokenCount);
}

// Nap du lieu tu file
void NapDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int& soLuongDauSach){
    std::ifstream fin(path);
    if(!fin){ thongBao(std::cout, std::string("Không mở được ") + path, CANH_BAO); return; }
    // Dùng kiểm tra trùng trong bộ nhớ để tránh lặp, không cần mảng phụ

    std::string line; int lineNo = 0; int bookTotal = 0; int bookOk = 0; int bookSkip = 0;
    while(std::getline(fin, line)){
        ++lineNo;
        if(line.empty()) continue;
        ++bookTotal;
        std::string f[6];
        if(!TachTruong(line, '|', f, 6)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng: " + line), CANH_BAO); ++bookSkip; continue;
        }
        int soTrang = 0, nam = 0;
        if(!chuyenChuoiThanhSoBatKy(CatKhoangTrang(f[2]), soTrang) || !chuyenChuoiThanhSoBatKy(CatKhoangTrang(f[4]), nam)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng số: " + line), CANH_BAO); ++bookSkip; continue;
        }
        std::string isbnChuan;
        if(!ChuanHoaISBNFile(f[0], isbnChuan, std::cout)) {
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng ISBN, bỏ qua."), CANH_BAO);
            ++bookSkip; continue;
        }
        // Kiểm tra trùng trong bộ nhớ (đã có) để tránh lặp
        if(TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "ISBN trùng (" + isbnChuan + "), bỏ qua."), CANH_BAO);
            ++bookSkip; continue;
        }

        std::string tenChuan, tgChuan, tlChuan;
        if(!ChuanHoaTenUnicode(f[1], 1, 200, std::cout, tenChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng tên sách, bỏ qua."), CANH_BAO); ++bookSkip; continue;
        }
        if(!ChuanHoaTenUnicode(f[3], 1, 150, std::cout, tgChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng tác giả, bỏ qua."), CANH_BAO); ++bookSkip; continue;
        }
        if(!ChuanHoaTenUnicode(f[5], 1, 100, std::cout, tlChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng thể loại, bỏ qua."), CANH_BAO); ++bookSkip; continue;
        }
        if(!themDauSach(dsDauSach, soLuongDauSach, isbnChuan, tenChuan, soTrang, tgChuan, nam, tlChuan, true)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Không thể thêm đầu sách, bỏ qua."), CANH_BAO); ++bookSkip;
        } else {
            ++bookOk;
        }
    }
    InTongKet("Nạp đầu sách", bookOk, bookTotal, bookSkip);
}

// Kiem tra ma sach da xu ly
bool KiemTraMaSachDaXuLy(std::string* danhSachDaXuLy, int soLuongDaXuLy, const std::string& maSach) {
    for(int i = 0; i < soLuongDaXuLy; i++) {
        if(danhSachDaXuLy[i] == maSach) {
            return true;
        }
    }
    return false;
}

// Hàm giải phóng bộ nhớ danh sách mã sách
void GiaiPhongDanhSachMaSach(std::string*& danhSach) {
    if(danhSach) {
        delete[] danhSach;
        danhSach = nullptr;
    }
}

// Giai phong linked list DanhMucSach
void GiaiPhongDanhMucSach(PTRDMS& dms) {
    while(dms) {
        PTRDMS current = dms;
        dms = dms->next;
        delete current;
    }
    dms = nullptr;
}

// Giai phong mot dau sach
void GiaiPhongDauSach(PTRDAUSACH& dauSach) {
    if(dauSach) {
        // Giải phóng linked list danh mục sách
        GiaiPhongDanhMucSach(dauSach->dms);
        // Giải phóng đầu sách
        delete dauSach;
        dauSach = nullptr;
    }
}

// Ghi du lieu ra file
// Phien ban silent cho backup
bool GhiDanhSachDauSachSilent(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach);
bool GhiDanhMucSachSilent(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach);

// Ham backup an toan
bool BackupTruocKhiGiaiPhong(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    if(soLuongDauSach <= 0) return true; // Không có gì để backup
    
    // Kiểm tra xem có file backup gần đây không (trong vòng 1 phút)
    time_t now = time(nullptr);
    tm* timeinfo = localtime(&now);
    char currentTime[32];
    strftime(currentTime, sizeof(currentTime), "%Y%m%d_%H%M", timeinfo);
    
    // Tạo pattern để tìm backup gần đây  
    std::string pattern = std::string("data/backup_") + currentTime;
    
    // Tạo backup với timestamp đầy đủ chỉ khi cần
    char fullTimestamp[32];
    strftime(fullTimestamp, sizeof(fullTimestamp), "%Y%m%d_%H%M%S", timeinfo);
    
    std::string backupPath = std::string("data/backup_") + fullTimestamp + "_dausach.txt";
    std::string backupDMSPath = std::string("data/backup_") + fullTimestamp + "_dms.txt";
    
    bool success = true;
    if(!GhiDanhSachDauSachSilent(backupPath.c_str(), dsDauSach, soLuongDauSach)) {
        success = false;
    }
    
    if(!GhiDanhMucSachSilent(backupDMSPath.c_str(), dsDauSach, soLuongDauSach)) {
        success = false;
    }
    
    return success;
}

// Giai phong toan bo danh sach dau sach
void GiaiPhongToanBoDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach) {
    if(soLuongDauSach <= 0) {
        return;
    }
    
    // Giải phóng bộ nhớ
    for(int i = 0; i < soLuongDauSach; i++) {
        if(dsDauSach[i]) {
            GiaiPhongDauSach(dsDauSach[i]);
        }
    }
    soLuongDauSach = 0;
}

bool ThemMaSachDaXuLy(std::string*& danhSachDaXuLy, int& soLuongDaXuLy, int& dungLuongToiDa, const std::string& maSach) {
    // Mở rộng mảng nếu cần
    if(soLuongDaXuLy >= dungLuongToiDa) {
        int dungLuongMoi = (dungLuongToiDa == 0) ? 256 : (dungLuongToiDa * 2);
        std::string* mangMoi = new (std::nothrow) std::string[dungLuongMoi];
        if(!mangMoi) {
            thongBao(std::cout, "Không đủ bộ nhớ để mở rộng danh sách theo dõi mã sách.", CANH_BAO);
            return false; // Trả về false để báo lỗi
        }
        
        // Sao chép dữ liệu cũ
        for(int i = 0; i < soLuongDaXuLy; i++) {
            mangMoi[i] = danhSachDaXuLy[i];
        }
        
        // Giải phóng bộ nhớ cũ an toàn
        delete[] danhSachDaXuLy;
        danhSachDaXuLy = mangMoi;
        dungLuongToiDa = dungLuongMoi;
    }
    
    danhSachDaXuLy[soLuongDaXuLy++] = maSach;
    return true;
}

void NapDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach){
    // Kiểm tra điều kiện tiên quyết trước khi cấp phát bộ nhớ
    std::ifstream fdm(path);
    if(!fdm){ 
    thongBao(std::cout, std::string("Không mở được ") + path, CANH_BAO); 
        return; 
    }
    
    if(soLuongDauSach <= 0){
    thongBao(std::cout, "Chưa có đầu sách trong bộ nhớ, bỏ qua nạp danh mục sách.", CANH_BAO);
        return;
    }
    
    // Theo dõi mã sách đã xử lý trong file này - sử dụng scope để tự động cleanup
    {
        std::string* danhSachMaSachDaXuLy = nullptr; 
        int soLuongMaSachDaXuLy = 0; 
        int dungLuongToiDa = 0;
        
        // Khởi tạo biến đếm và theo dõi
        std::string line; 
        int soThuTuDong = 0; 
        int tongSoDong = 0; 
        int soLuongThanhCong = 0; 
        int soLuongBoQua = 0; 
        int soLuongCapNhat = 0;
    // Đọc và xử lý từng dòng
    while(std::getline(fdm, line)){
        soThuTuDong++;
        
        // Bỏ qua dòng trống
        if(line.empty()) continue; 
        tongSoDong++;

        // Tách các trường dữ liệu
        std::string cacTruong[3];
        if(!TachTruong(line, '|', cacTruong, 3)){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng: " + line), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }

        const std::string& maSach = cacTruong[0];
        const std::string& trangThaiStr = cacTruong[1];
        const std::string& viTri = cacTruong[2];
        
        // Kiểm tra định dạng mã sách: <ISBN-13>-<số thứ tự dương>
        size_t viTriGachNgang = maSach.find('-');
        if(viTriGachNgang == std::string::npos){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (thiếu '-'), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Tách phần ISBN và số thứ tự
        std::string phanISBN = maSach.substr(0, viTriGachNgang);
        std::string phanSoThuTu = maSach.substr(viTriGachNgang + 1);
        
        // Kiểm tra ISBN có đúng 13 chữ số không
        if(phanISBN.size() != 13){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (ISBN phải 13 chữ số), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiểm tra ISBN có phải toàn số không
        bool laToanSo = true; 
        for(char kyTu : phanISBN){ 
            if(kyTu < '0' || kyTu > '9'){ 
                laToanSo = false; 
                break; 
            } 
        }
        if(!laToanSo){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (ISBN không phải số), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiểm tra số thứ tự
        int soThuTu = 0; 
        if(phanSoThuTu.empty() || !chuyenChuoiThanhSoBatKy(CatKhoangTrang(phanSoThuTu), soThuTu) || soThuTu <= 0){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (số thứ tự > 0), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiểm tra trạng thái sách
        int trangThai = 0;
        if(!PhanTichTrangThaiSach(trangThaiStr, trangThai)){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng trạng thái, bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        if(trangThai < 0 || trangThai > 2){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Trạng thái ngoài phạm vi (0-2), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }

        std::string isbn = phanISBN;
        // Tìm đầu sách theo ISBN
        PTRDAUSACH dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        if(!dauSach){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "ISBN không thuộc danh sách đầu sách đã nạp (" + isbn + "), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }

        // Kiểm tra xem mã sách đã tồn tại trong bộ nhớ chưa
        PTRDMS sachDaTonTai = timDanhMucTheoMaSach(maSach);
        if(sachDaTonTai){
            // Cập nhật thông tin sách đã có
            sachDaTonTai->trangThai = (TrangThaiSach)trangThai;
            sachDaTonTai->viTri = ChuanHoaKhoangTrang(viTri);
            soLuongCapNhat++; 
            soLuongThanhCong++;
            continue;
        }

        // Kiểm tra trùng mã sách trong file này
        if(KiemTraMaSachDaXuLy(danhSachMaSachDaXuLy, soLuongMaSachDaXuLy, maSach)){
            // Tìm và cập nhật bản ghi đã được thêm trước đó trong file
            PTRDMS sachTrongFile = timDanhMucTheoMaSach(maSach);
            if(sachTrongFile){
                sachTrongFile->trangThai = (TrangThaiSach)trangThai;
                sachTrongFile->viTri = ChuanHoaKhoangTrang(viTri);
                soLuongCapNhat++; 
                soLuongThanhCong++;
                continue;
            } else {
                // Trường hợp bất thường - mã đã được xử lý nhưng không tìm thấy
                thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Trùng mã sách trong file (" + maSach + "), bỏ qua."), CANH_BAO);
                soLuongBoQua++; 
                continue;
            }
        }

        // Thêm sách mới vào danh mục
        if(!ChenNodeDMSVaoDauSach(dauSach, maSach, trangThai, viTri)){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Không đủ bộ nhớ để thêm DMS, bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Đánh dấu mã sách đã được xử lý
        if(!ThemMaSachDaXuLy(danhSachMaSachDaXuLy, soLuongMaSachDaXuLy, dungLuongToiDa, maSach)) {
            // Nếu không thể thêm vào danh sách theo dõi, vẫn coi như thành công
            // vì sách đã được thêm vào hệ thống, chỉ là không theo dõi được trùng lặp
        }
        soLuongThanhCong++;
    }
    
        // In tổng kết
        InTongKet("Nạp bản sao (DMS)", soLuongThanhCong, tongSoDong, soLuongBoQua);
        if(soLuongCapNhat > 0) {
            thongBao(std::cout, std::string("(Trong đó cập nhật: ") + std::to_string(soLuongCapNhat) + ")", THONG_TIN);
        }
        
        // Giải phóng bộ nhớ an toàn
        GiaiPhongDanhSachMaSach(danhSachMaSachDaXuLy);
    } // Kết thúc scope - đảm bảo tất cả biến local được hủy
}

// Ghi du lieu ra file (second section)
bool GhiDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach){
    std::ofstream fout(path, std::ios::trunc);
    if(!fout){ thongBao(std::cout, std::string("Không mở được ") + path + " để ghi!", LOI); return false; }
    int wrote = 0;
    for(int i=0;i<soLuongDauSach;++i){ PTRDAUSACH d = dsDauSach[i]; if(!d) continue;
        fout << d->ISBN << '|' << d->tenSach << '|' << d->soTrang << '|' << d->tacGia << '|' << d->namXuatBan << '|' << d->theLoai << "\n";
        ++wrote;
    }
    return true;
}

// Phiên bản silent cho backup
bool GhiDanhSachDauSachSilent(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach){
    std::ofstream fout(path, std::ios::trunc);
    if(!fout) return false;
    for(int i=0;i<soLuongDauSach;++i){ PTRDAUSACH d = dsDauSach[i]; if(!d) continue;
        fout << d->ISBN << '|' << d->tenSach << '|' << d->soTrang << '|' << d->tacGia << '|' << d->namXuatBan << '|' << d->theLoai << "\n";
    }
    return true;
}

bool GhiDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach){
    std::ofstream fdm(path, std::ios::trunc);
    if(!fdm){ thongBao(std::cout, std::string("Không mở được ") + path + " để ghi!", LOI); return false; }
    int wrote = 0;
    for(int i=0;i<soLuongDauSach;++i){ PTRDAUSACH d = dsDauSach[i]; if(!d) continue;
        for(PTRDMS q = d->dms; q; q = q->next){ fdm << q->maSach << '|' << (int)q->trangThai << '|' << q->viTri << "\n"; ++wrote; }
    }
    return true;
}

// Phiên bản silent cho backup
bool GhiDanhMucSachSilent(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach){
    std::ofstream fdm(path, std::ios::trunc);
    if(!fdm) return false;
    for(int i=0;i<soLuongDauSach;++i){ PTRDAUSACH d = dsDauSach[i]; if(!d) continue;
        for(PTRDMS q = d->dms; q; q = q->next){ fdm << q->maSach << '|' << (int)q->trangThai << '|' << q->viTri << "\n"; }
    }
    return true;
}


// Chuan hoa ten Unicode
bool ChuanHoaTenUnicode(const std::string& dauVao, size_t minLength, size_t maxLength,
                        std::ostream& out, std::string& ketQua){
    if(dauVao.empty()){ thongBao(out, "Không được để trống!", LOI); return false; }
    if(laChuoiRongHoacChiKhoangTrang(dauVao)){ thongBao(out, "Chỉ khoảng trắng!", LOI); return false; }
    std::string loc = locChuCaiUnicodeKhoang1(dauVao);
    std::string chuan = VietnameseUtils::toTitleCase(loc);
    if(chuan.empty()){ thongBao(out, "Sau lọc rỗng!", LOI); return false; }
    if(!hopLeTheoDoDai(chuan, minLength, maxLength, out, true)) return false;
    ketQua = chuan; return true;
}

// NHẬP LIỆU TỪ NGƯỜI DÙNG

// Nhập tên với nhãn tuỳ chỉnh
std::string NhapTenNhan(const char* nhan, size_t minLen, size_t maxLen, std::istream& in, std::ostream& out){
    std::string goc; int soLanThu = 0;
    while(soLanThu < SO_LAN_THU_TOI_DA){
        out << "Nhập " << (nhan?nhan:"tên") << " (" << minLen << "-" << maxLen << " ký tự): ";
        std::getline(in, goc);
        
        // Kiểm tra EOF hoặc lỗi stream
        if(in.eof() || in.fail()) {
            throw std::invalid_argument("Lỗi: Kết thúc dữ liệu đầu vào!");
        }
        
        std::string chuan;
        if(ChuanHoaTenUnicode(goc, minLen, maxLen, out, chuan)) return chuan;
        ++soLanThu; thongBaoSoLanThuConLai(soLanThu, out);
    }
    throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
}

// Nhập số nguyên với nhãn tuỳ chỉnh
int NhapSoNguyenNhan(const char* nhan, int minVal, int maxVal, std::istream& in, std::ostream& out){
    std::string dong; int soLanThu = 0; int giaTri = 0;
    while(soLanThu < SO_LAN_THU_TOI_DA){
        out << "Nhập " << (nhan?nhan:"giá trị") << " (" << minVal << "-" << maxVal << "): ";
        std::getline(in, dong);
        
        // Kiểm tra EOF hoặc lỗi stream
        if(in.eof() || in.fail()) {
            throw std::invalid_argument("Lỗi: Kết thúc dữ liệu đầu vào!");
        }
        
        std::string sauCat = CatKhoangTrang(dong);
        if(!KiemTraChuoiRong(sauCat, (nhan?nhan:"Giá trị"), out)){ ++soLanThu; }
    else if(sauCat.length() > 30){ thongBao(out, "Quá dài!", LOI); ++soLanThu; }
    else if(!chuyenChuoiThanhSoBatKy(sauCat, giaTri)){ thongBao(out, "Sai định dạng số!", LOI); ++soLanThu; }
        else {
            if(giaTri < minVal || giaTri > maxVal){ (void)thongBaoLoiNgoaiPhamVi(out, (nhan?nhan:"Giá trị"), minVal, maxVal); ++soLanThu; }
            else return giaTri;
        }
        thongBaoSoLanThuConLai(soLanThu, out);
    }
    throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
}

// KIỂM TRA VÀ XÁC THỰC

// Kiem tra ISBN trung lap
bool KiemTraTrungISBN(const string& ISBN, ostream& out) {
    bool coDauSach = (TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN) != nullptr);
    if(coDauSach) thongBao(out, "Trùng ISBN!", LOI);
    return coDauSach;
}

// Kiem tra chuoi rong
bool KiemTraChuoiRong(const string& chuoi, const string& tenTruong, ostream& out){
    if(chuoi.empty()) { thongBao(out, tenTruong + " không được rỗng!", LOI); return false; }
    if(laChuoiRongHoacChiKhoangTrang(chuoi)) { thongBao(out, tenTruong + " chỉ khoảng trắng!", LOI); return false; }
    return true;
}

// XỬ LÝ CHUỖI NÂNG CAO

// Chuyển đổi chuỗi thành chữ thường
string ChuyenInThuong(const string& dauVao){
    string kq = dauVao;
    for(char &c: kq) c = chuyenThanhThuong(c);
    return kq;
}

// Bo dau tieng Viet
std::string BoDauVaThuong(const std::string& s){
    std::string out; out.reserve(s.size());
    size_t i = 0;
    while(i < s.size()){
        uint32_t cp = VietnameseUtils::readCodePoint(s, i);
        char ascii = 0;
        if(cp <= 0x7F){
            char c = static_cast<char>(cp);
            c = chuyenThanhThuong(c);
            ascii = c;
        } else {
            // Map các chữ cái Việt có dấu về gốc không dấu (a,e,i,o,u,y,d)
            // Mặc định bỏ qua ký tự không phải Latin chữ cái
            if(VietnameseUtils::isVietnameseLetter(cp)){
                uint32_t low = VietnameseUtils::toLower(cp);
                switch(low){
                    // a, ă, â + dấu
                    case 0x0061: case 0x00E0: case 0x00E1: case 0x1EA3: case 0x00E3: case 0x1EA1:
                    case 0x0103: case 0x1EB1: case 0x1EAF: case 0x1EB3: case 0x1EB5: case 0x1EB7:
                    case 0x00E2: case 0x1EA7: case 0x1EA5: case 0x1EA9: case 0x1EAB: case 0x1EAD:
                        ascii = 'a'; break;
                    // e, ê + dấu
                    case 0x0065: case 0x00E8: case 0x00E9: case 0x1EBB: case 0x1EBD: case 0x1EB9:
                    case 0x00EA: case 0x1EC1: case 0x1EBF: case 0x1EC3: case 0x1EC5: case 0x1EC7:
                        ascii = 'e'; break;
                    // i + dấu
                    case 0x0069: case 0x00EC: case 0x00ED: case 0x1EC9: case 0x0129: case 0x1ECB:
                        ascii = 'i'; break;
                    // o, ô, ơ + dấu
                    case 0x006F: case 0x00F2: case 0x00F3: case 0x1ECF: case 0x00F5: case 0x1ECD:
                    case 0x00F4: case 0x1ED3: case 0x1ED1: case 0x1ED5: case 0x1ED7: case 0x1ED9:
                    case 0x01A1: case 0x1EDD: case 0x1EDB: case 0x1EDF: case 0x1EE1: case 0x1EE3:
                        ascii = 'o'; break;
                    // u, ư + dấu
                    case 0x0075: case 0x00F9: case 0x00FA: case 0x1EE7: case 0x0169: case 0x1EE5:
                    case 0x01B0: case 0x1EEB: case 0x1EE9: case 0x1EED: case 0x1EEF: case 0x1EF1:
                        ascii = 'u'; break;
                    // y + dấu
                    case 0x0079: case 0x1EF3: case 0x00FD: case 0x1EF7: case 0x1EF9: case 0x1EF5:
                        ascii = 'y'; break;
                    // đ
                    case 0x0111:
                        ascii = 'd'; break;
                    default:
                        // các chữ Latin khác -> cố gắng hạ về ASCII thường an toàn nếu trong phạm vi
                        ascii = 0; // bỏ qua
                }
            }
        }
        if(ascii){
            // Chuẩn hóa khoảng trắng về 1 space
            if(ascii == '\t' || ascii == '\n' || ascii == '\r' || ascii == '\v' || ascii == '\f') ascii = ' ';
            out.push_back(ascii);
        } else if(cp <= 0x7F && (char)cp == ' '){
            out.push_back(' ');
        }
    }
    // Tái sử dụng ChuanHoaKhoangTrang thay vì viết lại logic rút gọn khoảng trắng
    return ChuanHoaKhoangTrang(out);
}

// Helper cho chuẩn hóa dữ liệu sách
ThongTinSachChuanHoa ChuanHoaDuLieuSach(const std::string& tenSach, const std::string& tacGia, 
                                        const std::string& theLoai, const std::string& isbn) {
    ThongTinSachChuanHoa ketQua;
    
    // Bỏ dấu và chuyển thường + chuẩn hóa khoảng trắng
    ketQua.tenSach = BoDauVaThuong(tenSach);
    ketQua.tacGia = BoDauVaThuong(tacGia);
    ketQua.theLoai = BoDauVaThuong(theLoai);
    ketQua.isbn = BoDauVaThuong(isbn);
    
    return ketQua;
}

// Nhập phái giới (Nam/Nữ)
string NhapPhai(istream& in, ostream& out){
    string phai; int soLanThu = 0;
    while(soLanThu < SO_LAN_THU_TOI_DA){
        out << "Nhập phái (Nam/Nữ): ";
        getline(in, phai);
        if(!KiemTraChuoiRong(phai, "Phái", out)){ ++soLanThu; }
        else {
            string loc = chuanHoaPhaiForCompare(phai);
            if(loc == "nam") return "Nam";
            if(loc == "nu") return "Nữ";
            thongBao(out, "Chỉ Nam hoặc Nữ!", LOI); ++soLanThu;
        }
        thongBaoSoLanThuConLai(soLanThu, out);
    }
    throw invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
}

// XỬ LÝ MÃ THẺ VÀ TRẠNG THÁI

// Kiểm tra trạng thái thẻ (0 hoặc 1)
bool KiemTraTrangThaiThe(int trangThai, ostream& out) {
    if (trangThai == 0 || trangThai == 1) return true;
    thongBao(out, "Trạng thái thẻ phải 0 hoặc 1!", LOI);
    return false;
}

// Kiểm tra định dạng mã thẻ
bool KiemTraMaThe(const string& maThe, ostream& out) {
    int v = 0;
    if (!chuyenChuoiThanhSoDuong(CatKhoangTrang(maThe), v)) { thongBao(out, "Mã thẻ không hợp lệ!", LOI); return false; }
    return true;
}

// Sinh mã thẻ ngẫu nhiên
string SinhMaTheNgauNhien(size_t length, ostream& out) {
    (void)out;
    return taoChuoiSoNgauNhien(length);
}

// XỬ LÝ ISBN

// Tính checksum EAN-13 cho 12 chữ số đầu
static int checksumEAN13(const std::string& s12){
    int sum = 0;
    for(size_t i=0;i<12;++i){ int d = s12[i]-'0'; sum += (i%2==0) ? d : d*3; }
    int c = (10 - (sum % 10)) % 10; return c;
}

/*
// Kiểm tra định dạng ISBN
bool KiemTraISBN(const string& isbn, ostream& out){
    std::string tmp;
    return ChuanHoaISBNNhapTay(isbn, tmp, out);
}
*/

// Chuan hoa ISBN 13
static bool ChuanHoaISBNCore(const std::string& raw, std::string& outISBN, bool lenient, bool warnOnFix, bool quiet, std::ostream& out){
    std::string digits = layChiCacKyTuSo(raw);
    if(digits.length() != 13){ if(!quiet) thongBao(out, "Sai định dạng ISBN (phải đủ 13 chữ số)!", LOI); return false; }
    int cks = checksumEAN13(digits.substr(0,12));
    int last = digits[12]-'0';
    if(cks != last){
        if(lenient){
            std::string cu = digits; digits[12] = char('0' + cks);
            if(warnOnFix) thongBao(out, std::string("ISBN có số kiểm tra sai, đã tự động sửa: ") + cu + " -> " + digits, CANH_BAO);
        } else {
            if(!quiet) thongBao(out, "Sai định dạng ISBN (sai số kiểm tra)!", LOI);
            return false;
        }
    }
    if(!(digits.size() >= 6 && digits.compare(0,6,"978604") == 0)){
    if(!quiet) thongBao(out, "Sai định dạng ISBN (phải bắt đầu bằng 978604)!", LOI);
        return false;
    }
    outISBN = digits; return true;
}

bool ChuanHoaISBNFile(const std::string& raw, std::string& outISBN, std::ostream& out){
    // Đọc file: im lặng lỗi chi tiết, để caller in tóm tắt theo dòng
    return ChuanHoaISBNCore(raw, outISBN, /*lenient=*/g_choPhepSuaFile, /*warnOnFix=*/g_canhBaoKhiSua, /*quiet=*/true, out);
}

// Phien ban lenient cho file
// KHÔNG DÙNG - đã comment out
/*
static bool ChuanHoaISBNFileLenient(const std::string& raw, std::string& outISBN, std::ostream& out){
    // Giữ lại wrapper cho tương thích nội bộ (im lặng lỗi)
    return ChuanHoaISBNCore(raw, outISBN, true, g_canhBaoKhiSua, true, out);
}
*/

std::string SinhISBN13TuDong(std::ostream& out, bool allowManualInput){
    // KHÔNG CẦN manual input nữa - chỉ sinh tự động
    /*
    // Cho phép nhập manual hoặc tự động
    if(allowManualInput) {
        while(true){
            out << "Nhập ISBN 13 chữ số (bắt đầu 978604), gõ 'auto' để tạo tự động: ";
            std::string isbn; 
            std::getline(std::cin, isbn);
            
            // Kiểm tra EOF
            if(std::cin.eof() || std::cin.fail()) {
                throw std::invalid_argument("Kết thúc nhập liệu");
            }
            
            std::string lower = ChuyenInThuong(isbn);
            if(lower == "auto") break; // Tiếp tục tạo tự động
            
            if(KiemTraISBN(isbn, out)) return isbn; // Manual input hợp lệ
            // Không hợp lệ -> loop lại
        }
    }
    */
    
    // Tạo ISBN tự động
    (void)out;
    (void)allowManualInput;  // Bỏ qua parameter không sử dụng
    // Cấu trúc Việt Nam: 978-604-<NXB:1..3> - <đầu sách:5..3> - <checksum>
    // 978(3) + 604(3) + NXB(p in {1,2,3}) + Đầu sách(6-p) = 12, rồi thêm checksum
    for(int tries=0; tries<1000; ++tries){
        int p = 1 + (rand()%3); // độ dài mã NXB
        int t = 6 - p;          // độ dài mã đầu sách
        std::string core = std::string("978604") + taoChuoiSoNgauNhien(p) + taoChuoiSoNgauNhien(t);
        int cks = checksumEAN13(core);
        std::string isbn = core + char('0' + cks);
        if(!TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn)) return isbn; // duy nhất
    }
    // Bất đắc dĩ: trả về 13 số ngẫu nhiên không đảm bảo duy nhất (hiếm khi chạm)
    return taoChuoiSoNgauNhien(13);
}

bool KiemTraTongSoBanSao(const string& isbn, int soLuongThem, ostream& out){
    int dem = 0;
    if(PTRDAUSACH ds = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn)){
        for(DanhMucSach* p = ds->dms; p; p = p->next) ++dem;
    }
    if(dem + soLuongThem > 1000){ thongBao(out, "Tổng số bản sao > 1000!", LOI); return false; }
    return true;
}

// Tái sử dụng timDanhMucTheoMaSach 
bool KiemTraTrungmaSach(const string& maSach, ostream& out) {
    bool coMaSach = (timDanhMucTheoMaSach(maSach) != nullptr);
    if(coMaSach) thongBao(out, "Trùng mã sách!", LOI);
    return coMaSach;
}



// SINH MÃ VÀ CHUỖI NGẪU NHIÊN

// Khởi tạo seed ngẫu nhiên (chỉ một lần)
static inline void seedNgauNhienMotLan(){
    static bool seeded = false; 
    if(!seeded){ 
        srand((unsigned)time(nullptr)); 
        seeded = true; 
    }
}

// Tao chuoi so ngau nhien
static std::string taoChuoiSoNgauNhien(size_t length){
    seedNgauNhienMotLan();
    std::string id; id.reserve(length);
    for(size_t i=0;i<length;++i) id.push_back(char('0' + rand()%10));
    return id;
}

string sinhMaSach(string isbn, int soThuTu, ostream& out){
    isbn = CatKhoangTrang(isbn);
    if(isbn.empty()){
    thongBao(out, "ISBN rỗng!", LOI); throw invalid_argument("Lỗi: ISBN rỗng!");
    }
    string chiSo = layChiCacKyTuSo(isbn);
    
    if(chiSo.empty()){
    thongBao(out, "ISBN không có chữ số!", LOI); throw invalid_argument("Lỗi: ISBN không có chữ số!");
    }
    if(soThuTu <= 0){ soThuTu = 1; }

    string ma = chiSo + "-" + to_string(soThuTu);
    int soLanDieuChinh = 0;
    while(KiemTraTrungmaSach(ma, out) && soLanDieuChinh < 100){
        ++soLanDieuChinh; ++soThuTu; ma = chiSo + "-" + to_string(soThuTu);
    }
    // không in thông báo điều chỉnh mã sách
    return ma;
}

int sinhMaThe(PTRDG goc, ostream& out){
    (void)goc;
    static int coSo = (int)(time(nullptr) % 100000);
    ++coSo;
    thongBao(out, std::string("Sinh mã thẻ mới: ") + to_string(coSo), THONG_TIN);
    return coSo;
}

// HELPER FUNCTIONS
// Kiem tra danh sach hop le
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out) {
    if (!danhSach) {
        thongBao(out, tenDS + " không hợp lệ (null pointer)!", LOI);
        return false;
    }
    
    if (soLuong < 0) {
        thongBao(out, "Số lượng " + tenDS + " không thể âm (" + std::to_string(soLuong) + ")!", LOI);
        return false;
    }
    
    if (soLuong > maxSize) {
        thongBao(out, tenDS + " vượt quá giới hạn (" + std::to_string(soLuong) + "/" + std::to_string(maxSize) + ")!", LOI);
        return false;
    }
    
    return true;
}

// Kiem tra chuoi va do dai
bool KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen, std::ostream& out) {
    if (!KiemTraChuoiRong(str, fieldName, out)) {
        return false;
    }
    
    if (str.length() > static_cast<size_t>(maxLen)) {
        thongBao(out, fieldName + " quá dài (tối đa " + std::to_string(maxLen) + " ký tự)!", LOI);
        return false;
    }
    
    return true;
}

// Lấy năm hiện tại với fallback safety
int LayNamHienTai() {
    time_t now = time(nullptr);
    tm* current_time = localtime(&now);
    int namHienTai = current_time ? (current_time->tm_year + 1900) : 2025;
    
    // Edge case protection: nếu system time sai (quá xa trong quá khứ/tương lai)
    if (namHienTai < 1900 || namHienTai > 3000) {
        return 2025;  // fallback to reasonable default
    }
    
    return namHienTai;
}

// Kiểm tra năm xuất bản hợp lý
bool KiemTraNamXuatBan(int nam, std::ostream& out) {
    int namHienTai = LayNamHienTai();
    
    if (nam < 1000 || nam > namHienTai + 5) {
        thongBao(out, "Năm xuất bản không hợp lý!", LOI);
        return false;
    }
    
    return true;
}

// DYNAMIC ARRAY MANAGEMENT

/**
 * @brief Thay đổi kích thước mảng đầu sách động
 * @param ds Con trỏ tới mảng đầu sách (sẽ được cập nhật)
 * @param size Kích thước hiện tại (sẽ được cập nhật)
 * @param newSize Kích thước mới mong muốn
 * @param out Stream đầu ra cho thông báo
 * @return true nếu thành công, false nếu thất bại
 */




} // namespace InputUtils

// HÀM TOÀN CỤC NGÀY / QUÁ HẠN
bool kiemTraQuaHan(string ngayMuon, ostream& out){
    time_t thoiDiemMuon;
    if(!InputUtils::ChuyenNgaySangTimeT(ngayMuon, thoiDiemMuon)) return false;
    time_t hienTai = time(nullptr);
    double khoangCachGiay = difftime(hienTai, thoiDiemMuon);
    bool quaHan = khoangCachGiay > 7 * 24 * 3600; // > 7 ngày
    if(quaHan){
        int soNgayMuon = static_cast<int>(khoangCachGiay / 86400);
        int soNgayQuaHan = soNgayMuon - 7;
        if(soNgayQuaHan < 0) soNgayQuaHan = 0;
    InputUtils::thongBao(out, std::string("Quá hạn ") + to_string(soNgayQuaHan) + " ngày!", InputUtils::CANH_BAO);
    }
    return quaHan;
}

string layNgayHienTai(ostream& out){
    time_t hienTai = time(nullptr);
    if(hienTai == (time_t)-1)
        throw runtime_error("Lỗi: Không lấy được thời gian!");
    tm* tp = localtime(&hienTai);
    if(!tp)
        throw runtime_error("Lỗi: Không chuyển đổi được thời gian!");
    char buf[11];
    if(strftime(buf, sizeof(buf), "%d/%m/%Y", tp) == 0)
        throw runtime_error("Lỗi: Không định dạng được ngày!");
    string ngayChuoi(buf);
    InputUtils::thongBao(out, std::string("Ngày hiện tại: ") + ngayChuoi, InputUtils::THONG_TIN);
    return ngayChuoi;
}

int tinhSoNgayQuaHan(string ngayMuon, ostream& out){
    time_t thoiDiemMuon;
    if(!InputUtils::ChuyenNgaySangTimeT(ngayMuon, thoiDiemMuon)) return -1;
    time_t hienTai = time(nullptr);
    if(hienTai == (time_t)-1){
    InputUtils::thongBao(out, "Không lấy được thời gian hiện tại!", InputUtils::LOI);
        return -1;
    }
    int soNgayMuon = static_cast<int>( difftime(hienTai, thoiDiemMuon) / 86400 );
    int soNgayQuaHan = soNgayMuon > 7 ? soNgayMuon - 7 : 0;
    if(soNgayQuaHan > 0)
    InputUtils::thongBao(out, std::string("Quá hạn ") + to_string(soNgayQuaHan) + " ngày (đã mượn " + to_string(soNgayMuon) + " ngày).", InputUtils::THONG_TIN);
    else
    InputUtils::thongBao(out, std::string("Chưa quá hạn (") + to_string(soNgayMuon) + " ngày).", InputUtils::THONG_TIN);
    return soNgayQuaHan;
}