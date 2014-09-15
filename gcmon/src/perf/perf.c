/*!**************************************************************
 *@file perf.cpp
 *@brief    解析Hotspot PerfMemory
 *@author   zhaohm3
 *@date 2014-9-11 15:03
 *@note
 * 
 ****************************************************************/

#include "perf/perf.h"

//! 检测Hotspot的PerfDataEntry的内容变化
#define PERFDATA_MAJOR_VERSION 2
#define PERFDATA_MINOR_VERSION 0

//! 字节序存储定义
#define PERFDATA_BIG_ENDIAN     0
#define PERFDATA_LITTLE_ENDIAN  1

//! Hotspot原生PerfDataPrologue结构定义
typedef struct tagPerfDataPrologue{
    jint   magic;                       //!< magic number - 0xcafec0c0
    jbyte  byte_order;                  //!< byte order of the buffer
    jbyte  major_version;               //!< major version number
    jbyte  minor_version;               //!< minor version number
    jbyte  accessible;                  //!< ready to access
    jint   used;                        //!< number of PerfData memory bytes used
    jint   overflow;                    //!< number of bytes of overflow
    jlong  mod_time_stamp;              //!< time stamp of last structural modification
    jint   entry_offset;                //!< offset of the first PerfDataEntry
    jint   num_entries;                 //!< number of allocated PerfData entries
} PerfDataPrologue;

//! Hotspot原生PerfDataEntry结构定义
typedef struct tagPerfDataEntry {
    jint entry_length;                  //!< entry length in bytes
    jint name_offset;                   //!< offset of the data item name
    jint vector_length;                 //!< length of the vector. If 0, then scalar
    jbyte data_type;                    //!< type of the data item -
                                        //!< 'B','Z','J','I','S','C','D','F','V','L','['
    jbyte flags;                        //!< flags indicating misc attributes
    jbyte data_units;                   //!< unit of measure for the data type
    jbyte data_variability;             //!< variability classification of data type
    jint  data_offset;                  //!< offset of the data item
} PerfDataEntry;

//! 与vm/agent/sun/jvm/hotspot/runtime/BasicType.java中的定义保持同步
typedef enum tagBasicType {
    T_BOOLEAN = 4,
    T_CHAR = 5,
    T_FLOAT = 6,
    T_DOUBLE = 7,
    T_BYTE = 8,
    T_SHORT = 9,
    T_INT = 10,
    T_LONG = 11,
    T_OBJECT = 12,
    T_ARRAY = 13,
    T_VOID = 14,
    T_ADDRESS = 15,
    T_NARROWOOP = 16,
    T_CONFLICT = 17,                    //!< for stack value type with conflicting contents
    T_ILLEGAL = 99
} BasicType;

//! 与Java类com.sun.hotspot.perfdata.Variability保持同步，用于给PerfDataEntry.data_variability赋值
typedef enum tagVariability {
    V_Constant = 1,
    V_Monotonic = 2,
    V_Variable = 3,
    V_last = V_Variable
} Variability;

//! 与Java类com.sun.hotspot.perfdata.Units保持同步，用于给PerfDataEntry.data_units赋值
typedef enum tagUnits {
    U_None = 1,
    U_Bytes = 2,
    U_Ticks = 3,
    U_Events = 4,
    U_String = 5,
    U_Hertz = 6,
    U_Last = U_Hertz
} Units;

//! Miscellaneous flags，用于给PerfDataEntry.flags赋值
typedef enum tagFlags {
    F_None = 0x0,
    F_Supported = 0x1
} Flags;

//! BasicType到签名的映射，用于给PerfDataEntry.data_type赋值
Char_t gTtype2char[T_CONFLICT + 1] = { 0, 0, 0, 0, 'Z', 'C', 'F', 'D', 'B', 'S', 'I', 'J', 'L', '[', 'V', 0, 0, 0 };

//! BasicType到Java基本类型名的映射
const CharP_t gTtype2name[T_CONFLICT + 1] = {
    NULL, NULL, NULL, NULL,
    "boolean",
    "char",
    "float",
    "double",
    "byte",
    "short",
    "int",
    "long",
    "object",
    "array",
    "void",
    "*address*",
    "*narrowoop*",
    "*conflict*"
};

GPrivate Char_t type2char(BasicType_t t)
{
    return (Uint32_t)t < T_CONFLICT + 1 ? gTtype2char[t] : 0;
}

GPrivate String_t type2name(BasicType_t t)
{
    return (Uint32_t)t < T_CONFLICT + 1 ? gTtype2name[t] : NULL;
}

GPrivate BasicType_t name2type(String_t szName)
{
    Int32_t i = 0;

    for (i = T_BOOLEAN; i <= T_VOID; i++)
    {
        BasicType_t t = (BasicType_t)i;

        if (gTtype2name[i] != NULL && 0 == strcmp(gTtype2name[i], szName))
        {
            return t;
        }
    }

    return T_ILLEGAL;
}

GPrivate BasicType_t char2type(char c) {
    switch (c) {
    case 'B': return T_BYTE;
    case 'C': return T_CHAR;
    case 'D': return T_DOUBLE;
    case 'F': return T_FLOAT;
    case 'I': return T_INT;
    case 'J': return T_LONG;
    case 'S': return T_SHORT;
    case 'Z': return T_BOOLEAN;
    case 'V': return T_VOID;
    case 'L': return T_OBJECT;
    case '[': return T_ARRAY;
    }
    return T_ILLEGAL;
}

GPrivate void perf_build_tree(void *address)
{

}

GPrivate void perf_get_require()
{

}

GPrivate void perf_analyze_tree()
{

}

GPrivate Int32_t gCounter = 0;

GPublic void perf_memory_analyze(void *address)
{
    PerfDataPrologueP_t pPerf = (PerfDataPrologueP_t)address;

    if (pPerf != NULL)
    {
        Int32_t i = 0;
        CharP_t pCurr = (CharP_t)(((CharP_t)(address)) + pPerf->entry_offset);

        GASSERT(pPerf->entry_offset == sizeof(PerfDataPrologue_t));

        for (i = 0; i < pPerf->num_entries; i++)
        {
            PerfDataEntryP_t pEntry = (PerfDataEntryP_t)pCurr;
            String_t szEntryName = (String_t)(pCurr + pEntry->name_offset);
            CharP_t pData = (CharP_t)(pCurr + pEntry->data_offset);

            if (10 == gCounter)
            {
                switch (pEntry->data_type)
                {
                case 'B':
                    gcmon_debug_msg("%s --> %s --> %s \n", szEntryName,
                        type2name(char2type(pEntry->data_type)),
                        (String_t)pData);
                    break;
                case 'J':
                    gcmon_debug_msg("%s --> %s --> %lld \n", szEntryName,
                        type2name(char2type(pEntry->data_type)),
                        *(jlong*)pData);
                    break;
                default:
                    break;
                }

            }

            pCurr += pEntry->entry_length;
        }

        gCounter += 1;

        if (gCounter > 100)
        {
            gCounter = 100;
        }

        gcmon_debug_flush();
    }
}
