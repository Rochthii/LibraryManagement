#ifndef NHAPLIEU_H
#define NHAPLIEU_H

#include <string>
#include <iostream>
#include "ThongBao.h"

// Số lần thử tối đa cho nhập liệu
extern const int SO_LAN_THU_TOI_DA;

// In thông báo số lần thử còn lại
void thongBaoSoLanThuConLai(int soLanDaThu, std::ostream& out, int maxLanThu = SO_LAN_THU_TOI_DA);

// Nhập dữ liệu
// Tên: có hai overload — với nhãn hoặc không
std::string NhapTen(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapTen(const char* nhan, size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout);
int NhapSoNguyen(int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout);
int NhapSoNguyen(const char* nhan, int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapPhai(std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapISBNThuCong(std::istream& in = std::cin, std::ostream& out = std::cout);

#endif // NHAPLIEU_H