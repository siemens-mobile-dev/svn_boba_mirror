@echo off

if %1a==a goto error

call :getprojname %1

rmdir /q /s temp
mkdir temp
copy "xcl\%projectname%.xcl" "temp\temp.xcl"
copy "asm\%projectname%.asm" "temp\temp.asm"
copy "target\%projectname%.h" "temp\target.h"

goto :eof


:error
echo use $TARGET_DIR$ as argument
goto :eof

:getprojname
for /f "tokens=1* delims=\/ " %%a in ( "%~1" ) do (
    set projectname=%prev%
    set prev=%%a
    call :getprojname "%%b"
)
