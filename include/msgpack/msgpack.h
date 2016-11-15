//
// MsgPack implementation (v3) in pure C 
//
// By Xiong.J, 2014-2016
//
//
// MsgPack Format Specification
//
// https://github.com/msgpack/msgpack/blob/master/spec.md
//
// CMP: https://github.com/camgunz/cmp/blob/master/cmp.c
// mpack: https://github.com/ludocode/mpack/blob/master/src/mpack/mpack-expect.c
// msgpack-c: https://github.com/msgpack/msgpack-c/blob/master/src/unpack.c
//
#ifndef MSGPACK_H
#define MSGPACK_H

#define MSGPACK_POSITIVE                0x00    // .. 0x7F
#define MSGPACK_FIX_MAP                 0x80    // .. 0x8F
#define MSGPACK_FIX_ARRAY               0x90    // .. 0x9F
#define MSGPACK_FIX_STRING              0xA0    // .. 0xBF

#define MSGPACK_NIL                     0xC0
#define MSGPACK_NEVER_USED              0xC1
#define MSGPACK_FALSE                   0xC2
#define MSGPACK_TRUE                    0xC3

#define MSGPACK_BIN8                    0xC4
#define MSGPACK_BIN16                   0xC5
#define MSGPACK_BIN32                   0xC6

#define MSGPACK_EXT8                    0xC7
#define MSGPACK_EXT16                   0xC8
#define MSGPACK_EXT32                   0xC9

#define MSGPACK_FLOAT                   0xCA
#define MSGPACK_DOUBLE                  0xCB

#define MSGPACK_UINT8                   0xCC
#define MSGPACK_UINT16                  0xCD
#define MSGPACK_UINT32                  0xCE
#define MSGPACK_UINT64                  0xCF

#define MSGPACK_SINT8                   0xD0
#define MSGPACK_SINT16                  0xD1
#define MSGPACK_SINT32                  0xD2
#define MSGPACK_SINT64                  0xD3

#define MSGPACK_FIX_EXT1                0xD4
#define MSGPACK_FIX_EXT2                0xD5
#define MSGPACK_FIX_EXT4                0xD6
#define MSGPACK_FIX_EXT8                0xD7
#define MSGPACK_FIX_EXT16               0xD8

#define MSGPACK_STR8                    0xD9
#define MSGPACK_STR16                   0xDA
#define MSGPACK_STR32                   0xDB

#define MSGPACK_ARRAY16                 0xDC
#define MSGPACK_ARRAY32                 0xDD
#define MSGPACK_MAP16                   0xDE
#define MSGPACK_MAP32                   0xDF

#define MSGPACK_NEGATIVE                0xE0    // .. 0xFF


struct MSGPACK_OBJECT
{
    unsigned char *s;
    unsigned char *e;
};

int MsgpackIsNil(struct MSGPACK_OBJECT *s);
int MsgpackIsTrue(struct MSGPACK_OBJECT *s);
int MsgpackIsFalse(struct MSGPACK_OBJECT *s);
int MsgpackIsSigned(struct MSGPACK_OBJECT *s);
int MsgpackIsUnsigned(struct MSGPACK_OBJECT *s);
int MsgpackIsFloat(struct MSGPACK_OBJECT *s);
int MsgpackIsDouble(struct MSGPACK_OBJECT *s);
int MsgpackIsBinary(struct MSGPACK_OBJECT *s);
int MsgpackIsString(struct MSGPACK_OBJECT *s);
int MsgpackIsArray(struct MSGPACK_OBJECT *s);
int MsgpackIsMap(struct MSGPACK_OBJECT *s);
int MsgpackIsExt(struct MSGPACK_OBJECT *s);

long long MsgpackGetSigned(struct MSGPACK_OBJECT *s, long long ev);
unsigned long long MsgpackGetUnsigned(struct MSGPACK_OBJECT *s, unsigned long long ev);
float MsgpackGetFloat(struct MSGPACK_OBJECT *s, float ev);
double MsgpackGetDouble(struct MSGPACK_OBJECT *s, double ev);
char *MsgpackGetStringPointer(struct MSGPACK_OBJECT *s);
unsigned int MsgpackGetStringLength(struct MSGPACK_OBJECT *s);
unsigned int MsgpackGetArrayLength(struct MSGPACK_OBJECT *s);
unsigned int MsgpackGetMapLength(struct MSGPACK_OBJECT *s);
int MsgpackGetExt(struct MSGPACK_OBJECT *s, char *type, unsigned int *len, unsigned char **data);

#endif // MSGPACK_H
