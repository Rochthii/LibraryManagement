// test_QuanLySach.cpp - Unit tests cho module QuanLySach
// Khong su dung STL containers (vector, set, map) theo yeu cau de bai

#include "../tests/TestFramework.h"
#include "../include/QuanLySach.h"
#include "../include/DauSach.h"
#include "../include/XuLyChuoi.h"
#include "../include/KiemTraDuLieu.h"
#include <sstream>
#include <cstring>

// Helper function de reset du lieu test
void resetTestData() {
    GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
    soLuongDauSach = 0;
    duLieuDaThayDoi = false;
}

// ========================================
// TEST 1: THEM DAU SACH
// ========================================
void test_ThemDauSach_ThanhCong() {
    TestFramework::startTest("ThemDauSach - Them sach hop le");
    resetTestData();

    bool result = themDauSach(dsDauSach, soLuongDauSach, 
                              "9780140449136", "The Odyssey", 300, 
                              "Homer", 1997, "Epic", true);

    TestFramework::assert_true(result, "Them sach thanh cong");
    TestFramework::assert_equal(1, soLuongDauSach, "So luong dau sach = 1");
    TestFramework::assert_not_null(dsDauSach[0], "dsDauSach[0] khong null");
    
    if (dsDauSach[0]) {
        TestFramework::assert_equal(std::string("9780140449136"), dsDauSach[0]->ISBN, "ISBN dung");
        TestFramework::assert_equal(std::string("The Odyssey"), dsDauSach[0]->tenSach, "Ten sach dung");
        TestFramework::assert_equal(300, dsDauSach[0]->soTrang, "So trang dung");
    }
}

void test_ThemDauSach_ISBNTrung() {
    TestFramework::startTest("ThemDauSach - ISBN trung");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, 
                "9780140449136", "The Odyssey", 300, "Homer", 1997, "Epic", true);
    
    bool result = themDauSach(dsDauSach, soLuongDauSach, 
                              "9780140449136", "Another Book", 200, "Someone", 2000, "Fiction", true);

    TestFramework::assert_true(!result, "Khong them duoc sach co ISBN trung");
    TestFramework::assert_equal(1, soLuongDauSach, "So luong van la 1");
}

void test_ThemDauSach_SoTrangKhongHopLe() {
    TestFramework::startTest("ThemDauSach - So trang khong hop le");
    resetTestData();

    bool result1 = themDauSach(dsDauSach, soLuongDauSach, 
                               "9780140449136", "Test Book", 0, "Author", 2000, "Fiction", true);
    TestFramework::assert_true(!result1, "Khong them duoc sach co so trang = 0");

    bool result2 = themDauSach(dsDauSach, soLuongDauSach, 
                               "9780140449137", "Test Book 2", -10, "Author", 2000, "Fiction", true);
    TestFramework::assert_true(!result2, "Khong them duoc sach co so trang am");
}

// ========================================
// TEST 2: TIM KIEM DAU SACH
// ========================================
void test_TimDauSachTheoISBN_ThanhCong() {
    TestFramework::startTest("TimDauSachTheoISBN - Tim thay");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, 
                "9780140449136", "The Odyssey", 300, "Homer", 1997, "Epic", true);
    themDauSach(dsDauSach, soLuongDauSach, 
                "9780140449137", "The Iliad", 250, "Homer", 1995, "Epic", true);

    PTRDS found = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, "9780140449136");
    
    TestFramework::assert_not_null(found, "Tim thay sach");
    if (found) {
        TestFramework::assert_equal(std::string("The Odyssey"), found->tenSach, "Ten sach dung");
    }
}

void test_TimDauSachTheoISBN_KhongThay() {
    TestFramework::startTest("TimDauSachTheoISBN - Khong tim thay");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, 
                "9780140449136", "The Odyssey", 300, "Homer", 1997, "Epic", true);

    PTRDS found = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, "1234567890123");
    
    TestFramework::assert_null(found, "Khong tim thay sach voi ISBN khong ton tai");
}

