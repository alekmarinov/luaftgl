#ifndef __LUAFTGL_H__
#define __LUAFTGL_H__

#if defined _WIN32 || defined __CYGWIN__
#  include <windows.h>
#  ifndef LUAFTGL_API
#    ifdef DLL_EXPORTS
#      define LUAFTGL_DLL
#    endif
#    ifdef LUAFTGL_DLL
#      ifdef __GNUC__
#        define LUAFTGL_API __attribute__((dllexport))
#      else
         // Note: actually gcc seems to also supports this syntax.
#        define LUAFTGL_API __declspec(dllexport)
#      endif
#    else
#      define LUAFTGL_API extern
#    endif
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <lualib.h>

LUAFTGL_API int luaopen_luaftgl (lua_State *L);

#ifdef __cplusplus
}
#endif

#include <FTGL/ftgl.h>

#define ENUM_ERROR (int)-2

typedef struct ftgl_str_value
{
  const char *str;
  int value;
} ftgl_str_value;

typedef enum
{
	FTGL_BITMAP,
	FTGL_PIXMAP,
	FTGL_OUTLINE,
	FTGL_POLYGON,
	FTGL_EXTRUDE,
	FTGL_TEXTURE,
	FTGL_BUFFER
} font_class_t;

static const ftgl_str_value ftgl_render_str[] =
{
	{"RENDER_FRONT",  FTGL::RENDER_FRONT},
	{"RENDER_BACK",   FTGL::RENDER_BACK},
	{"RENDER_SIDE",   FTGL::RENDER_SIDE},
	{"RENDER_ALL",    FTGL::RENDER_ALL},
	{ 0, 0}
};

static const ftgl_str_value ftgl_align_str[] =
{
	{"ALIGN_LEFT",    FTGL::ALIGN_LEFT},
	{"ALIGN_CENTER",  FTGL::ALIGN_CENTER},
	{"ALIGN_RIGHT",   FTGL::ALIGN_RIGHT},
	{"ALIGN_JUSTIFY", FTGL::ALIGN_JUSTIFY},
	{ 0, 0}
};

static const ftgl_str_value ftgl_fontclass_str[] =
{
	{"BITMAP",        FTGL_BITMAP},
	{"PIXMAP",        FTGL_PIXMAP},
	{"OUTLINE",       FTGL_OUTLINE},
	{"POLYGON",       FTGL_POLYGON},
	{"EXTRUDE",       FTGL_EXTRUDE},
	{"TEXTURE",       FTGL_TEXTURE},
	{"BUFFER",        FTGL_BUFFER},
	{ 0, 0}
};

static const ftgl_str_value ftgl_encoding_str[] =
{
	{"ENCODING_NONE", FT_ENCODING_NONE},
	{"ENCODING_MS_SYMBOL", FT_ENCODING_MS_SYMBOL},
	{"ENCODING_UNICODE", FT_ENCODING_UNICODE},
	{"ENCODING_SJIS", FT_ENCODING_SJIS},
	{"ENCODING_GB2312", FT_ENCODING_GB2312},
	{"ENCODING_BIG5", FT_ENCODING_BIG5},
	{"ENCODING_WANSUNG", FT_ENCODING_WANSUNG},
	{"ENCODING_JOHAB", FT_ENCODING_JOHAB},
	{"ENCODING_ADOBE_STANDARD", FT_ENCODING_ADOBE_STANDARD},
	{"ENCODING_ADOBE_EXPERT", FT_ENCODING_ADOBE_EXPERT},
	{"ENCODING_ADOBE_CUSTOM", FT_ENCODING_ADOBE_CUSTOM},
	{"ENCODING_ADOBE_LATIN_1", FT_ENCODING_ADOBE_LATIN_1},
	{"ENCODING_OLD_LATIN_2", FT_ENCODING_OLD_LATIN_2},
	{"ENCODING_APPLE_ROMAN", FT_ENCODING_APPLE_ROMAN},
};

#endif
