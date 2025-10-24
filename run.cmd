@echo off
pushd "%~dp0"
setlocal

echo ==============================
echo 📚 Library Management System
echo ==============================

REM Đảm bảo console dùng UTF-8 (Tùy chọn, không ảnh hưởng BGI)
chcp 65001 >nul

REM Tạo thư mục bin nếu chưa có
if not exist bin mkdir bin

echo.
echo 🔧 Building project...
echo ------------------------------

REM Kiểm tra và dừng tiến trình main.exe (Giữ nguyên)
echo.
echo 🔍 Checking for running main.exe processes...
tasklist /FI "IMAGENAME eq main.exe" 2>NUL | find /I "main.exe" >NUL
if %ERRORLEVEL%==0 (
    echo ⚠️ Found running main.exe processes — terminating them to allow rebuild...
    taskkill /F /IM main.exe >nul 2>&1
    if %ERRORLEVEL%==0 (
        echo ✅ main.exe processes terminated.
    ) else (
        echo ⚠️ Could not terminate main.exe processes. You may need to close them manually.
    )
) else (
    echo ✅ No running main.exe processes found.
)

REM *** LỆNH BIÊN DỊCH HOÀN CHỈNH ***
g++ -std=c++17 -Wall -Wextra -Wno-unused-parameter ^
-g ^
main.cpp GiaoDien.cpp data\KiemTraDuLieu.cpp data\QuanLySach.cpp data\ThaoTacFile.cpp ^
data\NhapLieu.cpp ^
utils\ThongBao.cpp utils\NgayThang.cpp utils\XuLyChuoi.cpp ^
-Iinclude -Ilib/winbgim ^
-o bin\main.exe ^
-Llib/winbgim ^
-lbgi ^
-lgdi32 -luser32 -lcomdlg32 -luuid -loleaut32 -lole32 ^
-mwindows

REM Kiểm tra lỗi biên dịch (Giữ nguyên)
if errorlevel 1 (
    echo.
    echo ❌ Build failed! Please check for errors above.
    pause
    popd
    endlocal
    exit /b 1
)

REM Kiểm tra file thực thi có tồn tại không (Giữ nguyên)
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

REM Chạy chương trình (Giữ nguyên cách chạy PowerShell)
echo ▶️ Running program...
echo ==============================
REM Dùng Start-Process để chạy tách biệt, không cần OutputEncoding nữa vì console ẩn
powershell -NoProfile -Command "Start-Process -FilePath '.\bin\main.exe' -WindowStyle Normal"

echo.
pause
popd
endlocal
exit /b 0