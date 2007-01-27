#ifndef _OpenPL2Library_libpl2_h_
#define _OpenPL2Library_libpl2_h_ INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
#	define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#ifndef WINDOWS
#include <dirent.h>
#	include <stdint.h>
#	include <unistd.h>
#	include <strings.h>
#endif
#ifdef WITH_ICONV
#	include <iconv.h>
#endif


#ifdef WINDOWS
#include <windirent.h>
#include <direct.h>
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef signed int int32_t;
typedef signed short int16_t;
typedef signed char int8_t;
typedef unsigned int uptr_t;
#define strcasecmp _stricmp
#define chdir _chdir
#define malloca _malloca
#define freea _freea
#define snprintf sprintf_s
#endif

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;
typedef int32_t   s32;
typedef int16_t   s16;
typedef int8_t    s8;
typedef uintptr_t uptr;

typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;

typedef int32_t  int32;
typedef int16_t  int16;
typedef int8_t   int8;


#ifndef WINDOWS
#	ifdef DEBUG
#		define output_debug(_format, _va...)    pl2_output(3, __FUNCTION__, __LINE__, _format, ## _va)
#	else
#		define output_debug(_format, _va...)    while(0)
#	endif
#	define output_message(_format, _va...)  pl2_output(2, __FUNCTION__, __LINE__, _format,  ## _va)
#	define output_warning(_format, _va...)  pl2_output(1, __FUNCTION__, __LINE__, _format,  ## _va)
#	define output_error(_format, _va...)    pl2_output(0, __FUNCTION__, __LINE__, _format,  ## _va)
#else
#	ifdef DEBUG
#		define output_debug(_format, ...)    pl2_output(3, __FUNCTION__, __LINE__, _format, __VA_ARGS__)
#	else
#		define output_debug(_format, ...)    while(0)
#	endif
#	define output_message(_format, ...)  pl2_output(2, __FUNCTION__, __LINE__, _format, __VA_ARGS__)
#	define output_warning(_format, ...)  pl2_output(1, __FUNCTION__, __LINE__, _format, __VA_ARGS__)
#	define output_error(_format, ...)    pl2_output(0, __FUNCTION__, __LINE__, _format, __VA_ARGS__)
#endif

#define endian_u32(_x)      (_x)
#define endian_u16(_x)      (_x)
#define endian_float(_x)    (_x)
#define endian_Vector4(_x)  (_x)
#define endian_Matrix44(_x) (_x)



///////////////////////////////////////
// Standard
///////////////////////////////////////



#define \
strskip( \
	_str, \
	_end \
) \
	while (_str<_end) { \
		if (!isspace(*(u8*)_str)) \
			break; \
		_str++; \
	}

#define \
strterm( \
	_conststr \
) \
	_conststr[sizeof(_conststr)-1] = '\0'


int
strfilename(
	char *dst,
	int sz,
	const char *src
);

int
strdirectory(
	char *dst,
	int sz,
	const char *src
);

int
strendcmp(
	const char *b,
	const char *s
);

int
strendcasecmp(
	const char *b,
	const char *s
);

int
dirsweep(
	const char *name,
	int
	(*f_file)(
		const char *name,
		void *user
	),
	int
	(*f_dir)(
		const char *name,
		void *user
	),
	void *user
);

int
dirsweep_recursive(
	const char *name,
	void *user
);

typedef void
(*pl2_outputComplexFunc)(
	u32 level,
	const char *function,
	int line,
	const char *msg,
	void *user
);

typedef void
(*pl2_outputFunc)(
	const char *msg,
	void *user
);

void
pl2_output(
	u32 level,
	const char *function,
	int line,
	const char *format,
	...
);

void
pl2_outputHookComplex(
	pl2_outputComplexFunc func,
	void *user
);

void
pl2_outputHook(
	pl2_outputFunc func,
	void *user
);

const char *
pl2_outputGet(
	u32 id
);


///////////////////////////////////////
// TMB files
///////////////////////////////////////

#pragma pack(push, 1)
typedef struct {
	char Name[32]; 
	u16 Width, Height;
	u8  Buffer[];
} pl2TMB_Texture;

typedef struct {
	float Ambient[4];  //!< Probably wrong
	float Diffuse[4];  //!< Probably wrong
	float Specular[4]; //!< Probably wrong
	float Emission[4]; //!< Probably wrong
	float Shininess;   //!< Probably wrong
	u32   Texture;     //!< Correct!
} pl2TMB_Material;

typedef struct {
	float Position[3];
	float Weights[3];
	u8 Bone[4];
	float Normal[3];
	u8 RGBA[4];
	float TexCoord[2];
} pl2TMB_Vertex;

typedef struct {
	u32 Material; //Material ID
	u32 Start;    //In verts
	u32 Count;    //In triangles
} pl2TMB_Bind;
#pragma pack(pop)

typedef struct {
	char *Name;
	float *Matrix;
	u32 TriangleCount;
	u32 Unknown;
	u32 BindCount;
	pl2TMB_Bind *Binds;
	pl2TMB_Vertex *Vertices;
} pl2TMB_Mesh;

typedef struct {
	u32 Flags;
	char Name[32];
	u8 *Data; 
	u32 Size;

	u32 TextureCount;
	pl2TMB_Texture **Textures;  // free()

	u32 MaterialCount;          
	pl2TMB_Material *Materials; // no free()

	u32 MeshCount;
	pl2TMB_Mesh *Meshes; // free()

	u32 BoneCount;
	float **Bones; // free()

	u32 UnknownCount;

} pl2TMB;

pl2TMB *
pl2TMB_load(
	const char *file
);

pl2TMB *
pl2TMB_create(
	void *buf,
	u32 size,
	const char *name
);

void
pl2TMB_destroy(
	pl2TMB *tmb
);


///////////////////////////////////////
// Attribute files
///////////////////////////////////////

typedef struct {
	char Name[32];
	char Text[512];
} pl2AttrEntry;

typedef struct {
	u32 EntryCount;
	pl2AttrEntry Entries[];
} pl2Attr;

pl2Attr *
pl2Attr_load(
	void *buf,
	u32 size
);

const char *
pl2Attr_get(
	pl2Attr *attr,
	const char *name,
	int id
);

void
pl2Attr_destroy(
	pl2Attr *attr
);


///////////////////////////////////////
// Pak/PL2 files
///////////////////////////////////////

#pragma pack(push, 1)
typedef struct {
	char Name[32];  //!< Name of entry 
	u32 Offset;     //!< Offset (in bytes) of data from start of file
	u32 PackedSize; //!< Size (in bytes) of packed data
	u32 Size;       //!< Size (in bytes) of unpacked data
	u32 Pad;        //!< Padding?
} pl2PakEntry;
#pragma pack(pop)

typedef struct {
	char Name[32];
	u8 *Data;
	u32 Size;
	u32 Flags;
	u32 EntryCount;
	pl2PakEntry *Entries;
} pl2Pak;

pl2Pak *
pl2Pak_load(
	const char *file
);

pl2Pak *
pl2Pak_create(
	void *buf,
	u32 size,
	const char *name
);

void *
pl2Pak_unpack(
	pl2Pak *pak,
	const char *name,
	u32 *size
);

void *
pl2Pak_unpackID(
	pl2Pak *pak,
	u32 id,
	u32 *size
);

void 
pl2Pak_destroy(
	pl2Pak *pak
);

#ifdef __cplusplus
}
#endif

#endif

