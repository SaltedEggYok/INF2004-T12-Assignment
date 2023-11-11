#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

// Generally you would define your own explicit list of lwIP options
// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html)
//
// This example uses a common include to avoid repetition
#include "lwipopts_examples_common.h"

// allow override in some examples
#ifndef NO_SYS
#define NO_SYS                      0
#endif
// allow override in some examples
#ifndef LWIP_SOCKET
#define LWIP_SOCKET                 1
#endif

#endif
