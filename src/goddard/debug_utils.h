#ifndef GD_DEBUGGING_UTILS_H
#define GD_DEBUGGING_UTILS_H

#include <PR/ultratypes.h>

#include "gd_types.h"
#include "macros.h"

union PrintVal {
    f32 f;
    s32 i;
    s64 pad;
};

/* based on fields set in gd_fopen; gd_malloc_perm(84) for size */
struct GdFile {
    /* 0x04 */ u32 pos;
    /* 0x08 */ s8 *stream;
    /* Known Flags for +0xC field:
    ** 1 : write mode
    ** 2 : binary mode
    ** 4 : eof */
    /* 0x0C */ u32 flags;
    /* 0x50 */ u32 size;
}; /* sizeof() = 0x54 */

// bss
extern u8 *gGdStreamBuffer;

// functions
extern f32 gd_rand_float(void);
extern s32 gd_atoi(const char *);
extern f64 gd_lazy_atof(const char *, u32 *);
extern char *sprint_val_withspecifiers(char *, union PrintVal, char *);
extern void gd_strcpy(char *, const char *);
extern char *gd_strdup(const char *);
extern u32 gd_strlen(const char *);
extern char *gd_strcat(char *, const char *);
extern s32 gd_str_not_equal(const char *, const char *);
extern s32 gd_str_contains(const char *, const char *);
extern s32 gd_feof(struct GdFile *);
extern struct GdFile *gd_fopen(const char *, const char *);
extern s32 gd_fread(s8 *, s32, s32, struct GdFile *);
extern void gd_fclose(struct GdFile *);
extern u32 gd_get_file_size(struct GdFile *);
extern s32 gd_fread_line(char *, u32, struct GdFile *);

#endif // GD_DEBUGGING_UTILS_H
