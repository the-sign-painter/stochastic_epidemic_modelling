#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>


#include "modelling.h"


static uint16_t generate_random_integer(uint16_t start, uint16_t end)
{
    uint16_t rand_int = rand() % (end + 1 - start) + start;
    return rand_int;
}


static void generate_random_mpf(mpf_t rand_float)
{
    uint64_t step = 10000;
    uint16_t rand_int = generate_random_integer(0, step);
    mpf_set_ui(rand_float, step);
    mpf_ui_div(rand_float, 1, rand_float);
    mpf_mul_ui(rand_float, rand_float, rand_int);
}


static markovian_frame_t markovian_SIR_timestep(markovian_frame_t frame, mpf_t infection_rate, mpf_t recovery_rate, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection, mpf_t rand_float)
{
    mpf_mul_ui(avg_infected, infection_rate, frame.susceptibles);
    mpf_mul_ui(avg_recovered, recovery_rate, frame.infectives);
    mpf_add(prob_infection, avg_infected, avg_recovered);
    mpf_ui_div(prob_infection, 1, prob_infection);
    mpf_mul(prob_infection, prob_infection, avg_infected);
    generate_random_mpf(rand_float);
    if (mpf_cmp(rand_float, prob_infection) < 0)
    {
        frame.susceptibles--;
        frame.infectives++;
    }
    else
    {
        frame.infectives--;
        frame.removed++;
    }
    return frame;
}


markovian_frame_t markovian_SIS_timestep(markovian_frame_t frame, mpf_t infection_rate, mpf_t recovery_rate, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection, mpf_t rand_float)
{
    mpf_mul_ui(avg_infected, infection_rate, frame.susceptibles);
    mpf_mul_ui(avg_recovered, recovery_rate, frame.infectives);
    mpf_add(prob_infection, avg_infected, avg_recovered);
    mpf_ui_div(prob_infection, 1, prob_infection);
    mpf_mul(prob_infection, prob_infection, avg_infected);
    generate_random_mpf(rand_float);
    if (mpf_cmp(rand_float, prob_infection) < 0)
    {
        frame.susceptibles--;
        frame.infectives++;
    }
    else
    {
        frame.infectives--;
        frame.susceptibles++;
    }
    return frame;
}


static timestep_t simulate_markovian(mpf_t infection_rate, mpf_t recovery_rate, uint8_t initial_susceptibles, uint8_t initial_infectives, mpf_t rand_float)
{
    markovian_frame_t frame;
    frame.susceptibles = initial_susceptibles;
    frame.infectives = initial_infectives;
    frame.removed = 0;
    timestep_t timestep = 0;

    mpf_t avg_infected;
    mpf_init(avg_infected);

    mpf_t avg_recovered;
    mpf_init(avg_recovered);

    mpf_t prob_infection;
    mpf_init(prob_infection);

    while (frame.infectives > 0)
    {
        frame = markovian_SIR_timestep(frame, infection_rate, recovery_rate, avg_infected, avg_recovered, prob_infection, rand_float);
        timestep++;
    }

    mpf_clear(avg_infected);
    mpf_clear(avg_recovered);
    mpf_clear(prob_infection);

    return timestep;
}


void simulate(bin_array_t bin_array, uint64_t iterations, double infection_rate_d, double recovery_rate_d, uint8_t initial_susceptibles, uint8_t initial_infectives)
{
    mpf_t infection_rate;
    mpf_init(infection_rate);
    mpf_set_d(infection_rate, infection_rate_d);
    mpf_t recovery_rate;
    mpf_init(recovery_rate);
    mpf_set_d(recovery_rate, recovery_rate_d);

    for (int i = 0; i < bin_array.size; i++)
    {
        bin_array.array[i] = 0;
    }
    mpf_t rand_float;
    mpf_init(rand_float);

    for (uint64_t i = 0; i < iterations; i++)
    {
        timestep_t age = simulate_markovian(infection_rate, recovery_rate, initial_susceptibles, initial_infectives, rand_float);
        if (bin_array.size > age)
        {
            bin_array.array[age] += 1;
        }
    }
    
    mpf_clear(rand_float);
    mpf_clear(infection_rate);
    mpf_clear(recovery_rate);
}
