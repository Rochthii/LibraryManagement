#ifndef MUONTRA_H
#define MUONTRA_H

#include "DauSach.h"
#include "DocGia.h"
#include <string>

// --- DTOs (Data Transfer Objects) ---

struct DocGiaTableDTO_Backend {
  PTRDG docGia;
  int loaiKhop;    // 0: Exact, 1: Partial, 2: Show all
  int overdueDays; // So ngay qua han
};

struct SachTableDTO_Backend {
  PTRDS dauSach;
  int loaiKhop;
};

// --- DATA ACCESS & SEARCH ---

/**
 * Searches and filters readers.
 * @param tuKhoa Keyword to search for (ma the or ho ten)
 * @param laCheDoQuaHan True if we only want overdue readers
 * @param sapXepTheoTen True to sort by name, False to keep original order (ID)
 * @param ketQua Array to store results
 * @param soLuong Out: Total results found
 */
void LayDanhSachDocGiaBackend(PTRDG rootDocGia, std::string &tuKhoa, bool laCheDoQuaHan, bool sapXepTheoTen, DocGiaTableDTO_Backend *ketQua, int &soLuong);

/**
 * Searches for books (DauSach) available for borrowing.
 */
void LayDanhSachSachBackend(const std::string &tuKhoa, SachTableDTO_Backend *ketQua, int &soLuong);

/**
 * Populates the list of books currently borrowed by a specific reader.
 */
int LayDSSachDangMuonBackend(PTRDG docGia, ThongTinSachDangMuon_DTO *ketQua);

// --- TRANSACTIONS ---

std::string ThucHienMuonSachBackend(PTRDG docGia, const std::string &maSach);
std::string ThucHienTraSachBackend(PTRDG docGia, const std::string &maSach);
std::string ThucHienMatSachBackend(PTRDG docGia, const std::string &maSach);

// --- UTILITIES ---
/**
 * Validates reader information (Ho, Ten) according to business rules:
 * - Not empty
 * - Max length
 * - No special characters/digits (Strictly unaccented ASCII + space)
 * @return Error message, or empty string if valid.
 */
std::string KiemTraDuLieuDocGia(const std::string &ho, const std::string &ten);

std::string TimMaSachCoTheMuon(const std::string &isbn);

#endif // MUONTRA_H
