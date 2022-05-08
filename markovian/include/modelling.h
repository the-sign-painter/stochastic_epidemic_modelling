#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <gmp.h>


typedef uint16_t bin_t;
typedef uint16_t timestep_t;


typedef struct
{
    uint8_t susceptibles;
    uint8_t infectives;
    uint8_t removed;
} markovian_frame_t;


typedef struct
{
    uint8_t size;
    bin_t* array;
    mpz_t sum;
} bin_array_t;


void simulate(bin_array_t bin_array, uint64_t iterations, double infection_rate_d, double recovery_rate_d, uint8_t initial_susceptibles, uint8_t initial_infectives);
