# Library Management System

## Mô tả
Chương trình quản lý thư viện sử dụng C++ với các cấu trúc dữ liệu:
- Danh sách tuyến tính (mảng con trỏ) cho đầu sách.
- Danh sách liên kết đơn cho danh mục sách và mượn trả.
- Cây nhị phân tìm kiếm cho thẻ độc giả.

## Cấu trúc thư mục
- `include/`: Chứa các file header (.h) định nghĩa struct và prototype hàm.
- `src/`: Chứa các file source (.cpp) triển khai logic.
- `data/`: Chứa các file dữ liệu (text) cho đầu sách, danh mục sách, độc giả, mượn trả.

## Yêu cầu
- Compiler: g++ (MinGW trên Windows).
- VS Code với extension C/C++ (Microsoft).
- Python (để chạy script tạo cấu trúc).

## Cách biên dịch và chạy
1. Biên dịch:
   ```bash
   g++ -I include src/*.cpp -o library_management.exe
   ```
2. Chạy chương trình:
   ```bash
   .\library_management.exe
   ```

## Chức năng
1. Nhập thẻ độc giả (thêm/xóa/hiệu chỉnh).
2. In danh sách độc giả (theo tên hoặc mã).
3. Nhập đầu sách và danh mục sách.
4. In danh sách đầu sách theo thể loại.
5. Tìm sách theo tên.
6. Mượn sách.
7. Trả sách.
8. Liệt kê sách đang mượn của độc giả.
9. Liệt kê độc giả mượn quá hạn.
10. In 10 sách được mượn nhiều nhất.

## Lưu ý
- File dữ liệu được lưu trong thư mục `data/`.
- Đảm bảo giải phóng bộ nhớ khi thoát chương trình.
- Kiểm tra lỗi đầu vào và file I/O cẩn thận.
