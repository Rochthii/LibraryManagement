#include "DocGia.h"
#include "ThongBao.h"
#include <string>

// Ham them doc gia moi
std::string themDocGia(PTRDG& root, const std::string& ho, const std::string& ten, bool phai, int trangThai) {
    if (ho.empty() || ten.empty())
        return thongBaoLoi(-1, "Ho hoac ten khong duoc de trong");

    PTRDG dg = taoDocGia(ho, ten, phai, trangThai, root);
    if (!dg)
        return thongBaoLoi(-1, "Khong cap phat duoc bo nho");

    InsertDocGia(root, dg);
    saveDocGia(root);
    return "Da them doc gia moi voi ma the " + std::to_string(dg->data.MaThe);
}

// Ham xoa doc gia
std::string xoaDocGiaTheoMa(PTRDG& root, int maThe) {
    PTRDG p = timDocGia(root, maThe);
    if (!p) return thongBaoLoi(-1, "Khong tim thay doc gia co ma the " + std::to_string(maThe));
    if (demSachDangMuon(p->data.dsmt) > 0)
        return thongBaoLoi(-1, "Doc gia van con sach chua tra");

    xoaDocGia(root, maThe);
    saveDocGia(root);
    return "Da xoa doc gia co ma the " + std::to_string(maThe);
}

// Ham cap nhat doc gia
std::string capNhatDocGia(PTRDG root, int maThe, const std::string& ho, const std::string& ten, bool phai, int trangThai) {
    PTRDG p = timDocGia(root, maThe);
    if (!p) return thongBaoLoi(-1, "Khong tim thay doc gia " + std::to_string(maThe));

    PTRDG temp = new NodeDG;
    *temp = *p;
    if (!ho.empty()) temp->data.Ho = ho;
    if (!ten.empty()) temp->data.Ten = ten;
    temp->data.Phai = phai;
    temp->data.TrangThai = trangThai;

    hieuChinhDocGia(root, temp, maThe);
    delete temp;
    saveDocGia(root);
    return "Da cap nhat thong tin doc gia " + std::to_string(maThe);
}

// Ham tra danh sach doc gia don gian (cho UI)
void LayDanhSachDocGia(PTRDG root, DocGiaDonGian arr[], int& count, int maxCount) {
    count = 0;
    if (!root) return;
    PTRDG stack[1000];
    int top = 0;
    PTRDG cur = root;
    while (cur || top > 0) {
        while (cur) {
            stack[top++] = cur;
            cur = cur->left;
        }
        cur = stack[--top];
        if (count < maxCount) {
            arr[count].MaThe = cur->data.MaThe;
            arr[count].Ho = cur->data.Ho;
            arr[count].Ten = cur->data.Ten;
            arr[count].Phai = cur->data.Phai;
            arr[count].TrangThai = cur->data.TrangThai;
            count++;
        }
        cur = cur->right;
    }
}
