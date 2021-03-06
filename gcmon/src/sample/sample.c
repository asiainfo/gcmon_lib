/*!**************************************************************
 *@file sample.c
 *@brief    对PerfMemory的指定项进行数据采样的相关接口实现
 *@author   zhaohm3
 *@date 2014-9-20 20:12
 *@note
 * 
 ****************************************************************/

#include "sample/sample.h"
#include "perf/perf.h"


//! 性能采样项结构体声明
typedef struct SampleItem SampleItem_t, *SampleItemP_t;
struct SampleItem
{
    String_t szHeader;                  //!< 采样项的说民，用于输出，同jstat的输出头
    String_t szReference;               //!< 采样项所引用的性能技术项的名称
    PerfDataItemP_t pItem;              //!< 采样项所对应的pdi项，指向性能红黑树中的对应项
};

GPrivate SampleItem_t gTicks = { "Timestamp", "sun.os.hrt.ticks", NULL };
GPrivate SampleItem_t gFrequency = { "Timestamp", "sun.os.hrt.frequency", NULL };

GPrivate SampleItem_t gS0C = { "S0C", "sun.gc.generation.0.space.1.capacity", NULL };
GPrivate SampleItem_t gS1C = { "S1C", "sun.gc.generation.0.space.2.capacity", NULL };

GPrivate SampleItem_t gS0U = { "S0U", "sun.gc.generation.0.space.1.used", NULL };
GPrivate SampleItem_t gS1U = { "S1U", "sun.gc.generation.0.space.2.used", NULL };

GPrivate SampleItem_t gEC = { "EC", "sun.gc.generation.0.space.0.capacity", NULL };
GPrivate SampleItem_t gEU = { "EU", "sun.gc.generation.0.space.0.used", NULL };

GPrivate SampleItem_t gOC = { "OC", "sun.gc.generation.1.space.0.capacity", NULL };
GPrivate SampleItem_t gOU = { "OU", "sun.gc.generation.1.space.0.used", NULL };

GPrivate SampleItem_t gPC = { "PC", "sun.gc.generation.2.space.0.capacity", NULL };
GPrivate SampleItem_t gPU = { "PU", "sun.gc.generation.2.space.0.used", NULL };

GPrivate SampleItem_t gNGCMN = { "NGCMN", "sun.gc.generation.0.minCapacity", NULL };
GPrivate SampleItem_t gNGCMX = { "NGCMX", "sun.gc.generation.0.maxCapacity", NULL };
GPrivate SampleItem_t gNGC = { "NGC", "sun.gc.generation.0.capacity", NULL };

GPrivate SampleItem_t gOGCMN = { "OGCMN", "sun.gc.generation.1.minCapacity", NULL };
GPrivate SampleItem_t gOGCMX = { "OGCMX", "sun.gc.generation.1.maxCapacity", NULL };
GPrivate SampleItem_t gOGC = { "OGC", "sun.gc.generation.1.capacity", NULL };

GPrivate SampleItem_t gPGCMN = { "PGCMN", "sun.gc.generation.2.minCapacity", NULL };
GPrivate SampleItem_t gPGCMX = { "PGCMX", "sun.gc.generation.2.maxCapacity", NULL };
GPrivate SampleItem_t gPGC = { "PGC", "sun.gc.generation.2.capacity", NULL };

GPrivate SampleItem_t gYGC = { "YGC", "sun.gc.collector.0.invocations", NULL };
GPrivate SampleItem_t gFGC = { "FGC", "sun.gc.collector.1.invocations", NULL };

GPrivate SampleItem_t gYGCT = { "YGCT", "sun.gc.collector.0.time", NULL };
GPrivate SampleItem_t gFGCT = { "FGCT", "sun.gc.collector.1.time", NULL };

GPrivate Double_t gdfPrevYGCT = 0.000;
GPrivate Double_t gdfPrevFGCT = 0.000;

#define S_ITEM(s) s.pItem = pdi_search_item(pTree, s.szReference)
GPublic void sample_init(RBTreeP_t pTree)
{
    S_ITEM(gTicks);
    S_ITEM(gFrequency);
    S_ITEM(gS0C);
    S_ITEM(gS1C);
    S_ITEM(gS0U);
    S_ITEM(gS1U);
    S_ITEM(gEC);
    S_ITEM(gEU);
    S_ITEM(gOC);
    S_ITEM(gOU);
    S_ITEM(gPC);
    S_ITEM(gPU);
    S_ITEM(gNGCMN);
    S_ITEM(gNGCMX);
    S_ITEM(gNGC);
    S_ITEM(gOGCMN);
    S_ITEM(gOGCMX);
    S_ITEM(gOGC);
    S_ITEM(gPGCMN);
    S_ITEM(gPGCMX);
    S_ITEM(gPGC);
    S_ITEM(gYGC);
    S_ITEM(gFGC);
    S_ITEM(gYGCT);
    S_ITEM(gFGCT);
}

#define S_JLONG(s) pdi_get_jlong(s.pItem)
#define S_DOUBLE(s) pdi_get_double(s.pItem)

GPublic Double_t s_ticks()
{
    return S_DOUBLE(gTicks);
}

GPublic Double_t s_frequency()
{
    return S_DOUBLE(gFrequency);
}

GPublic Double_t s_timstamp()
{
    return (Double_t)(s_ticks() / s_frequency());
}

