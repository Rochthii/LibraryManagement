#include <iostream>
#include <new>
#include "../include/DauSach.h"
#include "../include/InputUtils.h"

using namespace std;

// Them danh muc sach moi vao danh sach lien ket dua tren ISBN va so luong
// Kiem tra hop le dau vao, sinh ma sach, cap phat bo nho, noi vao danh sach
void themDanhMucSach(PTRDMS& dms, std::string ISBN, int soLuong) {
    using namespace InputUtils;
    
    // Sử dụng hàm kiểm tra có sẵn thay vì viết riêng
    if (!KiemTraTongSoBanSao(ISBN, soLuong)) return;
    if (soLuong == 0) return;
    
    // Kiểm tra định dạng ISBN bằng hàm có sẵn
    string isbnTest;
    if (!ChuanHoaISBNFile(ISBN, isbnTest, std::cout)) return;
    
    try {
        // Tim node cuoi de noi nhanh
        PTRDMS tail = dms;
        while (tail && tail->next) tail = tail->next;
        int daTao = 0;
        
        for (int i = 1; i <= soLuong; ++i) {
            // Sử dụng hàm sinh mã sách có sẵn
            std::string ma = sinhMaSach(ISBN, i, std::cout);
            if (ma.empty()) {
                thongBao(std::cout, "Không thể sinh mã sách!", LOI);
                break;
            }
            
            PTRDMS node = new (std::nothrow) DanhMucSach;
            if (!node) { 
                thongBao(std::cout, "Không đủ bộ nhớ để tạo bản sao!", LOI); 
                break; 
            }
            
            // Gan du lieu cho node moi
            node->maSach = ma;
            node->trangThai = CHO_MUON_DUOC;
            node->viTri = "";
            node->next = nullptr;
            
            // Noi node moi vao cuoi danh sach
            if (!dms) { 
                dms = node; 
                tail = node; 
            } else { 
                tail->next = node; 
                tail = node; 
            }
            ++daTao;
        }
        
        // Sử dụng hàm in tổng kết có sẵn
        InTongKet("Tạo bản sao", daTao, soLuong, soLuong - daTao);
        
    } catch (const std::exception& e) {
        thongBao(std::cout, std::string("Lỗi trong quá trình tạo danh mục: ") + e.what(), LOI);
    }
}

// Cap nhat trang thai cho mot ban sao sach theo ma sach
// Tim kiem node theo ma, kiem tra hop le, cap nhat trang thai
void capNhatTrangThaiSach(PTRDMS dms, std::string maSach, TrangThaiSach trangThai) {
    using namespace InputUtils;
    
    // Sử dụng hàm kiểm tra có sẵn
    if (!KiemTraDanhSachHopLe(dms, 1, 100000, "Danh mục sách")) return;
    if (!KiemTraChuoiVaDodai(maSach, "Mã sách", 50)) return;
    
    // Kiểm tra trạng thái (0, 1, 2)
    if (trangThai < 0 || trangThai > 2) {
        thongBao(std::cout, "Trạng thái sách không hợp lệ (0-2)!", LOI);
        return;
    }
    
    // Tim kiem node theo ma sach va cap nhat trang thai
    try {
        PTRDMS current = dms;
        int dem = 0;  // Dem so lan lap de tranh vong lap vo han
        const int MAX_ITERATIONS = 100000;
        
        while (current && dem < MAX_ITERATIONS) {
            if (current->maSach == maSach) { 
                TrangThaiSach trangThaiCu = current->trangThai;
                current->trangThai = trangThai;
                thongBao(std::cout, std::string("Cập nhật mã sách ") + maSach + 
                    " từ " + TenTrangThai(trangThaiCu) + " thành " + TenTrangThai(trangThai), THONG_TIN);
                return; 
            }
            current = current->next;
            ++dem;
        }
        
        if (dem >= MAX_ITERATIONS) {
            thongBao(std::cout, "Phát hiện vòng lặp vô hạn trong danh sách!", LOI);
        } else {
            thongBao(std::cout, std::string("Không tìm thấy mã sách: ") + maSach, LOI);
        }
    } catch (const std::exception& e) {
        thongBao(std::cout, std::string("Lỗi khi cập nhật trạng thái: ") + e.what(), LOI);
    }
}
