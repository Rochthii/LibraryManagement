
#include "../include/DauSach.h"
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

const int DO_DAI_ISBN = 13;
const int DO_DAI_ISBN_CHECKSUM = 12;
const int DO_DAI_VI_TRI_TOI_DA = 100;
const int DUNG_LUONG_CACHE_MAC_DINH = 256;
const int MAX_LAN_THU_SINH_MA = 100;

const char* FILE_DAUSACH = "data/dausach.txt";
const char* FILE_DANHMUCSACH = "data/danhmucsach.txt";

static inline void seedNgauNhienMotLan();
static std::string taoChuoiSoNgauNhien(size_t length);
int checksumEAN13(const std::string& s12);

inline bool laKyTuSo(char c)           { return c >= '0' && c <= '9'; }
inline bool laKyTuChuHoa(char c)       { return c >= 'A' && c <= 'Z'; }  
inline bool laKyTuChuThuong(char c)    { return c >= 'a' && c <= 'z'; }
inline bool laKyTuChuCai(char c)       { return laKyTuChuHoa(c) || laKyTuChuThuong(c); }
inline bool laKyTuKhoangTrang(char c)  { return c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v'; }

// hien thi thong bao
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

// Kiem tra EOF hoac loi stream va nem exception neu co
static inline void kiemTraEOF(std::istream& in) {
    if(in.eof() || in.fail()) {
        throw std::invalid_argument("Lỗi: Kết thúc dữ liệu đầu vào!");
    }
}

// Nem exception khi qua so lan nhap
[[noreturn]] static inline void nemLoiQuaSoLanNhap() {
    throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
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
    if(s.length() < minL) return -1;  // Qua ngan
    if(s.length() > maxL) return 1;   // Qua dai  
    return 0;                         // Hop le
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
    bool truocSpace = true; // de tranh space dau
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
                // giu so
                out.push_back(c); truocSpace = false;
            } else if(c == '+' || c == '#' || c == '*' || c == '-' || c == '_' || 
                     c == '.' || c == '&' || c == '@' || c == '$' || c == '%' ||
                     c == '!' || c == '?' || c == ':' || c == ';' || c == ',' ||
                     c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
                     c == '\'' || c == '"' || c == '/' || c == '\\') {
                // Giu cac ky tu dac biet thuong gap trong ten sach
                out.push_back(c); truocSpace = false;
            }
            // else bo qua ky tu khac
        } else {
            if(VietnameseUtils::isVietnameseLetter(cp)){
                out.append(s, start, i - start); // giu nguyen byte unicode
                truocSpace = false;
            } else {
                // Ky tu unicode khong thuoc Latin: bo qua;
            }
        }
    }
    return CatKhoangTrang(out);
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

// Phan tich trang thai sach
static bool PhanTichTrangThaiSach(const std::string& s, int& out){
    std::string t = loaiBoKhoangTrangDauCuoi(s);
    if(t.empty()) return false;
    int giaTri = -1;
    if(chuyenChuoiThanhSoNguyen(t, giaTri, false)){
        out = giaTri; return true;
    }
    std::string norm = BoDauVaThuong(t);
    if(norm == "cho muon duoc" || norm == "cho muon duoc (0)" || norm == "0"){ out = 0; return true; }
    if(norm == "dang muon" || norm == "1"){ out = 1; return true; }
    if(norm == "thanh ly" || norm == "2"){ out = 2; return true; }
    return false;
}

