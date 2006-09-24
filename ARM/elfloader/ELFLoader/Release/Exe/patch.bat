rem hex2vkp -fC:\1.bin elfloader.hex
elf2vkp.exe ELFLoader.elf aaa.vkp C:\1.bin
copy /b E:\ARM\swilib.vkp+aaa.vkp E:\ARM\ElfPack.vkp
