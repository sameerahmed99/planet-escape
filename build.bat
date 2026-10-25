
:: @NOTE on the -static flag
:: it statically links mingw libraries so that when you ship the game, the user doesn't need mingw dlls installed
:: To check what DLLs the game asks for from windows, run this command in windows terminal: objdump -p game.exe | findstr /i "DLL"
:: you will notice that if compiled without the static flag, it requires some other DLLs as well which you would need to ship
@echo off
mkdir build
gcc src/windows/win32_core.c -o build/PlanetEscape.exe  -g -O0 -DBUILD_DEBUG -DWRITE_ASSETS -std=c99 -municode -Wall -Werror=return-type -Wconversion -Wuninitialized -Wmaybe-uninitialized -Wpointer-sign -Wextra -Wno-missing-braces  -static -lgdi32 -luser32 -lole32 -luuid -lopengl32

:: add this to the parameter list if you want to genereate intermediate files
::-save-temps=obj


::gcc src/windows/win32_core.c -o build/game.exe -g -Og -DBUILD_DEBUG -DWRITE_ASSETS -std=c99 -municode  -lgdi32 -luser32 -lole32 -luuid -lopengl32
