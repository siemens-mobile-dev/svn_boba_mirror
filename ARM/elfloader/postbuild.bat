pushd %1
..\!bin\elf2vkp %2 patches.vkp ..\!ff\s75_32.bin
..\!bin\equ2scat ..\s75_32.inc vkpheader.txt bobapack_%3.vkp lang=%3
popd