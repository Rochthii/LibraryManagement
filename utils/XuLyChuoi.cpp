#include "XuLyChuoi.h"
#include "NgayThang.h"
#include "ThongBao.h"
#include "Constants.h"
#include <stdexcept>
#include <cctype>

//tien ich nho (ho tro) 
inline bool laKyTuSo(char c) { return c >= '0' && c <= '9'; }
inline bool laKyTuChuHoa(char c) { return c >= 'A' && c <= 'Z'; }
inline bool laKyTuChuThuong(char c) { return c >= 'a' && c <= 'z'; }
inline bool laKyTuChuCai(char c) { return laKyTuChuHoa(c) || laKyTuChuThuong(c); }
inline bool laKyTuKhoangTrang(char c) { return c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v'; }
inline char chuyenThanhThuong(char c) { return laKyTuChuHoa(c) ? char(c - 'A' + 'a') : c; }


std::string ChuyenInHoa(const std::string& dauVao){
    std::string ketQua = dauVao;
    for(size_t i = 0; i < ketQua.length(); ++i) {
        ketQua[i] = std::toupper(static_cast<unsigned char>(ketQua[i]));
    }
    return ketQua;
}

 std::string loaiBoKhoangTrangDauCuoi(const std::string& s){
    size_t trai = 0, phai = s.size();
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
    for(size_t i = 0; i < s.length(); ++i) {
        if(laKyTuSo(s[i])) ketQua.push_back(s[i]);
    }
    return ketQua;
}

/* // Ham nay khong duoc su dung, co the xoa hoac de lai (gay warning)
static bool hopLeTheoDoDai(const std::string& s, size_t minL, size_t maxL, std::ostream& out, bool themTienToSauLoc){
    int cmp = soSanhDoDaiChuoi(s, minL, maxL);
    if(cmp == 0) return true;
    const char* tienTo = themTienToSauLoc ? "Sau loc " : "";
    if(cmp < 0) thongBao(out, std::string(tienTo) + "qua ngan (>= " + std::to_string(minL) + ")!", LOI);
    else        thongBao(out, std::string(tienTo) + "qua dai (<= " + std::to_string(maxL) + ")!", LOI);
    return false;
}
*/

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
    bool prevSpace = false;
    for(size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        char x = laKyTuKhoangTrang(c) ? ' ' : c;
        if(x==' '){
            if(!prevSpace){ out.push_back(' '); prevSpace = true; }
        } else {
            out.push_back(x); prevSpace = false;
        }
    }
    return loaiBoKhoangTrangDauCuoi(out);
}

std::string ChuyenThanhTitleCase(const std::string& s) {
    std::string out = ChuyenInThuong(s);
    bool laDauTu = true;
    for (size_t i = 0; i < out.length(); ++i) {
        char c = out[i];
        if (laDauTu && (c >= 'a' && c <= 'z')) {
            out[i] = c - 'a' + 'A';
            laDauTu = false;
        }
        else if (laKyTuKhoangTrang(c)) {
            laDauTu = true;
        }
        else {
            laDauTu = false;
        }
    }
    return out;
}

std::string ChuanHoaPhai(const std::string& raw){
    std::string loc = ChuyenInThuong(CatKhoangTrang(raw));
    if(loc == "nam") return "Nam";
    if(loc == "nu" ) return "Nu";
    return std::string();
}

std::string ChuanHoaISBNCore(const std::string& chuoiGoc, std::string& ketQua, bool tuDong, bool nhapThuCong, bool laDocFile) {
    std::string chuoi = CatKhoangTrang(chuoiGoc);
    if (chuoi.empty()) {
        return "Loi: ISBN khong duoc rong!";
    }
    if (tuDong || nhapThuCong || laDocFile) {
        std::string so = layChiCacKyTuSo(chuoi);
        if (so.length() != 10 && so.length() != 13) {
            return "Loi: ISBN phai co 10 hoac 13 chu so!";
        }
        ketQua = so;
        return ""; // Thanh cong
    }
    return "Loi: Che do chuan hoa ISBN khong hop le!";
}

std::string ChuanHoaISBNFile(const std::string& chuoiGoc, std::string& ketQua){
    return ChuanHoaISBNCore(chuoiGoc, ketQua, true, false, true);
}

