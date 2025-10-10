
#ifndef DAUSACH_H
#define DAUSACH_H

#include <string>
#include <iostream>
#include "Constants.h"

enum TrangThaiSach {
    CHO_MUON_DUOC = 0,  // Co san de cho muon
    DANG_MUON = 1,      // Dang duoc muon
    THANH_LY = 2        // Da thanh ly / loai bo
};

struct DanhMucSach {
    std::string maSach;         // Ma duy nhat cho tung cuon
    TrangThaiSach trangThai;    // 0,1,2 theo enum
    std::string viTri;          // Vi tri tren ke
    DanhMucSach* next = NULL; // Lien ket node ke tiep
};

typedef DanhMucSach* PTRDMS;   

struct DauSach {
    std::string ISBN;
    std::string tenSach;
    int soTrang;
    std::string tacGia;
    int namXuatBan;
    std::string theLoai;
    PTRDMS dms = NULL;  
    int tongBanSao;
};

typedef DauSach* PTRDAUSACH;   

// bien toan cuc
extern PTRDAUSACH dsDauSach[MAX_DAUSACH];
extern int soLuongDauSach;

bool ChenNodeDMSVaoDauSach(PTRDAUSACH d, const std::string& maSach, int tt, const std::string& viTri);

#endif
