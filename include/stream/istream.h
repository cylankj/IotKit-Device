#ifndef __ISTREAM_H__
#define __ISTREAM_H__

struct IRSTREAM
{
    unsigned char *(*rptr)(struct IRSTREAM *);
    int (*rpop)(struct IRSTREAM *, int n);
    int (*read)(struct IRSTREAM *, void *p, int n);
    int (*reof)(struct IRSTREAM *);
    int (*rcapacity)(struct IRSTREAM *);
    void (*underflow)(struct IRSTREAM *);
};

#define rstream_ptr(_s)             (_s)->rptr(_s)
#define rstream_pop(_s, _n)         (_s)->rpop((_s), (_n))
#define rstream_read(_s, _p, _n)    (_s)->read((_s), (_p), (_n))
#define rstream_eof(_s)             (_s)->reof(_s)
#define rstream_capacity(_s)        (_s)->rcapacity(_s)

struct IWSTREAM
{
    unsigned char *(*wptr)(struct IWSTREAM *);
    int (*wpush)(struct IWSTREAM *, int n);
    int (*write)(struct IWSTREAM *, const void *p, int n);
    int (*weof)(struct IWSTREAM *);
    int (*wcapacity)(struct IWSTREAM *);
    void (*overflow)(struct IWSTREAM *);
};

#define wstream_ptr(_s)             (_s)->wptr(_s)
#define wstream_push(_s, _n)        (_s)->wpush((_s), (_n))
#define wstream_write(_s, _p, _n)   (_s)->write((_s), (_p), (_n))
#define wstream_eof(_s)             (_s)->weof(_s)
#define wstream_capacity(_s)        (_s)->wcapacity(_s)

#endif // #define __ISTREAM_H__