// Phan tich ngay tu chuoi
static bool phanTichNgay(const std::string& s, int& ngay, int& thang, int& nam){
    // Ho tro: D/M/YY hoac DD/MM/YYYY; chap nhan '-' '.' va tu quy ve '/'
    int thanhPhan[3] = {0, 0, 0};
    int chiSo = 0;
    std::string phan;

    // Tach chuoi theo '/'
    for(char c : s){
        if(c=='-' || c=='.') c = '/';
        if(c == '/'){
            if(phan.empty() || chiSo >= 3) return false;
            int giaTri = 0;
            if(!chuyenChuoiThanhSoNguyen(phan, giaTri, true)) return false;
            thanhPhan[chiSo++] = giaTri;
            phan.clear();
        } else {
            if(!laKyTuSo(c)) return false;
            phan.push_back(c);
        }
    }

    // Phai co dung 2 dau '/', con lai la phan thu 3
    if(phan.empty() || chiSo != 2) return false;
    int giaTri = 0;
    if(!chuyenChuoiThanhSoNguyen(phan, giaTri, true)) return false;
    thanhPhan[chiSo] = giaTri;

    ngay  = thanhPhan[0];
    thang = thanhPhan[1];
    nam   = thanhPhan[2];

    // Chuan hoa nam 2 chu so
    if(nam < 100) nam = (nam <= 30 ? nam + 2000 : nam + 1900);

    // Kiem tra pham vi co ban
    if(thang < 1 || thang > 12) return false;
    if(ngay  < 1 || ngay  > 31) return false;
    if(nam   < 1900 || nam > 2100) return false;

    // Kiem tra so ngay hop le theo thang, co xet nam nhuan
    int soNgayThang[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    bool namNhuan = (nam % 400 == 0) || (nam % 4 == 0 && nam % 100 != 0);
    if(namNhuan) soNgayThang[2] = 29;
    if(ngay > soNgayThang[thang]) return false;

    return true;
}

// Chuan hoa chuoi ngay
bool ChuanHoaNgay(std::string& s, std::ostream& out){
    (void)out; // khong dung tham so out trong ham nay
    s = loaiBoKhoangTrangDauCuoi(s);
    if(s.empty()) return false;
    int ngay = 0, thang = 0, nam = 0;
    if(!phanTichNgay(s, ngay, thang, nam)) return false;
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", ngay, thang, nam);
    s.assign(buf);
    return true;
}

// ham phu tro cong khai
// Trien khai cac wrapper cong khai

// HAM XU LY KHOANG TRANG TONG HOP
std::string CatKhoangTrang(const std::string& s){
    return loaiBoKhoangTrangDauCuoi(s);
}

// Chuan hoa khoang trang
std::string ChuanHoaKhoangTrang(const std::string& s){
    std::string out; out.reserve(s.size());
    bool prevSpace = false;
    for(char c: s){
        char x = laKyTuKhoangTrang(c) ? ' ' : c;
        if(x==' '){
            if(!prevSpace){ out.push_back(' '); prevSpace = true; }
        } else {
            out.push_back(x); prevSpace = false;
        }
    }
    return loaiBoKhoangTrangDauCuoi(out);
}

bool ChuyenNgaySangTimeT(const std::string& sNgay, time_t& outTime){
    std::string chuoiNgay = CatKhoangTrang(sNgay);
    if(chuoiNgay.empty()) return false;
    int ngay = 0, thang = 0, nam = 0; 
    if(!phanTichNgay(chuoiNgay, ngay, thang, nam)) return false;
    tm thoiGian{}; 
    thoiGian.tm_mday = ngay; 
    thoiGian.tm_mon = thang - 1; 
    thoiGian.tm_year = nam - 1900;
    time_t ketQuaTimeT = mktime(&thoiGian); 
    if(ketQuaTimeT == -1) return false; 
    outTime = ketQuaTimeT; 
    return true;
}

std::string ChuanHoaPhai(const std::string& raw){
    // Normalize string for comparison (merged from chuanHoaPhaiForCompare)
    std::string loc; 
    loc.reserve(raw.size());
    size_t i = 0;
    while(i < raw.size()){
        uint32_t cp = VietnameseUtils::readCodePoint(raw, i);
        if(cp <= 0x7F){
            char c = static_cast<char>(cp);
            if(!laKyTuKhoangTrang(c)){
                c = chuyenThanhThuong(c);
                loc.push_back(c);
            }
            continue;
        }
        // Map tat ca bien the 'U'/'u' (co/khong dau) -> 'u'
        if(cp == 0x01AF || cp == 0x01B0 ||
           cp == 0x1EE8 || cp == 0x1EE9 || cp == 0x1EEA || cp == 0x1EEB ||
           cp == 0x1EEC || cp == 0x1EED || cp == 0x1EEE || cp == 0x1EEF ||
           cp == 0x1EF0 || cp == 0x1EF1){ loc.push_back('u'); continue; }
    }
    
    // Check normalized result and return proper capitalized form
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
    int soToken = 0; 
    std::string token;
    for(char c: s){
        if(laKyTuKhoangTrang(c)){ 
            if(!token.empty()){ 
                if(soToken < maxTok) tokens[soToken++] = token; 
                token.clear(); 
            } 
        }
        else token.push_back(c);
    }
    if(!token.empty() && soToken < maxTok) tokens[soToken++] = token;
    return soToken;
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
    for(int chiSo = 0; chiSo < n; ++chiSo){ 
        PTRDAUSACH dauSach = ds[chiSo]; 
        if(dauSach && dauSach->ISBN == isbn) return dauSach; 
    }
    return nullptr;
}

bool ChenNodeDMSVaoDauSach(PTRDAUSACH d, const std::string& maSach, int tt, const std::string& viTri){
    PTRDMS node = new (std::nothrow) DanhMucSach; 
    if(!node) return false;
    node->maSach = maSach;
    node->trangThai = (TrangThaiSach)tt;
    node->viTri = ChuanHoaKhoangTrang(viTri);
    node->next = nullptr;
    if(!d->dms) {
        d->dms = node; 
    } else { 
        PTRDMS cuoi = d->dms; 
        while(cuoi->next) cuoi = cuoi->next; 
        cuoi->next = node; 
    }
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
        // Cap phat bo nho an toan
        PTRDAUSACH dauSachMoi = new (std::nothrow) DauSach;
        if (!dauSachMoi) {
            thongBao(out, "Không đủ bộ nhớ để tạo đầu sách mới!", LOI);
            return nullptr;
        }
        
        // Khoi tao du lieu
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
        // Dich chuyen cac phan tu de tao cho trong
        for (int i = soLuongDauSach; i > viTri; i--) {
            if (i >= MAX_DAUSACH || (i-1) < 0) {
                thongBao(out, "Lỗi overflow trong quá trình dịch chuyển!", LOI);
                
                return false;
            }
            dsDauSach[i] = dsDauSach[i - 1];
        }
        
        // Chen dau sach moi
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
    
    // In danh sách bản sao (nếu có)
    if (s->dms) {
        std::cout << "\nDANH SÁCH BẢN SAO:\n";
        int tongBanSao = 0;
        for (PTRDMS p = s->dms; p; p = p->next) {
            ++tongBanSao;
            std::string tenTrangThai = TenTrangThai(p->trangThai);
            std::string viTriHienThi = p->viTri.empty() ? "Chưa xác định" : p->viTri;
            std::cout << "  " << tongBanSao << ". Mã: " << p->maSach
                      << " | Trạng thái: " << tenTrangThai
                      << " | Vị trí: " << viTriHienThi << "\n";
        }
        std::cout << "Tổng số bản sao: " << tongBanSao << "\n";
    } else {
        std::cout << "\nChưa có bản sao nào\n";
    }
    std::cout << "\n";
}

// Kiem tra tieu de (khong dau) co chua tu khoa (khong dau) hoac chua tat ca cac token khong
bool KhopTieuDeKhongDau(const std::string& tieuDe, const std::string& tuKhoa,
                        const std::string tokens[], int soToken) {
    // Neu tu khoa khong rong va xuat hien trong tieu de thi khop
    if (!tuKhoa.empty() && tieuDe.find(tuKhoa) != std::string::npos) {
        return true;
    }
    // Neu khong, kiem tra tieu de co chua tat ca cac token khong
    return ChuaTatCaTuKhoa(tieuDe, tokens, soToken);
}

// Nap du lieu tu file
void NapDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int& soLuongDauSach){
    std::ifstream fin(path);
    if(!fin){ thongBao(std::cout, std::string("Không mở được ") + path, CANH_BAO); return; }
    // Dung kiem tra trung trong bo nho de tranh lap, khong can mang phu

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
        if(!chuyenChuoiThanhSoNguyen(CatKhoangTrang(f[2]), soTrang, false) || !chuyenChuoiThanhSoNguyen(CatKhoangTrang(f[4]), nam, false)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng số: " + line), CANH_BAO); ++bookSkip; continue;
        }
        std::string isbnChuan;
        if(!ChuanHoaISBNFile(f[0], isbnChuan, std::cout)) {
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng ISBN, bỏ qua."), CANH_BAO);
            ++bookSkip; continue;
        }
        // Kiem tra trung trong bo nho (da co) de tranh lap
        if(TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "ISBN trùng (" + isbnChuan + "), bỏ qua."), CANH_BAO);
            ++bookSkip; continue;
        }

        std::string tenChuan, tgChuan, tlChuan;
        if(!ChuanHoaTenUnicode(f[1], MIN_TEN_SACH, MAX_TEN_SACH, std::cout, tenChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng tên sách, bỏ qua."), CANH_BAO); ++bookSkip; continue;
        }
        if(!ChuanHoaTenUnicode(f[3], MIN_TAC_GIA, MAX_TAC_GIA, std::cout, tgChuan)){
            thongBao(std::cout, thongBaoLoi(lineNo, "Sai định dạng tác giả, bỏ qua."), CANH_BAO); ++bookSkip; continue;
        }
        if(!ChuanHoaTenUnicode(f[5], MIN_THE_LOAI, MAX_THE_LOAI, std::cout, tlChuan)){
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

// Ham giai phong bo nho danh sach ma sach
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
        // Giai phong linked list danh muc sach
        GiaiPhongDanhMucSach(dauSach->dms);
        // Giai phong dau sach
        delete dauSach;
        dauSach = nullptr;
    }
}

