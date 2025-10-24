#ifndef CONSTANTS_H
#define CONSTANTS_H

// Gioi han kich thuoc 
constexpr int MAX_DAUSACH = 1000;                // Gioi han so dau sach
constexpr int MAX_BAN_SAO = 100;                 // Gioi han so ban sao moi dau sach
constexpr int MAX_KET_QUA_TIM_KIEM = 100;        // So ket qua tim kiem toi da hien thi
constexpr int MAX_VI_TRI_KE = 50;                // Do dai vi tri ke toi da
constexpr int MAX_TEN_SACH = 100;                // Do dai toi da ten sach
constexpr int MAX_TAC_GIA = 50;                  // Do dai toi da ten tac gia
constexpr int MAX_THE_LOAI = 50;                 // Do dai toi da the loai
constexpr int DO_DAI_MA_SACH_MAX = 50;           // Do dai toi da ma sach (ISBN-xxx)
constexpr int MAX_SO_TRANG = 10000;              // So trang toi da

// Gioi han do dai toi thieu 
constexpr int MIN_TEN_SACH = 1;                  // Do dai toi thieu ten sach
constexpr int MIN_TAC_GIA = 1;                   // Do dai toi thieu ten tac gia
constexpr int MIN_THE_LOAI = 1;                  // Do dai toi thieu the loai

//  Cau hinh ISBN 
constexpr int DO_DAI_ISBN = 13;                  // Do dai ISBN-13 tieu chuan
constexpr int DO_DAI_ISBN_CHECKSUM = 12;         // Do dai chuoi ISBN truoc checksum

//  Cau hinh Nam Xuat Ban 
constexpr int NAM_XUAT_BAN_MIN = 1800;           // Nam xuat ban toi thieu hop le
constexpr int NAM_XUAT_BAN_OFFSET_MAX = 0;       // Cho phep nam XB toi da la nam hien tai

//  Cau hinh Nhap Lieu 
constexpr int SO_LAN_THU_TOI_DA = 3;             // So lan thu nhap sai toi da

//  Cau hinh xu ly noi bo 
constexpr int SO_VONG_LAP_DMS_MAX = 1000;        // Gioi han vong lap de tranh treo khi duyet DSLK loi
constexpr int MAX_LAN_THU_SINH_MA = 100;         // So lan thu sinh ma ngau nhien (neu can)

//  Cau hinh Doc/Ghi File 
constexpr int DUNG_LUONG_CACHE_MAC_DINH = 256;   // Kich thuoc buffer doc file
constexpr int SO_TRUONG_DAU_SACH = 6;            // So truong trong file DauSach.txt (ISBN|Ten|Trang|TG|Nam|TheLoai)
constexpr int SO_TRUONG_DMS = 3;                 // So truong trong file DanhMucSach.txt (MaSach|TrangThai|ViTri)

//  Cau hinh Muon Tra Sach (Neu co) 
constexpr int SO_NGAY_MUON_TOI_DA = 7;           // So ngay muon toi da cho phep

#endif // CONSTANTS_H