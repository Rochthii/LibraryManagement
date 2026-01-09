# Module: DocGia (Quản lý độc giả)

## Mục đích
Quản lý dữ liệu độc giả (thẻ thư viện) bằng một cây AVL để đảm bảo thời gian tìm/insert/xóa O(log N). Cung cấp các hàm quản lý danh sách mượn trả (MUONTRA) cho từng độc giả, cùng tính toán độc giả quá hạn.

---

## Cấu trúc dữ liệu chính
- TheDocGia
  - int MaThe
  - string Ho, Ten
  - bool Phai
  - int TrangThai (0 = khoá, 1 = hoạt động)
  - int soSachDangMuon
  - MUONTRA dsmt (linked list các muon tra)

- NodeDG (cây AVL): data=TheDocGia, bf, height, left, right

- MuonTra (node danh sách mượn): PTRDMS banSaoSach; string NgayMuon; string NgayTra; int TrangThai (0=đang mượn,1=đã trả,2=làm mất)

---

## Các hàm công khai quan trọng (tóm tắt)
- InsertDocGia(PTRDG& root, PTRDG node)
  - Chèn node vào cây AVL, duy trì cân bằng bằng RotateLeft/RotateRight.

- taoDocGia(...)
  - Tạo PTRDG mới với dữ liệu TheDocGia.

- xoaDocGia(PTRDG& root, int mathe)
  - Xóa độc giả theo mã thẻ, giải phóng danh sách mượn nếu có.

- timDocGia(PTRDG root, int mathe)
  - Tìm và trả về PTRDG nếu tồn tại.

- DuyetCayRaMang(PTRDG root, PTRDG arr[], int &count)
  - Duyệt toàn bộ cây, xuất ra mảng (dùng cho tìm kiếm theo tên và bảng hiển thị).

- LayDSDocGiaQuaHan(PTRDG root, DocGiaQuaHanDTO arr[], int maxKetQua)
  - Lấy danh sách độc giả đang quá hạn (> 7 ngày) kèm số ngày quá hạn.

- giaiPhongDsmt(MUONTRA& dsmt) / GiaiPhongCay(PTRDG& root)
  - Giải phóng bộ nhớ.

- MuonSach, TraSach, BaoMatSach, themMuonTra (xử lý nghiệp vụ mượn/trả)
  - Thực hiện giao dịch và cập nhật soSachDangMuon, trạng thái bản sao.

- saveDocGia / loadDocGia
  - Lưu và nạp dữ liệu độc giả xuống/từ `files/docgia.txt` (định dạng block DG + MT + END_DG).

---

## Quy tắc nghiệp vụ chính
- Mỗi độc giả có `soSachDangMuon` để kiểm tra giới hạn mượn nhanh (O(1)).
- Quá hạn được tính dựa trên `NgayMuon` và `NgayTra` hoặc ngày hiện tại nếu chưa trả.
- Khi nạp file, `themMuonTra(PTRDG, MuonTra&)` được dùng để khôi phục trạng thái danh sách mượn.

---

## Ví dụ sử dụng (C++)
```cpp
// Tạo và chèn độc giả
PTRDG root = nullptr;
PTRDG node = taoDocGia("Nguyen","Viet",1,1,10001);
InsertDocGia(root, node);

// Tìm độc giả
PTRDG found = timDocGia(root, 10001);
if (found) {
  std::cout << "Ten: " << found->data.Ho << " " << found->data.Ten << std::endl;
}

// Lưu và nạp
saveDocGia(root);
root = loadDocGia(dsDauSach, soLuongDauSach);
```

---

## Ghi chú & Lời khuyên
- Kiểm tra và xử lý `NULL` khi thao tác với PTRDG và MUONTRA.
- Khi thêm tính năng mới (ví dụ: trường địa chỉ), cập nhật logic nạp/lưu file tương ứng.
- Thêm unit tests cho hàm cân bằng AVL và các thao tác xóa/insert.
