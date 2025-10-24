#ifndef XULYCHUOI_H
#define XULYCHUOI_H

#include <string>
#include <iostream>
#include "ThongBao.h"

// Xu ly chuoi
std::string ChuyenInThuong(const std::string& str);
std::string ChuyenInHoa(const std::string& str); 
std::string ChuyenThanhTitleCase(const std::string& s);
std::string CatKhoangTrang(const std::string& s);
std::string ChuanHoaKhoangTrang(const std::string& s);
std::string layChiCacKyTuSo(const std::string& s);
std::string ChuanHoaPhai(const std::string& raw);
bool ChuanHoaNgay(std::string& ngayChuoi, std::ostream& out);
bool ChuanHoaISBNFile(const std::string& raw, std::string& outISBN, std::ostream& out = std::cout);
bool ChuanHoaISBNCore(const std::string& chuoiGoc, std::string& ketQua, bool tuDong, bool nhapThuCong, bool laDocFile, std::ostream& out);
bool chuyenChuoiThanhSoNguyen(const std::string& chuoi, int& ketQua, bool chiChoPhepSoDuong = false);
std::string ChuanHoaViTri(const std::string& s);
bool ChuanHoaTenUnicode(const std::string& dauVao, size_t minLength, size_t maxLength, std::ostream& out, std::string& ketQua);
std::string BoDauVaThuong(const std::string& s); // Luu y: Chi chuyen thuong, khong bo dau
bool laChuoiRongHoacChiKhoangTrang(const std::string& s);

// Struct chuan hoa du lieu sach (dung cho tim kiem)
struct ThongTinSachChuanHoa {
    std::string tenSach;
    std::string tacGia;
    std::string theLoai;
    std::string isbn;
};
ThongTinSachChuanHoa ChuanHoaDuLieuSach(const std::string& tenSach, const std::string& tacGia,
                                        const std::string& theLoai, const std::string& isbn);

// Tach chuoi
bool TachTruong(const std::string& line, char sep, std::string out[], int expected);
int TachTokenKhoangTrang(const std::string& s, std::string tokens[], int maxTok);
bool ChuaTatCaTuKhoa(const std::string& haystack, const std::string tokens[], int count);
bool KhopTieuDeKhongDau(const std::string& titleNorm, const std::string& kwNorm,
                        const std::string tokens[], int tokenCount);
std::string TachISBNTuMaSach(const std::string& maSach);

#endif // XULYCHUOI_H