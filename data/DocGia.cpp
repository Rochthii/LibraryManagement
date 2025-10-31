#include "DocGia.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Constants.h"
#include "NgayThang.h"
// Cần thêm thư viện XuLyChuoi.h cho hàm BoDauVaThuong()
#include "XuLyChuoi.h" 
// Cần thêm thư viện QuanLySach.h nếu sử dụng TimDanhMucSachTuMaSach
// #include "QuanLySach.h" 

// helper thay the max/abs tu <algorithm>
static inline int my_max(int a, int b) { return (a > b) ? a : b; }
static inline int my_abs(int x) { return (x < 0) ? -x : x; }

// forward-declare static helpers
static PTRDG InsertDocGiaRec(PTRDG pavtree, PTRDG node);
static PTRDG xoaDocGiaRec(PTRDG root, int mathe);
// Lay lai prototype vi static helper dung
PTRDG timDocGia(PTRDG root, int mathe); 

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

    // LL (Left Left Case)
    if (balance > 1 && getBalance(pavltree->left) >= 0)
        return RotateRight(pavltree);
    // RR (Right Right Case)
    if (balance < -1 && getBalance(pavltree->right) <= 0)
        return RotateLeft(pavltree);
    // LR (Left Right Case)
    if (balance > 1 && getBalance(pavltree->left) < 0) {
        pavltree->left = RotateLeft(pavltree->left);
        return RotateRight(pavltree);
    }
    // RL (Right Left Case)
    if (balance < -1 && getBalance(pavltree->right) > 0) {
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
    dg->data.soSachDangMuon = 0; 
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
    dg->data.soSachDangMuon = 0; 
    dg->left = dg->right = nullptr;
    dg->height = 1;
    return dg;
}

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
        if (root->left == nullptr || root->right == nullptr) {
            // Trường hợp 0 hoặc 1 con
            giaiPhongDsmt(root->data.dsmt);
            PTRDG temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        } else {
            // Trường hợp 2 con: lấy inorder successor (succ)
            PTRDG succ = root->right;
            while (succ->left != nullptr) succ = succ->left;

            // CẢI TIẾN: Sửa lỗi bộ nhớ và cập nhật trường mới
            giaiPhongDsmt(root->data.dsmt); 
            
            // Sao chép dữ liệu của succ sang root
            root->data.MaThe = succ->data.MaThe;
            root->data.Ho = succ->data.Ho;
            root->data.Ten = succ->data.Ten;
            root->data.Phai = succ->data.Phai;
            root->data.TrangThai = succ->data.TrangThai;
            root->data.soSachDangMuon = succ->data.soSachDangMuon; // Cập nhật trường mới
            
            // Chuyển quyền sở hữu con trỏ dsmt từ succ sang root
            root->data.dsmt = succ->data.dsmt;
            succ->data.dsmt = nullptr;

            // Đệ quy xóa nút successor
            root->right = xoaDocGiaRec(root->right, root->data.MaThe);
        }
    }

    // cap nhat height va rebalance 
    if (root == nullptr) return root;
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
void hieuChinhDocGia(PTRDG root, const TheDocGia& new_data) {
    PTRDG p = timDocGia(root, new_data.MaThe);
    if (p == nullptr) {
        std::cerr << "Khong tim thay doc gia co ma the " << new_data.MaThe << std::endl;
        return;
    }

    // Chỉ cập nhật các trường được phép thay đổi
    if (p->data.Ho != new_data.Ho) p->data.Ho = new_data.Ho;
    if (p->data.Ten != new_data.Ten) p->data.Ten = new_data.Ten;
    if (p->data.Phai != new_data.Phai) p->data.Phai = new_data.Phai;
    if (p->data.TrangThai != new_data.TrangThai) p->data.TrangThai = new_data.TrangThai;
    // Không cập nhật MaThe, dsmt, soSachDangMuon
}

// hien thi
void display(PTRDG root) {
    if (root == nullptr) {
        std::cerr << "Khong the hien thi: con tro doc gia null!" << std::endl;
        return;
    }

    std::cout << "MaThe: " << root->data.MaThe
         << " | HoTen: " << root->data.Ho << " " << root->data.Ten
         << " | Phai: " << (root->data.Phai ? "Nu" : "Nam")
         << " | TrangThai: " << (root->data.TrangThai ? "Hoat dong" : "Khoa")
         << " | Sach Dang Muon: " << root->data.soSachDangMuon
         << " | AVL Height: " << root->height
         << std::endl;
}

void inDocGiaInOrder(PTRDG root) {
    if (root != nullptr) {
        inDocGiaInOrder(root->left);
        display(root);
        inDocGiaInOrder(root->right);
    }
}

// --- LOGIC CHO CHỨC NĂNG (b): SẮP XẾP ĐỘC GIẢ ---

