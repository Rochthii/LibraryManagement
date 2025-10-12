#include "DocGia.h"
//#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "NgayThang.h"

static PTRDG InsertDocGiaRec(PTRDG pavtree, PTRDG node);
static PTRDG xoaDocGiaRec(PTRDG root, int mathe);

int sinhMaTheNgauNhien(PTRDG root) {
    srand((unsigned int)time(NULL));
    int ma;
    do {
        ma = rand() % 9000 + 1000;
    } while (timDocGia(root, ma) != NULL);
    return ma;
}

//bst-avl
int DoCao(PTRDG root){
    if(root == NULL) return 0;
    int trai = DoCao(root->left);
    int phai = DoCao(root->right);
    return 1 + max(trai, phai);
}

int bfCalc(PTRDG root){
    int lh, rh;
    lh = DoCao(root->left);
    rh = DoCao(root->right);
    return lh - rh;
}

PTRDG vitriImbal(PTRDG root){
    PTRDG vitri;
    if(root == NULL) return NULL;
    
    if(abs(bfCalc(root)) > 1) return root;
    
    vitri = vitriImbal(root->left);
    if(vitri != NULL) return vitri;
    
    vitri = vitriImbal(root->right);
    return vitri;
}
PTRDG RotateLeft(PTRDG p){
    PTRDG cp = p->right;
    
    p->right = cp->left;
    cp->left = p;
    return cp;
}
PTRDG RotateRight(PTRDG p){
    PTRDG cp = p->left;
    
    p->left = cp->right;
    cp->right = p;
    return cp;
}
//Them AVL
static PTRDG InsertDocGiaRec(PTRDG pavltree, PTRDG node){
        // insert node vao
        if (pavltree == nullptr)
            return node;

        if (node->data.MaThe < pavltree->data.MaThe)
            pavltree->left = InsertDocGiaRec(pavltree->left, node);
        else if (node->data.MaThe > pavltree->data.MaThe)
            pavltree->right = InsertDocGiaRec(pavltree->right, node);
        else
            return pavltree;
        // kiem tra chieu cao
        int balance = bfCalc(pavltree);
        // Logic AVL
        if (balance > 1 && node->data.MaThe < pavltree->left->data.MaThe) {
            return RotateRight(pavltree);//LL
        }
        if (balance < -1 && node->data.MaThe > pavltree->right->data.MaThe) {
            return RotateLeft(pavltree);//RR
        }
        if (balance > 1 && node->data.MaThe > pavltree->left->data.MaThe) {
            pavltree->left = RotateLeft(pavltree->left);
            return RotateRight(pavltree);//LR
        }
        if (balance < -1 && node->data.MaThe < pavltree->right->data.MaThe) {
            pavltree->right = RotateRight(pavltree->right);
            return RotateLeft(pavltree);//RL
        }
        return pavltree;
}
void InsertDocGia(PTRDG &root, PTRDG node) {
    root = InsertDocGiaRec(root, node);
}

//quan ly cay doc gia
//tao PTRDG docgia
PTRDG taoDocGia(string ho, string ten, bool phai, int trangthai, PTRDG root){
    PTRDG dg = new NodeDG;
    dg->data.MaThe = sinhMaTheNgauNhien(root);
    dg->data.Ho = ho;
    dg->data.Ten = ten;
    dg->data.Phai = phai;
    dg->data.TrangThai = trangthai;
    dg->data.dsmt = NULL;
    dg->left = dg->right = NULL;
    return dg;
}
PTRDG taoDocGia(string ho, string ten, bool phai, int trangthai, int mathe){
    PTRDG dg = new NodeDG;
    dg->data.MaThe = mathe;
    dg->data.Ho = ho;
    dg->data.Ten = ten;
    dg->data.Phai = phai;
    dg->data.TrangThai = trangthai;
    dg->data.dsmt = NULL;
    dg->left = dg->right = NULL;
    return dg;
}
//Them BST
PTRDG themDocGia(PTRDG &root, PTRDG node){
    if(root == NULL) return node;
    
    if(node->data.MaThe < root->data.MaThe) root->left = themDocGia(root->left, node);
    else if(node->data.MaThe > root->data.MaThe) root->right = themDocGia(root->right, node);
    else{
        cerr << "Ma PTRDG bi trung, khong the them!"<< endl;
    }
    return root;
}

