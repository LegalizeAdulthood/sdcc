/*
 */
#ifndef SDCCCONF_HEADER
#define SDCCCONF_HEADER


#define SDCC_VERSION_HI 2
#define SDCC_VERSION_LO 2
#define SDCC_VERSION_P 1
#define SDCC_VERSION_STR "2.2.2"

/* MSVC 6 does not have __FUNCTION__ preprocessor macro defined */

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#define __FUNCTION__ __FILE__
#endif

#undef PREFIX
#define PREFIX                ""  // this should be probably set to
                                  // something else, but currently I
                                  // don't have idea to what.
                                  // I defined it as empty string,
                                  // so that SDCC at least compiles
                                  // on VC. [borutr 07.aug.2002]

#undef DATADIR
#undef SRCDIR

//#define STANDARD_INCLUDE_DIR  "\\sdcc\\local\\share\\include"
#define SDCC_INCLUDE_DIR      "\\sdcc\\include"
#define SDCC_LIB_DIR          "\\sdcc\\lib"
#define STD_LIB               "libsdcc"
#define STD_INT_LIB           "libint"
#define STD_LONG_LIB          "liblong"
#define STD_FP_LIB            "libfloat"
#define STD_DS390_LIB         "libds390"

// #undef HAVE_SYS_SOCKET_H
// #undef HAVE_SYS_ISA_DEFS_H	
// #undef HAVE_ENDIAN_H

#undef HAVE_STRERROR

#undef OPT_DISABLE_Z80
#undef OPT_DISABLE_GBZ80
#undef OPT_DISABLE_MCS51
#undef OPT_DISABLE_AVR

#endif

/* End of config.h */
