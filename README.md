# Library Management System

## Mô tả
Dự án quản lý thư viện bằng C++, hỗ trợ quản lý đầu sách, danh mục sách, độc giả, và mượn trả.

## Cấu trúc thư mục
- `include/`: Chứa file header (.h)
- `src/`: Chứa file nguồn (.cpp)
- `data/`: Chứa file dữ liệu (thuvien.dat)

## Cách build và chạy
1. Cài g++ hoặc IDE hỗ trợ C++ (Visual Studio, Code::Blocks).
2. Build: `g++ src/*.cpp -o library`
3. Run: `./library` (Linux/macOS) hoặc `library.exe` (Windows)

## Phân công
- A: DauSach.h, DauSach.cpp, README.md, .gitignore
- B: DocGia.h, DocGia.cpp, Utils.h, Utils.cpp
- Cả hai: Main.cpp, data/thuvien.dat
