#include "DocGia.h"
//#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Constants.h"
#include "NgayThang.h"

// helper thay the max/abs tu <algorithm>
static inline int my_max(int a, int b) { return (a > b) ? a : b; }
static inline int my_abs(int x) { return (x < 0) ? -x : x; }

// forward-declare static helpers
static PTRDG InsertDocGiaRec(PTRDG pavtree, PTRDG node);
static PTRDG xoaDocGiaRec(PTRDG root, int mathe);

// hang so cho sinh ma
static const int MA_THE_MIN = 10000;
static const int MA_THE_MAX = 99999;
static const int NUM_RANGES = 9; // chia range de phan tan

// bien static theo doi RNG
static bool daKhoiTao = false;
static int khoangHienTai = 0;
static int countTrongKhoang = 0;

int static sinhMaTheNgauNhien(PTRDG root) {
    if (!daKhoiTao) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        daKhoiTao = true;
    }

    const int sizeKhoang = (MA_THE_MAX - MA_THE_MIN + 1) / NUM_RANGES;
    for (int i = 0; i < MAX_LAN_THU_SINH_MA; ++i) {
        int rangeStart = MA_THE_MIN + khoangHienTai * sizeKhoang;
        int rangeEnd = rangeStart + sizeKhoang - 1;
        int ma = rangeStart + (std::rand() % (rangeEnd - rangeStart + 1));
        if (timDocGia(root, ma) == nullptr) {
            ++countTrongKhoang;
            if (countTrongKhoang >= sizeKhoang / NUM_RANGES) {
                countTrongKhoang = 0;
                khoangHienTai = (khoangHienTai + 1) % NUM_RANGES;
            }
            return ma;
        }
    }

    // fallback: linear search
    int ma = MA_THE_MIN;
    while (ma <= MA_THE_MAX && timDocGia(root, ma) != nullptr) ++ma;
    return (ma <= MA_THE_MAX) ? ma : -1;
}

// ---- AVL with stored height ----
// get/update height and balance helpers
static inline int getHeight(PTRDG n) {
    return (n == nullptr) ? 0 : n->height;
}
static inline void updateHeight(PTRDG n) {
    if (n != nullptr) n->height = 1 + my_max(getHeight(n->left), getHeight(n->right));
}
static inline int getBalance(PTRDG n) {
    return (n == nullptr) ? 0 : getHeight(n->left) - getHeight(n->right);
}

// rotations update heights
PTRDG RotateLeft(PTRDG p) {
    PTRDG q = p->right;
    PTRDG B = q->left;

    q->left = p;
    p->right = B;

    updateHeight(p);
    updateHeight(q);
    return q;
}

PTRDG RotateRight(PTRDG p) {
    PTRDG q = p->left;
    PTRDG B = q->right;

    q->right = p;
    p->left = B;

    updateHeight(p);
    updateHeight(q);
    return q;
}

// Them AVL - dung height de tinh balance O(1)
static PTRDG InsertDocGiaRec(PTRDG pavltree, PTRDG node) {
    if (pavltree == nullptr) {
        // node phai co height da duoc khoi tao ben ngoai (taoDocGia)
        return node;
    }

    if (node->data.MaThe < pavltree->data.MaThe)
        pavltree->left = InsertDocGiaRec(pavltree->left, node);
    else if (node->data.MaThe > pavltree->data.MaThe)
        pavltree->right = InsertDocGiaRec(pavltree->right, node);
    else {
        // duplicate id -> do nothing
        return pavltree;
    }

    // cap nhat height va rebalance
    updateHeight(pavltree);
    int balance = getBalance(pavltree);

    // LL
    if (balance > 1 && node->data.MaThe < pavltree->left->data.MaThe)
        return RotateRight(pavltree);
    // RR
    if (balance < -1 && node->data.MaThe > pavltree->right->data.MaThe)
        return RotateLeft(pavltree);
    // LR
    if (balance > 1 && node->data.MaThe > pavltree->left->data.MaThe) {
        pavltree->left = RotateLeft(pavltree->left);
        return RotateRight(pavltree);
    }
    // RL
    if (balance < -1 && node->data.MaThe < pavltree->right->data.MaThe) {
        pavltree->right = RotateRight(pavltree->right);
        return RotateLeft(pavltree);
    }

    return pavltree;
}

void InsertDocGia(PTRDG &root, PTRDG node) {
    // ensure new node has height = 1
    if (node != nullptr) node->height = 1;
    root = InsertDocGiaRec(root, node);
}

