
#ifndef HARDWARE_H
#define HARDWARE_H

#include <limits.h>
#include <float.h>
// =============================================================================

/*******************************************************************************
 * Hardware acceleration                                                       *
 ******************************************************************************/

// This "patch" is required since the type m128 is not always recognized
// in all environments.


// =============================================================================


#ifdef __GNUC__
    #define MSVC_ALIGN_PREFIX
    #define GCC_ALIGN_SUFFIX  __attribute__ ((aligned (16)))
    #define INLINE_PREFIX
#else
    #define MSVC_ALIGN_PREFIX __declspec(align(16))
    #define GCC_ALIGN_SUFFIX
    #define INLINE_PREFIX
#endif

// In some environment, the constant PI is not define with the math.h library
#ifndef M_PI
        #define M_PI	3.1415926535897932384626433832795f
#endif

//Define infinity if not defined
#ifndef INFINITY
        #define INFINITY FLT_MAX
#endif

#endif
