// TODO: split into iso9660.c and iso9660.h

// Thanks to https://github.com/ge0rg/libxenon/blob/master/libxenon/drivers/iso9660/iso9660.c,
// some code snippets were taken from there

#include <stddef.h> // Also includes stdint.h

// ISO 9660 directory entry
typedef struct {
    uint8_t length;
    uint8_t ext_attr_length;
    uint8_t extent[8];
    uint8_t size[8];
    uint8_t date[7];
    uint8_t flags;
    uint8_t file_unit_size;
    uint8_t interleave;
    uint8_t vol_sequence[4];
    uint8_t name_len;
    char    name[1];
} ISODirEntry;