// ghi du lieu ra file

// ham backup an toan
bool BackupTruocKhiGiaiPhong(PTRDAUSACH dsDauSach[], int soLuongDauSach) {
    if(soLuongDauSach <= 0) return true; // Khong co gi de backup
    
    // Kiem tra xem co file backup gan day khong (trong vong 1 phut)
    time_t now = time(nullptr);
    tm* timeinfo = localtime(&now);
    char currentTime[32];
    strftime(currentTime, sizeof(currentTime), "%Y%m%d_%H%M", timeinfo);
    
    // Tao pattern de tim backup gan day  
    std::string pattern = std::string("data/backup_") + currentTime;
    
    // Tao backup voi timestamp day du chi khi can
    char fullTimestamp[32];
    strftime(fullTimestamp, sizeof(fullTimestamp), "%Y%m%d_%H%M%S", timeinfo);
    
    std::string backupPath = std::string("data/backup_") + fullTimestamp + "_dausach.txt";
    std::string backupDMSPath = std::string("data/backup_") + fullTimestamp + "_dms.txt";
    
    bool success = true;
    if(!GhiDanhSachDauSach(backupPath.c_str(), dsDauSach, soLuongDauSach, true)) {
        success = false;
    }
    
    if(!GhiDanhMucSach(backupDMSPath.c_str(), dsDauSach, soLuongDauSach, true)) {
        success = false;
    }
    
    return success;
}

// Giai phong toan bo danh sach dau sach
void GiaiPhongToanBoDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach) {
    if(soLuongDauSach <= 0) {
        return;
    }
    
    // Giai phong bo nho
    for(int i = 0; i < soLuongDauSach; i++) {
        if(dsDauSach[i]) {
            GiaiPhongDauSach(dsDauSach[i]);
        }
    }
    soLuongDauSach = 0;
}

bool ThemMaSachDaXuLy(std::string*& danhSachDaXuLy, int& soLuongDaXuLy, int& dungLuongToiDa, const std::string& maSach) {
    // Mo rong mang neu can
    if(soLuongDaXuLy >= dungLuongToiDa) {
        int dungLuongMoi = (dungLuongToiDa == 0) ? DUNG_LUONG_CACHE_MAC_DINH : (dungLuongToiDa * 2);
        std::string* mangMoi = new (std::nothrow) std::string[dungLuongMoi];
        if(!mangMoi) {
            thongBao(std::cout, "Không đủ bộ nhớ để mở rộng danh sách theo dõi mã sách.", CANH_BAO);
            return false; // Tra ve false de bao loi
        }
        
        // Sao chep du lieu cu
        for(int i = 0; i < soLuongDaXuLy; i++) {
            mangMoi[i] = danhSachDaXuLy[i];
        }
        
        // Giai phong bo nho cu an toan
        delete[] danhSachDaXuLy;
        danhSachDaXuLy = mangMoi;
        dungLuongToiDa = dungLuongMoi;
    }
    
    danhSachDaXuLy[soLuongDaXuLy++] = maSach;
    return true;
}

