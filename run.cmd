@echo off
pushd "%~dp0"
setlocal

echo ==============================
echo 📚 Library Management System
echo ==============================

REM Đảm bảo console dùng UTF-8
chcp 65001 >nul

REM Tạo thư mục bin nếu chưa có
if not exist bin mkdir bin

echo.
echo 🔧 Building project...
echo ------------------------------

REM Build tất cả file .cpp
g++ -std=c++17 -Wall -Wextra -Wno-unused-parameter ^
-finput-charset=UTF-8 -fexec-charset=UTF-8 -g ^
main.cpp data\KiemTraDuLieu.cpp data\QuanLySach.cpp data\ThaoTacFile.cpp ^
data\NhapLieu.cpp data\DocGia.cpp data\TheDocGia.cpp ^
utils\ThongBao.cpp utils\NgayThang.cpp utils\XuLyChuoi.cpp utils\VietnameseUtils.cpp ^
-Iinclude -o bin\main.exe

if errorlevel 1 (
    echo.
    echo ❌ Build failed! Please check for errors above.
    pause
    popd
    endlocal
    exit /b 1
)

if not exist bin\main.exe (
    echo ❌ Binary not found: bin\main.exe
    pause
    popd
    endlocal
    exit /b 1
)

echo.
echo ✅ Build successful!
echo ------------------------------
echo.

REM Chạy chương trình
echo ▶️ Running program...
echo ==============================
powershell -NoProfile -Command "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8; & '.\bin\main.exe'"

echo.
pause
popd
endlocal
exit /b 0