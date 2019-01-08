#include <string.h>
#include <malloc.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lauxlib.h>

#ifdef __cplusplus
}
#endif

#include "LuaFTGL.h"

using namespace FTGL;

#define MT(typen) #typen

typedef struct {
		FTLayout *layout;
		int fontref;
} FTLayoutUD;

static const char* error_memory = "Can't instanciate object of class `%s': %s:%d";
#define ALLOC_ERROR(classname) \
	{ lua_pushnil(L); \
	lua_pushfstring(L, error_memory, classname, __FILE__, __LINE__); \
	return 2; }

/* set field of a lua table with a number */
static void set_field(lua_State *L, unsigned int index, lua_Number value)
{
   lua_pushnumber(L, index);
   lua_pushnumber(L, value);
   lua_settable(L, -3);
}

static int get_enum(const ftgl_str_value ftgl_str[], const char *str)
{
   int i = 0;

   while(ftgl_str[i].str != 0)
   {
      if(strcmp(str, ftgl_str[i].str) == 0)
         return ftgl_str[i].value;

      i++;
   }
   return ENUM_ERROR;
}

static const char* get_enum_str(const ftgl_str_value ftgl_str[], int value)
{
   int i = 0;

   while(ftgl_str[i].str != 0)
   {
      if(value == ftgl_str[i].value)
         return ftgl_str[i].str;

      i++;
   }
   return NULL;
}

static void createmeta (lua_State *L, const char* mt)
{
	luaL_newmetatable(L, mt);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -2);
	lua_rawset(L, -3);
}

static int ftgl_userdata_new(lua_State* L, const char* mt, void* object)
{
	if (object)
	{
		void* p = (void*)lua_newuserdata(L, sizeof(void*));
		memcpy(p, &object, sizeof(void*));
		/* set metatable to CGcontext object */
		luaL_getmetatable(L, mt);
		lua_setmetatable(L, -2);
		return 1;
	}
	ALLOC_ERROR(mt);
}

static void* to_ftgl_userdata(lua_State* L, int idx, const char* mt)
{
	void* p = (void *)luaL_checkudata(L, idx, mt);
	if (!p)
	{
		luaL_argerror(L, idx, "null object reference");
	}
	else
	if (0 == *(int*)(p))
	{
		luaL_error(L, "attempt to use closed %s object", mt);
	}
	return (void*)(*(int*)p);
}

static int ftgl_new_font(lua_State* L)
{
	int ft_error;
	font_class_t font_class = (font_class_t)get_enum(ftgl_fontclass_str, luaL_checkstring(L, 1));
	const char* font_file_path = luaL_checkstring(L, 2);
	FTFont* font = NULL;
	if (font_class == ENUM_ERROR)
	{
		lua_pushnil(L);
		lua_pushliteral(L, "Invalid font class");
		return 2;
	}
	switch (font_class)
	{
	case FTGL_BITMAP:
		font = new FTBitmapFont(font_file_path);
		if (!font) ALLOC_ERROR("FTBitmapFont");
	break;
	case FTGL_PIXMAP:
		font = new FTPixmapFont(font_file_path);
		if (!font) ALLOC_ERROR("FTPixmapFont");
	break;
	case FTGL_OUTLINE:
		font = new FTOutlineFont(font_file_path);
		if (!font) ALLOC_ERROR("FTOutlineFont");
	break;
	case FTGL_POLYGON:
		font = new FTPolygonFont(font_file_path);
		if (!font) ALLOC_ERROR("FTPolygonFont");
	break;
	case FTGL_EXTRUDE:
		font = new FTExtrudeFont(font_file_path);
		if (!font) ALLOC_ERROR("FTExtrudeFont");
	break;
	case FTGL_TEXTURE:
		font = new FTTextureFont(font_file_path);
		if (!font) ALLOC_ERROR("FTTextureFont");
	break;
	case FTGL_BUFFER:
		font = new FTBufferFont(font_file_path);
		if (!font) ALLOC_ERROR("FTBufferFont");
	break;
	}

	ft_error = font->Error();
	if (ft_error)
	{
		lua_pushnil(L);
		lua_pushfstring(L, "Failed to open font %s, freetype error %d", font_file_path, ft_error);
		return 2;
	}

	return ftgl_userdata_new(L, MT(FTFont), (void*)font);
}

