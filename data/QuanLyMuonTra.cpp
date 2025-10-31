#include "QuanLyMuonTra.h"
#include "ThongBao.h"

// ------------------------------------------------------
// Kiem tra sach qua han (>7 ngay)
// ------------------------------------------------------
bool kiemTraQuaHan(const MUONTRA* dsmt, const Date& ngayHienTai) {
    const MUONTRA* p = dsmt;
    while (p) {
        if (p->data.TrangThai == 0) { // Sach dang muon
            int ngayMuon = tinhKhoangCachNgay(p->data.NgayMuon, ngayHienTai);
            if (ngayMuon > 7) return true;
        }
        p = p->next;
    }
    return false;
}

// ------------------------------------------------------
// Dem so luong sach dang muon (TrangThai = 0)
// ------------------------------------------------------
int demSachDangMuon(const MUONTRA* dsmt) {
    int count = 0;
    const MUONTRA* p = dsmt;
    while (p) {
        if (p->data.TrangThai == 0) count++;
        p = p->next;
    }
    return count;
}

// ------------------------------------------------------
// Muon sach
// ------------------------------------------------------
std::string muonSach(PTRDG root, const std::string& maSach, int maThe, const Date& ngayMuon) {
    PTRDG dg = timDocGia(root, maThe);
    if (dg == nullptr)
        return thongBaoLoi(0, "Khong tim thay doc gia co ma the " + std::to_string(maThe));

    if (dg->data.TrangThai == 0)
        return thongBaoLoi(0, "The doc gia nay da bi khoa, khong the muon sach");

    if (demSachDangMuon(dg->data.dsmt) >= 3)
        return thongBaoLoi(0, "Doc gia da muon toi da 3 sach");

    if (kiemTraQuaHan(dg->data.dsmt, ngayMuon))
        return thongBaoLoi(0, "Doc gia co sach qua han, khong duoc muon tiep");

    // Them sach moi vao danh sach muon tra
    MUONTRA* node = new MUONTRA;
    node->data.MaSach = maSach;
    node->data.NgayMuon = ngayMuon;
    node->data.TrangThai = 0;
    node->next = dg->data.dsmt;
    dg->data.dsmt = node;

    return "Doc gia " + std::to_string(maThe) + " da muon sach " + maSach;
}

// ------------------------------------------------------
// Tra sach
// ------------------------------------------------------
std::string traSach(PTRDG root, const std::string& maSach, int maThe, const Date& ngayTra, bool matSach) {
    PTRDG dg = timDocGia(root, maThe);
    if (dg == nullptr)
        return thongBaoLoi(0, "Khong tim thay doc gia co ma the " + std::to_string(maThe));

    MUONTRA* p = dg->data.dsmt;
    while (p) {
        if (p->data.MaSach == maSach && p->data.TrangThai == 0) {
            p->data.TrangThai = matSach ? 2 : 1;
            p->data.NgayTra = ngayTra;
            return matSach ? "Doc gia lam mat sach " + maSach
                : "Doc gia " + std::to_string(maThe) + " da tra sach " + maSach;
        }
        p = p->next;
    }

    return thongBaoLoi(0, "Khong tim thay sach " + maSach + " trong danh sach muon");
}

// ------------------------------------------------------
// Liet ke sach dang muon cua doc gia
// ------------------------------------------------------
int lietKeSachDangMuon(PTRDG root, int maThe, MUONTRA arr[], int maxCount) {
    PTRDG dg = timDocGia(root, maThe);
    if (dg == nullptr) return 0;

    int count = 0;
    MUONTRA* p = dg->data.dsmt;
    while (p && count < maxCount) {
        if (p->data.TrangThai == 0) {
            arr[count] = *p;
            count++;
        }
        p = p->next;
    }
    return count;
}

// ------------------------------------------------------
// Tinh so ngay qua han
// ------------------------------------------------------
int tinhSoNgayQuaHan(const Date& ngayMuon, const Date& ngayHienTai) {
    int delta = tinhKhoangCachNgay(ngayMuon, ngayHienTai);
    return (delta > 7) ? (delta - 7) : 0;
}

// ------------------------------------------------------
// Liet ke doc gia qua han
// ------------------------------------------------------
int lietKeDocGiaQuaHan(PTRDG root, int maThe[], int& count, const Date& ngayHienTai, int maxCount) {
    count = 0;
    if (!root) return 0;

    int left = lietKeDocGiaQuaHan(root->left, maThe, count, ngayHienTai, maxCount);

    if (kiemTraQuaHan(root->data.dsmt, ngayHienTai) && count < maxCount)
        maThe[count++] = root->data.MaThe;

    lietKeDocGiaQuaHan(root->right, maThe, count, ngayHienTai, maxCount);

    return count;
}
