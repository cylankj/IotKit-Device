//
// MsgPack implementation (v3) in pure C 
//
// By Xiong.J, 2014-2016
//
#ifndef MSGPACK_WRITER_H
#define MSGPACK_WRITER_H

#include "stream/istream.h"

#ifdef __cplusplus
extern "C" 
{
#endif

void MsgpackPackNil(struct IWSTREAM *s);
void MsgpackPackBoolean(struct IWSTREAM *s, int val);
void MsgpackPackPositive(struct IWSTREAM *s, unsigned char v);
void MsgpackPackNegative(struct IWSTREAM *s, char v);
void MsgpackPackSINT8(struct IWSTREAM *s, char v);
void MsgpackPackSINT16(struct IWSTREAM *s, short v);
void MsgpackPackSINT32(struct IWSTREAM *s, int v);
void MsgpackPackSINT64(struct IWSTREAM *s, long long v);
void MsgpackPackSigned(struct IWSTREAM *s, long long v);
void MsgpackPackUINT8(struct IWSTREAM *s, unsigned char v);
void MsgpackPackUINT16(struct IWSTREAM *s, unsigned short v);
void MsgpackPackUINT32(struct IWSTREAM *s, unsigned int v);
void MsgpackPackUINT64(struct IWSTREAM *s, unsigned long long v);
void MsgpackPackUnsigned(struct IWSTREAM *s, unsigned long long v);
void MsgpackPackFloat(struct IWSTREAM *s, float f);
void MsgpackPackDouble(struct IWSTREAM *s, double d);
void MsgpackPackString(struct IWSTREAM *s, const char *str, int len);
void MsgpackPackArray(struct IWSTREAM *s, int len);
void MsgpackPackMap(struct IWSTREAM *s, int len);
void MsgpackPackExt(struct IWSTREAM *s, char type, const void *data, unsigned int len);

#ifdef __cplusplus
};
#endif

#endif // MSGPACK_WRITER_H
