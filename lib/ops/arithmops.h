#ifndef ARITHMOPS_H
#define ARITHMOPS_H

#include "lib/ops/ops.h"

class WImage;

typedef unsigned char uchar;
typedef unsigned int uint;

class ArithmOps: public Ops
{
public:
    ArithmOps();

    typedef uchar ArithmOption;

//    typedef enum arithmoption{
//        MATCH_FULLY = 0x00, // 必须保证两图像大小、通道数完全匹配
//        MATCH_SIZE_FULLY = 0x00, // 保证两图像大小完全匹配，不匹配抛出异常
//        MATCH_SIZE_RESIZE = 0x01, // 图像大小不匹配时，第二图像的大小按照第一张图像大小 Resize
//        MATCH_SIZE_USEMAX = 0x02, // 图像大小不匹配时，寻找可包含两张图的最小的图像
//        MATCH_SIZE_CENTER_ALIGNED = 0x03, // 图像大小不匹配时，进行中心对齐，寻找可包含两张图的最小的图像
//        MATCH_CHANNEL_FULLY = 0x00, // 保证两图像通道数完全匹配，否则抛出异常
//        MATCH_CHANNEL_REJUST = 0x10, // 通道数不匹配时，第二图像的通道数按第一图像通道数变化
//    }ArithmOption;

//    // 状态控制位，不能用于检测相等，而只能用于表示状态
//    static const ArithmOption MATCH_FULLY = 0x00; // 必须保证两图像大小、通道数完全匹配
//    // Size 由最低四位控制
//    static const ArithmOption MATCH_SIZE_FULLY = 0x00; // 保证两图像大小完全匹配，不匹配抛出异常
//    static const ArithmOption MATCH_SIZE_RESIZE = 0x01; // 图像大小不匹配时，第二图像的大小按照第一张图像大小 Resize
//    static const ArithmOption MATCH_SIZE_USEMAX = 0x02; // 图像大小不匹配时，寻找可包含两张图的最小的图像
//    static const ArithmOption MATCH_SIZE_CENTER_ALIGNED = 0x03; // 图像大小不匹配时，进行中心对齐，寻找可包含两张图的最小的图像
//    // Channel 由低5-8位控制
//    static const ArithmOption MATCH_CHANNEL_FULLY = 0x00; // 保证两图像通道数完全匹配，否则抛出异常
//    static const ArithmOption MATCH_CHANNEL_REJUST = 0x10; // 通道数不匹配时，第二图像的通道数按第一图像通道数变化

    // 和 flag 有关的计算
    static uint constructFlag(bool doCenterAligned = false, bool doScale = false,
                              bool useMode = false);
    static void destructFlag(uint flag, bool& doCenterAligned, bool& doScale,
                             bool& useMod);


    // 逐点计算的
    static bool calPerPixel(WImage& image, const WImage& image2, int(*fun)(int, int), uint flag);

    // arithmetic
    static bool add(WImage& , const WImage& , uint );
    static WImage add(const WImage&, const WImage& , uint );
    static bool minus(WImage& , const WImage&, uint );
    static WImage minus(const WImage&, const WImage&, uint );
    static bool mul(WImage& , const WImage&, uint );
    static WImage mul(const WImage&, const WImage&, uint );
    static bool div(WImage& , const WImage&, uint );
    static WImage div(const WImage&, const WImage&, uint );

    // logicstic
    static bool neg(WImage& );
    static WImage neg(const WImage& );
    static bool orI(WImage& , const WImage&, uint );
    static WImage orI(const WImage&, const WImage&, uint );
    static bool andI(WImage& , const WImage&, uint );
    static WImage andI(const WImage&, const WImage&, uint );

    /* 四则运算最需要注意的是，两张图的大小以及不要超过[0,255]范围 */

};

#endif // ARITHMOPS_H
