#include <stdio.h>
#include <Mary/Utils.h>
#include <Mary/Regex.h>
#include <Mary/OS.h>
#include <Mary/App/GCC.h>
#include MARY_Vector_Namespace

MARY_Primitives;

// will need to have a func in OS that gives me a directory list
// also, this might be a good time to try out Regex, even though it's not perfect.
// it will be useful in getting the ranges we need to work in our mmake file.

void Mary_GCC_Create_Bat()
{
  // I would also like to have bin, and name of exe in make file.

  Mary_C_String_8_t compiler_path = "\"%HOMEDRIVE%/Program Files/mingw-w64/x86_64-8.1.0-win32-seh-rt_v6-rev0/mingw64/bin/gcc\"\n";

  MARY_String_Stack(make_file_path, 8, "./gcc.mmake");
  Mary_File_t make_file; Mary_File_String_Create(&make_file, &make_file_path, MARY_FILE_READ, 8);
  MARY_Vector_Heap(make_file_lines, Mary_String_t, 0);
  Mary_String_t *make_file_line;
  while (MARY_False(Mary_File_Has_Ended(&make_file)))
  {
    make_file_line = MARY_Vector_Point_Push(&make_file_lines);
    MARY_String_Heap_p(make_file_line, 8, u8"");
    Mary_File_String_Read_Line(&make_file, make_file_line);
  }
  Mary_File_Destroy(&make_file);
  Mary_String_Destroy(&make_file_path);

  MARY_String_Heap(build_debug_str, 8, "");
  MARY_String_Heap(build_release_str, 8, "");
  MARY_String_Heap(run_debug_str, 8, "");
  MARY_String_Heap(run_release_str, 8, "");

  MARY_String_Append_Back(&build_debug_str, 8, "@echo off\n");
  MARY_String_Append_Back(&build_debug_str, 8, "set GCC=");
  MARY_String_Append_Back(&build_debug_str, 8, compiler_path);
  MARY_String_Append_Back(&build_debug_str, 8, "if not exist \"bin/debug\" mkdir \"bin/debug\"\n");
  MARY_String_Append_Back(&build_debug_str, 8, "@echo on\n");
  MARY_String_Append_Back(&build_debug_str, 8, "%GCC% -O0 -Wall ");
  MARY_String_Append_Back(&build_debug_str, 8, "-I ./Mary/include "); // this is a hack just to get includes and srcs in there.

  MARY_String_Stack(src_path, 16, u"./Mary/src/");
  Mary_OS_Directory_t src_dir; Mary_OS_Directory_Create(&src_dir, &src_path);
  MARY_Vector_Each(&src_dir.items, Mary_OS_Directory_Item_t)
  {
    MARY_String_Append_Back(&build_debug_str, 8, "./Mary/src/");
    MARY_String_Append_Back(&build_debug_str, 16, it.ptr->name.data);
    MARY_String_Append_Back(&build_debug_str, 8, " ");
  }
  Mary_OS_Directory_Destroy(&src_dir);

  MARY_String_Stack(app_path, 16, u"./Mary/app/");
  Mary_OS_Directory_t app_dir; Mary_OS_Directory_Create(&app_dir, &app_path);
  MARY_Vector_Each(&app_dir.items, Mary_OS_Directory_Item_t)
  {
    MARY_String_Append_Back(&build_debug_str, 8, "./Mary/app/");
    MARY_String_Append_Back(&build_debug_str, 16, it.ptr->name.data);
    MARY_String_Append_Back(&build_debug_str, 8, " ");
  }
  Mary_OS_Directory_Destroy(&app_dir);

  MARY_String_Append_Back(&build_debug_str, 8, "-o bin/debug/main\n");

  printf("%s\n", (u8 *)build_debug_str.data);

  MARY_String_Stack(build_debug_path, 8, "build_debug.bat");
  Mary_File_t build_debug_file; Mary_File_String_Create(&build_debug_file, &build_debug_path, MARY_FILE_WRITE, 8);
  Mary_File_String_Write_All(&build_debug_file, &build_debug_str);
  Mary_File_Destroy(&build_debug_file);

  Mary_String_Destroy(&build_debug_str);
  Mary_String_Destroy(&build_release_str);
  Mary_String_Destroy(&run_debug_str);
  Mary_String_Destroy(&run_release_str);

  MARY_Vector_Each(&make_file_lines, Mary_String_t)
  {
    Mary_String_Destroy(it.ptr);
  }
  Mary_Vector_Destroy(&make_file_lines);

  Mary_Exit_Success();
}