// quan ly cay doc gia
// tao PTRDG docgia
PTRDG taoDocGia(std::string ho, std::string ten, bool phai, int trangthai, PTRDG root) {
    PTRDG dg = new NodeDG;
    dg->data.MaThe = sinhMaTheNgauNhien(root);
    dg->data.Ho = ho;
    dg->data.Ten = ten;
    dg->data.Phai = phai;
    dg->data.TrangThai = trangthai;
    dg->data.dsmt = nullptr;
    dg->left = dg->right = nullptr;
    dg->height = 1;
    return dg;
}
PTRDG taoDocGia(std::string ho, std::string ten, bool phai, int trangthai, int mathe) {
    PTRDG dg = new NodeDG;
    dg->data.MaThe = mathe;
    dg->data.Ho = ho;
    dg->data.Ten = ten;
    dg->data.Phai = phai;
    dg->data.TrangThai = trangthai;
    dg->data.dsmt = nullptr;
    dg->left = dg->right = nullptr;
    dg->height = 1;
    return dg;
}

// removed themDocGia implementation (use InsertDocGia for AVL)

// giai phong dsmt
void giaiPhongDsmt(MUONTRA& dsmt) {
    while (dsmt != nullptr) {
        MUONTRA tmp = dsmt;
        dsmt = dsmt->next;
        delete tmp;
    }
}

// them ham giai phong toan bo cay
void GiaiPhongCay(PTRDG &root) {
    if (root == nullptr) return;
    GiaiPhongCay(root->left);
    GiaiPhongCay(root->right);
    giaiPhongDsmt(root->data.dsmt);
    delete root;
    root = nullptr;
}

// xoa node - cap nhat height va rebalance
static PTRDG xoaDocGiaRec(PTRDG root, int mathe) {
    if (root == nullptr) return root;

    if (mathe < root->data.MaThe) {
        root->left = xoaDocGiaRec(root->left, mathe);
    } else if (mathe > root->data.MaThe) {
        root->right = xoaDocGiaRec(root->right, mathe);
    } else {
        // found node
        // giai phong dsmt hien tai (node se bi xoa hoac duoc thay bang successor)
        giaiPhongDsmt(root->data.dsmt);
        if (root->left == nullptr || root->right == nullptr) {
            PTRDG temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        } else {
            // node co 2 con: lay inorder successor
            PTRDG succ = root->right;
            while (succ->left != nullptr) succ = succ->left;

            // free current dsmt (already done), transfer fields and ownership of dsmt
            root->data.MaThe = succ->data.MaThe;
            root->data.Ho = succ->data.Ho;
            root->data.Ten = succ->data.Ten;
            root->data.Phai = succ->data.Phai;
            root->data.TrangThai = succ->data.TrangThai;
            // transfer dsmt pointer ownership
            root->data.dsmt = succ->data.dsmt;
            succ->data.dsmt = nullptr;

            // delete successor by key
            root->right = xoaDocGiaRec(root->right, root->data.MaThe);
        }
    }

    // cap nhat height va rebalance
    updateHeight(root);
    int balance = getBalance(root);
    int leftBalance = getBalance(root->left);
    int rightBalance = getBalance(root->right);

    // LL
    if (balance > 1 && leftBalance >= 0)
        return RotateRight(root);
    // LR
    if (balance > 1 && leftBalance < 0) {
        root->left = RotateLeft(root->left);
        return RotateRight(root);
    }
    // RR
    if (balance < -1 && rightBalance <= 0)
        return RotateLeft(root);
    // RL
    if (balance < -1 && rightBalance > 0) {
        root->right = RotateRight(root->right);
        return RotateLeft(root);
    }

    return root;
}
void xoaDocGia(PTRDG& root, int mathe) {
    root = xoaDocGiaRec(root, mathe);
}

// tim doc gia
PTRDG timDocGia(PTRDG root, int mathe) {
    while (root != nullptr && mathe != root->data.MaThe) {
        if (mathe < root->data.MaThe) root = root->left;
        else root = root->right;
    }
    return root;
}

// hieu chinh doc gia
void hieuChinhDocGia(PTRDG root, PTRDG pAlt, int mathe) {
    if (pAlt == nullptr) {
        std::cerr << "Du lieu cap nhat khong hop le!" << std::endl;
        return;
    }

    PTRDG p = timDocGia(root, mathe);
    if (p == nullptr) {
        std::cerr << "Khong tim thay doc gia co ma the " << mathe << std::endl;
        return;
    }

    if (p->data.Ho != pAlt->data.Ho) p->data.Ho = pAlt->data.Ho;
    if (p->data.Ten != pAlt->data.Ten) p->data.Ten = pAlt->data.Ten;
    if (p->data.Phai != pAlt->data.Phai) p->data.Phai = pAlt->data.Phai;
    if (p->data.TrangThai != pAlt->data.TrangThai) p->data.TrangThai = pAlt->data.TrangThai;
}