// ========================================
// TEST 3: SAP XEP DAU SACH
// ========================================
void test_SapXepDauSachTheoTen() {
    TestFramework::startTest("SapXepDauSachTheoTen - Kiem tra thu tu");
    resetTestData();

    // Them cac sach khong theo thu tu
    themDauSach(dsDauSach, soLuongDauSach, "1111111111111", "Zebra", 100, "A1", 2000, "Fiction", true);
    themDauSach(dsDauSach, soLuongDauSach, "2222222222222", "Apple", 100, "A2", 2000, "Fiction", true);
    themDauSach(dsDauSach, soLuongDauSach, "3333333333333", "Mango", 100, "A3", 2000, "Fiction", true);

    // Kiem tra sau khi them da duoc sap xep
    TestFramework::assert_equal(std::string("Apple"), dsDauSach[0]->tenSach, "Sach dau tien la Apple");
    TestFramework::assert_equal(std::string("Mango"), dsDauSach[1]->tenSach, "Sach thu hai la Mango");
    TestFramework::assert_equal(std::string("Zebra"), dsDauSach[2]->tenSach, "Sach thu ba la Zebra");
}

void test_SapXepDauSachTheoTen_MangRong() {
    TestFramework::startTest("SapXepDauSachTheoTen - Mang rong (Guard test)");
    resetTestData();

    // Test fix bug: guard for empty array
    sapXepDauSachTheoTen(dsDauSach, 0, -1); // left >= right
    
    TestFramework::assert_true(true, "Khong crash voi mang rong");
}

// ========================================
// TEST 4: THEM BAN SAO
// ========================================
void test_ThemNhieuBanSao_ThanhCong() {
    TestFramework::startTest("ThemNhieuBanSao - Them ban sao hop le");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, 
                "9780140449136", "The Odyssey", 300, "Homer", 1997, "Epic", true);
    
    PTRDS dauSach = dsDauSach[0];
    std::string loi = ThemNhieuBanSao(dauSach, 3, "A-12");

    TestFramework::assert_equal(std::string(""), loi, "Khong co loi");
    TestFramework::assert_equal(3, dauSach->tongBanSao, "Tong ban sao = 3");
    
    // Kiem tra cac ban sao da duoc them
    int count = 0;
    PTRDMS p = dauSach->dms;
    while (p && count < 10) {
        count++;
        p = p->next;
    }
    TestFramework::assert_equal(3, count, "Co 3 node trong danh sach lien ket");
}

void test_ThemNhieuBanSao_SoLuongKhongHopLe() {
    TestFramework::startTest("ThemNhieuBanSao - So luong khong hop le");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, 
                "9780140449136", "The Odyssey", 300, "Homer", 1997, "Epic", true);
    
    PTRDS dauSach = dsDauSach[0];
    
    std::string loi1 = ThemNhieuBanSao(dauSach, 0, "A-12");
    TestFramework::assert_true(loi1.length() > 0, "Loi khi so luong = 0");

    std::string loi2 = ThemNhieuBanSao(dauSach, -5, "A-12");
    TestFramework::assert_true(loi2.length() > 0, "Loi khi so luong am");
}

// ========================================
// TEST 5: TIM KIEM LOGIC
// ========================================
void test_TimKiemLogic_TheoTen() {
    TestFramework::startTest("TimKiemLogic - Tim kiem theo ten");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, "1111111111111", "Harry Potter", 300, "JK Rowling", 1997, "Fantasy", true);
    themDauSach(dsDauSach, soLuongDauSach, "2222222222222", "Lord of the Rings", 500, "Tolkien", 1954, "Fantasy", true);

    KetQuaTimKiem ketQua[10];
    int soKetQua = timKiemLogic(dsDauSach, soLuongDauSach, "harry", ketQua);

    TestFramework::assert_equal(1, soKetQua, "Tim thay 1 ket qua");
    if (soKetQua > 0 && ketQua[0].sach) {
        TestFramework::assert_equal(std::string("Harry Potter"), ketQua[0].sach->tenSach, "Tim dung sach Harry Potter");
    }
}

void test_TimKiemLogic_TheoTacGia() {
    TestFramework::startTest("TimKiemLogic - Tim kiem theo tac gia");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, "1111111111111", "Harry Potter", 300, "JK Rowling", 1997, "Fantasy", true);
    themDauSach(dsDauSach, soLuongDauSach, "2222222222222", "The Hobbit", 300, "Tolkien", 1937, "Fantasy", true);

    KetQuaTimKiem ketQua[10];
    int soKetQua = timKiemLogic(dsDauSach, soLuongDauSach, "tolkien", ketQua);

    TestFramework::assert_equal(1, soKetQua, "Tim thay 1 ket qua voi Tolkien");
}

