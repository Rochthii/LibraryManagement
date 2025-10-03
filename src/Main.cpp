
#include <iostream>
#include <string>
#include <ctime>
#include <cctype>

#include "../include/DauSach.h"
#include "../include/InputUtils.h"

using namespace std;

// Ham nhap va them dau sach tu dong (sinh ISBN, tao ban sao)
void nhapVaThemDauSachTuDong() {
    using namespace InputUtils;
    thongBao(cout, "=== Thêm đầu sách và đánh mã sách tự động ===", THONG_TIN);

    try {
        // Tự động sinh ISBN (đánh mã sách tự động)
        string isbn = SinhISBN13TuDong(cout, false); // Không cho phép nhập thủ công
        thongBao(cout, string("✓ Đã tạo ISBN: ") + isbn, THONG_TIN);
        
        // Nhập thông tin đầu sách - sử dụng hàm có sẵn
        cout << "\nNhập thông tin đầu sách:\n";
        string tenSach = NhapTenNhan("tên sách", 1, 200, cin, cout);
        int soTrang = NhapSoNguyenNhan("số trang", 1, 50000, cin, cout);
        string tacGia = NhapTenNhan("tác giả", 1, 150, cin, cout);
        
        // Sử dụng hàm LayNamHienTai() có sẵn thay vì tính toán thủ công
        int namHienTai = LayNamHienTai();
        int namXB = NhapSoNguyenNhan("năm xuất bản", 1000, namHienTai + 5, cin, cout);
        
        string theLoai = NhapTenNhan("thể loại", 1, 100, cin, cout);

        // Thêm đầu sách vào danh sách (tự động sắp xếp tăng dần theo tên)
        cout << "\nĐang thêm vào danh sách...\n";
        bool ok = themDauSach(dsDauSach, soLuongDauSach, isbn, tenSach, soTrang, tacGia, namXB, theLoai);
        if (ok) {
            thongBao(cout, "✓ Đã thêm đầu sách và tự động sắp xếp theo tên!", THONG_TIN);
            
            // Nhap so ban sao
            int soLuong = NhapSoNguyenNhan("số bản sao", 0, 10000, cin, cout);
            if (soLuong > 0) {
                // Tim dau sach vua them va tao ban sao
                PTRDAUSACH p = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
                if (p) {
                    // Dem so ban sao hien tai truoc khi them
                    int soHienTai = 0;
                    PTRDMS temp = p->dms;
                    while (temp) {
                        soHienTai++;
                        temp = temp->next;
                    }
                    
                    // Tao ban sao moi
                    themDanhMucSach(p->dms, isbn, soLuong);
                    
                    // Nhap vi tri ke chung cho tat ca cac ban sao moi
                    string viTriChung = NhapTenNhan("vị trí kệ", 0, 50, cin, cout);
                    
                    // Gan vi tri chung cho tat ca cac ban sao moi
                    temp = p->dms;
                    int dem = 0;
                    while (temp && dem < soHienTai) {
                        temp = temp->next;
                        dem++;
                    }
                    
                    // Gan vi tri cho cac ban sao moi
                    int stt = 1;
                    while (temp && stt <= soLuong) {
                        temp->viTri = viTriChung;
                        cout << "✓ Bản sao " << stt << " (Mã: " << temp->maSach << ") -> " << viTriChung << "\n";
                        temp = temp->next;
                        stt++;
                    }
                    
                    thongBao(cout, string("✓ Tạo ") + to_string(soLuong) + " bản sao tại kệ '" + viTriChung + "' thành công!", THONG_TIN);
                }
            }
            
            // Tu dong luu file - sử dụng hàm có sẵn
            if (GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach)) {
                thongBao(cout, "✓ Dữ liệu đã được lưu tự động.", THONG_TIN);
            }
        } else {
            thongBao(cout, "✗ Không thể thêm đầu sách!", LOI);
        }
    } catch (const exception& e) {
        thongBao(cout, string("Lỗi: ") + e.what(), LOI);
    }
}

// Ham in danh sach dau sach theo the loai
void inDanhSachDauSach() {
    using namespace InputUtils;
    thongBao(cout, "=== In đầu sách theo thể loại ===", THONG_TIN);
    
    // Sử dụng hàm kiểm tra có sẵn thay vì kiểm tra thủ công
    if (!KiemTraDanhSachHopLe(dsDauSach, soLuongDauSach, MAX_DAUSACH, "Danh sách đầu sách")) {
        return;
    }
    
    if (soLuongDauSach == 0) { 
        thongBao(cout, "Chưa có đầu sách nào để hiển thị.", THONG_TIN); 
        return; 
    }
    
    try {
        // Goi ham in danh sach
        inDanhSachTheoTheLoai(dsDauSach, soLuongDauSach);
        
        // Dem so dau sach thuc te va su dung ham InTongKet
        int count = 0;
        for (int i = 0; i < soLuongDauSach; ++i) {
            if (dsDauSach[i]) count++;
        }
        InTongKet("Tổng số đầu sách", count, count, 0);
        
    } catch (const exception& e) {
        thongBao(cout, string("Lỗi khi hiển thị danh sách: ") + e.what(), LOI);
    }
}

