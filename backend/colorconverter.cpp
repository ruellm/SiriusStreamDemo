#include<math.h>
#include <memory.h>
#include "colorconverter.h"

namespace colorconverter {
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024];   //for clip in CCIR601

void init_yuv420p_table()
{
    long int crv,cbu,cgu,cgv;
    int i,ind;

    crv = 104597; cbu = 132201;  /* fra matrise i global.h */
    cgu = 25675;  cgv = 53279;

    for (i = 0; i < 256; i++)
    {
        crv_tab[i] = (i-128) * crv;
        cbu_tab[i] = (i-128) * cbu;
        cgu_tab[i] = (i-128) * cgu;
        cgv_tab[i] = (i-128) * cgv;
        tab_76309[i] = 76309*(i-16);
    }

    for (i = 0; i < 384; i++)
        clp[i] = 0;
    ind = 384;
    for (i = 0;i < 256; i++)
        clp[ind++] = i;
    ind = 640;
    for (i = 0;i < 384; i++)
        clp[ind++] = 255;
}
void Yuv420ProgPlanarToRgb (uint8_t* yuvbuffer, uint8_t* rgbbuffer, const int& width, const int& height) {
    int y1, y2, u, v;
        unsigned char *py1, *py2;
        int i, j, c1, c2, c3, c4;
        unsigned char *d1, *d2;
        unsigned char *src_u, *src_v;
        static int init_yuv420p = 0;

        src_u = yuvbuffer + width * height;   // u
        src_v = src_u + width * height / 4;  // v

        py1 = yuvbuffer;   // y
        py2 = py1 + width;
        d1 = rgbbuffer;
        d2 = d1 + 3 * width;

        if (init_yuv420p == 0)
        {
            init_yuv420p_table();
            init_yuv420p = 1;
        }

        for (j = 0; j < height; j += 2)
        {
            for (i = 0; i < width; i += 2)
            {
                u = *src_u++;
                v = *src_v++;

                c1 = crv_tab[v];
                c2 = cgu_tab[u];
                c3 = cgv_tab[v];
                c4 = cbu_tab[u];

                //up-left
                y1 = tab_76309[*py1++];
                *d1++ = clp[384+((y1 + c1)>>16)];
                *d1++ = clp[384+((y1 - c2 - c3)>>16)];
                *d1++ = clp[384+((y1 + c4)>>16)];

                //down-left
                y2 = tab_76309[*py2++];
                *d2++ = clp[384+((y2 + c1)>>16)];
                *d2++ = clp[384+((y2 - c2 - c3)>>16)];
                *d2++ = clp[384+((y2 + c4)>>16)];

                //up-right
                y1 = tab_76309[*py1++];
                *d1++ = clp[384+((y1 + c1)>>16)];
                *d1++ = clp[384+((y1 - c2 - c3)>>16)];
                *d1++ = clp[384+((y1 + c4)>>16)];

                //down-right
                y2 = tab_76309[*py2++];
                *d2++ = clp[384+((y2 + c1)>>16)];
                *d2++ = clp[384+((y2 - c2 - c3)>>16)];
                *d2++ = clp[384+((y2 + c4)>>16)];
            }
            d1  += 3*width;
            d2  += 3*width;
            py1 += width;
            py2 += width;
        }
    }
}