// Helper: So sánh hai chuỗi theo Tên + Họ (sử dụng BoDauVaThuong)
static int SoSanhTenHo(PTRDG dg1, PTRDG dg2) {
    // 1. So sánh theo Tên (không dấu, không phân biệt hoa/thường)
    std::string ten1 = BoDauVaThuong(dg1->data.Ten);
    std::string ten2 = BoDauVaThuong(dg2->data.Ten);
    if (ten1 != ten2) {
        return (ten1 < ten2) ? -1 : 1;
    }
    
    // 2. Nếu Tên bằng nhau, so sánh theo Họ
    std::string ho1 = BoDauVaThuong(dg1->data.Ho);
    std::string ho2 = BoDauVaThuong(dg2->data.Ho);
    if (ho1 != ho2) {
        return (ho1 < ho2) ? -1 : 1;
    }
    
    // 3. Nếu Tên và Họ giống nhau, so sánh theo Mã thẻ để đảm bảo thứ tự ổn định
    if (dg1->data.MaThe < dg2->data.MaThe) return -1;
    if (dg1->data.MaThe > dg2->data.MaThe) return 1;
    
    return 0; 
}

static void SwapDG(PTRDG& a, PTRDG& b) {
    PTRDG temp = a; a = b; b = temp;
}

static int PartitionDG(PTRDG arr[], int low, int high) {
    PTRDG pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (SoSanhTenHo(arr[j], pivot) < 0) {
            i++;
            SwapDG(arr[i], arr[j]);
        }
    }
    SwapDG(arr[i + 1], arr[high]);
    return (i + 1);
}

void QuickSortDocGia(PTRDG arr[], int low, int high) {
    if (low < high) {
        int pi = PartitionDG(arr, low, high);
        QuickSortDocGia(arr, low, pi - 1);
        QuickSortDocGia(arr, pi + 1, high);
    }
}

// Duyệt cây InOrder ra mảng
void DuyetCayRaMang(PTRDG root, PTRDG arr[], int &count) {
    if (root != nullptr) {
        DuyetCayRaMang(root->left, arr, count);
        // Giả định MAX_DAUSACH (1000) là kích thước mảng an toàn
        if (count < MAX_DAUSACH) { 
            arr[count++] = root;
        }
        DuyetCayRaMang(root->right, arr, count);
    }
}

// Triển khai hàm chính cho chức năng b: In DS theo Tên+Họ
void InDocGiaTheoTenHo(PTRDG root) {
    PTRDG arr[MAX_DAUSACH]; 
    int count = 0;
    
    DuyetCayRaMang(root, arr, count); 
    
    if (count == 0) {
        std::cout << "\nDanh sách độc giả rỗng." << std::endl;
        return;
    }
    
    QuickSortDocGia(arr, 0, count - 1);

    std::cout << "\n=== DANH SÁCH ĐỘC GIẢ (" << count << " ĐỘC GIẢ) ===\n";
    std::cout << "--- SẮP XẾP THEO TÊN + HỌ TĂNG DẦN ---\n";
    for (int i = 0; i < count; ++i) {
        display(arr[i]);
    }
}
// --- HẾT LOGIC SẮP XẾP ---


// quan ly muon tra
void themMuonTra(PTRDG docgia, std::string maSach) {
    if (docgia == nullptr) return;
    
    // Kiểm tra giới hạn mượn
    if (docgia->data.soSachDangMuon >= SO_LAN_THU_TOI_DA) { // SO_LAN_THU_TOI_DA = 3
        std::cerr << "Lỗi: Độc giả đã mượn tối đa 3 cuốn sách." << std::endl;
        return;
    }

    MUONTRA node = new NodeMT;
    node->data.MaSach = maSach;
    node->data.NgayMuon = layNgayHienTai();
    node->data.NgayTra = "";
    node->data.TrangThai = 0;
    node->next = nullptr;

    node->next = docgia->data.dsmt;
    docgia->data.dsmt = node;
    
    // Cập nhật trường mới
    docgia->data.soSachDangMuon++;
}

void themMuonTra(PTRDG docgia, const MuonTra &mt) {
    if (docgia == nullptr) return;

    MUONTRA node = new NodeMT;
    node->data = mt;
    node->next = docgia->data.dsmt;
    docgia->data.dsmt = node;
    
    // Cập nhật trường mới khi load file
    if (mt.TrangThai == 0) {
        docgia->data.soSachDangMuon++;
    }
}

// loadDocGia
PTRDG loadDocGia() {
    std::ifstream in("docgia.txt");
    if (!in.is_open()) {
        std::cout << "khong mo duoc file docgia.txt de doc!" << std::endl;
        return nullptr;
    }

    PTRDG root = nullptr;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        
        // Cần phải chuẩn hóa logic phân tích file docgia.txt mẫu: 10001|Nguyen|Van A|Nam|1
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

        // Phân tích thông tin độc giả (tách bằng '|')
        std::string parts[5];
        size_t last = 0;
        int idx = 0;
        for (size_t i = 0; i < info.size(); ++i) {
            if (info[i] == '|' && idx < 4) {
                parts[idx++] = info.substr(last, i - last);
                last = i + 1;
            }
        }
        parts[idx] = info.substr(last);

        if (idx < 4) {
             // Thử phân tích lại bằng ký tự phân cách (comma) cũ
             // Bỏ qua nếu vẫn lỗi
             std::cerr << "Dong doc gia bi loi dinh dang: " << info << std::endl;
             continue;
        }

        try {
            mathe = std::stoi(parts[0]);
            ho = parts[1];
            ten = parts[2];
            phai = (parts[3] == "Nu" || parts[3] == "Nữ"); 
            trangthai = std::stoi(parts[4]);
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
                // logic parse MaSach, TrangThai
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