// cap nhat thong tin sach
static inline void capNhatThongTinSach(PTRDMS sach, int trangThai, const std::string& viTri) {
    sach->trangThai = (TrangThaiSach)trangThai;
    sach->viTri = ChuanHoaKhoangTrang(viTri);
}

void NapDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach){
    // Kiem tra dieu kien tien quyet truoc khi cap phat bo nho
    std::ifstream fdm(path);
    if(!fdm){ 
    thongBao(std::cout, std::string("Không mở được ") + path, CANH_BAO); 
        return; 
    }
    
    if(soLuongDauSach <= 0){
    thongBao(std::cout, "Chưa có đầu sách trong bộ nhớ, bỏ qua nạp danh mục sách.", CANH_BAO);
        return;
    }
    
    // Theo doi ma sach da xu ly trong file nay - su dung scope de tu dong cleanup
    {
        std::string* danhSachMaSachDaXuLy = nullptr; 
        int soLuongMaSachDaXuLy = 0; 
        int dungLuongToiDa = 0;
        
        // Khoi tao bien dem va theo doi
        std::string line; 
        int soThuTuDong = 0; 
        int tongSoDong = 0; 
        int soLuongThanhCong = 0; 
        int soLuongBoQua = 0; 
        int soLuongCapNhat = 0;
    // Doc va xu ly tung dong
    while(std::getline(fdm, line)){
        soThuTuDong++;
        
        // Bo qua dong trong
        if(line.empty()) continue; 
        tongSoDong++;

        // Tach cac truong du lieu
        std::string cacTruong[3];
        if(!TachTruong(line, '|', cacTruong, 3)){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng: " + line), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }

        const std::string& maSach = cacTruong[0];
        const std::string& trangThaiStr = cacTruong[1];
        const std::string& viTri = cacTruong[2];
        
        // Kiem tra dinh dang ma sach: <ISBN-13>-<so thu tu duong>
        size_t viTriGachNgang = maSach.find('-');
        if(viTriGachNgang == std::string::npos){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (thiếu '-'), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        // Tach phan ISBN va so thu tu
        std::string phanISBN = maSach.substr(0, viTriGachNgang);
        std::string phanSoThuTu = maSach.substr(viTriGachNgang + 1);
        // Kiem tra ISBN co dung 10 hoac 13 chu so khong
        if(phanISBN.size() != 10 && phanISBN.size() != 13) {
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (ISBN phải 10 hoặc 13 chữ số), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiem tra ISBN co phai toan so khong
        bool laToanSo = true;
        for(char kyTu : phanISBN) {
            if(!laKyTuSo(kyTu)) {
                laToanSo = false;
                break;
            }
        }
        if(!laToanSo){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (ISBN không phải số), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiem tra so thu tu
        int soThuTu = 0; 
        if(phanSoThuTu.empty() || !chuyenChuoiThanhSoNguyen(CatKhoangTrang(phanSoThuTu), soThuTu, false) || soThuTu <= 0){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Sai định dạng mã sách (số thứ tự > 0), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiem tra so thu tu khong qua lon (tranh overflow)
    if(soThuTu > MAX_BAN_SAO){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Số thứ tự quá lớn (>" + std::to_string(MAX_BAN_SAO) + "), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiem tra trang thai sach
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
        
        // Kiem tra vi tri khong qua dai
        std::string viTriSauCat = CatKhoangTrang(viTri);
        if(viTriSauCat.length() > DO_DAI_VI_TRI_TOI_DA) {
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Vị trí quá dài (>" + std::to_string(DO_DAI_VI_TRI_TOI_DA) + " ký tự), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }

        std::string isbn = phanISBN;
        // Tim dau sach theo ISBN
        PTRDAUSACH dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        if(!dauSach){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "ISBN không thuộc danh sách đầu sách đã nạp (" + isbn + "), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Kiem tra khong vuot qua gioi han so ban sao
        int soBanSaoHienTai = 0;
        for(PTRDMS temp = dauSach->dms; temp; temp = temp->next) soBanSaoHienTai++;
    if(soBanSaoHienTai >= MAX_BAN_SAO){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Đã đạt giới hạn số bản sao (" + std::to_string(MAX_BAN_SAO) + "), bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }

        // Kiem tra xem ma sach da ton tai trong bo nho chua
        PTRDMS sachDaTonTai = timDanhMucTheoMaSach(maSach);
        if(sachDaTonTai){
            // Cap nhat thong tin sach da co
            capNhatThongTinSach(sachDaTonTai, trangThai, viTri);
            soLuongCapNhat++; 
            soLuongThanhCong++;
            continue;
        }

        // Kiem tra trung ma sach trong file nay
        if(KiemTraMaSachDaXuLy(danhSachMaSachDaXuLy, soLuongMaSachDaXuLy, maSach)){
            // Tim va cap nhat ban ghi da duoc them truoc do trong file
            PTRDMS sachTrongFile = timDanhMucTheoMaSach(maSach);
            if(sachTrongFile){
                capNhatThongTinSach(sachTrongFile, trangThai, viTri);
                soLuongCapNhat++; 
                soLuongThanhCong++;
                continue;
            } else {
                // Truong hop bat thuong - ma da duoc xu ly nhung khong tim thay
                thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Trùng mã sách trong file (" + maSach + "), bỏ qua."), CANH_BAO);
                soLuongBoQua++; 
                continue;
            }
        }

        // Them sach moi vao danh muc
        if(!ChenNodeDMSVaoDauSach(dauSach, maSach, trangThai, viTri)){
            thongBao(std::cout, thongBaoLoiDMS(soThuTuDong, "Không đủ bộ nhớ để thêm DMS, bỏ qua."), CANH_BAO);
            soLuongBoQua++; 
            continue;
        }
        
        // Danh dau ma sach da duoc xu ly
        if(!ThemMaSachDaXuLy(danhSachMaSachDaXuLy, soLuongMaSachDaXuLy, dungLuongToiDa, maSach)) {
            // Neu khong the them vao danh sach theo doi, van coi nhu thanh cong
            // vi sach da duoc them vao he thong, chi la khong theo doi duoc trung lap
        }
        soLuongThanhCong++;
    }
    
        // In tong ket
        InTongKet("Nạp bản sao (DMS)", soLuongThanhCong, tongSoDong, soLuongBoQua);
        if(soLuongCapNhat > 0) {
            thongBao(std::cout, std::string("(Trong đó cập nhật: ") + std::to_string(soLuongCapNhat) + ")", THONG_TIN);
        }
        
        // Giai phong bo nho an toan
        GiaiPhongDanhSachMaSach(danhSachMaSachDaXuLy);
    } // Ket thuc scope - dam bao tat ca bien local duoc huy
}


// Ghi du lieu ra file
bool GhiDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, bool silent){
    std::ofstream fout(path, std::ios::trunc);
    if(!fout){ 
        if(!silent) thongBao(std::cout, std::string("Không mở được ") + path + " để ghi!", LOI); 
        return false; 
    }
    for(int i=0;i<soLuongDauSach;++i){ PTRDAUSACH d = dsDauSach[i]; if(!d) continue;
        fout << d->ISBN << '|' << d->tenSach << '|' << d->soTrang << '|' << d->tacGia << '|' << d->namXuatBan << '|' << d->theLoai << "\n";
    }
    return true;
}

bool GhiDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, bool silent){
    std::ofstream fdm(path, std::ios::trunc);
    if(!fdm){ 
        if(!silent) thongBao(std::cout, std::string("Không mở được ") + path + " để ghi!", LOI); 
        return false; 
    }
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

// nhap lieu tu nguoi dung

// Nhap ten voi nhan tuy chinh
std::string NhapTenNhan(const char* nhan, size_t minLen, size_t maxLen, std::istream& in, std::ostream& out){
    std::string goc; 
    int soLanThu = 0;
    while(soLanThu < SO_LAN_THU_TOI_DA){
        out << "Nhập " << (nhan ? nhan : "tên") << " (" << minLen << "-" << maxLen << " ký tự): ";
        std::getline(in, goc);
        kiemTraEOF(in);
        std::string gocCat = CatKhoangTrang(goc);
        if(gocCat.empty()) {
            thongBao(out, (nhan ? nhan : "Trường") + std::string(" không được rỗng!"), LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out);
            continue;
        }
        std::string chuan;
        if(ChuanHoaTenUnicode(goc, minLen, maxLen, out, chuan)) {
            return chuan;
        }
        ++soLanThu; 
        thongBaoSoLanThuConLai(soLanThu, out);
    }
    nemLoiQuaSoLanNhap();
}

// Nhap so nguyen voi nhan tuy chinh
int NhapSoNguyenNhan(const char* nhan, int minVal, int maxVal, std::istream& in, std::ostream& out){
    std::string dong; 
    int soLanThu = 0; 
    int giaTri = 0;
    while(soLanThu < SO_LAN_THU_TOI_DA){
        out << "Nhập " << (nhan ? nhan : "giá trị") << " (" << minVal << "-" << maxVal << "): ";
        std::getline(in, dong);
        kiemTraEOF(in);
        std::string sauCat = CatKhoangTrang(dong);
        if(sauCat.empty()) {
            thongBao(out, (nhan ? nhan : "Trường") + std::string(" không được rỗng!"), LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out);
            continue;
        }
        if(sauCat.length() > 30) { 
            thongBao(out, "Quá dài!", LOI); 
            ++soLanThu; 
        } else if(!chuyenChuoiThanhSoNguyen(sauCat, giaTri, false)) { 
            thongBao(out, "Sai định dạng số!", LOI); 
            ++soLanThu; 
        } else {
            if(giaTri < minVal || giaTri > maxVal) { 
                (void)thongBaoLoiNgoaiPhamVi(out, (nhan ? nhan : "Giá trị"), minVal, maxVal); 
                ++soLanThu; 
            } else {
                return giaTri;
            }
        }
        thongBaoSoLanThuConLai(soLanThu, out);
    }
    nemLoiQuaSoLanNhap();
}



// nhap isbn thu cong
std::string NhapISBNThuCong(std::istream& in, std::ostream& out) {
    int soLanThu = 0;
    while (soLanThu < SO_LAN_THU_TOI_DA) {
        out << "Nhập ISBN (10 hoặc 13 ký tự): ";
        std::string isbnRaw;
        std::getline(in, isbnRaw);
        kiemTraEOF(in);
        std::string isbnChuan;
        if (!ChuanHoaISBNCore(isbnRaw, isbnChuan, true, true, false, out)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out);
            continue;
        }
        if (KiemTraTrungISBN(isbnChuan, out)) {
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out);
            continue;
        }
        return isbnChuan;
    }
    nemLoiQuaSoLanNhap();
    return std::string();
}
bool KiemTraTrungISBN(const string& ISBN, ostream& out) {
    bool coDauSach = (TimDauSachTheoISBN(dsDauSach, soLuongDauSach, ISBN) != nullptr);
    if(coDauSach) thongBao(out, "Trùng ISBN!", LOI);
    return coDauSach;
}

bool KiemTraChuoiRong(const string& chuoi, const string& tenTruong, ostream& out){
    if(chuoi.empty()) { thongBao(out, tenTruong + " không được rỗng!", LOI); return false; }
    if(laChuoiRongHoacChiKhoangTrang(chuoi)) { thongBao(out, tenTruong + " chỉ khoảng trắng!", LOI); return false; }
    return true;
}

string ChuyenInThuong(const string& dauVao){
    string kq = dauVao;
    for(char &c: kq) c = chuyenThanhThuong(c);
    return kq;
}

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
            if(VietnameseUtils::isVietnameseLetter(cp)){
                uint32_t low = VietnameseUtils::toLower(cp);
                switch(low){
                    case 0x0061: case 0x00E0: case 0x00E1: case 0x1EA3: case 0x00E3: case 0x1EA1:
                    case 0x0103: case 0x1EB1: case 0x1EAF: case 0x1EB3: case 0x1EB5: case 0x1EB7:
                    case 0x00E2: case 0x1EA7: case 0x1EA5: case 0x1EA9: case 0x1EAB: case 0x1EAD:
                        ascii = 'a'; break;
                    case 0x0065: case 0x00E8: case 0x00E9: case 0x1EBB: case 0x1EBD: case 0x1EB9:
                    case 0x00EA: case 0x1EC1: case 0x1EBF: case 0x1EC3: case 0x1EC5: case 0x1EC7:
                        ascii = 'e'; break;
                    case 0x0069: case 0x00EC: case 0x00ED: case 0x1EC9: case 0x0129: case 0x1ECB:
                        ascii = 'i'; break;
                    case 0x006F: case 0x00F2: case 0x00F3: case 0x1ECF: case 0x00F5: case 0x1ECD:
                    case 0x00F4: case 0x1ED3: case 0x1ED1: case 0x1ED5: case 0x1ED7: case 0x1ED9:
                    case 0x01A1: case 0x1EDD: case 0x1EDB: case 0x1EDF: case 0x1EE1: case 0x1EE3:
                        ascii = 'o'; break;
                    case 0x0075: case 0x00F9: case 0x00FA: case 0x1EE7: case 0x0169: case 0x1EE5:
                    case 0x01B0: case 0x1EEB: case 0x1EE9: case 0x1EED: case 0x1EEF: case 0x1EF1:
                        ascii = 'u'; break;
                    case 0x0079: case 0x1EF3: case 0x00FD: case 0x1EF7: case 0x1EF9: case 0x1EF5:
                        ascii = 'y'; break;
                    case 0x0111:
                        ascii = 'd'; break;
                    default:
                        ascii = 0;
                }
            }
        }
        if(ascii){
            if(ascii == '\t' || ascii == '\n' || ascii == '\r' || ascii == '\v' || ascii == '\f') ascii = ' ';
            out.push_back(ascii);
        } else if(cp <= 0x7F && (char)cp == ' '){
            out.push_back(' ');
        }
    }
    return ChuanHoaKhoangTrang(out);
}

ThongTinSachChuanHoa ChuanHoaDuLieuSach(const std::string& tenSach, const std::string& tacGia, 
                                        const std::string& theLoai, const std::string& isbn) {
    ThongTinSachChuanHoa ketQua;
    ketQua.tenSach = BoDauVaThuong(tenSach);
    ketQua.tacGia = BoDauVaThuong(tacGia);
    ketQua.theLoai = BoDauVaThuong(theLoai);
    ketQua.isbn = BoDauVaThuong(isbn);
    return ketQua;
}

string NhapPhai(istream& in, ostream& out){
    string phaiNhapVao; 
    int soLanThu = 0;
    while(soLanThu < SO_LAN_THU_TOI_DA){
        out << "Nhập phái (Nam/Nữ): ";
        getline(in, phaiNhapVao);
        std::string phaiCat = CatKhoangTrang(phaiNhapVao);
        if(phaiCat.empty()) {
            thongBao(out, "Phái không được rỗng!", LOI);
            ++soLanThu;
            thongBaoSoLanThuConLai(soLanThu, out);
            continue;
        }
        string phaiChuanHoa = ChuanHoaPhai(phaiNhapVao);
        if(!phaiChuanHoa.empty()) {
            return phaiChuanHoa;
        }
        thongBao(out, "Chỉ Nam hoặc Nữ!", LOI); 
        ++soLanThu;
        InputUtils::thongBaoSoLanThuConLai(soLanThu, out);
    }
    InputUtils::nemLoiQuaSoLanNhap();
}

bool KiemTraTrangThaiThe(int trangThai, ostream& out) {
    if (trangThai == 0 || trangThai == 1) return true;
    InputUtils::thongBao(out, "Trạng thái thẻ phải 0 hoặc 1!", InputUtils::LOI);
    return false;
}

bool KiemTraMaThe(const string& maThe, ostream& out) {
    int giaTri = 0;
    if (!InputUtils::chuyenChuoiThanhSoNguyen(InputUtils::CatKhoangTrang(maThe), giaTri, true)) { 
        InputUtils::thongBao(out, "Mã thẻ không hợp lệ!", InputUtils::LOI); 
        return false; 
    }
    return true;
}

string SinhMaTheNgauNhien(size_t length, ostream& out) {
    (void)out;
    return InputUtils::taoChuoiSoNgauNhien(length);
}

int checksumEAN13(const std::string& s12){
    int tongTichLuy = 0;
    for(size_t i = 0; i < DO_DAI_ISBN_CHECKSUM; ++i) { 
        int chuSo = s12[i] - '0'; 
        tongTichLuy += (i % 2 == 0) ? chuSo : chuSo * 3; 
    }
    int chuSoKiemTra = (10 - (tongTichLuy % 10)) % 10; 
    return chuSoKiemTra;
}


// xu ly isbn

// kiem tra va chuan hoa isbn
static bool ChuanHoaISBNCore(const std::string& chuoiGoc, std::string& ketQua, bool choPhepSua, bool hienThiCanhBao, bool imLang, std::ostream& out){
    // Lay chuoi so va kiem tra ky tu khong phai so
    std::string chuSo = layChiCacKyTuSo(chuoiGoc);
    bool coKyTuKhongPhaiSo = false;
    for (char c : chuoiGoc) {
        if (!isdigit(c) && !isspace(c) && c != '-' && c != '_') {
            coKyTuKhongPhaiSo = true;
            break;
        }
    }
    if (coKyTuKhongPhaiSo) {
        if(!imLang) thongBao(out, "Sai định dạng ISBN: chỉ được phép nhập số (0-9), dấu gạch ngang (-), gạch dưới (_), hoặc khoảng trắng!", LOI);
        return false;
    }
    // Chi chap nhan ISBN co dung 10 hoac 13 chu so
    if(chuSo.length() != 10 && chuSo.length() != 13) {
        if(!imLang) thongBao(out, "Sai định dạng ISBN: phải nhập đúng 10 hoặc 13 chữ số! Bạn đã nhập " + std::to_string(chuSo.length()) + " chữ số.", LOI);
        return false;
    }
    // Neu la ISBN-10 thi khong kiem tra checksum, chi nhan dang
    if(chuSo.length() == 10) {
        ketQua = chuSo;
        return true;
    }

    
    int checksumTinhToan = InputUtils::checksumEAN13(chuSo.substr(0, DO_DAI_ISBN_CHECKSUM));
    int checksumThucTe = chuSo[DO_DAI_ISBN_CHECKSUM] - '0';

    // Kiểm tra checksum có đúng không
    if(checksumTinhToan != checksumThucTe) {
        if(!imLang) {
            thongBao(out, "Sai định dạng ISBN: số kiểm tra (checksum) không đúng! Số kiểm tra đúng phải là " 
                + std::to_string(checksumTinhToan) + ", bạn nhập: " + std::to_string(checksumThucTe), LOI);
        }
        return false;
    }
    ketQua = chuSo;
    return true;
}

// doc isbn tu file
bool ChuanHoaISBNFile(const std::string& chuoiGoc, std::string& ketQua, std::ostream& out){
    return ChuanHoaISBNCore(chuoiGoc, ketQua, 
        true,   // choPhepSua: cho phep tu dong sua checksum sai
        false,  // hienThiCanhBao: khong hien thi canh bao khi sua
        true,   // imLang: khong in loi chi tiet (caller se xu ly)
        out);
}
/*
std::string SinhISBN13TuDong(){
    // Sinh ISBN ngau nhien theo dinh dang Viet Nam: 978604-XXX-YYY-Z
    // Trong do:
    //   978604: Ma quoc gia Viet Nam (co dinh)
    //   XXX: Ma nha xuat ban (1-3 chu so ngau nhien)
    //   YYY: Ma dau sach (5-3 chu so ngau nhien, bu voi XXX de du 6 chu so)
    //   Z: Chu so kiem tra (checksum)
    
    for(int lanThu = 0; lanThu < MAX_BAN_SAO; ++lanThu) {
        // Chon ngau nhien do dai ma NXB (1, 2 hoac 3 chu so)
        int doDaiMaNXB = 1 + (rand() % 3);
        int doDaiMaDauSach = 6 - doDaiMaNXB;
        
        // Tao 12 chu so dau: 978604 + ma NXB + ma dau sach
        std::string isbn12ChuSo = std::string(MA_QUOC_GIA_VIET_NAM) 
                                + taoChuoiSoNgauNhien(doDaiMaNXB) 
                                + taoChuoiSoNgauNhien(doDaiMaDauSach);
        
        // Tinh chu so kiem tra (chu so thu 13)
    int chuSoKiemTra = InputUtils::checksumEAN13(isbn12ChuSo);
        std::string isbnDayDu = isbn12ChuSo + char('0' + chuSoKiemTra);
        
        // Kiem tra xem ISBN nay da ton tai chua
        if(!TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnDayDu)) {
            return isbnDayDu;  // Tim thay ISBN duy nhat, tra ve
        }
    }
    
    // Truong hop hiem: khong tim duoc ISBN duy nhat sau nhieu lan thu
    // Tra ve ISBN ngau nhien (co the trung)
    return taoChuoiSoNgauNhien(DO_DAI_ISBN);
}
*/

bool KiemTraTongSoBanSao(const string& isbn, int soLuongThem, ostream& out){
    // Dem so ban sao hien tai cua dau sach nay
    int soBanSaoHienTai = 0;
    PTRDAUSACH dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    
    if(dauSach) {
        // Duyet qua danh sach lien ket de dem
        for(DanhMucSach* p = dauSach->dms; p != nullptr; p = p->next) {
            soBanSaoHienTai++;
        }
    }
    
    // Kiem tra tong so sau khi them co vuot qua gioi han khong
    int tongSoSauKhiThem = soBanSaoHienTai + soLuongThem;
    if(tongSoSauKhiThem > MAX_BAN_SAO) { 
        thongBao(out, "Tổng số bản sao > " + std::to_string(MAX_BAN_SAO) + "!", LOI);
        return false; 
    }
    
    return true;
}

bool KiemTraTrungmaSach(const string& maSach, ostream& out) {
    bool coMaSach = (timDanhMucTheoMaSach(maSach) != nullptr);
    if(coMaSach) thongBao(out, "Trùng mã sách!", LOI);
    return coMaSach;
}



// sinh ma va chuoi ngau nhien

// Khoi tao seed ngau nhien (chi mot lan)
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
    // Định nghĩa mặc định nếu chưa có
    #ifndef MAX_LAN_THU_SINH_MA
    #define MAX_LAN_THU_SINH_MA 100
    #endif
    while(KiemTraTrungmaSach(ma, out) && soLanDieuChinh < MAX_LAN_THU_SINH_MA){
        ++soLanDieuChinh; ++soThuTu; ma = chiSo + "-" + to_string(soThuTu);
    }
    // khong in thong bao dieu chinh ma sach
    return ma;
}

