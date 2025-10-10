#include "include/XuLyChuoi.h"
#include "include/NgayThang.h"
#include "include/ThongBao.h"
#include "Constants.h"
#include "../include/VietnameseUtils.h"
#include <stdexcept>

// Tiện ích nhỏ (hỗ trợ)
inline bool laKyTuSo(char c) { return c >= '0' && c <= '9'; }
inline bool laKyTuChuHoa(char c) { return c >= 'A' && c <= 'Z'; }  
inline bool laKyTuChuThuong(char c) { return c >= 'a' && c <= 'z'; }
inline bool laKyTuChuCai(char c) { return laKyTuChuHoa(c) || laKyTuChuThuong(c); }
inline bool laKyTuKhoangTrang(char c) { return c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v'; }
inline char chuyenThanhThuong(char c) { return laKyTuChuHoa(c) ? char(c - 'A' + 'a') : c; }


 std::string loaiBoKhoangTrangDauCuoi(const std::string& s){
    size_t trai = 0, phai = s.size();
    while(trai < phai && laKyTuKhoangTrang(s[trai])) ++trai;
    while(phai > trai && laKyTuKhoangTrang(s[phai-1])) --phai;
    return s.substr(trai, phai - trai);
}

bool laChuoiRongHoacChiKhoangTrang(const std::string& s){
    for(char c: s) {
        if(!laKyTuKhoangTrang(c)) return false;
    }
    return true;
}

int soSanhDoDaiChuoi(const std::string& s, size_t minL, size_t maxL){
    if(s.length() < minL) return -1;
    if(s.length() > maxL) return 1;
    return 0;
}

std::string layChiCacKyTuSo(const std::string& s){
    std::string ketQua; 
    ketQua.reserve(s.size());
    for(char c: s) {
        if(laKyTuSo(c)) ketQua.push_back(c);
    }
    return ketQua;
}


static std::string locChuCaiUnicodeKhoang1(const std::string& s){
    std::string out; out.reserve(s.size());
    bool truocSpace = true;
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
                out.push_back(c); truocSpace = false;
            } else if(c == '+' || c == '#' || c == '*' || c == '-' || c == '_' || 
                     c == '.' || c == '&' || c == '@' || c == '$' || c == '%' ||
                     c == '!' || c == '?' || c == ':' || c == ';' || c == ',' ||
                     c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
                     c == '\'' || c == '"' || c == '/' || c == '\\') {
                out.push_back(c); truocSpace = false;
            }
        } else {
            if(VietnameseUtils::isVietnameseLetter(cp)){
                out.append(s, start, i - start);
                truocSpace = false;
            }
        }
    }
    return CatKhoangTrang(out);
}

// Kiểm tra hợp lệ theo độ dài
static bool hopLeTheoDoDai(const std::string& s, size_t minL, size_t maxL, std::ostream& out, bool themTienToSauLoc){
    int cmp = soSanhDoDaiChuoi(s, minL, maxL);
    if(cmp == 0) return true;
    const char* tienTo = themTienToSauLoc ? "Sau lọc " : "";
    if(cmp < 0) thongBao(out, std::string(tienTo) + "quá ngắn (>= " + std::to_string(minL) + ")!", LOI);
    else        thongBao(out, std::string(tienTo) + "quá dài (<= " + std::to_string(maxL) + ")!", LOI);
    return false;
}

// Chuyển đổi chuỗi thành số nguyên
bool chuyenChuoiThanhSoNguyen(const std::string& chuoi, int& ketQua, bool chiChoPhepSoDuong){
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

// Chuyển in thường
std::string ChuyenInThuong(const std::string& dauVao){
    return VietnameseUtils::toLowerCase(dauVao);
}

// Cắt khoảng trắng
std::string CatKhoangTrang(const std::string& s){
    return loaiBoKhoangTrangDauCuoi(s);
}

// Chuẩn hóa khoảng trắng
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

// Chuẩn hóa phái
std::string ChuanHoaPhai(const std::string& raw){
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
        if(cp == 0x01AF || cp == 0x01B0 ||
           cp == 0x1EE8 || cp == 0x1EE9 || cp == 0x1EEA || cp == 0x1EEB ||
           cp == 0x1EEC || cp == 0x1EED || cp == 0x1EEE || cp == 0x1EEF ||
           cp == 0x1EF0 || cp == 0x1EF1){ loc.push_back('u'); continue; }
    }
    if(loc == "nam") return "Nam";
    if(loc == "nu") return "Nữ";
    return std::string();
}


