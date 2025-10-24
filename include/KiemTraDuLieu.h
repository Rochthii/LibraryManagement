#ifndef KIEMTRADULIEU_H
#define KIEMTRADULIEU_H

#include <string>
#include <iostream> 
#include "DauSach.h"

std::string KiemTraChuoiRong(const std::string& chuoi, const std::string& tenTruong);
bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out);
bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out);
std::string KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen);
std::string KiemTraNamXuatBan(int nam);
bool KiemTraISBN(const std::string& ISBN, std::ostream& out);
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out);
bool KiemTraTongSoBanSao(const std::string& isbn, int soLuongThem, std::ostream& out);

#endif
