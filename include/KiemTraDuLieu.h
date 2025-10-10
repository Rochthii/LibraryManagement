#ifndef KIEMTRADULIEU_H
#define KIEMTRADULIEU_H

#include <string>
#include <iostream>
#include "DauSach.h"
#include "ThongBao.h"

// Kiểm tra dữ liệu
bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out = std::cout);
bool KiemTraChuoiRong(const std::string& str, const std::string& fieldName, std::ostream& out = std::cout);
bool KiemTraTrangThaiThe(int trangThai, std::ostream& out = std::cout);
bool KiemTraMaThe(const std::string& maThe, std::ostream& out = std::cout);
bool KiemTraISBN(const std::string& ISBN, std::ostream& out = std::cout);
bool KiemTraTongSoBanSao(const std::string& isbn, int soLuongSach, std::ostream& out = std::cout);
bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out = std::cout);
bool KiemTraDanhSachHopLe(void* danhSach, int soLuong, int maxSize, const std::string& tenDS, std::ostream& out = std::cout);
bool KiemTraChuoiVaDodai(const std::string& str, const std::string& fieldName, int maxLen, std::ostream& out = std::cout);
bool KiemTraNamXuatBan(int nam, std::ostream& out = std::cout);

#endif // KIEMTRADULIEU_H