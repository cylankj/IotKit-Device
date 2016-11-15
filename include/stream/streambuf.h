#ifndef __STREAMBUF_H__
#define __STREAMBUF_H__

#include "stream/istream.h"

struct STREAMBUF
{
    struct IRSTREAM rs;
    struct IWSTREAM ws;

    unsigned char *bptr;
    unsigned char *rptr;
    unsigned char *wptr;
    unsigned char *eptr;
    unsigned int hi_watermark;
    unsigned int lo_watermark;
};

struct IRSTREAM *streambuf_getrstream(struct STREAMBUF *sb);

struct IWSTREAM *streambuf_getwstream(struct STREAMBUF *sb);

void streambuf_set_high_watermark(struct STREAMBUF *sb, unsigned int v);

void streambuf_set_low_watermark(struct STREAMBUF *sb, unsigned int v);

unsigned int streambuf_get_high_watermark(struct STREAMBUF *sb);

unsigned int streambuf_get_low_watermark(struct STREAMBUF *sb);

void streambuf_clear(struct STREAMBUF *sb);

void streambuf_init(struct STREAMBUF *sb, void *p, int size, int len);

#endif
