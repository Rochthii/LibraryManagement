# Module: QuanLySach (Các thao tác trên đầu sách)

## Mục đích
Cung cấp các hàm thao tác đầu sách (PTRDS) và danh mục bản sao (PTRDMS): thêm đầu sách, thêm bản sao, cập nhật thông tin, xóa, sắp xếp, và các hàm tiện ích cho giao diện.

---

## Hàm chính (tóm tắt theo nhóm)

1. Mã sách & Trạng thái
- `sinhMaSach` — sinh mã bản sao theo `ISBN` + số thứ tự
- `TenTrangThai`, `PhanTichTrangThaiSach` — lấy/chuyển đổi trạng thái sách

2. Quản lý DSLK (Danh Mục Sách)
- `themDanhMucSach(PTRDMS &dms, ma, tt, viTri)` — chèn node vào đầu danh sách (O(1)).
- `ChenNodeDMSVaoDauSach`, `capNhatTrangThaiSach` — cập nhật trạng thái/vi trí.
- `TimBanSaoTheoMa` — tìm node bản sao theo mã (search linear).

3. Quản lý mảng đầu sách (PTRDS dsDauSach[])
- `themDauSach` / `ThemDauSachMoi` — thêm đầu sách mới vào mảng.
- `ThemNhieuBanSao`, `ThemBanSaoMoi` — thêm nhiều/1 bản sao cho 1 đầu sách.
- `TimDauSachTheoISBN` — tìm đầu sách theo ISBN (linear search).

4. Tìm kiếm & liệt kê
- `timKiemLogic` — tìm theo keyword (ISBN/Tên/Tác giả).
- `LayDanhSachTimKiem`, `LayDanhSachTheoTheLoai`, `LayDanhSachViTri` — hỗ trợ UI lấy danh sách.

5. Sắp xếp
- `sapXepDauSachTheoTen` (quicksort), `SapXepBanSaoTheoMa` (insertion sort)

6. Quản lý bộ nhớ
- `GiaiPhongDanhMucSach`, `GiaiPhongDauSach`, `GiaiPhongToanBoDauSach` — giải phóng tài nguyên

7. Thao tác & Tính năng khác
- `XoaDauSach`, `XoaSachTheoMaSach`, `ThanhLyBanSaoTheoMa`, `CapNhatThongTinDauSach`, `CapNhatViTri`
- `LayThongTinDauSach` — trả về số bản sao theo từng trạng thái

---

## Độ phức tạp & khuyến nghị
- Hầu hết các tìm kiếm hiện là linear O(N) trên mảng `dsDauSach` hoặc trên DSLK; hoạt động tốt với quy mô nhỏ-vừa.
- Nếu muốn mở rộng tới hàng chục ngàn đầu sách, cân nhắc thay mảng thành `unordered_map` (key=ISBN) để đạt O(1) tìm kiếm.

---

## Ví dụ (C++)
```cpp
// Thêm đầu sách mới
std::string err = ThemDauSachMoi(ds, soLuong, "9786040000644", "Ai Trong Doi Song", 733, "Pham Thao", 2023, "Tri Tue", 3, "Ke M1-01", changed);
if (!err.empty()) std::cerr << err << std::endl;

// Tìm theo thể loại
PTRDS ketQua[100];
int so = LayDanhSachSachTheoTheLoai(ds, soLuong, "Hoc May", ketQua, 100);
```

---

## Ghi chú
- Luôn kiểm tra biến `duLieuDaThayDoi` sau thao tác để quyết định có cần lưu dữ liệu hay không.
- Khi thay đổi định dạng mã sách, cập nhật `sinhMaSach` và định dạng trong file lưu.
- Thực hiện unit test cho các hàm xóa/sửa để đảm bảo không có rò rỉ bộ nhớ.
