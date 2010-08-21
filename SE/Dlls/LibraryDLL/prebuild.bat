@echo off

if %1a==a goto error

call :getprojname %1

del /q include.tmp
copy "target\%projectname%.h" include.tmp

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