// Chuẩn hóa ISBN từ file
bool ChuanHoaISBNFile(const std::string& chuoiGoc, std::string& ketQua, std::ostream& out){
    return ChuanHoaISBNCore(chuoiGoc, ketQua, true, false, true, out);
}

// Chuẩn hóa tên Unicode
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

std::string BoDauVaThuong(const std::string& s){
    std::string noAccents = VietnameseUtils::removeAccents(s);
    return ChuanHoaKhoangTrang(noAccents);
}

// Chuẩn hóa dữ liệu sách
ThongTinSachChuanHoa ChuanHoaDuLieuSach(const std::string& tenSach, const std::string& tacGia, 
                                        const std::string& theLoai, const std::string& isbn) {
    ThongTinSachChuanHoa ketQua;
    ketQua.tenSach = BoDauVaThuong(tenSach);
    ketQua.tacGia = BoDauVaThuong(tacGia);
    ketQua.theLoai = BoDauVaThuong(theLoai);
    ketQua.isbn = BoDauVaThuong(isbn);
    return ketQua;
}

// Tách trường
bool TachTruong(const std::string& line, char sep, std::string out[], int expected){
    int idx = 0; std::string cur;
    for(char c: line){
        if(c == sep){ if(idx < expected) out[idx++] = cur; cur.clear(); }
        else cur.push_back(c);
    }
    if(idx < expected) out[idx++] = cur;
    return idx == expected;
}

// Tách token khoảng trắng
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

// Kiểm tra chứa tất cả từ khóa
bool ChuaTatCaTuKhoa(const std::string& haystack, const std::string tokens[], int count){
    for(int i=0;i<count;++i){ if(haystack.find(tokens[i]) == std::string::npos) return false; }
    return true;
}

// Kiểm tra khớp tiêu đề không dấu
bool KhopTieuDeKhongDau(const std::string& titleNorm, const std::string& kwNorm,
                        const std::string tokens[], int tokenCount){
    return ChuaTatCaTuKhoa(titleNorm, tokens, tokenCount);
}

// Tách ISBN từ mã sách
std::string TachISBNTuMaSach(const std::string& maSach){
    size_t pos = maSach.find('-');
    return (pos == std::string::npos) ? maSach : maSach.substr(0, pos);
}
std::string ChuanHoaViTri(const std::string& s) {
    std::string raw = ChuanHoaKhoangTrang(CatKhoangTrang(s));
    if (raw.empty()) return std::string();

    std::string tokens[8];
    int n = TachTokenKhoangTrang(raw, tokens, 8);
    if (n == 0) return std::string();

    auto containsDigit = [](const std::string& t){ for(char c: t) if (c >= '0' && c <= '9') return true; return false; };
    auto isAsciiLetter = [](char c){ return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };

    std::string firstLower = VietnameseUtils::toLowerCase(tokens[0]);
    if (firstLower == "ke" || firstLower == "kệ" || firstLower == "kê") {
        if (n == 1) return std::string();
        std::string rest = tokens[1];
        for (int i = 2; i < n; ++i) rest += ' ' + tokens[i];
        return std::string("Kệ ") + VietnameseUtils::toUpperCase(rest);
    }
    if (raw.find('-') != std::string::npos && containsDigit(raw)) {
        return std::string("Kệ ") + VietnameseUtils::toUpperCase(raw);
    }
    if (!raw.empty() && isAsciiLetter(raw[0]) && raw.size() > 1 && (raw[1] >= '0' && raw[1] <= '9')) {
        return std::string("Kệ ") + VietnameseUtils::toUpperCase(raw);
    }
    return raw;
}

bool ChuanHoaISBNCore(const std::string& chuoiGoc, std::string& ketQua, bool tuDong, bool nhapThuCong, bool laDocFile, std::ostream& out) {
    std::string chuoi = CatKhoangTrang(chuoiGoc);
    if (chuoi.empty()) {
        thongBao(out, "ISBN không được rỗng!", LOI);
        return false;
    }
    if (tuDong || nhapThuCong || laDocFile) {
        std::string so = layChiCacKyTuSo(chuoi);
        if (so.length() != 10 && so.length() != 13) {
            thongBao(out, "ISBN phải có 10 hoặc 13 chữ số!", LOI);
            return false;
        }
        ketQua = so;
        return true;
    }
    thongBao(out, "Chế độ chuẩn hóa không hợp lệ!", LOI);
    return false;
}