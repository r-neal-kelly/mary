#include <Mary/File.h>
#include <Mary/OS.h>

MARY_Primitives;

static void File_Error(Mary_File_t *mary_file);

void Mary_File_String_Create(Mary_File_t *mary_file, Mary_String_t *path, Mary_Enum_t mode, Mary_UTF_t utf)
{
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "'utf' must be either 8, 16, or 32.");

    MARY_Bit_Clear(mary_file->flags);
    MARY_Bit_Set(mary_file->flags, MARY_FILE_STRING);
    mary_file->unit = utf >> 3;
    mary_file->stream = Mary_OS_File_Create(path, mode);

    if (mary_file->stream == 0) {
        File_Error(mary_file);
    } else {
        if (mode == MARY_FILE_WRITE ||
            mode == MARY_FILE_WRITE_READ) { // need to verify append is okay.
            mary_file->units = 0;
        } else {
            u64 bytes = 0;
            while (fgetc(mary_file->stream),
                   feof(mary_file->stream) == 0) {
                ++bytes;
            }
            rewind(mary_file->stream);
            mary_file->units = bytes / mary_file->unit;
        }
    }
}

void Mary_File_Bytes_Create(Mary_File_t *mary_file, Mary_String_t *path, Mary_Enum_t mode, Mary_Size_t unit)
{
    MARY_Bit_Clear(mary_file->flags);
    MARY_Bit_Set(mary_file->flags, MARY_FILE_BYTES);
    mary_file->unit = unit;
    mary_file->stream = Mary_OS_File_Create(path, mode);

    if (mary_file->stream == 0) {
        File_Error(mary_file);
    } else {
        if (mode == MARY_FILE_WRITE ||
            mode == MARY_FILE_WRITE_READ) { // need to verify append is okay.
            mary_file->units = 0;
        } else {
            u64 bytes = 0;
            while (fgetc(mary_file->stream),
                   feof(mary_file->stream) == 0) {
                ++bytes;
            }
            rewind(mary_file->stream);
            mary_file->units = bytes / unit;
        }
    }
}

void Mary_File_Destroy(Mary_File_t *mary_file)
{
    if (fclose(mary_file->stream) == EOF) {
        File_Error(mary_file);
    } else {
        MARY_Bit_Set(mary_file->flags, MARY_FILE_ENDED);
    }
}

static void File_Error(Mary_File_t *mary_file)
{
    mary_file->units = 0;
    MARY_Bit_Set(mary_file->flags, MARY_FILE_ERROR);
    MARY_Assert(MARY_FALSE, "Encountered a file handling error.");
}

Mary_Bool_t Mary_File_Has_String(Mary_File_t *mary_file)
{
    return MARY_Bit_Is_Set(mary_file->flags, MARY_FILE_STRING);
}

Mary_Bool_t Mary_File_Has_Bytes(Mary_File_t *mary_file)
{
    return MARY_Bit_Is_Set(mary_file->flags, MARY_FILE_BYTES);
}

Mary_Bool_t Mary_File_Has_Error(Mary_File_t *mary_file)
{
    return MARY_Bit_Is_Set(mary_file->flags, MARY_FILE_ERROR);
}

Mary_Bool_t Mary_File_Has_Ended(Mary_File_t *mary_file)
{
    if (MARY_Bit_Is_Set(mary_file->flags, MARY_FILE_ENDED)) {
        return MARY_TRUE;
    } else if (MARY_Truthy(feof(mary_file->stream))) {
        MARY_Bit_Set(mary_file->flags, MARY_FILE_ENDED);
        return MARY_TRUE;
    } else {
        return MARY_FALSE;
    }
}

Mary_Bool_t Mary_File_String_Read_Unit(Mary_File_t *mary_file, void *out_unit)
{
    if (MARY_Bit_Is_Set(mary_file->flags, MARY_FILE_ENDED)) {
        return MARY_FALSE;
    } else {
        U cr = '\r', lf = '\n';
        fread(out_unit, mary_file->unit, 1, mary_file->stream);
        if (MARY_Truthy(feof(mary_file->stream))) {
            MARY_Bit_Set(mary_file->flags, MARY_FILE_ENDED);
            return MARY_FALSE;
        } else {
            if (memcmp(out_unit, &cr, mary_file->unit) == 0) {
                fpos_t pos; fgetpos(mary_file->stream, &pos);
                fread(out_unit, mary_file->unit, 1, mary_file->stream);
                if (MARY_Truthy(feof(mary_file->stream))) {
                    Mary_Copy(&lf, out_unit, mary_file->unit);
                    MARY_Bit_Set(mary_file->flags, MARY_FILE_ENDED);
                } else if (memcmp(out_unit, &lf, mary_file->unit) != 0) {
                    fsetpos(mary_file->stream, &pos);
                    Mary_Copy(&lf, out_unit, mary_file->unit);
                }
            }
        }
    }
    return MARY_TRUE;
}

Mary_Bool_t Mary_File_Bytes_Read_Unit(Mary_File_t *mary_file, void *out_unit)
{
    if (MARY_Bit_Is_Set(mary_file->flags, MARY_FILE_ENDED)) {
        return MARY_FALSE;
    } else {
        fread(out_unit, mary_file->unit, 1, mary_file->stream);
        if (MARY_Truthy(feof(mary_file->stream))) {
            MARY_Bit_Set(mary_file->flags, MARY_FILE_ENDED);
            return MARY_FALSE;
        }
    }
    return MARY_TRUE;
}