static int ftgl_delete_font(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	delete font;
	return 0;
}

static int ftgl_font_attach(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	const char* font_file_path = luaL_checkstring(L, 2);
	if (font->Attach(font_file_path))
	{
		lua_pushnil(L);
		lua_pushfstring(L, "Can't attach `%s'", font_file_path);
		return 2;
	}
	lua_pushboolean(L, 1);
	return 1;
}

static int ftgl_font_face_size(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	bool status;
	unsigned int face_size;
	switch (lua_gettop(L))
	{
	case 1:
	{
		lua_pushnumber(L, font->FaceSize());
		return 1;
	}
	case 2:
	{
		face_size = luaL_checkinteger(L, 2);
		status = font->FaceSize(face_size);
		break;
	}
	case 3:
	{
		face_size = luaL_checkinteger(L, 2);
		unsigned int res = luaL_checkinteger(L, 3);
		status = font->FaceSize(face_size, res);
		break;
	}
	}
	if (!status)
	{
		lua_pushnil(L);
		lua_pushfstring(L, "Failed to set size %d", face_size);
		return 2;
	}
	lua_pushboolean(L, 1);
	return 1;
}

static int ftgl_font_depth(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	double depth = luaL_checknumber(L, 2);
	font->Depth((float)depth);
	return 0;
}

static int ftgl_font_outset(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	double front = luaL_checknumber(L, 2);
	if (lua_gettop(L) == 3)
	{
		double back = luaL_checknumber(L, 3);
		font->Outset((float)front, (float)back);
	}
	else
	{
		font->Outset((float)front);
	}
	return 0;
}

static int ftgl_font_charmap(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	const char* encodingname = luaL_checkstring(L, 2);
	int encoding = get_enum(ftgl_encoding_str, encodingname);	
	if (ENUM_ERROR == encoding)
	{
		lua_pushnil(L);
		lua_pushfstring(L, "Uknown encoding `%s'", encodingname);
		return 2;
	}
	font->CharMap((FT_Encoding)encoding);
	lua_pushboolean(L, 1);
	return 1;
}

static int ftgl_font_render(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	const char* str = luaL_checkstring(L, 2);
	font->Render(str);
	return 0;
}

static int ftgl_font_ascender(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	lua_pushnumber(L, font->Ascender());
	return 1;
}

static int ftgl_font_descender(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	lua_pushnumber(L, font->Descender());
	return 1;
}

static int ftgl_font_line_height(lua_State* L)
{
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 1, MT(FTFont));
	lua_pushnumber(L, font->LineHeight());
	return 1;
}

static int ftgl_new_layout(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)malloc(sizeof(FTLayoutUD));
	if (!layoutud) ALLOC_ERROR("FTLayoutUD");
	layoutud->layout = new FTSimpleLayout();
	if (!layoutud->layout)
	{
		free(layoutud);
		ALLOC_ERROR("FTSimpleLayout");
	}
	layoutud->fontref = 0;
	return ftgl_userdata_new(L, MT(FTLayout), (void*)layoutud);
}