GPublic Double_t s_s0c()
{
    return S_DOUBLE(gS0C) / 1024.000;
}

GPublic Double_t s_s1c()
{
    return S_DOUBLE(gS1C) / 1024.000;
}

GPublic Double_t s_s0u()
{
    return S_DOUBLE(gS0U) / 1024.000;
}

GPublic Double_t s_s1u()
{
    return S_DOUBLE(gS1U) / 1024.000;
}

GPublic Double_t s_ec()
{
    return S_DOUBLE(gEC) / 1024.000;
}

GPublic Double_t s_eu()
{
    return S_DOUBLE(gEU) / 1024.000;
}

GPublic Double_t s_oc()
{
    return S_DOUBLE(gOC) / 1024.000;
}

GPublic Double_t s_ou()
{
    return S_DOUBLE(gOU) / 1024.000;
}

GPublic Double_t s_pc()
{
    return S_DOUBLE(gPC) / 1024.000;
}

GPublic Double_t s_pu()
{
    return S_DOUBLE(gPU) / 1024.000;
}

GPublic Double_t s_s0f()
{
    return s_s0c() - s_s0u();
}

GPublic Double_t s_s1f()
{
    return s_s1c() - s_s1u();
}

GPublic Double_t s_ef()
{
    return s_ec() - s_eu();
}

GPublic Double_t s_of()
{
    return s_oc() - s_ou();
}

GPublic Double_t s_pf()
{
    return s_pc() - s_pu();
}

GPublic Double_t s_s0p()
{
    return (Double_t)((1.000 - ((s_s0c() - s_s0u()) / s_s0c())) * 100.000);
}

GPublic Double_t s_s1p()
{
    return (Double_t)((1.000 - ((s_s1c() - s_s1u()) / s_s1c())) * 100.000);
}

GPublic Double_t s_ep()
{
    return (Double_t)((1.000 - ((s_ec() - s_eu()) / s_ec())) * 100.000);
}

GPublic Double_t s_op()
{
    return (Double_t)((1.000 - ((s_oc() - s_ou()) / s_oc())) * 100.000);
}

GPublic Double_t s_pp()
{
    return (Double_t)((1.000 - ((s_pc() - s_pu()) / s_pc())) * 100.000);
}

GPublic jlong s_ygc()
{
    return S_JLONG(gYGC);
}

GPublic jlong s_fgc()
{
    return S_JLONG(gFGC);
}

GPublic Double_t s_ygct()
{
    return (Double_t)(S_DOUBLE(gYGCT) / s_frequency());
}

GPublic Double_t s_aygct()
{
    return (s_ygc() > 0) ? s_ygct() / S_DOUBLE(gYGC) : 0.000;
}

GPrivate Double_t s_cygct()
{
    Double_t dfCurrYGCT = s_ygct();
    Double_t dfRet = dfCurrYGCT - gdfPrevYGCT;
    gdfPrevYGCT = dfCurrYGCT;
    return dfRet;
}

GPublic Double_t s_fgct()
{
    return (Double_t)(S_DOUBLE(gFGCT) / s_frequency());
}

GPublic Double_t s_afgct()
{
    return (s_fgc() > 0) ? s_fgct() / S_DOUBLE(gFGC) : 0.000;
}

GPrivate Double_t s_cfgct()
{
    Double_t dfCurrFGCT = s_fgct();
    Double_t dfRet = dfCurrFGCT - gdfPrevFGCT;
    gdfPrevFGCT = dfCurrFGCT;
    return dfRet;
}

GPublic Double_t s_gct()
{
    return (Double_t)((S_DOUBLE(gYGCT) + S_DOUBLE(gFGCT)) / s_frequency());
}

GPublic Double_t s_ngcmn()
{
    return S_DOUBLE(gNGCMN) / 1024.000;
}

GPublic Double_t s_ngcmx()
{
    return S_DOUBLE(gNGCMX) / 1024.000;
}

GPublic Double_t s_ngc()
{
    return S_DOUBLE(gNGC) / 1024.000;
}

GPublic Double_t s_ogcmn()
{
    return S_DOUBLE(gOGCMN) / 1024.000;
}

GPublic Double_t s_ogcmx()
{
    return S_DOUBLE(gOGCMX) / 1024.000;
}

GPublic Double_t s_ogc()
{
    return S_DOUBLE(gOGC) / 1024.000;
}

GPublic Double_t s_pgcmn()
{
    return S_DOUBLE(gPGCMN) / 1024.000;
}

GPublic Double_t s_pgcmx()
{
    return S_DOUBLE(gPGCMX) / 1024.000;
}

GPublic Double_t s_pgc()
{
    return S_DOUBLE(gPGC) / 1024.000;
}

GPublic Double_t s_ygctp()
{
    return (s_gct() > 0) ? (Double_t)((1.000 - ((s_gct() - s_ygct()) / s_gct())) * 100.000) : 0.000;
}

GPublic Double_t s_fgctp()
{
    return (s_gct() > 0) ? (Double_t)((1.000 - ((s_gct() - s_fgct()) / s_gct())) * 100.000) : 0.000;
}

GPublic Double_t s_gctp()
{
    return (Double_t)((1.000 - ((s_timstamp() - s_gct()) / s_timstamp())) * 100.000);
}