// hien thi
void display(PTRDG root) {
    // kiem tra con tro null
    if (root == nullptr) {
        std::cerr << "Khong the hien thi: con tro doc gia null!" << std::endl;
        return;
    }

    std::cout << "MaThe: " << root->data.MaThe
         << " | HoTen: " << root->data.Ho << " " << root->data.Ten
         << " | Phai: " << (root->data.Phai ? "Nu" : "Nam")
         << " | TrangThai: " << (root->data.TrangThai ? "Hoat dong" : "Khoa")
         << std::endl;
}

void inDocGiaInOrder(PTRDG root) {
    if (root != nullptr) {
        inDocGiaInOrder(root->left);
        display(root);
        inDocGiaInOrder(root->right);
    }
}

// quan ly muon tra - giu nguyen logic, thay NULL -> nullptr
void themMuonTra(PTRDG docgia, std::string maSach) {
    if (docgia == nullptr) return;

    MUONTRA node = new NodeMT;
    node->data.MaSach = maSach;
    node->data.NgayMuon = layNgayHienTai();
    node->data.NgayTra = "";
    node->data.TrangThai = 0;
    node->next = nullptr;

    node->next = docgia->data.dsmt;
    docgia->data.dsmt = node;
}

void themMuonTra(PTRDG docgia, const MuonTra &mt) {
    if (docgia == nullptr) return;

    MUONTRA node = new NodeMT;
    node->data = mt;
    node->next = docgia->data.dsmt;
    docgia->data.dsmt = node;
}

// kiem tra qua han, demSachDangMuon, muonSach, traSach, lietKe... (giu nguyen)
// saveDocGia (giu nguyen), loadDocGia: thay themDocGia -> InsertDocGia

PTRDG loadDocGia() {
    std::ifstream in("docgia.txt");
    if (!in.is_open()) {
        std::cout << "khong mo duoc file de doc!" << std::endl;
        return nullptr;
    }

    PTRDG root = nullptr;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        size_t posSlash = line.find('|');
        std::string info;
        std::string muontras;
        if (posSlash != std::string::npos) {
            info = line.substr(0, posSlash);
            muontras = line.substr(posSlash + 1);
        } else {
            info = line;
            muontras = "";
        }

        int mathe, phai, trangthai;
        std::string ho, ten;
        PTRDG dg;
        size_t p1 = info.find(',');
        size_t p2 = (p1 == std::string::npos) ? std::string::npos : info.find(',', p1 + 1);
        size_t p3 = (p2 == std::string::npos) ? std::string::npos : info.find(',', p2 + 1);
        size_t p4 = (p3 == std::string::npos) ? std::string::npos : info.find(',', p3 + 1);
        if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos || p4 == std::string::npos) {
            std::cerr << "Dong doc gia bi loi dinh dang: " << info << std::endl;
            continue;
        }

        try {
            mathe = std::stoi(info.substr(0, p1));
            ho = info.substr(p1 + 1, p2 - p1 - 1);
            ten = info.substr(p2 + 1, p3 - p2 - 1);
            phai = std::stoi(info.substr(p3 + 1, p4 - p3 - 1));
            trangthai = std::stoi(info.substr(p4 + 1));
        } catch (...) {
            std::cerr << "Loi khi parse doc gia: " << info << std::endl;
            continue;
        }

        dg = taoDocGia(ho, ten, phai != 0, trangthai, mathe);

        size_t start = 0;
        while (start < muontras.size()) {
            size_t posComma = muontras.find(',', start);
            std::string token;
            if (posComma == std::string::npos) {
                token = muontras.substr(start);
                start = muontras.size();
            } else {
                token = muontras.substr(start, posComma - start);
                start = posComma + 1;
            }
            if (!token.empty()) {
                MuonTra mt;
                if (token.size() >= 3 && token.find("(T)") == token.size() - 3) {
                    mt.MaSach = token.substr(0, token.size() - 3);
                    mt.TrangThai = 1;
                } else if (token.size() >= 3 && token.find("(M)") == token.size() - 3) {
                    mt.MaSach = token.substr(0, token.size() - 3);
                    mt.TrangThai = 2;
                } else {
                    mt.MaSach = token;
                    mt.TrangThai = 0;
                }
                themMuonTra(dg, mt);
            }
        }

        // insert vao cay AVL de giu can bang
        InsertDocGia(root, dg);
    }
    in.close();
    return root;
}
