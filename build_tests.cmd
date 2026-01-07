@echo off
rem build_tests.cmd - Build and run unit tests

pushd "%~dp0" || exit /b 1
setlocal

chcp 65001 >nul

echo ========================================
echo BUILDING UNIT TESTS
echo ========================================

set "ROOT=%~dp0"
set "TESTS_DIR=%ROOT%tests"

rem Find g++
set "GPP=C:\msys64\mingw64\bin\g++.exe"
if not exist "%GPP%" (
    if exist "C:\mingw64\bin\g++.exe" set "GPP=C:\mingw64\bin\g++.exe"
)
if not exist "%GPP%" (
    where g++ >nul 2>&1 && for /f "delims=" %%p in ('where g++') do set "GPP=%%p" && goto :GPP_FOUND
)
:GPP_FOUND
if not exist "%GPP%" (
    echo ❌ No g++ compiler found!
    pause
    exit /b 1
)

echo Using compiler: %GPP%
echo.

rem Build test_QuanLySach
echo 🔧 Building test_QuanLySach...
"%GPP%" -std=c++17 -Wall -Wextra ^
    -Iinclude ^
    -I. ^
    tests\test_QuanLySach.cpp ^
    data\QuanLySach.cpp ^
    data\KiemTraDuLieu.cpp ^
    data\ThaoTacFile.cpp ^
    data\NhapLieu.cpp ^
    utils\XuLyChuoi.cpp ^
    utils\NgayThang.cpp ^
    utils\ThongBao.cpp ^
    -o tests\test_QuanLySach.exe

if errorlevel 1 (
    echo.
    echo ❌ Build failed!
    pause
    exit /b 1
)

echo ✅ Build successful!
echo.
echo ========================================
echo RUNNING TESTS
echo ========================================
echo.

tests\test_QuanLySach.exe

set TEST_RESULT=%ERRORLEVEL%

echo.
if %TEST_RESULT% == 0 (
    echo ✅ All tests passed!
) else (
    echo ❌ Some tests failed!
)

pause
popd
endlocal
exit /b %TEST_RESULT%
