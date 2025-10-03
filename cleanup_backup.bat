@echo off
title Cleanup Backup Files
echo 🧹 Dọn dẹp file backup cũ...

cd /d "%~dp0\data"

echo 📊 Số file backup hiện tại:
dir backup_*.txt /b 2>nul | find /c ".txt"

echo.
echo 🗑️ Xóa backup cũ hơn 7 ngày...
forfiles /m backup_*.txt /d -7 /c "cmd /c del @path" 2>nul

echo.
echo 📦 Giữ lại tối đa 10 backup mới nhất...
for /f "skip=10 delims=" %%i in ('dir backup_*.txt /b /o:-d 2^>nul') do (
    echo Xóa: %%i
    del "%%i" 2>nul
)

echo.
echo ✅ Hoàn tất! Số file backup còn lại:
dir backup_*.txt /b 2>nul | find /c ".txt"

echo.
echo 📋 Danh sách backup còn lại:
dir backup_*.txt /b 2>nul

cd ..
pause