// ========================================
// TEST 6: THONG TIN DAU SACH
// ========================================
void test_LayThongTinDauSach() {
    TestFramework::startTest("LayThongTinDauSach - Kiem tra thong tin");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, "9780140449136", "The Odyssey", 300, "Homer", 1997, "Epic", true);
    PTRDS dauSach = dsDauSach[0];
    
    ThemNhieuBanSao(dauSach, 5, "A-12");
    
    // Set trang thai cac ban sao
    PTRDMS p = dauSach->dms;
    if (p) p->trangThai = CHO_MUON_DUOC;
    if (p) p = p->next;
    if (p) p->trangThai = DANG_MUON;
    if (p) p = p->next;
    if (p) p->trangThai = THANH_LY;

    ThongTinDauSach thongTin = LayThongTinDauSach(dauSach);

    TestFramework::assert_equal(5, thongTin.tongBanSao, "Tong ban sao = 5");
    TestFramework::assert_equal(1, thongTin.choMuonDuoc, "1 ban sao cho muon duoc");
    TestFramework::assert_equal(1, thongTin.dangMuon, "1 ban sao dang muon");
    TestFramework::assert_equal(1, thongTin.thanhLy, "1 ban sao thanh ly");
}

// ========================================
// TEST 7: XOA DAU SACH
// ========================================
void test_XoaDauSach_ThanhCong() {
    TestFramework::startTest("XoaDauSach - Xoa thanh cong");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, "1111111111111", "Book A", 100, "Author A", 2000, "Fiction", true);
    themDauSach(dsDauSach, soLuongDauSach, "2222222222222", "Book B", 100, "Author B", 2000, "Fiction", true);

    std::string loi = XoaDauSach(dsDauSach, soLuongDauSach, "1111111111111");

    TestFramework::assert_equal(std::string(""), loi, "Xoa thanh cong");
    TestFramework::assert_equal(1, soLuongDauSach, "Con lai 1 dau sach");
    TestFramework::assert_equal(std::string("Book B"), dsDauSach[0]->tenSach, "Sach con lai la Book B");
}

void test_XoaDauSach_ConBanSao() {
    TestFramework::startTest("XoaDauSach - Khong xoa duoc vi con ban sao");
    resetTestData();

    themDauSach(dsDauSach, soLuongDauSach, "1111111111111", "Book A", 100, "Author A", 2000, "Fiction", true);
    ThemNhieuBanSao(dsDauSach[0], 2, "A-12");

    std::string loi = XoaDauSach(dsDauSach, soLuongDauSach, "1111111111111");

    TestFramework::assert_true(loi.length() > 0, "Co loi vi con ban sao");
    TestFramework::assert_equal(1, soLuongDauSach, "So luong dau sach khong doi");
}

// ========================================
// MAIN TEST RUNNER
// ========================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "UNIT TESTS - QUANLYSACH MODULE" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // Test 1: Them dau sach
        test_ThemDauSach_ThanhCong();
        test_ThemDauSach_ISBNTrung();
        test_ThemDauSach_SoTrangKhongHopLe();

        // Test 2: Tim kiem
        test_TimDauSachTheoISBN_ThanhCong();
        test_TimDauSachTheoISBN_KhongThay();

        // Test 3: Sap xep
        test_SapXepDauSachTheoTen();
        test_SapXepDauSachTheoTen_MangRong();

        // Test 4: Them ban sao
        test_ThemNhieuBanSao_ThanhCong();
        test_ThemNhieuBanSao_SoLuongKhongHopLe();

        // Test 5: Tim kiem logic
        test_TimKiemLogic_TheoTen();
        test_TimKiemLogic_TheoTacGia();

        // Test 6: Thong tin
        test_LayThongTinDauSach();

        // Test 7: Xoa
        test_XoaDauSach_ThanhCong();
        test_XoaDauSach_ConBanSao();

        // Cleanup
        resetTestData();

        // Print summary
        TestFramework::printSummary();

    } catch (const std::exception& e) {
        std::cerr << "\n❌ EXCEPTION: " << e.what() << std::endl;
        return 1;
    }

    return TestFramework::getFailedCount() == 0 ? 0 : 1;
}
