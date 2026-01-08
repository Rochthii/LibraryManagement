# Module: GiaoDienSFML (UI bằng SFML)

## Mục đích
Quản lý giao diện đồ họa (cửa sổ, màn hình, event loop) bằng SFML.

---

## Điểm khởi tạo & vòng đời
- `main.cpp`:
  - Tạo `sf::RenderWindow window(..)` và `sf::Font font`.
  - Tìm font theo thứ tự: `bin/DejaVuSans.ttf`, `files/DejaVuSans.ttf`, `arial.ttf`, `C:/Windows/Fonts/arial.ttf`.
  - `KhoiTaoGiaoDienSFML(window, font, fontPath, dsDauSach, soLuongDauSach)` — khởi tạo các màn hình.
  - `ChayChuongTrinhSFML(window, font, rootDocGia, dsDauSach, soLuongDauSach, duLieuDaThayDoi)` — vòng lặp chính xử lý sự kiện và vẽ.

## Kiến trúc màn hình
- Các màn hình (Menu, QuanLySach, QuanLyDocGia, MuonTra, ThongTin) nằm trong `UI/`.
- Mỗi màn hình chịu trách nhiệm:
  - Vẽ các widgets (text, buttons, input fields)
  - Nhận events của chuột/bàn phím
  - Thực hiện thao tác logic bằng cách gọi backend (ví dụ: `ThucHienMuonSachBackend`)

## Lưu ý khi mở rộng UI
- Thêm font custom: đặt file .ttf trong `bin/` hoặc `files/` để nó được tự động phát hiện.
- Khi một thao tác thay đổi dữ liệu (thêm/sửa/xóa), đánh dấu `duLieuDaThayDoi = true` để main lưu khi thoát.
- Để thêm màn hình mới: tạo file trong `UI/`, export các hàm khởi tạo và đăng ký màn hình trong flow của `KhoiTaoGiaoDienSFML`.

---

## Gợi ý debug
- Nếu ứng dụng không hiện được font: kiểm tra đường dẫn font và quyền truy cập file.
- Lỗi SFML linking: kiểm tra `Makefile` `INCLUDES` và `LDFLAGS` trỏ đúng tới cài SFML trên máy.

---

## Ví dụ
- Xem `UI/ManHinhQuanLySach.cpp` để tham khảo cách tạo danh sách kết quả, xử lý selection và gọi hàm `QuanLySach`.
