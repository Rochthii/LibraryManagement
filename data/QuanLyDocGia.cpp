#include "QuanLyDocGia.h"
#include "ThongBao.h"
#include <cstring>

// -------------------------------------------------------------
// Them doc gia moi
// -------------------------------------------------------------
std::string themDocGia(PTRDG& root, const std::string& ho, const std::string& ten, bool phai, int trangThai) {
    if (ho.empty() || ten.empty()) {
        return thongBaoLoi(0, "Ho hoac ten khong duoc de trong");
    }

    PTRDG dg = taoDocGia(ho, ten, phai, trangThai, root);
    if (dg == nullptr) {
        return thongBaoLoi(0, "Khong cap phat duoc bo nho cho doc gia");
    }

    InsertDocGia(root, dg);
    return "Da them doc gia moi voi ma the " + std::to_string(dg->data.MaThe);
}

// -------------------------------------------------------------
// Xoa doc gia theo ma
// -------------------------------------------------------------
std::string xoaDocGiaTheoMa(PTRDG& root, int maThe) {
    PTRDG p = timDocGia(root, maThe);
    if (p == nullptr) {
        return thongBaoLoi(0, "Khong tim thay doc gia co ma the " + std::to_string(maThe));
    }

    // Kiem tra neu doc gia van con sach dang muon
    if (demSachDangMuon(p->data.dsmt) > 0) {
        return thongBaoLoi(0, "Doc gia van con sach chua tra, khong the xoa");
    }

    xoaDocGia(root, maThe);
    return "Da xoa doc gia co ma the " + std::to_string(maThe);
}

// -------------------------------------------------------------
// Cap nhat thong tin doc gia
// -------------------------------------------------------------
std::string capNhatDocGia(PTRDG root, int maThe, const std::string& ho, const std::string& ten, bool phai, int trangThai) {
    PTRDG p = timDocGia(root, maThe);
    if (p == nullptr) {
        return thongBaoLoi(0, "Khong tim thay doc gia co ma the " + std::to_string(maThe));
    }

    if (!ho.empty()) p->data.Ho = ho;
    if (!ten.empty()) p->data.Ten = ten;
    p->data.Phai = phai;
    p->data.TrangThai = trangThai;

    return "Da cap nhat thong tin doc gia co ma the " + std::to_string(maThe);
}

// -------------------------------------------------------------
// Tim doc gia theo ma
// -------------------------------------------------------------
PTRDG timTheoMa(PTRDG root, int maThe) {
    return timDocGia(root, maThe);
}

// -------------------------------------------------------------
// Duyet cay -> mang
// -------------------------------------------------------------
void duyetSangMang(PTRDG root, PTRDG* buffer, int& count, int maxCount) {
    if (root == nullptr || count >= maxCount) return;
    duyetSangMang(root->left, buffer, count, maxCount);
    if (count < maxCount) buffer[count++] = root;
    duyetSangMang(root->right, buffer, count, maxCount);
}

// -------------------------------------------------------------
// Sap xep theo ten + ho bang selection sort (khong dung STL)
// -------------------------------------------------------------
bool cmpTenHo(PTRDG a, PTRDG b) {
    int cmpTen = std::strcmp(a->data.Ten.c_str(), b->data.Ten.c_str());
    if (cmpTen == 0) {
        return std::strcmp(a->data.Ho.c_str(), b->data.Ho.c_str()) < 0;
    }
    return cmpTen < 0;
}

void sortTheoTen(PTRDG* buffer, int count) {
    for (int i = 0; i < count - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < count; ++j) {
            if (cmpTenHo(buffer[j], buffer[minIdx])) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            PTRDG tmp = buffer[i];
            buffer[i] = buffer[minIdx];
            buffer[minIdx] = tmp;
        }
    }
}

// -------------------------------------------------------------
// Luu va tai du lieu doc gia tu file
// -------------------------------------------------------------
std::string luuDocGia(PTRDG root) {
    saveDocGia(root);
    return "Da luu du lieu doc gia vao file docgia.txt";
}

std::string taiDocGia(PTRDG& root) {
    root = loadDocGia();
    if (root == nullptr) {
        return thongBaoLoi(0, "Khong the tai file docgia.txt (file rong hoac loi dinh dang)");
    }
    return "Tai du lieu doc gia tu file docgia.txt thanh cong";
}
