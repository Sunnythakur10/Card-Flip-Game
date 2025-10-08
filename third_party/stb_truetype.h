// stb_truetype - v1.26 - public domain
// Minimal subset required for this project (declaration/implementation combined)
// Full version available at https://github.com/nothings/stb
#ifndef STB_TRUETYPE_H_MINIMAL
#define STB_TRUETYPE_H_MINIMAL

#define STBTT_STATIC
#define STBTT_DEF static
typedef unsigned char stbtt_uint8; typedef signed char stbtt_int8; typedef unsigned short stbtt_uint16; typedef signed short stbtt_int16; typedef unsigned int stbtt_uint32; typedef signed int stbtt_int32; typedef unsigned int stbtt_uint;
typedef float stbtt_float;
typedef struct { unsigned char *data; int fontstart; } stbtt_fontinfo; // extremely trimmed version

// We only expose the small API surface we use; for any expansion, replace with full stb_truetype.h
extern "C" {
    STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char *data, int index);
    STBTT_DEF int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset);
}

// Dummy implementations (we won't actually rasterize in this minimal header). This acts as a stub so code compiles
STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char *data, int index) { (void)data; (void)index; return 0; }
STBTT_DEF int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset) { info->data = (unsigned char*)data; info->fontstart = offset; return 1; }

#endif // STB_TRUETYPE_H_MINIMAL