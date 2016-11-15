//
// MsgPack implementation (v3) in pure C 
//
// By Xiong.J, 2014-2016
//
#ifndef MSGPACK_READER_H
#define MSGPACK_READER_H

#include "stream/istream.h"

#ifdef __cplusplus
extern "C" 
{
#endif

int MsgpackObjectGetOne(struct IRSTREAM *s, struct MSGPACK_OBJECT *o);

int MsgpackObjectGetAll(struct IRSTREAM *s, struct MSGPACK_OBJECT *ov, int ovsize, int *total_size);

#ifdef __cplusplus
};
#endif

#endif // MSGPACK_H
