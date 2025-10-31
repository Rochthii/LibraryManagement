#ifndef QUANLYDOCGIA_H
#define QUANLYDOCGIA_H

#include "DocGia.h"
#include <string>

// Ham xu ly nghiep vu doc gia, su dung thong bao thong qua ThongBao.h
// Moi ham tra ve chuoi de UI co the hien thi

// Them doc gia moi vao cay
std::string themDocGia(PTRDG& root, const std::string& ho, const std::string& ten, bool phai, int trangThai);

// Xoa doc gia theo ma the
std::string xoaDocGiaTheoMa(PTRDG& root, int maThe);

// Cap nhat thong tin doc gia
std::string capNhatDocGia(PTRDG root, int maThe, const std::string& ho, const std::string& ten, bool phai, int trangThai);

// Tim doc gia theo ma the
PTRDG timTheoMa(PTRDG root, int maThe);

// Lay danh sach doc gia theo ten + ho tang dan
void duyetSangMang(PTRDG root, PTRDG* buffer, int& count, int maxCount);
void sortTheoTen(PTRDG* buffer, int count);

// Luu va tai du lieu doc gia tu file
std::string luuDocGia(PTRDG root);
std::string taiDocGia(PTRDG& root);

#endif // QUANLYDOCGIA_H
