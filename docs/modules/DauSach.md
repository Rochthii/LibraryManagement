# Module: DauSach (Đầu sách & Danh mục sách)

## Mục đích
Quản lý thông tin "đầu sách" (tựa sách) và danh sách bản sao (DanhMucSach). Hỗ trợ thao tác thêm/sửa/xóa đầu sách, thêm bản sao, cập nhật trạng thái bản sao và truy vấn thống kê.

---

## Cấu trúc dữ liệu chính
- DauSach
  - string ISBN, tenSach, tacGia, theLoai
  - int soTrang, namXuatBan
  - PTRDMS dms: con trỏ tới danh sách bản sao (DanhMucSach)
  - int tongBanSao, soLuotMuon

- DanhMucSach (DSLK)
  - string maSach (ví dụ: ISBN-1)
  - TrangThaiSach trangThai (CHO_MUON_DUOC=0, DANG_MUON=1, THANH_LY=2)
  - string viTri
  - DanhMucSach* next

---

## Các hàm công khai quan trọng (tóm tắt)
- themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt)
  - Thêm một bản sao vào đầu danh sách (O(1)).

- ChenNodeDMSVaoDauSach, CapNhatTrangThaiSach
  - Cập nhật trạng thái bản sao cụ thể.

- TimBanSaoTheoMa, TimSoThuTuLonNhat
  - Tìm bản sao theo mã (linear scan trên DSLK).

- ThemNhieuBanSao, ThemBanSaoMoi
  - Thêm nhiều bản sao (hàm thuận tiện cho thao tác UI/nhập nhanh).

- GiaiPhongDanhMucSach(PTRDMS &dms) / GiaiPhongDauSach / GiaiPhongToanBoDauSach
  - Giải phóng bộ nhớ khi xóa đầu sách hoặc khi thoát chương trình.

- LayThongTinDauSach(PTRDS dauSach)
  - Trả về số lượng bản sao theo từng trạng thái (cho UI/ thống kê).

---

## Định dạng dữ liệu liên quan
- `files/dausach.txt`: `ISBN|tenSach|soTrang|tacGia|nam|theLoai`
- `files/danhmucsach.txt`: `ISBN-SoThuTu|TrangThai|ViTri`

Khi nạp dữ liệu, chương trình dùng `ISBN` liên kết giữa `dausach.txt` và `danhmucsach.txt` để khôi phục DSLK cho từng đầu sách.

---

## Ví dụ sử dụng (C++)
```cpp
PTRDS ds[MAX_DAUSACH] = {nullptr};
int n = 0;
ThemDauSachMoi(ds, n, "9786040000644", "Ai Trong Doi Song", 733, "Pham Thao", 2023, "Tri Tue", 3, "Ke M1-01", changed);

// Thêm 1 bản sao mới cho dau sach
ThemBanSaoMoi(ds, n, "9786040000644", 1, "Ke M1-04", changed);

// Duyệt danh sách ban sao
PTRDS dau = TimDauSachTheoISBN(ds, n, "9786040000644");
PTRDMS dms = dau->dms;
while (dms) {
  std::cout << dms->maSach << " - " << dms->viTri << " - " << (int)dms->trangThai << std::endl;
  dms = dms->next;
}
```

---

## Ghi chú & Lời khuyên
- Việc duy trì `tongBanSao` là quan trọng để thống kê nhanh; đảm bảo cập nhật khi thêm/xóa bản sao.
- Đối với danh sách bản sao lớn, cân nhắc chuyển sang cấu trúc dữ liệu khác (vector hoặc balanced tree) để truy vấn theo mã nhanh hơn.
- Thêm test để kiểm tra tính nhất quán giữa `tongBanSao` và số node trong DSLK.
