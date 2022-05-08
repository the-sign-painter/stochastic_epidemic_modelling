#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

#include "common.h"
#include "modelling.h"
#include "data.h"
#include "gui.h"


static context_t context = {.bins={0},
                            .iterations=1000,
                            .time_range=100,
                            .infection_rate=0.01,
                            .recovery_rate=0.1,
                            .initial_susceptibles=99,
                            .initial_infectives=1,
                            .initial_removed=0,
                           };


int main(int argc, char **argv)
{
    gui_init(&context, &argc, &argv);


    clock_t begin = clock();
    srand(time(NULL));

    uint64_t time_range = 100;
    uint64_t iterations = 100000;

    double infection_rate = 0.01;
    double recovery_rate = 0.1;
    uint8_t initial_susceptibles = 49;
    uint8_t initial_infectives = 1;

    bin_t* bins = (bin_t*)malloc(time_range * sizeof(bin_t));
    bin_array_t bin_array;
    bin_array.size = time_range;
    bin_array.array = bins;
    mpz_init(bin_array.sum);

    simulate(bin_array, iterations, infection_rate, recovery_rate, initial_susceptibles, initial_infectives);

    //print_bin_array(bin_array);
    save_data(bin_array, iterations);
    make_graph_script();
    draw_graph();
    make_hist_script();
    draw_hist();

    free(bins);
    mpz_clear(bin_array.sum);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);

    return 0;
}
