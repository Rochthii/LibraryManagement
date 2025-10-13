#ifndef NHAPLIEU_H
#define NHAPLIEU_H

#include <string>
#include <iostream>
#include "ThongBao.h"
#include "Constants.h"

// Nhập dữ liệu
std::string NhapTen(const char* nhan, size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout);
int NhapSoNguyen(const char* nhan, int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapPhai(std::istream& in = std::cin, std::ostream& out = std::cout);
std::string NhapISBNThuCong(std::istream& in = std::cin, std::ostream& out = std::cout);

#endif // NHAPLIEU_H