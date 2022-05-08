#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

#include "common.h"
#include "gui.h"


static context_t _context = {.bins={.size=100, .array={0}},
                             .iterations=1000,
                             .infection_rate=0.01,
                             .recovery_rate=0.1,
                             .initial_susceptibles=99,
                             .initial_infectives=1,
                             .initial_removed=0,
                            };


int main(int argc, char **argv)
{
    gui_init(&_context, &argc, &argv);
    return 0;
}