Mary_Bool_t Mary_File_String_Read_Line(Mary_File_t *file, Mary_String_t *out_line)
{
    MARY_Assert(MARY_String_Is_Created(out_line),
                "'out_line' has not been created.");

    if (MARY_Bit_Is_Set(file->flags, MARY_FILE_BYTES) ||
        MARY_Bit_Is_Set(file->flags, MARY_FILE_ENDED)) {
        return MARY_FALSE;
    }

    Mary_UTF_t utf = MARY_String_Get_UTF(out_line);
    Mary_Vector_t *out_vec = MARY_Vector_Cast(out_line);
    Mary_Size_t codes_read = 0;

    MARY_Vector_Empty(out_vec);

    if (utf == 8) {
        Mary_Char_8_t code = 0;
        while (Mary_File_String_Read_Unit(file, &code)) {
            MARY_Vector_Push(out_vec, Mary_Char_8_t, code);
            ++codes_read;
            if (code == '\n') {
                break;
            }
        }
        code = 0;
        MARY_Vector_Push(out_vec, Mary_Char_8_t, code);
    } else if (utf == 16) {
        Mary_Char_16_t code = 0;
        while (Mary_File_String_Read_Unit(file, &code)) {
            if (code != '\n') {
                MARY_Vector_Push(out_vec, Mary_Char_16_t, code);
                ++codes_read;
            } else {
                break;
            }
        }
        code = 0;
        MARY_Vector_Push(out_vec, Mary_Char_16_t, code);
    } else if (utf == 32) {
        Mary_Char_32_t code = 0;
        while (Mary_File_String_Read_Unit(file, &code)) {
            if (code != '\n') {
                MARY_Vector_Push(out_vec, Mary_Char_32_t, code);
                ++codes_read;
            } else {
                break;
            }
        }
        code = 0;
        MARY_Vector_Push(out_vec, Mary_Char_32_t, code);
    }

    out_line->codes = Mary_C_String_Count_Codes(out_line->data, utf, MARY_TRUE);

    if (codes_read > 0) {
        return MARY_TRUE;
    } else {
        return MARY_FALSE;
    }
}

void Mary_File_String_Read_All(Mary_File_t *mary_file, Mary_String_t *out_string, Mary_Bool_t is_created)
{
    Mary_Vector_t *out_vec = MARY_Vector_Cast(out_string);

    rewind(mary_file->stream);

    if (MARY_Truthy(is_created)) {
        MARY_Assert(out_string->unit == mary_file->unit, 0);
        Mary_Vector_Reserve(out_vec, out_string->units + mary_file->units + 1);
    } else {
        Mary_Vector_Create(out_vec, MARY_Allocator_Heap, mary_file->unit, mary_file->units + 1);
    }

    u64 buffer_idx = out_vec->units;
    u8 *buffer_ptr = MARY_Vector_Point(out_vec, buffer_idx), *buffer_start = buffer_ptr;
    while (Mary_File_String_Read_Unit(mary_file, buffer_ptr)) {
        ++out_vec->units, buffer_ptr += out_vec->unit;
    }
    u64 null = '\0';
    Mary_Copy(&null, MARY_Vector_Point_Push(out_vec), out_vec->unit);
    out_string->codes += Mary_C_String_Count_Codes(buffer_start, MARY_Unit_To_UTF(out_string->unit), 1);
}

void Mary_File_Bytes_Read_All(Mary_File_t *mary_file, Mary_Vector_t *out_buffer, Mary_Bool_t is_created)
{
    rewind(mary_file->stream);

    if (MARY_Truthy(is_created)) {
        MARY_Assert(out_buffer->unit == mary_file->unit, 0);
        Mary_Vector_Reserve(out_buffer, out_buffer->units + mary_file->units);
    } else {
        Mary_Vector_Create(out_buffer, MARY_Allocator_Heap, mary_file->unit, mary_file->units);
    }

    u8 *buffer_ptr = MARY_Vector_Point(out_buffer, out_buffer->units);
    u64 units_read = fread(buffer_ptr, mary_file->unit, mary_file->units, mary_file->stream);
    if (MARY_Truthy(ferror(mary_file->stream)) || units_read != mary_file->units) {
        File_Error(mary_file);
    } else {
        out_buffer->units += units_read;
    }
}

void Mary_File_String_Write_All(Mary_File_t *mary_file, Mary_String_t *in_string)
{
    u64 written_units = fwrite(in_string->data, in_string->unit,
                               in_string->units - 1, mary_file->stream);
    if (written_units != in_string->units - 1) {
        File_Error(mary_file);
    }
}

void Mary_File_Bytes_Write_All(Mary_File_t *mary_file, Mary_Vector_t *in_buffer)
{
    u64 written_units = fwrite(in_buffer->data, in_buffer->unit,
                               in_buffer->units, mary_file->stream);
    if (written_units != in_buffer->units) {
        File_Error(mary_file);
    }
}

void Mary_File_Point_Front(Mary_File_t *mary_file)
{
    MARY_Bit_Unset(mary_file->flags, MARY_FILE_ENDED);
    rewind(mary_file->stream);
}
