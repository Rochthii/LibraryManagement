#include "DocGia.h"
#include "NgayThang.h"
#include <fstream>
#include <string>

// --------------------------------------------------
// CAC HAM THAO TAC TREN CAY DOC GIA (AVL TREE)
// --------------------------------------------------

void giaiPhongDsmt(MUONTRA& dsmt) {
    while (dsmt != nullptr) {
        MUONTRA tmp = dsmt;
        dsmt = dsmt->next;
        delete tmp;
    }
}

void GiaiPhongCay(PTRDG& root) {
    if (!root) return;
    GiaiPhongCay(root->left);
    GiaiPhongCay(root->right);
    giaiPhongDsmt(root->data.dsmt);
    delete root;
    root = nullptr;
}

// ----------------------------------------------
// CAC HAM CO BAN: TAO / THEM / TIM / XOA DOC GIA
// ----------------------------------------------

PTRDG taoDocGia(std::string ho, std::string ten, bool phai, int trangthai, PTRDG root) {
    PTRDG p = new NodeDG;
    p->data.MaThe = rand() % 9000 + 1000; // Sinh ngau nhien tam thoi
    p->data.Ho = ho;
    p->data.Ten = ten;
    p->data.Phai = phai;
    p->data.TrangThai = trangthai;
    p->data.dsmt = nullptr;
    p->left = p->right = nullptr;
    p->height = 1;
    return p;
}

PTRDG timDocGia(PTRDG root, int maThe) {
    while (root != nullptr && maThe != root->data.MaThe) {
        root = (maThe < root->data.MaThe) ? root->left : root->right;
    }
    return root;
}

static inline int getHeight(PTRDG n) { return n ? n->height : 0; }
static inline void updateHeight(PTRDG n) {
    if (n) n->height = 1 + (getHeight(n->left) > getHeight(n->right) ? getHeight(n->left) : getHeight(n->right));
}
static inline int getBalance(PTRDG n) {
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}

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

PTRDG InsertDocGiaRec(PTRDG root, PTRDG node) {
    if (!root) return node;
    if (node->data.MaThe < root->data.MaThe)
        root->left = InsertDocGiaRec(root->left, node);
    else if (node->data.MaThe > root->data.MaThe)
        root->right = InsertDocGiaRec(root->right, node);
    else
        return root;

    updateHeight(root);
    int balance = getBalance(root);

    if (balance > 1 && node->data.MaThe < root->left->data.MaThe) return RotateRight(root);
    if (balance < -1 && node->data.MaThe > root->right->data.MaThe) return RotateLeft(root);
    if (balance > 1 && node->data.MaThe > root->left->data.MaThe) {
        root->left = RotateLeft(root->left);
        return RotateRight(root);
    }
    if (balance < -1 && node->data.MaThe < root->right->data.MaThe) {
        root->right = RotateRight(root->right);
        return RotateLeft(root);
    }
    return root;
}

void InsertDocGia(PTRDG& root, PTRDG node) {
    if (node) node->height = 1;
    root = InsertDocGiaRec(root, node);
}

static PTRDG minValueNode(PTRDG node) {
    PTRDG cur = node;
    while (cur->left) cur = cur->left;
    return cur;
}

PTRDG xoaDocGiaRec(PTRDG root, int maThe) {
    if (!root) return root;

    if (maThe < root->data.MaThe)
        root->left = xoaDocGiaRec(root->left, maThe);
    else if (maThe > root->data.MaThe)
        root->right = xoaDocGiaRec(root->right, maThe);
    else {
        giaiPhongDsmt(root->data.dsmt);
        if (!root->left || !root->right) {
            PTRDG temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        }
        else {
            PTRDG succ = minValueNode(root->right);
            root->data = succ->data;
            root->right = xoaDocGiaRec(root->right, succ->data.MaThe);
        }
    }

    updateHeight(root);
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) return RotateRight(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = RotateLeft(root->left);
        return RotateRight(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) return RotateLeft(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = RotateRight(root->right);
        return RotateLeft(root);
    }

    return root;
}

void xoaDocGia(PTRDG& root, int maThe) {
    root = xoaDocGiaRec(root, maThe);
}

void hieuChinhDocGia(PTRDG root, PTRDG pAlt, int maThe) {
    PTRDG p = timDocGia(root, maThe);
    if (!p || !pAlt) return;
    p->data.Ho = pAlt->data.Ho;
    p->data.Ten = pAlt->data.Ten;
    p->data.Phai = pAlt->data.Phai;
    p->data.TrangThai = pAlt->data.TrangThai;
}

// --------------------------------------------------
// CAC HAM LAM VIEC VOI MUON TRA
// --------------------------------------------------

int demSachDangMuon(MUONTRA dsmt) {
    int count = 0;
    MUONTRA p = dsmt;
    while (p) {
        if (p->data.TrangThai == 0) count++;
        p = p->next;
    }
    return count;
}

bool coSachQuaHan(PTRDG dg) {
    if (!dg) return false;
    MUONTRA mt = dg->data.dsmt;
    std::string today = layNgayHienTai();
    while (mt) {
        if (mt->data.TrangThai == 0) {
            int days = tinhKhoangCachNgay(mt->data.NgayMuon, today);
            if (days > 7) return true;
        }
        mt = mt->next;
    }
    return false;
}

// --------------------------------------------------
// LUU / TAI FILE
// --------------------------------------------------

void saveDocGia(PTRDG root) {
    std::ofstream out("docgia.txt");
    if (!out.is_open()) return;

    std::function<void(PTRDG)> traverse = [&](PTRDG node) {
        if (!node) return;
        traverse(node->left);

        out << node->data.MaThe << "," << node->data.Ho << ","
            << node->data.Ten << "," << (node->data.Phai ? 1 : 0) << ","
            << node->data.TrangThai << "\n";

        traverse(node->right);
        };
    traverse(root);
    out.close();
}

PTRDG loadDocGia() {
    std::ifstream in("docgia.txt");
    if (!in.is_open()) return nullptr;

    PTRDG root = nullptr;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        int ma, phai, trangThai;
        std::string ho, ten;
        sscanf(line.c_str(), "%d,%[^,],%[^,],%d,%d", &ma, &ho[0], &ten[0], &phai, &trangThai);
        PTRDG node = taoDocGia(ho, ten, phai, trangThai, root);
        node->data.MaThe = ma;
        InsertDocGia(root, node);
    }
    in.close();
    return root;
}
