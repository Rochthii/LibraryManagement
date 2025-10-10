#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int MAX_DAUSACH = 1000;                // Giới hạn số đầu sách
constexpr int MAX_BAN_SAO = 100;                 // Giới hạn số bản sao mỗi đầu sách
constexpr int DO_DAI_ISBN = 13;                  // Độ dài ISBN
constexpr int DO_DAI_ISBN_CHECKSUM = 12;         // Độ dài chuỗi ISBN trước checksum
constexpr int DO_DAI_VI_TRI_TOI_DA = 100;        // Độ dài tối đa của vị trí
constexpr int MIN_TEN_SACH = 1;                  // Độ dài tối thiểu tên sách
constexpr int MAX_TEN_SACH = 100;                // Độ dài tối đa tên sách
constexpr int MIN_TAC_GIA = 1;                   // Độ dài tối thiểu tên tác giả
constexpr int MAX_TAC_GIA = 50;                  // Độ dài tối đa tên tác giả
constexpr int MIN_THE_LOAI = 1;                  // Độ dài tối thiểu thể loại
constexpr int MAX_THE_LOAI = 50;                 // Độ dài tối đa thể loại
constexpr int NAM_XUAT_BAN_MIN = 1800;           // Năm xuất bản tối thiểu
constexpr int SO_LAN_THU_TOI_DA = 3;             // Số lần thử nhập tối đa
constexpr int MAX_LAN_THU_SINH_MA = 100;         // Số lần thử sinh mã ngẫu nhiên tối đa
constexpr int SO_VONG_LAP_DMS_MAX = 1000;        // Giới hạn vòng lặp danh mục sách
constexpr int DUNG_LUONG_CACHE_MAC_DINH = 256;   // Kích thước buffer đọc file
constexpr int DO_DAI_MA_SACH_MAX = 50;           // Độ dài tối đa mã sách
constexpr int SO_NGAY_MUON_TOI_DA = 7;           // Số ngày mượn tối đa
constexpr int MAX_VI_TRI_KE = 50;               // Độ dài vị trí kệ mặc định
constexpr int NAM_XUAT_BAN_OFFSET_MAX = 5;      // Offset tối đa cho năm xuất bản
constexpr int SO_TRUONG_DAU_SACH = 6;            // Số trường trong dòng đầu sách
constexpr int SO_TRUONG_DMS = 3;                 // Số trường trong dòng danh mục sách
constexpr int MAX_SO_TRANG = 10000;               // Số trang tối đa
constexpr int MAX_KET_QUA_TIM_KIEM = 100;        // Số kết quả tìm kiếm tối đa

#endif // CONSTANTS_H