void giaiPhongDsmt(MUONTRA& dsmt) {
    while (dsmt != NULL) {
        MUONTRA tmp = dsmt;
        dsmt = dsmt->next;
        delete tmp;
    }
}

static PTRDG xoaDocGiaRec(PTRDG root, int mathe) {
    if (root == NULL) return root;
    //Tim node can xoa
    if (mathe < root->data.MaThe) {
        root->left = xoaDocGiaRec(root->left, mathe);
    }
    else if (mathe > root->data.MaThe) {
        root->right = xoaDocGiaRec(root->right, mathe);
    }
    else {//mathe == root->data.MaThe (tim duoc node can xoa)
        //TH1: khong con hoac 1 con
        giaiPhongDsmt(root->data.dsmt);

        if (root->left == NULL || root->right == NULL) {
            PTRDG temp = root->left ? root->left : root->right;
            delete root;
            root = NULL;//khac phu loi con tro treo
            return temp;
        }
        //TH2: co 2 con
        PTRDG temp = root->right;
        while (temp->left != NULL) temp = temp->left;
        //copy du lieu the mang
        root->data = temp->data;
        //xoa the mang
        root->right = xoaDocGiaRec(root->right, temp->data.MaThe);
    }
    //neu cay chi co mot la
    if (root == NULL) return root;
    //Can bang lai cay
    int balance = bfCalc(root);
    int leftBalance = bfCalc(root->left);
    int rightBalance = bfCalc(root->right);
    //LL
    if (balance > 1 && leftBalance >= 0) {
        return RotateRight(root);
    }
    //LR
    if (balance > 1 && leftBalance < 0) {
        root->left = RotateLeft(root->left);
        return RotateRight(root);
    }
    //RR
    if (balance < -1 && rightBalance <= 0) {
        return RotateLeft(root);
    }
    //RL
    if (balance < -1 && rightBalance > 0) {
        root->right = RotateRight(root->right);
        return RotateLeft(root);
    }
    return root;
}
void xoaDocGia(PTRDG& root, int mathe) {
    root = xoaDocGiaRec(root, mathe);
}

PTRDG timDocGia(PTRDG root, int mathe){
    while (root != NULL && mathe != root->data.MaThe) {
        if (mathe < root->data.MaThe)   
            root = root->left;
        else
            root = root->right;
    }
    return root;
}
void display(PTRDG root){
    cout << "MaThe: " << root->data.MaThe
         << " | HoTen: " << root->data.Ho << " " << root->data.Ten
         << " | Phai: " << (root->data.Phai ? "Nu" : "Nam")
         << " | TrangThai: " << (root->data.TrangThai ? "Hoat dong" : "Khoa")
         << endl;
}
void inDocGiaInOrder(PTRDG root){
    if(root != NULL){
        inDocGiaInOrder(root->left);
        display(root);
        inDocGiaInOrder(root->right);
    }
}

//quan ly muon tra
void themMuonTra(PTRDG docgia, string maSach){
    if(docgia == NULL)return;
    
    MUONTRA node = new NodeMT;
    node->data.MaSach = maSach;
    node->data.NgayMuon = layNgayHienTai();
    node->data.NgayTra = "";
    node->data.TrangThai = 0;//dang muon
    node->next = NULL;
    
    if(docgia->data.dsmt == NULL){
        docgia->data.dsmt = node;
    }
    else {
        node->next = docgia->data.dsmt;
        docgia->data.dsmt = node;
    }
}

void themMuonTra(PTRDG docgia, const MuonTra &mt) {
    if (docgia == NULL) return;

    MUONTRA node = new NodeMT;
    node->data = mt;
    node->next = docgia->data.dsmt;
    docgia->data.dsmt = node;
}

