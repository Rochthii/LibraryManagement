# Dokumentation dự án: LibraryManagement ✅

## Tổng quan ✨
**LibraryManagement** là một chương trình quản lý thư viện viết bằng C++ (C++17) với giao diện đồ họa dựa trên **SFML**. Chương trình hỗ trợ:
- Quản lý đầu sách (DauSach) và danh mục bản sao (DanhMucSach)
- Quản lý độc giả (DocGia) sử dụng cây AVL
- Theo dõi mượn/trả, báo mất sách, thống kê top sách
- Giao diện SFML (có panel quản lý sách, độc giả, mượn trả)
- Lưu/ nạp dữ liệu từ file text (định dạng riêng trong folder `files/`)

---

## Yêu cầu & Chuẩn bị môi trường 🔧
- Hệ điều hành: Windows (đã test trên Windows 10/11)
- Trình biên dịch: MinGW/GCC (g++), make (`mingw32-make`)
- Thư viện: **SFML** (ví dụ: C:/SFML-2.6.1). Cập nhật đường dẫn SFML trong `Makefile` nếu cần.
- Font: dự án tìm file font theo thứ tự `bin/DejaVuSans.ttf`, `files/DejaVuSans.ttf`, `arial.ttf`, `C:/Windows/Fonts/arial.ttf`. Đảm bảo có ít nhất một font hợp lệ.

---

## Cách build & chạy 🚀
1. Mở terminal (cmd) trong thư mục dự án
2. Chỉnh Makefile nếu cần (điều chỉnh `INCLUDES` và `LDFLAGS` tới nơi cài SFML trên máy bạn)
3. Chạy:

```bash
mingw32-make
```

4. Sau khi build xong, chạy file exe:

```bash
bin\main.exe
```

Hoặc dùng `run.cmd` (nếu có). Có thư mục `LibraryManagement_Portable/` chứa phiên bản portable kèm README.

---

## Cấu trúc dự án 📂
- `main.cpp` — Điểm vào chương trình, nạp dữ liệu và khởi tạo giao diện SFML
- `include/` — Header của các module chính
  - `DauSach.h` — Định nghĩa `DauSach`, `DanhMucSach`, trạng thái sách
  - `DocGia.h` — Định nghĩa độc giả (AVL), mượn trả (linked list) và prototype quản lý độc giả/mượn
  - `MuonTra.h` — API backend cho tìm kiếm, giao dịch mượn/trả và validate dữ liệu
  - `QuanLySach.h` — Hàm thao tác DauSach, DanhMucSach, tìm kiếm, sort, xóa, cập nhật
  - `Constants.h` — Hằng số chung (độ dài mảng, đường dẫn file,...)
- `data/` — Triển khai logic nghiệp vụ (MuonTra.cpp, ThaoTacFile.cpp, ...)
- `UI/` — Mã giao diện SFML và các màn hình
- `files/` — File dữ liệu (như `dausach.txt`, `danhmucsach.txt`, `docgia.txt`)
- `tests/` — Test unit (ví dụ `test_QuanLySach.cpp`)

---

## Định dạng file dữ liệu (quan trọng) 📄
Tập tin dữ liệu hiện có và định dạng mẫu:

- `files/dausach.txt`
  - Mỗi dòng: `ISBN|tenSach|soTrang|tacGia|nam|theLoai`
  - Ví dụ: `9786040000644|Ai Trong Doi Song|733|Pham Thao|2023|Tri Tue Nhan Tao`

- `files/danhmucsach.txt`
  - Mỗi dòng: `ISBN-SoThuTu|TrangThai|ViTri`
  - `TrangThai` theo enum: `0`(CHO_MUON_DUOC), `1`(DANG_MUON), `2`(THANH_LY)
  - Ví dụ: `9786040000644-1|0|Ke M1-01`

- `files/docgia.txt`
  - Cấu trúc theo block cho mỗi độc giả
  - Bắt đầu bằng `DG|MaThe|Ho|Ten|Phai|TrangThai`
  - Các dòng `MT|maSach|ngayMuon|ngayTra|trangThaiMuon` (trangThaiMuon: 0=đang mượn, 1=đã trả)
  - Kết thúc block bằng `END_DG`
  - Ví dụ:
    ```text
    DG|10022|Vo Minh|Tri|1|1
    MT|9786040000453-3|03/11/2025|08/11/2025|1
    END_DG
    ```

> Lưu ý: Khi thay đổi cấu trúc file, đảm bảo cập nhật cả hàm nạp/lưu trong `ThaoTacFile.cpp`.

---

## Các module chính & API tóm tắt 🧭
Dưới đây là mô tả ngắn các thành phần quan trọng (xem header trong `include/` để biết prototype chi tiết):

### DocGia (AVL)
- Dữ liệu: `TheDocGia` chứa `MaThe`, `Ho`, `Ten`, `Phai`, `soSachDangMuon`, `dsmt` (linked list danh sách mượn trả)
- Chức năng: tạo/xóa/tìm độc giả, duyệt cây, cân bằng (rotate left/right), load/save file độc giả
- Các hàm nổi bật: `taoDocGia`, `timDocGia`, `InsertDocGia`, `GiaiPhongCay`, `LayDSDocGiaQuaHan`.

