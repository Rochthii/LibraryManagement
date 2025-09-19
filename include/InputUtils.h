#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>
#include <iostream>
#include <ctime>
#include "DauSach.h"
#include "DocGia.h"

// Ngôn ngữ thông báo
enum class Language { VI, EN };

namespace InputUtils {

// Các hàm nhập / kiểm tra dữ liệu (chỉ khai báo, định nghĩa ở input_utils.cpp)
std::string NhapChuoiSo(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI);
std::string NhapTen(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI);
std::string NhapMa(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI);
int NhapSoNguyen(int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI);

bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTramaSach(const std::string& maSach, int mode, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraChuoiRong(const std::string& str, const std::string& fieldName, std::ostream& out = std::cout, Language lang = Language::VI);
std::string ChuyenInThuong(const std::string& str);
std::string NhapPhai(std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraTrangThaiThe(int trangThai, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraMaThe(const std::string& maThe, std::ostream& out = std::cout, Language lang = Language::VI);
std::string SinhMaTheNgauNhien(size_t length, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraISBN(const std::string& ISBN, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraTongSoBanSao(const std::string& ISBN, int soLuongSach, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out = std::cout, Language lang = Language::VI);
int NhapLuaChonSapXep(std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraSoSachDangMuon(const std::string& maThe, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraQuaHan(const std::string& maThe, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraNgayHienTai(time_t ngay, std::ostream& out = std::cout, Language lang = Language::VI);
bool KiemTraLuotMuon(int luotMuon, std::ostream& out = std::cout, Language lang = Language::VI);

// Các hàm sinh / xử lý mã và ngày
std::string sinhMaSach(std::string ISBN, int soThuTu, std::ostream& out = std::cout, Language lang = Language::VI);
int sinhMaThe(DocGia* root, std::ostream& out = std::cout, Language lang = Language::VI);

} // namespace InputUtils

// Các hàm toàn cục liên quan đến ngày mượn (ngoài namespace)
bool kiemTraQuaHan(std::string ngayMuon, std::ostream& out = std::cout, Language lang = Language::VI);
std::string layNgayHienTai(std::ostream& out = std::cout, Language lang = Language::VI);
int tinhSoNgayQuaHan(std::string ngayMuon, std::ostream& out = std::cout, Language lang = Language::VI);

#endif // INPUT_UTILS_H
