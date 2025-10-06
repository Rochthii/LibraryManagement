#include "DocGia.h"
#include "Utils.h"
#include <iostream>

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

void InsertDocGia(PTRDG &pavltree, PTRDG node){
//    PTRDG fp, p, cp, fya, ya, cya;//f:cha, c: con, ya la nut trc gan nhat co the mat can bang
//    int imbal;
//    //khoi dong cac gia tri;
//    fp = NULL; p = pavltree;
//    fya = NULL; ya = p;
//    //tim fp, ya, fya va cp, cya;
//    while(p != NULL){
//        if(x == p->data.MaThe)
//            return;
//        if(x < p->data.MaThe)
//            cp = p->left;
//        else
//            cp = p->right;
//        if(cp != NULL || cp->bf != 0){
//            fya = p;
//            ya = cp;
//        }
//        fp = p;
//        p = cp;
//    }
    
    
//    PTRDG p = pavltree;
//    //insert node vao
//    PTRDG temp = themDocGia(pavltree, node);
//    //kiem tra lai tinh can bang cua tung node
//        //su ly 3 node lien tiep thoi
//        //LL RR
//
//        //LR RL
    
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

PTRDG themDocGia(PTRDG &root, PTRDG node){
    if(root == NULL) return node;
    
    if(node->data.MaThe < root->data.MaThe) root->left = themDocGia(root->left, node);
    else if(node->data.MaThe > root->data.MaThe) root->right = themDocGia(root->right, node);
    else{
        cerr << "Ma PTRDG bi trung, khong the them!"<< endl;
    }
    return root;
}

void xoaDocGia(PTRDG &root, int mathe);

PTRDG timDocGia(PTRDG root, int mathe){
    if(root ==NULL) return NULL;
    if(mathe < root->data.MaThe)
        return timDocGia(root->left, mathe);
    else if(mathe > root->data.MaThe)
        return timDocGia(root->right, mathe);
    else return root;//da tim thay
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
        mathe = stoi(info.substr(0, p1));
        ho = info.substr(p1 + 1, p2 - p1 - 1);
        ten = info.substr(p2 + 1, p3 - p2 - 1);
        phai = stoi(info.substr(p3 + 1, p4 - p3 - 1));
        trangthai = stoi(info.substr(p4 + 1));
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
                if(token.find("(T)") != string::npos){//xet trang thai de xoa (T) va tra ve gia tri dung
                    mt.MaSach = token.substr(0, token.size() - 3);
                    mt.TrangThai = 1;
                }
                else if(token.find("(M)") != string::npos){//xet trang thai de xoa (M) va tra ve gia tri dung
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
