#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
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
} bin_array_t;


uint16_t generate_random_integer(uint16_t start, uint16_t end)
{
    uint16_t rand_int = rand() % (end + 1 - start) + start;
    return rand_int;
}

void generate_random_mpf(mpf_t rand_float)
{
    uint64_t step = 10000;
    uint16_t rand_int = generate_random_integer(0, step);
    mpf_set_ui(rand_float, step);
    mpf_ui_div(rand_float, 1, rand_float);
    mpf_mul_ui(rand_float, rand_float, rand_int);
}

timestep_t simulate_markovian(mpf_t infection_rate, mpf_t recovery_rate, uint8_t initial_susceptibles, uint8_t initial_infectives, mpf_t rand_float)
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

    for (int i = 0; i < iterations; i++)
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

void print_bin_array(bin_array_t bin_array)
{
    for (int i = 0; i < bin_array.size; i++)
    {
        printf("%u: %u\n", i, bin_array.array[i]);
    }
}

void draw_graph(bin_array_t bin_array)
{
    FILE *gnuplot = popen("gnuplot -persist", "w");
    fprintf(gnuplot, "plot '-'\n");
    for (int i = 0; i < bin_array.size; i++)
    {
        fprintf(gnuplot, "%u %u\n", i, bin_array.array[i]);
    }
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
}

int main(void)
{
    clock_t begin = clock();
    srand(time(NULL));

    uint8_t time_range = 100;
    uint64_t iterations = 100000;

    double infection_rate = 0.01;
    double recovery_rate = 0.1;
    uint8_t initial_susceptibles = 49;
    uint8_t initial_infectives = 1;

    bin_t* bins = (bin_t*)malloc(time_range * sizeof(bin_t));
    bin_array_t bin_array;
    bin_array.size = time_range;
    bin_array.array = bins;

    simulate(bin_array, iterations, infection_rate, recovery_rate, initial_susceptibles, initial_infectives);

    //print_bin_array(bin_array);
    draw_graph(bin_array);

    free(bins);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);

    return 0;
}
