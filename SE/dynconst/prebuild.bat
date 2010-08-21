@echo off

if %1a==a goto error

call :getprojname %1

echo #include "target\%projectname%.h" >include.tmp

for /f %%a in (..\include\DYN_syntax.cfg) do (
   echo #ifndef %%a >>include.tmp
   echo   #define %%a UNDEFINED >>include.tmp
   echo #endif >>include.tmp
)

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