// Ham tim kiem dau sach theo ten - nang cao
void timKiemDauSach() {
    using namespace InputUtils;
    thongBao(cout, "=== Tìm sách ===", THONG_TIN);
    
    try {
        // Nhap tu khoa tim kiem
        string tuKhoa = NhapTenNhan("từ khóa tìm kiếm", 1, 500, cin, cout);
        
        // Goi ham tim kiem nang cao va xu ly ket qua
        int found = timDauSach(dsDauSach, soLuongDauSach, tuKhoa);
        
        // Bao cao ket qua tim kiem
        if (found < 0) {
            thongBao(cout, "Có lỗi xảy ra trong quá trình tìm kiếm!", LOI);
        } else if (found == 0) {
            thongBao(cout, string("Không tìm thấy sách với từ khóa: \"") + tuKhoa + "\"", THONG_TIN);
            cout << "\nGợi ý:\n";
            cout << "- Thử tìm với từ khóa ngắn hơn\n";
            cout << "- Kiểm tra chính tả\n";
            cout << "- Thử tìm theo tác giả hoặc thể loại\n";
        } else {
            thongBao(cout, string("\n=== HOÀN TẤT: Tìm thấy ") + to_string(found) + " kết quả ===", THONG_TIN);
        }
        
    } catch (const exception& e) {
        thongBao(cout, string("Lỗi khi tìm kiếm: ") + e.what(), LOI);
    }
}

// Ham nhap lua chon menu - xu ly input an toan
int nhapLuaChonMenu() {
    using namespace InputUtils;
    
    string dong;
    
    // Xoa buffer neu co loi truoc do
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
    }
    
    cout << "Nhập lựa chọn (1-5): ";
    
    try {
        // Dung getline de tranh loi buffer
        if (!getline(cin, dong)) {
            // Kiểm tra nếu đã hết input (EOF)
            if (cin.eof()) {
                thongBao(cout, "Hết dữ liệu đầu vào, thoát chương trình.", THONG_TIN);
                return 0; // Trả về 0 để thoát
            }
            thongBao(cout, "Lỗi đọc input!", LOI);
            return -1;
        }
        
        dong = CatKhoangTrang(dong);
        if (dong.empty()) {
            thongBao(cout, "Lựa chọn không được rỗng!", LOI);
            return -1;
        }
        
        // Xu ly ky tu c, d, e
        if (dong.length() == 1) {
            char ch = tolower(dong[0]);
            if (ch == 'c') return 1;      // c -> option 1
            if (ch == 'd') return 2;      // d -> option 2  
            if (ch == 'e') return 3;      // e -> option 3
            if (ch >= '0' && ch <= '6') { // 0-6 -> direct mapping
                return ch - '0';
            }
        }
        
        thongBao(cout, "Lựa chọn không hợp lệ! Nhập c/d/e hoặc 0-6.", LOI);
        return -1;
        
    } catch (const exception& e) {
        thongBao(cout, "Input không hợp lệ, vui lòng thử lại!", LOI);
        return -1;
    }
}

