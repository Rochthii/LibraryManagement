# Module: Utils (Tiện ích)

## Thành phần chính
- `NgayThang` — xử lý ngày tháng, parse/format `dd/mm/yyyy`, tính số ngày giữa hai ngày (dùng để tính quá hạn)
- `XuLyChuoi` — các hàm xử lý chuỗi: `ChuanHoaKhoangTrang`, `CatKhoangTrang`, `ChuyenInThuong`, `ChuyenChuoiThanhSoNguyen`, ...
- `ThongBao` — hàm tiện ích in thông báo (INFO / WARNING / ERROR) chuẩn hoá đầu ra

---

## Lưu ý
- `NgayThang` cần kiểm tra hợp lệ ngày (tháng, năm, leap-year) để tránh lỗi khi tính số ngày quá hạn.
- `XuLyChuoi` có các hàm chuẩn hoá dùng khắp nơi — hãy tái sử dụng thay vì tái hiện logic chuỗi nhiều nơi.

---

## Ví dụ
```cpp
std::string s = "  NGuyen   Van   A ";
std::string chuan = ChuanHoaKhoangTrang(s); // "Nguyen Van A"

int d = TinhKhoangNgay("01/11/2025", "10/11/2025"); // vd: 9
```
