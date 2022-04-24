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
    mpz_t sum;
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


markovian_frame_t markovian_SIR_timestep(markovian_frame_t frame, mpf_t infection_rate, mpf_t recovery_rate, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection, mpf_t rand_float)
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


void save_data(bin_array_t bin_array, uint64_t iterations)
{
    FILE* fp = fopen("output/data", "w");
    if (fp == NULL)
    {
        printf("Cannot open data file.\n");
        exit(-1);
    }
    mpf_t normalised_freq;
    mpf_init(normalised_freq);
    double freq;
    for (int i = 0; i < bin_array.size; i++)
    {
        if (i%2 == 1)
        {
            mpf_set_ui(normalised_freq, iterations);
            mpf_ui_div(normalised_freq, bin_array.array[i], normalised_freq);
            freq = mpf_get_d(normalised_freq);
            fprintf(fp, "%u %f\n", i, freq);
        }
    }
    mpf_clear(normalised_freq);
    fclose(fp);
}


void make_graph_script(void)
{
    FILE *fp = fopen("output/plot_graph.p", "w");
    if (fp == NULL)
    {
        printf("Failed to create plot_graph.p file.\n");
        exit(-1);
    }
    char* project_path = realpath(".", NULL);
    char* data_path = realpath("./output/data", NULL);
    fprintf(fp, "reset\n");
    fprintf(fp, "set terminal png size 500,500\n");
    fprintf(fp, "set output \"%s/output/graph.png\"\n", project_path);
    fprintf(fp, "set title \"Markovian SIR Model Time Period\"\n");
    fprintf(fp, "set xlabel \"Time\"\n");
    fprintf(fp, "set ylabel \"Frequency\"\n");
    fprintf(fp, "plot \"%s\"\n", data_path);
    fclose(fp);
}


void make_hist_script(void)
{
    FILE *fp = fopen("output/plot_histogram.p", "w");
    if (fp == NULL)
    {
        printf("Failed to create plot_histogram.p file.\n");
        exit(-1);
    }
    char* project_path = realpath(".", NULL);
    char* data_path = realpath("./output/data", NULL);
    fprintf(fp, "reset\n");
    fprintf(fp, "n=100\n");
    fprintf(fp, "max=100\n");
    fprintf(fp, "min=0\n");
    fprintf(fp, "width=(max-min)/n\n");
    fprintf(fp, "hist(x,width)=width*floor(x/width)+width/2.0\n");
    fprintf(fp, "set terminal png size 500,500\n");
    fprintf(fp, "set output \"%s/output/hist.png\"\n", project_path);
    fprintf(fp, "set xrange [min:max]\n");
    fprintf(fp, "set yrange[0:]\n");
    fprintf(fp, "set offset graph 0.05,0.05,0.05,0.0\n");
    fprintf(fp, "set xtics min,(max-min)/5,max\n");
    fprintf(fp, "set boxwidth width*0.9\n");
    fprintf(fp, "set style fill solid 0.5\n");
    fprintf(fp, "set tics out nomirror\n");
    fprintf(fp, "set title \"Markovian SIR Model Time Period\"\n");
    fprintf(fp, "set xlabel \"Time\"\n");
    fprintf(fp, "set ylabel \"Frequency\"\n");
    fprintf(fp, "plot \"%s\" using (hist($2,width)):(1.0) smooth freq with boxes lc rgb\"green\"\n", data_path);
}


void draw_graph(void)
{
    FILE *gnuplot = popen("gnuplot output/plot_graph.p", "r");
    fflush(gnuplot);
}


void draw_hist(void)
{
    FILE* gnuplot = popen("gnuplot output/plot_histogram.p", "r");
    fflush(gnuplot);
}


void generate_deterministic_SIR(bin_array_t x, bin_array_t y, uint64_t time_range, mpf_t infection_rate, mpf_t recovery_rate, uint8_t initial_susceptibles, uint8_t initial_infectives)
{
    // TODO: Make deterministic SIR and SIS

    if (x.size != y.size)
    {
        printf("x array is not same length as y array\n");
        exit(-1);
    }
    double step_size = (double)x.size / (double)time_range;

    x.array[0] = 0;
    y.array[0] = 0;

    mpf_t dI;
    mpf_init(dI);
    bin_t dy;
    for (int i = 1; i < x.size; i++)
    {
        mpf_set(dI, infection_rate);
        x.array[i] = i * step_size;
        dy = 1;
        y.array[i] = y.array[i-1] + dy;
    }
    mpf_clear(dI);
}


int main(void)
{
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
