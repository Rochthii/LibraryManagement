@echo off
pushd "%~dp0"
setlocal

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