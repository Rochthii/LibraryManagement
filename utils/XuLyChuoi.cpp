#include "XuLyChuoi.h"
#include "NgayThang.h"
#include "ThongBao.h"
#include "Constants.h"
// #include "VietnameseUtils.h" // Da xoa
#include <stdexcept>
#include <cctype>    // Su dung cctype cho (isspace, isalpha, tolower, toupper)

// Tien ich nho (ho tro) 
inline bool laKyTuSo(char c) { return c >= '0' && c <= '9'; }
inline bool laKyTuChuHoa(char c) { return c >= 'A' && c <= 'Z'; }
inline bool laKyTuChuThuong(char c) { return c >= 'a' && c <= 'z'; }
inline bool laKyTuChuCai(char c) { return laKyTuChuHoa(c) || laKyTuChuThuong(c); }
inline bool laKyTuKhoangTrang(char c) { return c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v'; }
inline char chuyenThanhThuong(char c) { return laKyTuChuHoa(c) ? char(c - 'A' + 'a') : c; }

// Ham ChuyenInHoa
std::string ChuyenInHoa(const std::string& dauVao){
    std::string ketQua = dauVao;
    for(size_t i = 0; i < ketQua.length(); ++i) {
        ketQua[i] = std::toupper(static_cast<unsigned char>(ketQua[i]));
    }
    return ketQua;
}


 std::string loaiBoKhoangTrangDauCuoi(const std::string& s){
    size_t trai = 0, phai = s.size();
    // Su dung hai con tro (trai, phai) de tim vi tri bat dau va ket thuc
    while(trai < phai && laKyTuKhoangTrang(s[trai])) ++trai;
    while(phai > trai && laKyTuKhoangTrang(s[phai-1])) --phai;
    return s.substr(trai, phai - trai);
}


bool laChuoiRongHoacChiKhoangTrang(const std::string& s){
    for(size_t i = 0; i < s.length(); ++i) {
        if(!laKyTuKhoangTrang(s[i])) return false;
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
    // Xay dung chuoi moi chi chua cac ky tu '0'-'9'
    for(size_t i = 0; i < s.length(); ++i) {
        if(laKyTuSo(s[i])) ketQua.push_back(s[i]);
    }
    return ketQua;
}



static bool hopLeTheoDoDai(const std::string& s, size_t minL, size_t maxL, std::ostream& out, bool themTienToSauLoc){
    int cmp = soSanhDoDaiChuoi(s, minL, maxL);
    if(cmp == 0) return true;
    const char* tienTo = themTienToSauLoc ? "Sau loc " : "";
    if(cmp < 0) thongBao(out, std::string(tienTo) + "qua ngan (>= " + std::to_string(minL) + ")!", LOI);
    else        thongBao(out, std::string(tienTo) + "qua dai (<= " + std::to_string(maxL) + ")!", LOI);
    return false;
}


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
        // Kiem tra tran sotruoc khi gan vao int
        if(soTichLuy * dau > INT_MAX || soTichLuy * dau < INT_MIN) return false;
    }
    ketQua = int(soTichLuy * dau);
    return true;
}

// Ham ChuyenInThuong
std::string ChuyenInThuong(const std::string& dauVao){
    std::string ketQua = dauVao;
    for(size_t i = 0; i < ketQua.length(); ++i) {
        ketQua[i] = std::tolower(static_cast<unsigned char>(ketQua[i]));
    }
    return ketQua;
}


std::string CatKhoangTrang(const std::string& s){
    return loaiBoKhoangTrangDauCuoi(s);
}

std::string ChuanHoaKhoangTrang(const std::string& s){
    std::string out; out.reserve(s.size());
    bool prevSpace = false; // Su dung co (flag) de theo doi ky tu truoc do
    for(size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        char x = laKyTuKhoangTrang(c) ? ' ' : c;
        if(x==' '){
            // Chi them khoang trang neu ky tu truoc do khong phai khoang trang
            if(!prevSpace){ out.push_back(' '); prevSpace = true; }
        } else {
            out.push_back(x); prevSpace = false;
        }
    }
    // Tra ve chuoi "out" (da duoc xu ly) thay vi chuoi "s" (goc)
    return loaiBoKhoangTrangDauCuoi(out);
}

// Chuyen chuoi thanh dang Title Case (Viet Hoa Chu Cai Dau Moi Tu)
std::string ChuyenThanhTitleCase(const std::string& s) {
    std::string out = ChuyenInThuong(s); // 1. Chuyen tat ca thanh chu thuong
    bool laDauTu = true;

    // 2. Duyet qua chuoi (khong dung range-based for de tuan thu DSA)
    for (size_t i = 0; i < out.length(); ++i) {
        char c = out[i];

        // Neu la ky tu dau tu (flag=true) va la chu cai ('a'-'z')
        if (laDauTu && (c >= 'a' && c <= 'z')) {
            out[i] = c - 'a' + 'A'; // Chuyen thanh chu hoa (vi du: 'a' - 'a' + 'A' = 'A')
            laDauTu = false; // Dat lai co
        }
        // Neu la khoang trang -> ky tu tiep theo se la dau tu
        else if (laKyTuKhoangTrang(c)) {
            laDauTu = true;
        }
        // Neu la ky tu khac (so, chu da viet hoa, ky tu dac biet)
        else {
            laDauTu = false;
        }
    }
    return out;
}


