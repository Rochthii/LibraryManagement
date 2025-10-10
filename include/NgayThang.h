#ifndef NGAYTHANG_H
#define NGAYTHANG_H

#include <string>
#include <iostream>
#include <ctime>
#include "ThongBao.h"

// Ngày tháng
bool ChuanHoaNgay(std::string& s, std::ostream& out = std::cout);
bool phanTichNgay(const std::string& s, int& ngay, int& thang, int& nam);
int LayNamHienTai();
bool ChuyenNgaySangTimeT(const std::string& sNgay, time_t& outTime);
bool kiemTraQuaHan(std::string ngayMuon, std::ostream& out = std::cout);
std::string layNgayHienTai(std::ostream& out = std::cout);
int tinhSoNgayQuaHan(std::string ngayMuon, std::ostream& out = std::cout);

#endif // NGAYTHANG_H