### MuonTra (backend)
- Các hàm tìm kiếm & backend: `LayDanhSachDocGiaBackend`, `LayDanhSachSachBackend`, `LayDSSachDangMuonBackend`
- Giao dịch: `ThucHienMuonSachBackend`, `ThucHienTraSachBackend`, `ThucHienMatSachBackend`
- Kiểm tra dữ liệu: `KiemTraDuLieuDocGia`, `TimMaSachCoTheMuon`

### QuanLySach
- Quản lý `DauSach` (mảng con trỏ `PTRDS`), các thao tác: thêm/sửa/xóa, thêm bản sao, đổi trạng thái, tìm kiếm, thống kê
- Sắp xếp & tìm kiếm đơn giản (linear search, quicksort cho sắp xếp theo tên)
- Các hàm quan trọng: `ThemDauSachMoi`, `ThemBanSaoMoi`, `TimDauSachTheoISBN`, `LayThongTinDauSach`, `XoaDauSach`.

### Giao diện SFML (`UI/`)
- `GiaoDienSFML.cpp` chứa khởi tạo cửa sổ và vòng lặp
- Các màn hình (menu, quản lý sách, quản lý độc giả, mượn trả) triển khai với SFML
- Lưu ý: nếu tạo GUI mới, tuân theo cấu trúc sự kiện và cập nhật `duLieuDaThayDoi` để lưu dữ liệu khi thoát

---

## Thuật toán & cấu trúc dữ liệu 💡
- **DocGia**: dùng cây AVL để tìm/insert/xóa theo mã thẻ (log N đảm bảo hiệu năng với số độc giả lớn)
- **Danh sách mượn**: linked-list cho từng độc giả (dễ add/remove, thao tác nhật sử dụng O(1) tại đầu)
- **DauSach**: mảng con trỏ (`PTRDS dsDauSach[]`) + danh sách bản sao là DSLK (`DanhMucSach`) cho mỗi đầu sách
- **Tìm kiếm**: hầu hết là linear search O(N) (thích hợp cho quy mô nhỏ/nhỏ vừa). Một số chức năng tối ưu bằng tiền tố số (tìm theo mã sách).
- **Sắp xếp**: quicksort cho sắp xếp theo tên (dauSach), insertion sort cho các mảng nhỏ

---

## Tài liệu thuật toán & cấu trúc dữ liệu 📘
Mô tả chi tiết về các thuật toán và cấu trúc dữ liệu được tách ra thành file riêng: `docs/Algorithms_and_Data_Structures.md`.


---

## Ví dụ sử dụng (kịch bản) 🧾
- Thêm đầu sách mới: Dùng menu "Quản lý sách" → "Thêm đầu sách" → nhập `ISBN|Tên|Số trang|Tác giả|Năm|Thể loại` và thêm bản sao
- Thêm độc giả: Menu "Quản lý độc giả" → "Thêm độc giả" → nhập họ/tên/phái
- Mượn sách: Chọn độc giả → nhập mã sách (ví dụ `9786040000453-3`) → hệ thống kiểm tra trạng thái
- Trả sách / Báo mất: Tương tự giao diện mượn trả

---

## Testing 🧪
- Folder `tests/` chứa test cho `QuanLySach` (ví dụ `test_QuanLySach.cpp`).
- Hiện chưa có script test runner tự động; bạn có thể biên dịch các file test riêng hoặc dùng framework đơn giản.

---

## Gợi ý cải tiến & TODOs ✍️
- Thêm test coverage cho DocGia (AVL) và MuonTra (các trường hợp biên)
- Hỗ trợ tìm kiếm nhanh hơn cho sách (indexing theo ISBN hoặc hash)
- Sửa đổi định dạng file sang JSON cho khả năng mở rộng và đọc dễ dàng hơn
- Thêm tài liệu UML/diagram mô tả cấu trúc cây AVL và luồng mượn trả

---

## Đóng góp & License 📝
- Muốn đóng góp: fork repository → tạo branch → tạo PR mô tả rõ thay đổi
- License: (chưa chỉ định) — thêm file `LICENSE` nếu muốn chính thức hoá

---

## Liên hệ / Tài liệu tham khảo 📚
- Code chính: `main.cpp`, `include/*.h`, `data/*.cpp`, `UI/*.cpp`
- Các file dữ liệu mẫu: `files/dausach.txt`, `files/danhmucsach.txt`, `files/docgia.txt`

---

> Nếu bạn muốn, mình có thể: (1) tách phần mô tả hàm chi tiết vào `docs/modules/*.md`, (2) thêm ví dụ chạy cụ thể cho từng chức năng, hoặc (3) chuyển docs sang tiếng Anh để tiện chia sẻ. Chọn bước tiếp theo bạn muốn mình làm nhé! ✅