std::string ChuanHoaPhai(const std::string& raw){
    std::string loc = ChuyenInThuong(CatKhoangTrang(raw));
    if(loc == "nam") return "Nam";
    // Chap nhan ca "nu" (go khong dau) va "nữ" (go co dau)
    if(loc == "nu" ) return "Nu";
    return std::string();
}



bool ChuanHoaISBNFile(const std::string& chuoiGoc, std::string& ketQua, std::ostream& out){
    return ChuanHoaISBNCore(chuoiGoc, ketQua, true, false, true, out);
}

// Sửa hàm ChuanHoaTenUnicode 
bool ChuanHoaTenUnicode(const std::string& dauVao, size_t minLength, size_t maxLength,
                        std::ostream& out, std::string& ketQua){
    // B1: Kiem tra rong hoac chi co khoang trang
    if(dauVao.empty()){ thongBao(out, "Khong duoc de trong!", LOI); return false; }
    std::string chuoiDaCat = CatKhoangTrang(dauVao); // Cat khoang trang dau cuoi truoc
    if(chuoiDaCat.empty() || laChuoiRongHoacChiKhoangTrang(chuoiDaCat)){
        thongBao(out, "Chi chua khoang trang!", LOI); return false;
    }

    //B2: kiem tra ky tu dau tien
    // Ky tu dau tien PHAI la chu cai ASCII (a-z, A-Z)
    if (!laKyTuChuCai(chuoiDaCat[0])) {
        thongBao(out, "Ky tu dau tien phai la chu cai!", LOI);
        return false;
    }

    // B3: KIEM TRA CAC KY TU CON LAI
    // Cho phep: Chu cai (a-z, A-Z), So (0-9), Khoang trang, va cac ky tu: + # - . _
    // Bat dau kiem tra tu ky tu THU HAI (chi so 1)
    for (size_t i = 1; i < chuoiDaCat.length(); ++i) {
        char kyTu = chuoiDaCat[i];

        // Kiem tra xem ky tu co thuoc tap hop le khong
        bool kyTuHopLe =
            laKyTuChuCai(kyTu) ||      // La chu cai ASCII?
            laKyTuSo(kyTu) ||          // La so?
            laKyTuKhoangTrang(kyTu) || // La khoang trang?
            kyTu == '+' ||             // La dau + ? vi du: lap trinh c++
            kyTu == '#' ||             // La dau # ? c#
            kyTu == '-' ||             // La dau - ?
            kyTu == '.' ||             // La dau . ? (Vi du: Version 1.0)
            kyTu == '_';               // La dau _ ?

        // Neu ky tu khong hop le -> bao loi
        if (!kyTuHopLe) {
            // Cap nhat thong bao loi cho ro rang hon
            thongBao(out, "Chi duoc chua chu cai, so, khoang trang va cac ky tu: + # - . _ (va ky tu dau phai la chu cai)", LOI);
            return false;
        }
    }

    // B4: Chuan hoa khoang trang giua cac tu
    std::string chuan = ChuanHoaKhoangTrang(chuoiDaCat);

    // B5: Kiem tra do dai sau khi chuan hoa
    if(chuan.empty()){
        thongBao(out, "Sau khi chuan hoa bi rong!", LOI); return false;
    }
    if(!hopLeTheoDoDai(chuan, minLength, maxLength, out, true)) return false;

    // Buoc 6: Gan ket qua va tra ve true
    ketQua = chuan;
    return true;
}


