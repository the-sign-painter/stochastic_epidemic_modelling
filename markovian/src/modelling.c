#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>


#include "modelling.h"


typedef struct
{
    uint8_t susceptibles;
    uint8_t infectives;
    uint8_t removed;
} modelling_markovian_frame_t;


static uint16_t _modelling_generate_random_integer(uint16_t start, uint16_t end)
{
    uint16_t rand_int = rand() % (end + 1 - start) + start;
    return rand_int;
}


static void _modelling_generate_random_mpf(mpf_t rand_float)
{
    uint64_t step = 10000;
    uint16_t rand_int = _modelling_generate_random_integer(0, step);
    mpf_set_ui(rand_float, step);
    mpf_ui_div(rand_float, 1, rand_float);
    mpf_mul_ui(rand_float, rand_float, rand_int);
}


static modelling_markovian_frame_t _modelling_markovian_SIR_timestep(modelling_markovian_frame_t frame, mpf_t infection_rate, mpf_t recovery_rate, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection, mpf_t rand_float)
{
    mpf_mul_ui(avg_infected, infection_rate, frame.susceptibles);
    mpf_mul_ui(avg_recovered, recovery_rate, frame.infectives);
    mpf_add(prob_infection, avg_infected, avg_recovered);
    mpf_ui_div(prob_infection, 1, prob_infection);
    mpf_mul(prob_infection, prob_infection, avg_infected);
    _modelling_generate_random_mpf(rand_float);
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


modelling_markovian_frame_t _modelling_markovian_SIS_timestep(modelling_markovian_frame_t frame, mpf_t infection_rate, mpf_t recovery_rate, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection, mpf_t rand_float)
{
    mpf_mul_ui(avg_infected, infection_rate, frame.susceptibles);
    mpf_mul_ui(avg_recovered, recovery_rate, frame.infectives);
    mpf_add(prob_infection, avg_infected, avg_recovered);
    mpf_ui_div(prob_infection, 1, prob_infection);
    mpf_mul(prob_infection, prob_infection, avg_infected);
    _modelling_generate_random_mpf(rand_float);
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


static timestep_t _modelling_simulate_markovian(mpf_t infection_rate, mpf_t recovery_rate, uint8_t initial_susceptibles, uint8_t initial_infectives, mpf_t rand_float)
{
    modelling_markovian_frame_t frame;
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
        frame = _modelling_markovian_SIR_timestep(frame, infection_rate, recovery_rate, avg_infected, avg_recovered, prob_infection, rand_float);
        timestep++;
    }

    mpf_clear(avg_infected);
    mpf_clear(avg_recovered);
    mpf_clear(prob_infection);

    return timestep;
}


void modelling_simulate(context_t* context)
{
    mpf_t infection_rate;
    mpf_init(infection_rate);
    mpf_set_d(infection_rate, context->infection_rate);
    mpf_t recovery_rate;
    mpf_init(recovery_rate);
    mpf_set_d(recovery_rate, context->recovery_rate);

    for (int i = 0; i < context->bins.size; i++)
    {
        context->bins.array[i] = 0;
    }
    mpf_t rand_float;
    mpf_init(rand_float);

    for (uint64_t i = 0; i < context->iterations; i++)
    {
        timestep_t age = _modelling_simulate_markovian(infection_rate, recovery_rate, context->initial_susceptibles, context->initial_infectives, rand_float);
        if (context->bins.size > age)
        {
            context->bins.array[age] += 1;
        }
    }
    
    mpf_clear(rand_float);
    mpf_clear(infection_rate);
    mpf_clear(recovery_rate);
}