int sinhMaThe(PTRDG goc, ostream& out){
    (void)goc;
    static int coSo = (int)(time(nullptr) % 100000);
    ++coSo;
    thongBao(out, std::string("Sinh mã thẻ mới: ") + to_string(coSo), THONG_TIN);
    return coSo;
}
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out) {
    if (!danhSach) {
        InputUtils::thongBao(out, tenDS + " không hợp lệ (null pointer)!", InputUtils::LOI);
        return false;
    }
    if (soLuong < 0) {
        InputUtils::thongBao(out, "Số lượng " + tenDS + " không thể âm (" + std::to_string(soLuong) + ")!", InputUtils::LOI);
        return false;
    }
    if (soLuong > maxSize) {
        InputUtils::thongBao(out, tenDS + " vượt quá giới hạn (" + std::to_string(soLuong) + "/" + std::to_string(maxSize) + ")!", InputUtils::LOI);
        return false;
    }
    return true;
}

bool KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen, std::ostream& out) {
    if (!InputUtils::KiemTraChuoiRong(str, fieldName, out)) {
        return false;
    }
    if (str.length() > static_cast<size_t>(maxLen)) {
        InputUtils::thongBao(out, fieldName + " quá dài (tối đa " + std::to_string(maxLen) + " ký tự)!", InputUtils::LOI);
        return false;
    }
    return true;
}

