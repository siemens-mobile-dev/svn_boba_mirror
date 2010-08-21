@echo off

if %1a==a goto error

call :getprojname %1

rmdir /q /s temp
mkdir temp

echo #include "target\%projectname%.h" >temp\temp.h

for /f %%a in (..\include\DYN_syntax.cfg) do (
echo #ifndef %%a^

  #define %%a UNDEFINED^

#endif >>temp\temp.h
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
