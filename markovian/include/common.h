#pragma once

#include <stdint.h>

#include "modelling.h"


typedef struct
{
    bin_array_t bins;
    uint64_t iterations;
    uint16_t time_range;
    double infection_rate;
    double recovery_rate;
    uint16_t initial_susceptibles;
    uint16_t initial_infectives;
    uint16_t initial_removed;
} context_t;