int LayNamHienTai() {
    time_t now = time(nullptr);
    tm* current_time = localtime(&now);
    int namHienTai = current_time ? (current_time->tm_year + 1900) : 2025;
    if (namHienTai < 1900 || namHienTai > 3000) {
        return 2025; 
    }
    return namHienTai;
}

bool KiemTraNamXuatBan(int nam, std::ostream& out) {
    int namHienTai = InputUtils::LayNamHienTai();
    if (nam < ::NAM_XUAT_BAN_MIN || 
        nam > namHienTai + ::NAM_XUAT_BAN_OFFSET_MAX) {
        InputUtils::thongBao(out, "Năm xuất bản không hợp lý!", InputUtils::LOI);
        return false;
    }
    return true;
}

bool kiemTraQuaHan(std::string ngayMuon, std::ostream& out){
    time_t thoiDiemMuon;
    if(!InputUtils::ChuyenNgaySangTimeT(ngayMuon, thoiDiemMuon)) return false;
    time_t hienTai = time(nullptr);
    double khoangCachGiay = difftime(hienTai, thoiDiemMuon);
    bool quaHan = khoangCachGiay > ::SO_NGAY_MUON_TOI_DA * 24 * 3600; // > n ngay
    if(quaHan){
        int soNgayMuon = static_cast<int>(khoangCachGiay / 86400);
        int soNgayQuaHan = soNgayMuon - ::SO_NGAY_MUON_TOI_DA;
        if(soNgayQuaHan < 0) soNgayQuaHan = 0;
        InputUtils::thongBao(out, std::string("Quá hạn ") + std::to_string(soNgayQuaHan) + " ngày!", InputUtils::CANH_BAO);
    }
    return quaHan;
}

