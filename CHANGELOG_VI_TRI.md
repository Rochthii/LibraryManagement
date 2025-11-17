# Changelog: Thêm chức năng nhập vị trí cho Đầu Sách

## Ngày: 2025-11-16

### Tính năng mới:
- **Nhập vị trí kệ sách** khi thêm đầu sách mới
- Vị trí được **tự động chuẩn hóa** theo format "Ke A-12", "Ke B5", v.v.
- Cho phép để trống (không bắt buộc)

---

## Chi tiết thay đổi:

### 1. **Cấu trúc dữ liệu:**
- **File:** `include/DauSach.h`
  - Thêm trường `std::string viTriMacDinh` vào struct `DauSach`
  - Vị trí mặc định này sẽ được kế thừa khi thêm bản sao

### 2. **Backend:**
- **File:** `data/QuanLySach.cpp`, `include/QuanLySach.h`
  - Hàm `themDauSach()` thêm tham số `const std::string &viTri = ""`
  - Validation và chuẩn hóa vị trí bằng `ChuanHoaViTri()`
  - Nếu không hợp lệ → báo lỗi

- **File:** `data/ThaoTacFile.cpp`
  - Đọc/ghi file: thêm cột vị trí (trường thứ 7)
  - Format: `ISBN|Ten|Trang|TG|Nam|TheLoai|ViTri`
  - Cho phép rỗng khi đọc file cũ

### 3. **UI:**
- **File:** `UI/ManHinhQuanLySach.cpp`
  - Thêm TextBox `INPUT_VI_TRI` vào form thêm/sửa sách
  - Placeholder: "VD: A-12, B5, Ke C-3"
  - Tự động chuẩn hóa khi nhập: "a12" → "Ke A-12"

- **File:** `include/GiaoDienSFML.h`
  - Thêm enum `INPUT_VI_TRI`

- **File:** `include/TrangThaiManHinhSach.h`
  - Thêm `std::string chuoiViTri` vào struct `SachState`

### 4. **Constants:**
- **File:** `include/Constants.h`
  - Tăng `SO_TRUONG_DAU_SACH` từ 6 → 7

### 5. **File dữ liệu:**
- **File:** `files/dausach.txt`
  - Đã thêm cột rỗng cho tất cả dòng hiện có
  - Các đầu sách mới sẽ có vị trí nếu người dùng nhập

---

## Cách sử dụng:

### Thêm đầu sách mới:
1. Mở form "Thêm Sách"
2. Điền đầy đủ thông tin (ISBN, Tên, Số trang, Tác giả, Năm XB, Thể loại)
3. **Nhập vị trí** (tùy chọn): 
   - VD: `A-12`, `B5`, `Ke C-3`
   - Hệ thống tự động chuẩn hóa thành: `Ke A-12`, `Ke B5`, `Ke C-3`
4. Nhấn "Thêm Sách"

### Validation vị trí:
- ✅ Hợp lệ: `A-12`, `a12`, `B5`, `Ke C-3`, `ke d-7`
- ❌ Không hợp lệ: `123` (chỉ số), `ABC` (chỉ chữ), `@#$` (ký tự đặc biệt)
- ⚠️ Có thể để trống (không bắt buộc)

### Format chuẩn:
- **Luôn bắt đầu bằng "Ke "**
- **Phần sau:** Chữ + Số, có thể có dấu gạch ngang
- **VD:** `Ke A-12`, `Ke B5`, `Ke C-3`, `Ke D-100`

---

## Lợi ích:
1. **Quản lý thực tế hơn:** Biết chính xác sách nằm ở kệ nào
2. **Tìm kiếm dễ dàng:** Có thể mở rộng tính năng tìm theo vị trí sau này
3. **Tự động chuẩn hóa:** Không cần lo format, hệ thống tự động fix
4. **Linh hoạt:** Cho phép để trống nếu chưa xếp kệ

---

## Testing:
```bash
# Build project
cd e:\Code\CODE\LibraryManagement
C:/msys64/mingw64/bin/g++.exe -std=c++17 -Wall -Wextra -Iinclude -Ilib/SFML/include -g main.cpp UI/*.cpp data/*.cpp utils/*.cpp -I. -Llib/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lgdi32 -luser32 -lole32 -loleaut32 -lcomdlg32 -luuid -o bin/main.exe

# Run
.\bin\main.exe
```

Thử các test case:
1. Thêm sách với vị trí: `A-12` → Kiểm tra chuẩn hóa thành `Ke A-12`
2. Thêm sách để trống vị trí → Vẫn thêm được
3. Thêm sách với vị trí sai: `123` → Hiển thị lỗi
4. Lưu file và đọc lại → Vị trí được lưu đúng

---

## Notes:
- File backup dữ liệu cũ đã được lưu tại `files/backup_*.txt` (nếu có)
- Đầu sách cũ sẽ có vị trí rỗng, khi sửa có thể thêm vị trí
- Chuẩn hóa dùng hàm `ChuanHoaViTri()` đã có sẵn trong `utils/XuLyChuoi.cpp`
