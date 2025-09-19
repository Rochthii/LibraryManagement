#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <ctime>
#include "DocGia.h"

std::string sinhMaSach(std::string ISBN, int soThuTu);
int sinhMaThe(DocGia* root);
bool kiemTraQuaHan(std::string ngayMuon);
std::string layNgayHienTai();
int tinhSoNgayQuaHan(std::string ngayMuon);

#endif
