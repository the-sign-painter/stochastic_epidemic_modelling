#pragma once

#include <stdint.h>


#define MAX_NUM_BINS        1000
#define DATA_DIR            "output"


typedef uint16_t bin_t;
typedef uint16_t timestep_t;


typedef struct
{
    uint16_t size;
    bin_t array[MAX_NUM_BINS];
} bin_array_t;


typedef struct
{
    bin_array_t bins;
    uint64_t iterations;
    double infection_rate;
    double recovery_rate;
    uint16_t initial_susceptibles;
    uint16_t initial_infectives;
    uint16_t initial_removed;
} context_t;
