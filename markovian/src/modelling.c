#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>


#include "modelling.h"


typedef struct
{
    uint32_t susceptibles;
    uint32_t infectives;
    uint32_t removed;
} modelling_markovian_frame_t;


static uint16_t _modelling_generate_random_integer(uint16_t start, uint16_t end)
{
    uint16_t rand_int = rand() % (end + 1 - start) + start;
    return rand_int;
}


static void _modelling_generate_random_mpf(mpf_t* rand_float)
{
    uint64_t step = 10000;
    uint16_t rand_int = _modelling_generate_random_integer(0, step);
    mpf_set_ui(*rand_float, step);
    mpf_ui_div(*rand_float, 1, *rand_float);
    mpf_mul_ui(*rand_float, *rand_float, rand_int);
}


static void _modelling_markovian_SIR_timestep(modelling_markovian_frame_t* frame, double* infection_rate, double* recovery_rate)
{
    /*
     * Avg Infected  = Infection Rate · Number of Susceptibles
     * Avg Recovered = Recovery Rate  · Number of Infectives
     * Prob Infected =        Avg Infected          =                    Infection Rate · Number of Susceptibles
     *                 ----------------------------   -------------------------------------------------------------------------------
     *                 Avg Infected + Avg Recovered   Infection Rate · Number of Susceptibles + Recovery Rate  · Number of Infectives
     */

    mpf_t avg_infected;
    mpf_init(avg_infected);

    mpf_t avg_recovered;
    mpf_init(avg_recovered);

    mpf_t prob_infection;
    mpf_init(prob_infection);

    mpf_set_d(avg_infected, *infection_rate);
    mpf_set_d(avg_recovered, *recovery_rate);

    mpf_mul_ui(avg_infected, avg_infected, frame->susceptibles);
    mpf_mul_ui(avg_recovered, avg_recovered, frame->infectives);
    mpf_add(prob_infection, avg_infected, avg_recovered);
    mpf_div(prob_infection, avg_infected, prob_infection);

    mpf_clear(avg_infected);
    mpf_clear(avg_recovered);

    mpf_t rand_float;
    mpf_init(rand_float);

    _modelling_generate_random_mpf(&rand_float);
    if (mpf_cmp(rand_float, prob_infection) < 0)
    {
        frame->susceptibles--;
        frame->infectives++;
    }
    else
    {
        frame->infectives--;
        frame->removed++;
    }
    mpf_clear(rand_float);
    mpf_clear(prob_infection);
}


void _modelling_markovian_SIS_timestep(modelling_markovian_frame_t* frame, double* infection_rate, double* recovery_rate)
{
    mpf_t avg_infected;
    mpf_init(avg_infected);

    mpf_t avg_recovered;
    mpf_init(avg_recovered);

    mpf_t prob_infection;
    mpf_init(prob_infection);

    mpf_set_d(avg_infected, *infection_rate);
    mpf_set_d(avg_recovered, *recovery_rate);

    mpf_mul_ui(avg_infected, avg_infected, frame->susceptibles);
    mpf_mul_ui(avg_recovered, avg_recovered, frame->infectives);
    mpf_add(prob_infection, avg_infected, avg_recovered);
    mpf_div(prob_infection, avg_infected, prob_infection);

    mpf_clear(avg_infected);
    mpf_clear(avg_recovered);

    mpf_t rand_float;
    mpf_init(rand_float);

    _modelling_generate_random_mpf(&rand_float);
    if (mpf_cmp(rand_float, prob_infection) < 0)
    {
        frame->susceptibles--;
        frame->infectives++;
    }
    else
    {
        frame->infectives--;
        frame->susceptibles++;
    }
    mpf_clear(rand_float);
    mpf_clear(prob_infection);
}


static timestep_t _modelling_simulate_markovian(double* infection_rate, double* recovery_rate, uint32_t initial_susceptibles, uint32_t initial_infectives)
{
    modelling_markovian_frame_t frame;
    frame.susceptibles = initial_susceptibles;
    frame.infectives = initial_infectives;
    frame.removed = 0;
    timestep_t timestep = 0;

    while (frame.infectives > 0)
    {
        _modelling_markovian_SIR_timestep(&frame, infection_rate, recovery_rate);
        timestep++;
    }

    return timestep;
}


void modelling_simulate(context_t* context)
{
    printf("Infection Rate: %f\n", context->infection_rate);

    for (int i = 0; i < context->bins.size; i++)
    {
        context->bins.array[i] = 0;
    }

    for (uint64_t i = 0; i < context->iterations; i++)
    {
        timestep_t age = _modelling_simulate_markovian(&context->infection_rate, &context->recovery_rate, context->initial_susceptibles, context->initial_infectives);
        if (context->bins.size > age)
        {
            context->bins.array[age] += 1;
        }
    }
}
