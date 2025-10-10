@echo off
pushd "%~dp0"
setlocal

if not exist bin\main.exe (
    echo Binary not found: bin\main.exe
    popd
    endlocal
    exit /b 1
)

REM Nếu bạn đang chạy từ PowerShell, dùng `run_ps.ps1` để có hiển thị UTF-8 tốt hơn:
REM   powershell -NoProfile -File .\run_ps.ps1 -- args...

REM Đảm bảo console dùng UTF-8
chcp 65001 >nul
REM Đặt OutputEncoding cho PowerShell (nếu chạy từ VSCode)
powershell -NoProfile -Command "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8"

REM Chạy chương trình
bin\main.exe %*

popd
endlocal
exit /b 0