std::string ChuanHoaTenUnicode(const std::string& dauVao, size_t minLength, size_t maxLength, std::string& ketQua) {
    // kiem tra rong
    if(dauVao.empty()){ return "Loi: Khong duoc de trong!"; }
    std::string chuoiDaCat = CatKhoangTrang(dauVao);
    if(chuoiDaCat.empty() || laChuoiRongHoacChiKhoangTrang(chuoiDaCat)){
        return "Loi: Chi chua khoang trang!";
    }

    //kiem tra ky tu dau
    if (!laKyTuChuCai(chuoiDaCat[0])) {
        return "Loi: Ky tu dau tien phai la chu cai!";
    }

    // kiem tra ky tu con lai
    for (size_t i = 1; i < chuoiDaCat.length(); ++i) {
        char kyTu = chuoiDaCat[i];
        bool kyTuHopLe =
            laKyTuChuCai(kyTu) || laKyTuSo(kyTu) || laKyTuKhoangTrang(kyTu) ||
            kyTu == '+' || kyTu == '#' || kyTu == '-' || kyTu == '.' || kyTu == '_';
        if (!kyTuHopLe) {
            return "Loi: Chi duoc chua chu cai, so, khoang trang va cac ky tu: + # - . _";
        }
    }

    // chuan hoa khoang trang
    std::string chuan = ChuanHoaKhoangTrang(chuoiDaCat);

    //kiem tra do dai
    if(chuan.empty()){
        return "Loi: Sau khi chuan hoa bi rong!";
    }
    int cmp = soSanhDoDaiChuoi(chuan, minLength, maxLength);
    if (cmp != 0) {
        if (cmp < 0) return "Loi: Qua ngan (>= " + std::to_string(minLength) + ")!";
        else return "Loi: Qua dai (<= " + std::to_string(maxLength) + ")!";
    }

    // gan ket qua
    ketQua = chuan;
    return ""; 
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

// Ham trich xuat so hau to tu Ma Sach (vd: "ISBN-12" -> 12)
// Tra ve -1 neu khong co dang "XXX-YYY" hoac khong phai so
int LaySoHauToMaSach(PTRDMS node) {
    if (!node || node->maSach.empty())
        return -1;
    size_t lastDash = node->maSach.find_last_of('-');
    if (lastDash != std::string::npos && lastDash < node->maSach.length() - 1) {
        try {
            // Cat chuoi sau dau '-' va chuyen sang so
            return std::stoi(node->maSach.substr(lastDash + 1));
        }
        catch (...) {
            // Neu khong phai so thi bo qua, tra ve -1
            return -1;
        }
    }
    return -1;
}

bool TachTruong(const std::string& line, char sep, std::string out[], int expected){
    int idx = 0; std::string cur;
    for(size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if(c == sep){ if(idx < expected) out[idx++] = cur; cur.clear(); }
        else cur.push_back(c);
    }
    if(idx < expected) out[idx++] = cur;
    return idx == expected;
}

int TachTokenKhoangTrang(const std::string& s, std::string tokens[], int maxTok){
    int soToken = 0;
    std::string token;
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
    std::string chuoiSach = ChuanHoaKhoangTrang(CatKhoangTrang(chuoiVao));
    if (chuoiSach.empty()) return std::string();

    std::string cacTu[8];
    int soTu = TachTokenKhoangTrang(chuoiSach, cacTu, 8);
    if (soTu == 0) return std::string();

    auto coChuSo = [](const std::string& chuoiKiemTra){
        for(size_t i = 0; i < chuoiKiemTra.length(); ++i) {
            if (chuoiKiemTra[i] >= '0' && chuoiKiemTra[i] <= '9') return true;
        }
        return false;
    };
    auto laChuCaiAscii = [](char kyTu){
        return (kyTu >= 'A' && kyTu <= 'Z') || (kyTu >= 'a' && kyTu <= 'z');
    };

    std::string tuDauThuong = ChuyenInThuong(cacTu[0]);
    if (tuDauThuong == "ke") {
        if (soTu == 1) return std::string();
        std::string phanConLai = cacTu[1];
        for (int i = 2; i < soTu; ++i) {
            phanConLai += ' ' + cacTu[i];
        }
        return std::string("Ke ") + ChuyenInHoa(phanConLai);
    }

    if (chuoiSach.find('-') != std::string::npos && coChuSo(chuoiSach)) {
        return std::string("Ke ") + ChuyenInHoa(chuoiSach);
    }

    if (!chuoiSach.empty() && laChuCaiAscii(chuoiSach[0]) && chuoiSach.size() > 1 && (chuoiSach[1] >= '0' && chuoiSach[1] <= '9')) {
        return std::string("Ke ") + ChuyenInHoa(chuoiSach);
    }

    return chuoiSach;
}