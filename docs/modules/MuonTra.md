# Module: MuonTra (Tìm kiếm & Giao dịch mượn/trả)

## Mục đích
Cung cấp API backend cho phần giao diện và thao tác nghiệp vụ: tìm kiếm độc giả/sách, lọc quá hạn, thực hiện mượn/trả/báo mất, và một số hàm tiện ích kiểm tra dữ liệu.

---

## DTOs quan trọng
- `DocGiaTableDTO_Backend` { PTRDG docGia; int loaiKhop; int overdueDays }
  - loaiKhop: 0 = exact (Mã thẻ chính xác), 1 = partial (từ khóa khớp chuỗi tên), 2 = show all

- `SachTableDTO_Backend` { PTRDS dauSach; int loaiKhop }

- `ThongTinSachDangMuon_DTO` { string maSach; string tenSach; string ngayMuon; int soNgayGiu }

---

## Hàm truy xuất & tìm kiếm
- `LayDanhSachDocGiaBackend(PTRDG rootDocGia, std::string &tuKhoa, bool laCheDoQuaHan, bool sapXepTheoTen, DocGiaTableDTO_Backend *ketQua, int &soLuong)`
  - Chức năng:
    - Tự động tối ưu: nếu `tuKhoa` là chuỗi số và `laCheDoQuaHan == false`, sẽ tìm theo mã thẻ bằng AVL (O(log N)).
    - Ngược lại, nạp toàn bộ cây ra mảng và thực hiện tìm kiếm theo tên (partial match) O(N).
    - Nếu `laCheDoQuaHan` = true: lọc chỉ độc giả có sách quá hạn (> 7 ngày) và trả `overdueDays` lớn nhất cho từng độc giả.
    - Có thể sắp xếp theo tên (QuickSortByName) hoặc theo overdueDays (QuickSortByOverdue).

- `LayDanhSachSachBackend(PTRDS dsDauSach[], int soLuongDauSach, const std::string &tuKhoa, SachTableDTO_Backend *ketQua, int &soLuong)`
  - Tìm sách theo ISBN chính xác (loaiKhop=0) hoặc partial match (tên/tác giả/isbn)

- `LayDSSachDangMuonBackend(PTRDG docGia, ThongTinSachDangMuon_DTO *ketQua, PTRDS dsDauSach[], int soLuongDauSach)`
  - Trả về danh sách sách đang mượn bởi `docGia` (tương tác với `MuonTra` linked list)

---

## Giao dịch (Transactions)
- `ThucHienMuonSachBackend(PTRDG docGia, const std::string &maSach, PTRDS dsDauSach[], int soLuongDauSach, bool &duLieuDaThayDoi)`
  - Kiểm tra hợp lệ (docGia tồn tại, mã sách không rỗng), gọi `MuonSach` để xử lý chi tiết. Nếu thành công, `duLieuDaThayDoi=true`.

- `ThucHienTraSachBackend`, `ThucHienMatSachBackend` — tương tự, gọi `TraSach` và `BaoMatSach`.

- `TimMaSachCoTheMuon(PTRDS dsDauSach[], int soLuongDauSach, const std::string &isbn)`
  - Kiểm tra các bản sao theo ISBN, trả mã bản sao có trạng thái `CHO_MUON_DUOC`.

---

## Kiểm tra dữ liệu & Business rules
- `KiemTraDuLieuDocGia(ho, ten)`
  - Chuẩn hoá khoảng trắng, kiểm tra độ dài và ký tự (chỉ chữ ASCII không dấu + space). Trả về chuỗi lỗi nếu không hợp lệ.

- Luật quá hạn: quá 7 ngày được coi là quá hạn nặng.
- Hệ thống dùng `soSachDangMuon` để kiểm limit mượn nhanh.

---

## Ví dụ (C++)
```cpp
// Tìm độc giả theo từ khóa
DocGiaTableDTO_Backend ds[100]; int so=0;
std::string key = "Nguyen";
LayDanhSachDocGiaBackend(rootDocGia, key, false, true, ds, so);

// Mượn sách
bool changed=false;
std::string loi = ThucHienMuonSachBackend(foundDocGia, "9786040000453-1", dsDauSach, soLuongDauSach, changed);
if (!loi.empty()) std::cerr << loi << std::endl;

// Lấy danh sách đang mượn
ThongTinSachDangMuon_DTO ketqua[10];
int n = LayDSSachDangMuonBackend(foundDocGia, ketqua, dsDauSach, soLuongDauSach);
```

---

## Ghi chú & khuyến nghị
- `LayDanhSachDocGiaBackend` có 2 chế độ tìm: bằng `MaThe` (fast) hoặc bằng `Ho+Ten` (full scan). Khi cần hiệu năng cho tên, cân nhắc xây chỉ mục tên.
- Các hàm backend trả lỗi ở dạng chuỗi (bắt đầu bằng "Loi:") — UI nên hiển thị trực tiếp thông báo đó.
- Viết test cho các trường hợp cạnh: mượn sách không tồn tại, mượn khi bản sao đang bị mượn, trả sách với mã sai, v.v.
