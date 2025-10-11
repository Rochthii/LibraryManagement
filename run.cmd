@echo off
pushd "%~dp0"
setlocal

REM List of source files to check for changes
set SRC_LIST=main.cpp data\KiemTraDuLieu.cpp data\QuanLySach.cpp data\ThaoTacFile.cpp data\NhapLieu.cpp utils\ThongBao.cpp utils\NgayThang.cpp utils\XuLyChuoi.cpp utils\VietnameseUtils.cpp

REM Use PowerShell to determine whether any source is newer than binary or binary missing
for /f "usebackq delims=" %%B in (`powershell -NoProfile -Command "$srcs = @('main.cpp','data\\KiemTraDuLieu.cpp','data\\QuanLySach.cpp','data\\ThaoTacFile.cpp','data\\NhapLieu.cpp','utils\\ThongBao.cpp','utils\\NgayThang.cpp','utils\\XuLyChuoi.cpp','utils\\VietnameseUtils.cpp'); if (!(Test-Path 'bin\\main.exe')) { Write-Output 'BUILD'; exit }; $max = $null; foreach($s in $srcs){ if (Test-Path $s) { $t=(Get-Item $s).LastWriteTime; if ($max -eq $null -or $t -gt $max) { $max=$t } } }; $exe=(Get-Item 'bin\\main.exe').LastWriteTime; if ($max -gt $exe) { Write-Output 'BUILD' } else { Write-Output 'SKIP' }"`) do set ACTION=%%B

if "%ACTION%"=="BUILD" (
    "C:/mingw64/bin/g++.exe" -std=c++17 -Wall -Wextra -Wno-unused-parameter -finput-charset=UTF-8 -fexec-charset=UTF-8 -Iinclude -g main.cpp data/KiemTraDuLieu.cpp data/QuanLySach.cpp data/ThaoTacFile.cpp data/NhapLieu.cpp utils/ThongBao.cpp utils/NgayThang.cpp utils/XuLyChuoi.cpp utils/VietnameseUtils.cpp -I. -o bin/main.exe
    if errorlevel 1 (
        echo Build failed.
        popd
        endlocal
        exit /b 1
    )
)

REM Đảm bảo binary tồn tại
if not exist bin\main.exe (
    echo Binary not found: bin\main.exe
    popd
    endlocal
    exit /b 1
)

REM Đảm bảo console dùng UTF-8
chcp 65001 >nul
REM Đặt OutputEncoding cho PowerShell (nếu chạy từ VSCode)
powershell -NoProfile -Command "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8"

REM Chạy chương trình
bin\main.exe %*

popd
endlocal
exit /b 0