std::string layNgayHienTai(std::ostream& out){
    time_t hienTai = time(nullptr);
    if(hienTai == (time_t)-1)
        throw std::runtime_error("Lỗi: Không lấy được thời gian!");
    tm* tp = localtime(&hienTai);
    if(!tp)
        throw std::runtime_error("Lỗi: Không chuyển đổi được thời gian!");
    char buf[11];
    if(strftime(buf, sizeof(buf), "%d/%m/%Y", tp) == 0)
        throw std::runtime_error("Lỗi: Không định dạng được ngày!");
    std::string ngayChuoi(buf);
    InputUtils::thongBao(out, std::string("Ngày hiện tại: ") + ngayChuoi, InputUtils::THONG_TIN);
    return ngayChuoi;
}

int tinhSoNgayQuaHan(std::string ngayMuon, std::ostream& out){
    time_t thoiDiemMuon;
    if(!InputUtils::ChuyenNgaySangTimeT(ngayMuon, thoiDiemMuon)) return -1;
    time_t hienTai = time(nullptr);
    if(hienTai == (time_t)-1){
        InputUtils::thongBao(out, "Không lấy được thời gian hiện tại!", InputUtils::LOI);
        return -1;
    }
    int soNgayMuon = static_cast<int>( difftime(hienTai, thoiDiemMuon) / 86400 );
    int soNgayQuaHan = soNgayMuon > ::SO_NGAY_MUON_TOI_DA ? soNgayMuon - ::SO_NGAY_MUON_TOI_DA : 0;
    if(soNgayQuaHan > 0)
        InputUtils::thongBao(out, std::string("Quá hạn ") + std::to_string(soNgayQuaHan) + " ngày (đã mượn " + std::to_string(soNgayMuon) + " ngày).", InputUtils::THONG_TIN);
    else
        InputUtils::thongBao(out, std::string("Chưa quá hạn (") + std::to_string(soNgayMuon) + " ngày).", InputUtils::THONG_TIN);
    return soNgayQuaHan;
}
} // namespace InputUtils