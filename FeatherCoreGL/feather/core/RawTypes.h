#pragma once

typedef long long    i64;
typedef int          i32;
typedef short        i16;
typedef char         i8;

typedef unsigned long long    u64;
typedef unsigned int          u32;
typedef unsigned short        u16;
typedef unsigned char         u8;



typedef u8* buffer;

#ifdef VSTR_U16
// UTF-16 string
#define VSTR(cstr)   L##cstr
#define VSTRING_UNIT wchar_t
#define vfopen_s _wfopen_s
#else
// ANSI string
#define VSTR(cstr)   cstr
#define VSTRING_UNIT char
#define vfopen_s fopen_s
#endif // DEBUG

typedef VSTRING_UNIT* vstring;
typedef const VSTRING_UNIT* vcstring;
