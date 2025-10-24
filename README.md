Hệ thống Quản lý Thư viện 📚
Một dự án C++ console để quản lý thư viện, được xây dựng bằng C++17 và các cấu trúc dữ liệu cơ bản.

🎯 Đề bài: Tổ chức các danh sách sau
📖 Cấu trúc dữ liệu
Đầu sách: Mảng con trỏ (danh sách tuyến tính) - Sắp xếp tăng dần theo tên sách.

Danh mục sách: Danh sách liên kết đơn.

Thẻ độc giả: Cây nhị phân tìm kiếm (theo Mã thẻ).

Mượn trả: Danh sách liên kết đơn (theo Mã sách).

🔧 Chức năng chính
a-b. Quản lý độc giả (Thêm, xóa, sửa, in danh sách).

c. Nhập đầu sách và đánh mã sách tự động.

d. In danh sách đầu sách theo thể loại (sắp xếp theo tên).

e. Tìm thông tin sách dựa vào tên sách.

f-g. Xử lý mượn/trả sách (kiểm tra giới hạn 3 cuốn, quá hạn 7 ngày).

h. Liệt kê sách độc giả đang mượn.

i. In danh sách độc giả quá hạn (giảm dần).

j. Top 10 sách mượn nhiều nhất.

📁 Cấu trúc thư mục
📦 LibraryManagement/
├── 📂 data/          # Mã nguồn .cpp cho các đối tượng dữ liệu
│   ├── DocGia.cpp
│   ├── KiemTraDuLieu.cpp
│   ├── MuonTra.cpp
│   ├── NhapLieu.cpp
│   ├── QuanLySach.cpp
│   ├── ThaoTacFile.cpp
│   └── TheDocGia.cpp
├── 📂 files/         # Chứa các file dữ liệu .txt
├── 📂 include/      # Chứa TẤT CẢ các file header .h
│   ├── Constants.h
│   ├── DauSach.h
│   ├── DocGia.h
│   ├── KiemTraDuLieu.h
│   ├── ... (và các file .h khác)
├── 📂 utils/         # Mã nguồn .cpp cho các hàm tiện ích
│   ├── NgayThang.cpp
│   ├── ThongBao.cpp
│   └── XuLyChuoi.cpp
├── 📂 bin/           # Thư mục chứa file .exe đã biên dịch
├── 📂 .vscode/       # Cấu hình cho VS Code (tasks.json)
├── 📝 main.cpp       # Điểm vào chương trình (bản console)
├── .gitignore
└── 📖 README.md
🛠️ Yêu cầu & Biên dịch
Phiên bản này được thiết kế để chạy 100% trên console, không yêu cầu thư viện đồ họa (BGI) hay thư viện xử lý tiếng Việt phức tạp (VietnameseUtils).

Trình biên dịch: MinGW-w64 g++ (Hỗ trợ C++17)

Hệ điều hành: Windows

1. Biên dịch thủ công (Khuyên dùng)
Mở terminal (PowerShell, CMD, hoặc MSYS2 MinGW) tại thư mục gốc LibraryManagement và chạy lệnh sau:

Bash

g++ -std=c++17 -Wall -Wextra -Iinclude -g ^
    main.cpp ^
    data/DocGia.cpp ^
    data/KiemTraDuLieu.cpp ^
    data/MuonTra.cpp ^
    data/NhapLieu.cpp ^
    data/QuanLySach.cpp ^
    data/ThaoTacFile.cpp ^
    data/TheDocGia.cpp ^
    utils/NgayThang.cpp ^
    utils/ThongBao.cpp ^
    utils/XuLyChuoi.cpp ^
    -o bin/main_console.exe
Giải thích:

g++ ... -g: Gọi trình biên dịch với cờ debug.

main.cpp, data/*.cpp, utils/*.cpp: Gom tất cả các file mã nguồn .cpp cần thiết.

-Iinclude: Báo cho trình biên dịch tìm các file header (.h) trong thư mục include.

-o bin/main_console.exe: Tạo file thực thi tên là main_console.exe trong thư mục bin.

2. Chạy chương trình
Sau khi biên dịch thành công (không có lỗi), chạy lệnh:

Bash

.\bin\main.exe