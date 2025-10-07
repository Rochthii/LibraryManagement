@echo off
pushd "%~dp0"
chcp 65001 >nul

REM 
if not exist bin mkdir bin

REM
set NEED_BUILD=0
if not exist bin\main.exe set NEED_BUILD=1

for %%f in (src\*.cpp include\*.h) do (
    if exist bin\main.exe (
        for %%e in (bin\main.exe) do (
            if %%~tf GTR %%~te set NEED_BUILD=1
        )
    )
)

if %NEED_BUILD%==1 (
    goto build
) else (
    goto run
)

:build
if not exist bin\VietnameseUtils.o (
    C:\mingw64\bin\g++.exe -std=c++17 -O2 -c src/VietnameseUtils.cpp -o bin/VietnameseUtils.o
)
C:\mingw64\bin\g++.exe -std=c++17 -O2 src/Main.cpp src/DauSach.cpp src/DanhMucSach.cpp src/InputUtils.cpp bin/VietnameseUtils.o -o bin/main.exe

:run
bin\main.exe %*
popd