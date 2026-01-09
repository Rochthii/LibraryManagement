# Module: ThaoTacFile (Lưu / Nạp dữ liệu)

## Mục đích
Quản lý đọc/ghi dữ liệu giữa bộ nhớ trong chương trình và các file text trong `files/`.

---

## File chính & định dạng
- `files/dausach.txt` — mỗi dòng: `ISBN|tenSach|soTrang|tacGia|nam|theLoai`
- `files/danhmucsach.txt` — mỗi dòng: `ISBN-SoThuTu|TrangThai|ViTri`
- `files/docgia.txt` — block `DG|...` + zero or more `MT|...` + `END_DG`

---

## Hàm quan trọng
- `NapDanhSachDauSach(file, PTRDS ds[], int &n, out)`
  - Đọc `dausach.txt`, tạo các `DauSach` và lưu con trỏ vào mảng `ds`.

- `NapDanhMucSach(file, PTRDS ds[], int n, out)`
  - Đọc `danhmucsach.txt` và nối các `DanhMucSach` vào đầu sách tương ứng.

- `loadDocGia(PTRDS dsDauSach[], int soLuongDS)`
  - Đọc `docgia.txt`, dựng cây AVL độc giả và danh sách muon tra (sử dụng `MaSach_to_PTRDMS` để ánh xạ mã sách sang con trỏ ban sao).

- `GhiDanhSachDauSach`, `GhiDanhMucSach`, `saveDocGia`
  - Ghi lại trạng thái hiện tại vào file. Ghi theo cùng định dạng để đảm bảo khả năng tái nạp.

- Helper: `PTRDMS_to_String`, `MaSach_to_PTRDMS`, `LuuGiaoDich`, `NapGiaoDich`
  - Dùng để chuyển đổi giữa đối tượng trong bộ nhớ và chuỗi lưu trữ.

---

## Quy tắc & lưu ý
- Chú ý thứ tự nạp: `dausach.txt` → `danhmucsach.txt` → `docgia.txt` (vì `docgia` tham chiếu đến mã bản sao trong danh mục sách)
- Khi định dạng file thay đổi, hãy cập nhật cả đọc và ghi để tránh lỗi không tương thích.
- Hệ thống hiện dùng định dạng text đơn giản để dễ debug; nếu cần mở rộng, chuyển sang JSON/CSV sẽ tiện hơn.

---

## Ví dụ
- Khi nạp dữ liệu sẽ có thông báo lỗi nếu file không tồn tại hoặc có dòng không hợp lệ. Kiểm tra `std::cout`/`std::cerr` để debug.

---

## Testing
- Thêm test để kiểm tra:
  - Nạp file có record bị thiếu trường
  - Mã bản sao không tìm thấy khi nạp giao dịch (MT)
  - Ghi và nạp lại dữ liệu (round-trip consistency)