// Ham xoa dau sach(neu sau nay can sd)
void xoaDauSach() {
    using namespace InputUtils;
    thongBao(cout, "=== Tìm và Xóa đầu sách ===", THONG_TIN);
    
    if (soLuongDauSach == 0) {
        thongBao(cout, "Không có đầu sách nào để xóa.", THONG_TIN);
        return;
    }
    
    try {
        // Tim kiem truoc
        string tuKhoa = NhapTenNhan("từ khóa tìm sách (tên hoặc ISBN)", 1, 500, cin, cout);
        cout << "\nĐang tìm kiếm...\n";
        
        int found = timDauSach(dsDauSach, soLuongDauSach, tuKhoa);
        
        if (found <= 0) {
            thongBao(cout, "Không tìm thấy sách nào!", THONG_TIN);
            return;
        }
        
        // Chon ISBN de xoa
        string isbn = NhapTenNhan("ISBN của sách muốn xóa", 10, 20, cin, cout);
        PTRDAUSACH sachCanXoa = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
        
        if (!sachCanXoa) {
            thongBao(cout, "Không tìm thấy sách với ISBN này!", THONG_TIN);
            return;
        }
        
        // Hien thi va xac nhan
        cout << "\nThông tin sách sẽ bị xóa:\n";
        InMotDauSach(sachCanXoa);
        
        cout << " Xóa vĩnh viễn đầu sách và TẤT CẢ bản sao. Xác nhận? (y/n): ";
        string xacNhan;
        getline(cin, xacNhan);
        xacNhan = CatKhoangTrang(xacNhan);
        
        if (xacNhan == "y" || xacNhan == "Y") {
            // Tìm vị trí của đầu sách trong mảng
            int viTriXoa = -1;
            for (int i = 0; i < soLuongDauSach; i++) {
                if (dsDauSach[i] != nullptr && dsDauSach[i]->ISBN == isbn) {
                    viTriXoa = i;
                    break;
                }
            }
            
            if (viTriXoa != -1) {
                // Lưu tên sách để thông báo
                string tenSach = dsDauSach[viTriXoa]->tenSach;
                
                // Giải phóng danh mục sách liên kết
                if (dsDauSach[viTriXoa]->dms != nullptr) {
                    GiaiPhongDanhMucSach(dsDauSach[viTriXoa]->dms);
                }
                
                // Giải phóng đầu sách
                delete dsDauSach[viTriXoa];
                
                // Dịch chuyển các phần tử phía sau lên trước
                for (int i = viTriXoa; i < soLuongDauSach - 1; i++) {
                    dsDauSach[i] = dsDauSach[i + 1];
                }
                
                // Đặt phần tử cuối về nullptr và giảm số lượng
                dsDauSach[soLuongDauSach - 1] = nullptr;
                soLuongDauSach--;
                
                thongBao(cout, "✓ Đã xóa đầu sách thành công!", THONG_TIN);
                
                // Tu dong luu file
                if (GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach) &&
                    GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach)) {
                    thongBao(cout, "✓ Dữ liệu đã được lưu tự động.", THONG_TIN);
                }
            } else {
                thongBao(cout, "✗ Không tìm thấy đầu sách để xóa!", LOI);
            }
        } else {
            thongBao(cout, "Đã hủy thao tác xóa.", THONG_TIN);
        }
        
    } catch (const exception& e) {
        thongBao(cout, string("Lỗi khi xóa sách: ") + e.what(), LOI);
    }
}

// Ham main - chuong trinh chinh
int main() {
    using namespace InputUtils;
    
    // Nap du lieu tu file khi khoi dong
    NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach);
    NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach);
    
    // Thong bao neu chua co du lieu
    if(soLuongDauSach == 0){
        thongBao(cout, "Chưa có dữ liệu đầu sách. Bạn có thể thêm mới hoặc nạp file sau.", THONG_TIN);
    }
    
    // Vong lap menu chinh
    while(true) {
        // Hien thi menu
        thongBao(cout, "\n===== MENU THƯ VIỆN (chức năng c, d, e) =====", THONG_TIN);
        cout << "1. c) Nhập thông tin đầu sách và đánh mã sách tự động\n";
        cout << "2. d) In danh sách đầu sách theo thể loại (tên tăng dần)\n";
        cout << "3. e) Tìm thông tin sách theo tên sách\n";
        cout << "4. Tải dữ liệu từ file\n";
        cout << "5. Lưu dữ liệu ra file\n";
        cout << "0. Thoát\n";

        // Nhap lua chon tu nguoi dung
        int choice = nhapLuaChonMenu();
        
        // Xu ly input khong hop le
        if (choice == -1) {
            cout << "\n"; // Them khoang cach cho dep
            continue;  // Thu lai
        }
        
        // Thoat chuong trinh
        if (choice == 0) {
            break;
        }
        
        // Xu ly cac lua chon
        try {
            switch(choice) {
                case 1: 
                    nhapVaThemDauSachTuDong(); 
                    break;
                case 2: 
                    inDanhSachDauSach(); 
                    break;
                case 3: 
                    timKiemDauSach(); 
                    break;
                case 4: 
                    // Tai lai du lieu tu file
                    NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach);
                    NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach);
                    thongBao(cout, "Dữ liệu đã được tải lại.", THONG_TIN);
                    break;
                case 5: 
                    // Luu du lieu ra file
                    if (GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach) &&
                        GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach)) {
                        thongBao(cout, "Lưu dữ liệu thành công.", THONG_TIN);
                    } else {
                        thongBao(cout, "Có lỗi khi lưu dữ liệu.", LOI);
                    }
                    break;
                default: 
                    thongBao(cout, "Lựa chọn không hợp lệ! Vui lòng chọn từ 0-6.", LOI);
                    break;
            }
        } catch (const exception& e) {
            thongBao(cout, string("Lỗi khi thực thi: ") + e.what(), LOI);
        }
    }
    
    // Giai phong bo nho truoc khi thoat
    GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
    thongBao(cout, "Chương trình kết thúc. Cảm ơn bạn đã sử dụng!", THONG_TIN);
    
    return 0;
}
