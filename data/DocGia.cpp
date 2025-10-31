
#include "DocGia.h"
#include "ThongBao.h"
#include <string>

using namespace std;

// -------------------------------------------------
// Them doc gia moi
// -------------------------------------------------
string themDocGia(PTRDG& root, const string& ho, const string& ten, bool phai, int trangThai) {
    if (ho.empty() || ten.empty()) {
        return thongBaoLoi(0, "Ho hoac ten khong duoc de trong");
    }

    PTRDG dg = taoDocGia(ho, ten, phai, trangThai, root);
    if (dg == nullptr) {
        return thongBaoLoi(0, "Khong cap phat duoc bo nho cho doc gia");
    }

    InsertDocGia(root, dg);
    return "Da them doc gia moi voi ma the " + to_string(dg->data.MaThe);
}

// -------------------------------------------------
// Xoa doc gia theo ma
// -------------------------------------------------
string xoaDocGiaTheoMa(PTRDG& root, int maThe) {
    PTRDG p = timDocGia(root, maThe);
    if (p == nullptr) {
        return thongBaoLoi(0, "Khong tim thay doc gia co ma the " + to_string(maThe));
    }

    // Kiem tra neu doc gia con sach dang muon
    if (demSachDangMuon(p->data.dsmt) > 0) {
        return thongBaoLoi(0, "Doc gia van con sach chua tra");
    }

    xoaDocGia(root, maThe);
    return "Da xoa doc gia co ma the " + to_string(maThe);
}

// -------------------------------------------------
// Cap nhat thong tin doc gia
// -------------------------------------------------
string capNhatDocGia(PTRDG root, int maThe, const string& ho, const string& ten, bool phai, int trangThai) {
    PTRDG p = timDocGia(root, maThe);
    if (p == nullptr) {
        return thongBaoLoi(0, "Khong tim thay doc gia co ma the " + to_string(maThe));
    }

    PTRDG temp = new NodeDG;
    *temp = *p;
    if (!ho.empty()) temp->data.Ho = ho;
    if (!ten.empty()) temp->data.Ten = ten;
    temp->data.Phai = phai;
    temp->data.TrangThai = trangThai;

    hieuChinhDocGia(root, temp, maThe);
    delete temp;

    return "Da cap nhat thong tin doc gia " + to_string(maThe);
}

// -------------------------------------------------
// Tim doc gia theo ma
// -------------------------------------------------
PTRDG timTheoMa(PTRDG root, int maThe) {
    return timDocGia(root, maThe);
}

// -------------------------------------------------
// Tim doc gia theo ten (tra ve linked list tam)
// -------------------------------------------------
struct KetQuaTim {
    PTRDG* ds;
    int soLuong;
};

void timTheoTenRec(PTRDG node, const string& ten, KetQuaTim& kq) {
    if (!node) return;
    timTheoTenRec(node->left, ten, kq);
    if (node->data.Ten == ten && kq.soLuong < 1000) {
        kq.ds[kq.soLuong++] = node;
    }
    timTheoTenRec(node->right, ten, kq);
}

KetQuaTim timTheoTen(PTRDG root, const string& ten) {
    static PTRDG buffer[1000];
    KetQuaTim kq;
    kq.ds = buffer;
    kq.soLuong = 0;
    timTheoTenRec(root, ten, kq);
    return kq;
}

// -------------------------------------------------
// Lay danh sach doc gia sap xep theo ten + ho
// -------------------------------------------------
struct PtrArr {
    PTRDG arr[1000];
    int size;
};

void duyetSangMang(PTRDG root, PtrArr& list) {
    if (!root) return;
    duyetSangMang(root->left, list);
    if (list.size < 1000)
        list.arr[list.size++] = root;
    duyetSangMang(root->right, list);
}

bool cmpTenHo(PTRDG a, PTRDG b) {
    if (a->data.Ten == b->data.Ten)
        return a->data.Ho < b->data.Ho;
    return a->data.Ten < b->data.Ten;
}

void sortTenHo(PtrArr& list) {
    for (int i = 0; i < list.size - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < list.size; ++j) {
            if (cmpTenHo(list.arr[j], list.arr[minIdx]))
                minIdx = j;
        }
        if (minIdx != i) {
            PTRDG tmp = list.arr[i];
            list.arr[i] = list.arr[minIdx];
            list.arr[minIdx] = tmp;
        }
    }
}

PtrArr layDanhSachTheoTen(PTRDG root) {
    PtrArr list;
    list.size = 0;
    duyetSangMang(root, list);
    sortTenHo(list);
    return list;
}

// -------------------------------------------------
// Lay danh sach doc gia theo ma (duyet inorder)
// -------------------------------------------------
void duyetTheoMa(PTRDG node, PTRDG* buffer, int& count, int maxCount) {
    if (!node || count >= maxCount) return;
    duyetTheoMa(node->left, buffer, count, maxCount);
    if (count < maxCount) buffer[count++] = node;
    duyetTheoMa(node->right, buffer, count, maxCount);
}

PtrArr layDanhSachTheoMa(PTRDG root) {
    PtrArr list;
    list.size = 0;
    duyetSangMang(root, list);
    return list;
}

// -------------------------------------------------
// Wrapper luu / tai file
// -------------------------------------------------
string luuDocGia(PTRDG root) {
    saveDocGia(root);
    return "Da luu file docgia.txt";
}

string taiDocGia(PTRDG& root) {
    root = loadDocGia();
    if (root == nullptr)
        return thongBaoLoi(0, "Khong tai duoc file hoac file rong");
    return "Tai file docgia.txt thanh cong";
}