std::string BoDauVaThuong(const std::string& s){
    std::string lower = ChuyenInThuong(s);
    return ChuanHoaKhoangTrang(lower);
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


bool TachTruong(const std::string& line, char sep, std::string out[], int expected){
    int idx = 0; std::string cur;
    // Duyet qua tung ky tu, tach chuoi bang ky tu 'sep'
    for(size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if(c == sep){ if(idx < expected) out[idx++] = cur; cur.clear(); }
        else cur.push_back(c);
    }
    if(idx < expected) out[idx++] = cur; // Them phan con lai
    return idx == expected; // Tra ve true neu so truong khop voi mong doi
}


int TachTokenKhoangTrang(const std::string& s, std::string tokens[], int maxTok){
    int soToken = 0;
    std::string token;
    // Duyet qua tung ky tu, tach chuoi bang khoang trang
    for(size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
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
    // Kiem tra xem 'haystack' co chua TAT CA cac 'tokens' hay khong
    for(int i=0;i<count;++i){ 
        if(haystack.find(tokens[i]) == std::string::npos) return false; }
    return true;
}


bool KhopTieuDeKhongDau(const std::string& titleNorm, const std::string& kwNorm,
                        const std::string tokens[], int tokenCount){
    (void)kwNorm;
    return ChuaTatCaTuKhoa(titleNorm, tokens, tokenCount);
}


std::string TachISBNTuMaSach(const std::string& maSach){
    size_t pos = maSach.find('-');
    return (pos == std::string::npos) ? maSach : maSach.substr(0, pos);
}

std::string ChuanHoaViTri(const std::string& chuoiVao) {
    // Buoc 1: Lam sach chuoi dau vao (bo khoang trang thua, dau cuoi)
    std::string chuoiSach = ChuanHoaKhoangTrang(CatKhoangTrang(chuoiVao));
    if (chuoiSach.empty()) return std::string();

    // Buoc 2: Tach chuoi thanh cac "tu" (token)
    std::string cacTu[8]; // Mang C-style co dinh de luu cac tu
    int soTu = TachTokenKhoangTrang(chuoiSach, cacTu, 8); // Lay so luong tu tach duoc
    if (soTu == 0) return std::string();

    // Dinh nghia cac ham kiem tra (su dung lambda cho de doc)
    // auto chi la tu khoa, khong phai thu vien
    auto coChuSo = [](const std::string& chuoiKiemTra){
        for(size_t i = 0; i < chuoiKiemTra.length(); ++i) {
            if (chuoiKiemTra[i] >= '0' && chuoiKiemTra[i] <= '9') return true;
        }
        return false;
    };
    auto laChuCaiAscii = [](char kyTu){
        return (kyTu >= 'A' && kyTu <= 'Z') || (kyTu >= 'a' && kyTu <= 'z');
    };

    // Buoc 3: Ap dung cac quy tac chuan hoa

    // Quy tac 1: Neu tu dau tien la "ke" (khong phan biet hoa thuong)
    // Vi du: "ke a1" -> "Ke A1", "Ke b-2" -> "Ke B-2"
    std::string tuDauThuong = ChuyenInThuong(cacTu[0]);
    if (tuDauThuong == "ke") {
        if (soTu == 1) return std::string(); // Chi co "Ke" thi khong hop le
        // Lay phan con lai cua chuoi sau tu "ke"
        std::string phanConLai = cacTu[1];
        for (int i = 2; i < soTu; ++i) {
            phanConLai += ' ' + cacTu[i];
        }
        // Tra ve "Ke " + phan con lai chuyen thanh chu hoa
        return std::string("Ke ") + ChuyenInHoa(phanConLai);
    }

    // Quy tac 2: Neu chuoi goc (da lam sach) co dau '-' va co it nhat mot chu so
    // Vi du: "A-01" -> "Ke A-01", "Khu-B-05" -> "Ke KHU-B-05"
    if (chuoiSach.find('-') != std::string::npos && coChuSo(chuoiSach)) {
        // Tra ve "Ke " + chuoi goc chuyen thanh chu hoa
        return std::string("Ke ") + ChuyenInHoa(chuoiSach);
    }

    // Quy tac 3: Neu chuoi goc (da lam sach) bat dau bang mot chu cai, theo sau la mot chu so
    // Vi du: "A1" -> "Ke A1", "b20" -> "Ke B20"
    if (!chuoiSach.empty() && laChuCaiAscii(chuoiSach[0]) && chuoiSach.size() > 1 && (chuoiSach[1] >= '0' && chuoiSach[1] <= '9')) {
        // Tra ve "Ke " + chuoi goc chuyen thanh chu hoa
        return std::string("Ke ") + ChuyenInHoa(chuoiSach);
    }

    // Neu khong roi vao quy tac nao, tra ve chuoi goc (da chuan hoa khoang trang)
    // Vi du: "Tang 3", "Goc tu sach" -> giu nguyen
    return chuoiSach;
}


bool ChuanHoaISBNCore(const std::string& chuoiGoc, std::string& ketQua, bool tuDong, bool nhapThuCong, bool laDocFile, std::ostream& out) {
    std::string chuoi = CatKhoangTrang(chuoiGoc);
    if (chuoi.empty()) {
        thongBao(out, "ISBN khong duoc rong!", LOI);
        return false;
    }
    if (tuDong || nhapThuCong || laDocFile) {
        // Chi lay phan so tu ISBN (loai bo dau gach ngang, khoang trang)
        std::string so = layChiCacKyTuSo(chuoi);
        if (so.length() != 10 && so.length() != 13) {
            thongBao(out, "ISBN phai co 10 hoac 13 chu so!", LOI);
            return false;
        }
        ketQua = so;
        return true;
    }
    thongBao(out, "Che do chuan hoa khong hop le!", LOI);
    return false;
}