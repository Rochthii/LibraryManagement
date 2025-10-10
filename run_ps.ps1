# Run the program with proper UTF-8 encoding for PowerShell
chcp 65001
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
& "$PSScriptRoot\\bin\\main.exe" @args
