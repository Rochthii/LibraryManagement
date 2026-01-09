# Thuật toán & Cấu trúc dữ liệu

Tài liệu này mô tả các thuật toán và cấu trúc dữ liệu cốt lõi được dùng trong dự án, kèm độ phức tạp (time/space) và các lưu ý mở rộng.

---

## 1) Cấu trúc chính

- DocGia: **AVL tree** (NodeDG)
  - Lưu trữ độc giả theo `MaThe` để tìm/insert/xóa đạt O(log N).
  - Mỗi node chứa `bf` (balance factor) hoặc `height` để cân bằng.
  - Các phép quay: `RotateLeft`, `RotateRight` (cùng các trường hợp double-rotation khi cần).
  - Ưu điểm: tìm kiếm nhanh theo mã thẻ, phù hợp khi độc giả nhiều.

- DauSach: **Mảng con trỏ PTRDS[]** + cho mỗi đầu sách dùng **DSLK (DanhMucSach)** cho các bản sao
  - Tìm kiếm hiện dùng linear scan O(N) trên mảng đầu sách.
  - Danh mục bản sao là linked-list (O(1) chèn tại đầu, O(N) tìm theo mã bản sao).

- Danh sách mượn (MUONTRA): **Singly linked list** cho từng độc giả
  - Thêm/mở đầu O(1), duyệt O(k) với k = số sách đang mượn của độc giả

---

## 2) Thuật toán sắp xếp & tìm kiếm

- Quicksort
  - Dùng để sắp xếp độc giả theo tên (`QuickSortByName`) hoặc để sắp xếp đầu sách theo tên (`sapXepDauSachTheoTen`)
  - Độ phức tạp trung bình: O(n log n), worst-case O(n^2)

- Insertion sort
  - Dùng cho các mảng nhỏ (ví dụ `SapXepBanSaoTheoMa`, `SapXepTheLoaiTheoTen`)
  - Độ phức tạp O(n^2) nhưng hiệu quả với n nhỏ

- Tìm kiếm
  - Tìm theo `MaThe` trong cây AVL: O(log N)
  - Tìm theo tên/ISBN/tác giả hiện tại: linear scan O(N)
  - Tự động tối ưu: nếu từ khóa là chuỗi số, backend sẽ thử tìm theo mã thẻ (fast path)

---

## 3) Các hàm sắp xếp/so sánh cụ thể
- `QuickSortByName`: so sánh tên + họ (Ten + Ho) theo thứ tự chữ tự để sắp xếp tăng dần
- `QuickSortByOverdue`: sắp xếp giảm dần theo số ngày quá hạn (overdueDays)

---

## 4) Quy tắc nghiệp vụ liên quan đến thuật toán
- Quá hạn: > 7 ngày được coi là "qua hạn nặng"; danh sách quá hạn dùng để lọc & sắp xếp theo số ngày quá hạn
- Trạng thái sách: enum `TrangThaiSach` (CHO_MUON_DUOC, DANG_MUON, THANH_LY) quyết định khả năng mượn
- `soSachDangMuon` trong `TheDocGia` được duy trì để kiểm tra giới hạn mượn nhanh

---

## 5) Các điểm cần cân nhắc cải tiến
- Thay `PTRDS[]` bằng `unordered_map<string, PTRDS>` keyed by ISBN để đạt O(1) tìm kiếm theo ISBN
- Duy trì index phụ cho tìm kiếm theo tên (ví dụ: inverted index hoặc tiền tố trie) để cải thiện tìm theo tên từ O(N) xuống gần O(1)
- Nếu số lượng bản sao lớn, thay DSLK bằng vector hoặc balanced-tree để tìm kiếm/loại bỏ theo mã nhanh hơn
- Lưu ý multi-threading: hiện code chưa thread-safe; nếu mở rộng xử lý đồng thời, cần đồng bộ hoá truy cập tới cấu trúc dữ liệu

---

## 6) Độ phức tạp tổng quan
- Tìm theo mã thẻ: O(log N) (AVL)
- Tìm theo tên/từ khóa: O(N)
- Thêm/xóa đầu sách: O(N) (với mảng), thêm bản sao O(1)
- Sắp xếp: O(n log n) trung bình cho quicksort

---

## 7) Gợi ý test
- Kiểm tra cân bằng AVL sau nhiều lần insert/xóa (viết test để assert `height`/`bf` hợp lệ)
- Round-trip: Lưu & Nạp dữ liệu để đảm bảo không mất liên kết (mã bản sao → pointer)
- Tìm các trường hợp cạnh: quá hạn ranh giới (7 ngày), mượn hết bản sao, xóa đầu sách khi còn bản sao mượn