void saveDsMuonTra(MUONTRA ds, ofstream &out){
    while(ds != NULL){
        out << ds->data.MaSach;
        if(ds->data.TrangThai == 1) out << "(T)";//da tra
        else if(ds->data.TrangThai == 2) out << "(M)";//da mat
        
        if(ds->next != NULL) out << ",";
        ds = ds->next;
    }
}

void saveDocGia(PTRDG root, ofstream &out){
    if(root == NULL) return;
    
    //duyet LNR
    saveDocGia(root->left, out);
    // , , , , | , ,
    out << root->data.MaThe << ","
        << root->data.Ho << ","
        << root->data.Ten << ","
        <<root->data.Phai << ","
        << root->data.TrangThai << "|";
    saveDsMuonTra(root->data.dsmt, out);
    out << "\n";
    
    saveDocGia(root->right, out);
}

void saveDocGia(PTRDG root){
    ofstream out("docgia.txt");
    if(!out.is_open()){
        cout << "Khong mo duoc file de ghi!"<< endl;
        return;
    }
    saveDocGia(root, out);
    out.close();
}


PTRDG loadDocGia(){
    ifstream in("docgia.txt");//mo file
    if(!in.is_open()){//mo that bai
        cout << "khong mo duoc file de doc!" <<endl;
        return NULL;
    }
    
    PTRDG root = NULL;//khoi tao cay BST rong
    string line;
    while(getline(in, line)){//doc tung dong file
        if(line.empty()) continue;//bo qua dong trong
        
        //tach thong tin doc gia va ds muon tra
        size_t posSlash = line.find('|');//tim vi tri dau |
        string info = line.substr(0,posSlash);//thong tin doc gia trc dau |
        string muontras = (posSlash == string::npos) ? line.substr(posSlash +1) : "";//thong tin muon tra sau dau | neu khong co de rong
        
        //tach thong tin doc gia theo dau ,
        int mathe, phai, trangthai;
        string ho, ten;
        PTRDG dg;
        //tach tung dau ,
        size_t p1 = info.find(',');
        size_t p2 = info.find(',', p1 + 1);
        size_t p3 = info.find(',', p2 + 1);
        size_t p4 = info.find(',', p3 + 1);
        //chuyen doi du lieu de cho vao dg
        try {
            mathe = stoi(info.substr(0, p1));
            ho = info.substr(p1 + 1, p2 - p1 - 1);
            ten = info.substr(p2 + 1, p3 - p2 - 1);
            phai = stoi(info.substr(p3 + 1, p4 - p3 - 1));
            trangthai = stoi(info.substr(p4 + 1));
        }
        catch (...) {
            cerr << "Loi khi parse doc gia:" << info << endl;
            continue;
        }
        //tao node tu du lieu tren
        dg = taoDocGia(ho, ten, phai, trangthai, mathe);
        //xu li ds muon tra
        size_t start = 0;
        while(start < muontras.size()){//duyet tung token trong chuoi muon tra
            size_t posComma = muontras.find(',', start);//tim dau ,
            string token;
            if(posComma == string::npos){//truong hop cuoi cung
                token = muontras.substr(start);//lay token cuoi cung
                start = muontras.size();//ket thuc vong lap
            }
            else {
                token = muontras.substr(start, posComma - start);//gan vi tri giua 2 dau ,
                start = posComma + 1;//tiep tuc sau dau ,
            }
            if(!token.empty()){//xu ly token
                MuonTra mt;
                if(token.size() >= 3 && token.find("(T)") == token.size() - 3) {//xet trang thai de xoa (T) va tra ve gia tri dung
                    mt.MaSach = token.substr(0, token.size() - 3);
                    mt.TrangThai = 1;
                }
                else if(token.size() >= 3 && token.find("(M)") == token.size() - 3) {//xet trang thai de xoa (M) va tra ve gia tri dung
                    mt.MaSach = token.substr(0, token.size() - 3);
                    mt.TrangThai = 2;
                }
                else{//khong can xoa va chi can tra ve dang muon
                    mt.MaSach = token;
                    mt.TrangThai = 0;
                }
                themMuonTra(dg, mt);// Them muon tra vao node doc gia
            }
        }
        themDocGia(root, dg);//them vao BST
    }
    in.close();
    return root;
}
