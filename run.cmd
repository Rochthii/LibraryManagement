@echo off
pushd "%~dp0"
chcp 65001 >nul

if not exist bin mkdir bin
if not exist bin\main.exe goto build

rem Kiểm tra file mới hơn - nếu có thì build
for %%f in (src\*.cpp include\*.h) do (
    if "%%f" neq "" goto build
)

goto run

:build
if not exist bin\VietnameseUtils.o C:\mingw64\bin\g++.exe -std=c++17 -O2 -c src/VietnameseUtils.cpp -o bin/VietnameseUtils.o >nul
C:\mingw64\bin\g++.exe -std=c++17 -O2 src/Main.cpp src/DauSach.cpp src/DanhMucSach.cpp src/InputUtils.cpp bin/VietnameseUtils.o -o bin/main.exe >nul

:run
bin\main.exe %*
popd