static int ftgl_layout_set_font(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	FTFont* font = (FTFont*)to_ftgl_userdata(L, 2, MT(FTFont));
	((FTSimpleLayout *)layoutud->layout)->SetFont(font);

	/* unref previous font userdata if any */
	luaL_unref(L, LUA_REGISTRYINDEX, layoutud->fontref);

	/* add reference to the font userdata */
	lua_pushvalue(L, 2);
	layoutud->fontref = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

static int ftgl_layout_set_line_length(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	((FTSimpleLayout *)layoutud->layout)->SetLineLength((float)luaL_checknumber(L, 2));

	return 0;
}

static int ftgl_layout_get_line_length(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	lua_pushnumber(L, ((FTSimpleLayout *)layoutud->layout)->GetLineLength());

	return 1;
}

static int ftgl_layout_set_line_spacing(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	((FTSimpleLayout *)layoutud->layout)->SetLineSpacing((float)luaL_checknumber(L, 2)); // percentage of font line height

	return 0;
}

static int ftgl_layout_get_line_spacing(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	lua_pushnumber(L, ((FTSimpleLayout *)layoutud->layout)->GetLineSpacing()); // percentage of font line height

	return 1;
}

static int ftgl_layout_set_alignment(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	const char* alignstr = luaL_checkstring(L, 2);
	int alignment = get_enum(ftgl_align_str, alignstr);
	if (alignment == ENUM_ERROR)
	{
		lua_pushnil(L);
		lua_pushfstring(L, "invalid alignment `%s'", alignstr);
		return 2;
	}

	((FTSimpleLayout *)layoutud->layout)->SetAlignment((TextAlignment)alignment);
	return 0;
}

static int ftgl_layout_get_alignment(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	int alignment = ((FTSimpleLayout *)layoutud->layout)->GetAlignment();
	lua_pushstring(L, get_enum_str(ftgl_align_str, alignment));

	return 1;
}

static int ftgl_layout_render(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	const char* str = luaL_checkstring(L, 2);

	((FTSimpleLayout *)layoutud->layout)->Render(luaL_checkstring(L, 2), lua_strlen(L, 2));

	return 0;
}

static int ftgl_layout_bbox(lua_State* L)
{
	FTBBox bbox;
    double x1, y1, z1, x2, y2, z2;

	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	bbox = layoutud->layout->BBox(luaL_checkstring(L, 2));

	x1 = bbox.Lower().X(); 
	y1 = bbox.Lower().Y(); 
	z1 = bbox.Lower().Z();

    x2 = bbox.Upper().X(); 
	y2 = bbox.Upper().Y(); 
	z2 = bbox.Upper().Z();
	
	lua_newtable(L);
	set_field(L, 1, x1);
	set_field(L, 2, y1);
	set_field(L, 3, z1);
	
	lua_newtable(L);
	set_field(L, 1, x2);
	set_field(L, 2, y2);
	set_field(L, 3, z2);
	
	return 2;
}

static int ftgl_layout_delete(lua_State* L)
{
	FTLayoutUD* layoutud = (FTLayoutUD*)to_ftgl_userdata(L, 1, MT(FTLayout));
	luaL_unref(L, LUA_REGISTRYINDEX, layoutud->fontref);
	delete layoutud->layout;
	free(layoutud);
	return 0;
}

static const luaL_reg ftgl_font_meths[] =
{
	{"Attach", ftgl_font_attach},
	{"FaceSize", ftgl_font_face_size},
	{"Depth", ftgl_font_depth},
	{"Outset", ftgl_font_outset},
	{"CharMap", ftgl_font_charmap},
	{"Render", ftgl_font_render},
	{"Ascender", ftgl_font_ascender},
	{"Descender", ftgl_font_descender},
	{"LineHeight", ftgl_font_line_height},
	{"__gc", ftgl_delete_font},
	{NULL, NULL}
};

static const luaL_reg ftgl_layout_meths[] =
{
	{"SetFont", ftgl_layout_set_font},
	{"SetLineLength", ftgl_layout_set_line_length},
	{"GetLineLength", ftgl_layout_get_line_length},
	{"SetAlignment", ftgl_layout_set_alignment},
	{"SetLineSpacing", ftgl_layout_set_line_spacing},
	{"GetLineSpacing", ftgl_layout_get_line_spacing},
	{"Render", ftgl_layout_render},
	{"BBox", ftgl_layout_bbox},
	{"__gc", ftgl_layout_delete},
	{NULL, NULL}
};

static const luaL_reg ftgllib[] =
{
	{"new_font", ftgl_new_font},
	{"new_layout", ftgl_new_layout},
	{NULL, NULL}
};

#ifdef __cplusplus
extern "C" {
#endif

LUAFTGL_API int luaopen_luaftgl (lua_State *L)
{
  createmeta(L, MT(FTFont));
  luaL_openlib(L, 0, ftgl_font_meths, 0);
  
  createmeta(L, MT(FTLayout));
  luaL_openlib(L, 0, ftgl_layout_meths, 0);
  luaL_openlib(L, "ftgl", ftgllib, 0);
  return 1;
}

#ifdef __cplusplus
}
#endif
