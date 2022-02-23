@echo off
set GCC="%HOMEDRIVE%/Program Files/mingw-w64/x86_64-8.1.0-win32-seh-rt_v6-rev0/mingw64/bin/gcc"
if not exist "bin/debug" mkdir "bin/debug"
@echo on
%GCC% -O0 -Wall -I ./Mary/include ./Mary/src/Bitbool.c ./Mary/src/Bitmap.c ./Mary/src/Div.c ./Mary/src/Element.c ./Mary/src/Event.c ./Mary/src/File.c ./Mary/src/GL_Texture.c ./Mary/src/Hashmap.c ./Mary/src/Lang.c ./Mary/src/List.c ./Mary/src/Mary.c ./Mary/src/OpenGL.c ./Mary/src/OS.c ./Mary/src/Pool.c ./Mary/src/Regex.c ./Mary/src/String.c ./Mary/src/Text.c ./Mary/src/Utils.c ./Mary/src/Varray.c ./Mary/src/Vector.c ./Mary/src/Window.c ./Mary/src/Wordmap.c ./Mary/app/GCC.c ./Mary/app/Main.c ./Mary/app/Test.c -o bin/debug/main
