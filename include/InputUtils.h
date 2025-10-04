#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>
#include <iostream>
#include <ctime>
#include "DauSach.h"
#include "DocGia.h"
#include "VietnameseUtils.h"  // Them include cho ho tro tieng Viet

namespace InputUtils {

enum LoaiThongBao { LOI, THONG_TIN, CANH_BAO };

// HANG SO - Constants
extern const char* FILE_DAUSACH;
extern const char* FILE_DANHMUCSACH;

// NHAP DU LIEU - Input functions
std::string NhapTen(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout);
int NhapSoNguyen(int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapTenNhan(const char* nhan, size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout);
int NhapSoNguyenNhan(const char* nhan, int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapPhai(std::istream& in = std::cin, std::ostream& out = std::cout);

// KIEM TRA DU LIEU - Validation functions
bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out = std::cout);
bool KiemTraChuoiRong(const std::string& str, const std::string& fieldName, std::ostream& out = std::cout);
bool KiemTraTrangThaiThe(int trangThai, std::ostream& out = std::cout);
bool KiemTraMaThe(const std::string& maThe, std::ostream& out = std::cout);
bool KiemTraISBN(const std::string& ISBN, std::ostream& out = std::cout);
bool KiemTraTongSoBanSao(const std::string& ISBN, int soLuongSach, std::ostream& out = std::cout);
bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out = std::cout);
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out = std::cout);
bool KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen, std::ostream& out = std::cout);
bool KiemTraNamXuatBan(int nam, std::ostream& out = std::cout);

// XU LY CHUOI - String processing
std::string ChuyenInThuong(const std::string& str);
std::string CatKhoangTrang(const std::string& s);
std::string ChuanHoaKhoangTrang(const std::string& s);
std::string ChuanHoaPhai(const std::string& raw);
bool ChuanHoaNgay(std::string& ngayChuoi, std::ostream& out = std::cout);
bool ChuanHoaISBNFile(const std::string& raw, std::string& outISBN, std::ostream& out = std::cout);
inline std::string ChuanHoaViTri(const std::string& s){ return ChuanHoaKhoangTrang(s); }

// Unicode va tieng Viet
bool ChuanHoaTenUnicode(const std::string& dauVao, size_t minLength, size_t maxLength,
						std::ostream& out, std::string& ketQua);
std::string BoDauVaThuong(const std::string& s);

// Helper cho chuẩn hóa dữ liệu sách
struct ThongTinSachChuanHoa {
    std::string tenSach;
    std::string tacGia;
    std::string theLoai;
    std::string isbn;
};
ThongTinSachChuanHoa ChuanHoaDuLieuSach(const std::string& tenSach, const std::string& tacGia, 
                                        const std::string& theLoai, const std::string& isbn);

// TACH CHUOI - String parsing
bool TachTruong(const std::string& line, char sep, std::string out[], int expected);
int TachTokenKhoangTrang(const std::string& s, std::string tokens[], int maxTok);
bool ChuaTatCaTuKhoa(const std::string& haystack, const std::string tokens[], int count);
bool KhopTieuDeKhongDau(const std::string& titleNorm, const std::string& kwNorm,
						 const std::string tokens[], int tokenCount);
std::string TachISBNTuMaSach(const std::string& maSach);

// SINH MA TU DONG - Code generation
std::string SinhMaTheNgauNhien(size_t length, std::ostream& out = std::cout);
std::string SinhISBN13TuDong(std::ostream& out = std::cout, bool allowManualInput = false);
std::string sinhMaSach(std::string ISBN, int soThuTu, std::ostream& out = std::cout);
int sinhMaThe(PTRDG root, std::ostream& out = std::cout);

// NGAY THANG - Date/time utilities
int LayNamHienTai();
bool ChuyenNgaySangTimeT(const std::string& sNgay, time_t& outTime);

// TIM KIEM VA XU LY SACH - Book operations
const char* TenTrangThai(TrangThaiSach tt);
PTRDAUSACH TimDauSachTheoISBN(PTRDAUSACH ds[], int n, const std::string& isbn);
bool ChenNodeDMSVaoDauSach(PTRDAUSACH d, const std::string& maSach, int tt, const std::string& viTri);
PTRDAUSACH TaoDauSachMoi(const std::string& ISBN, const std::string& tenSach, int soTrang, 
                         const std::string& tacGia, int namXuatBan, const std::string& theLoai, std::ostream& out = std::cout);
int TimViTriChenDauSach(PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& tenSach, std::ostream& out = std::cout);
bool ChenDauSachVaoViTri(PTRDAUSACH dsDauSach[], int& soLuongDauSach, PTRDAUSACH dauSachMoi, int viTri, std::ostream& out = std::cout);

// DOC GHI FILE - File I/O
void NapDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int& soLuongDauSach);
void NapDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach);
bool GhiDanhSachDauSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, bool silent = false);
bool GhiDanhMucSach(const char* path, PTRDAUSACH dsDauSach[], int soLuongDauSach, bool silent = false);

// QUAN LY BO NHO - Memory management
void GiaiPhongDanhMucSach(PTRDMS& dms);
void GiaiPhongDauSach(PTRDAUSACH& dauSach);
void GiaiPhongToanBoDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach);
bool BackupTruocKhiGiaiPhong(PTRDAUSACH dsDauSach[], int soLuongDauSach);

// HIEN THI VA THONG BAO - Display functions
void InTongKet(const char* nhan, int ok, int total, int skip);
void InMotDauSach(PTRDAUSACH s);
void thongBao(std::ostream& out, const std::string& msg, LoaiThongBao loai);

} // namespace InputUtils

// CAC HAM TOAN CUC - Global functions (ngoai namespace)
bool kiemTraQuaHan(std::string ngayMuon, std::ostream& out = std::cout);
std::string layNgayHienTai(std::ostream& out = std::cout);
int tinhSoNgayQuaHan(std::string ngayMuon, std::ostream& out = std::cout);

#endif // INPUT_UTILS_H
