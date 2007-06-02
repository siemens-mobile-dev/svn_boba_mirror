if exist %2\ff.bin %1\elf2vkp.exe %2\ELFLoader.elf %2\temp.vkp %2\ff.bin
if not exist %2\ff.bin %1\elf2vkp.exe %2\ELFLoader.elf %2\temp.vkp
copy /b %2\temp.vkp+%2\swilib.vkp %2\ElfPack.vkp
del %2\temp.vkp
