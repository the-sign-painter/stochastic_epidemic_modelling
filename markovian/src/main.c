#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>

#define EP_MODEL_MARK_SIR 1
#define EP_MODEL_MARK_SIS 2

typedef uint16_t bin_t;
typedef uint16_t pop_t;
typedef uint16_t timestep_t;

typedef struct
{
    pop_t susceptibles;
    pop_t infectives;
    pop_t removed;
} markovian_frame_t;

typedef struct
{
    pop_t* susceptibles;
    pop_t* infectives;
    pop_t* removed;
    uint16_t size;
} markovian_frame_array_t;

typedef struct
{
    double* susceptibles;
    double* infectives;
    double* removed;
    uint16_t size;
} markovian_avg_frame_array_t;

typedef struct
{
    uint8_t size;
    bin_t* array;
} bin_array_t;

typedef struct
{
    double infection_rate_d;
    double recovery_rate_d;
    mpf_t infection_rate;
    mpf_t recovery_rate;
    uint8_t initial_susceptibles;
    uint8_t initial_infectives;
    uint64_t time_range;
    uint64_t iterations;
} context_t;


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

markovian_frame_t markovian_SIR_timestep(markovian_frame_t frame, context_t context, mpf_t rand_float, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection)
{
    mpf_mul_ui(avg_infected, context.infection_rate, frame.susceptibles);
    mpf_mul_ui(avg_recovered, context.recovery_rate, frame.infectives);
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

markovian_frame_t markovian_SIS_timestep(markovian_frame_t frame, context_t context, mpf_t rand_float, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection)
{
    mpf_mul_ui(avg_infected, context.infection_rate, frame.susceptibles);
    mpf_mul_ui(avg_recovered, context.recovery_rate, frame.infectives);
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

timestep_t markovian_age(context_t context, mpf_t rand_float, uint8_t mode, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection)
{
    markovian_frame_t frame;
    frame.susceptibles = context.initial_susceptibles;
    frame.infectives = context.initial_infectives;
    frame.removed = 0;
    timestep_t timestep = 0;

    while (frame.infectives > 0)
    {
        if (mode == EP_MODEL_MARK_SIR)
        {
            frame = markovian_SIR_timestep(frame, context, rand_float, avg_infected, avg_recovered, prob_infection);
        }
        else if (mode == EP_MODEL_MARK_SIS)
        {
            frame = markovian_SIS_timestep(frame, context, rand_float, avg_infected, avg_recovered, prob_infection);
        }
        else
        {
            printf("No markovian mode selected.\n");
            exit(-1);
        }
        timestep++;
    }
    return timestep;
}

void markovian_pop(context_t context, mpf_t rand_float, uint8_t mode, markovian_frame_array_t frame_array, mpf_t avg_infected, mpf_t avg_recovered, mpf_t prob_infection)
{
    markovian_frame_t frame;
    frame.susceptibles = context.initial_susceptibles;
    frame.infectives = context.initial_infectives;
    frame.removed = 0;
    timestep_t timestep = 0;
    frame_array.susceptibles[0] = frame.susceptibles;
    frame_array.infectives[0] = frame.infectives;
    frame_array.removed[0] = frame.removed;
    for (int i = 0; i <= context.time_range; i++)
    {
        if (mode == EP_MODEL_MARK_SIR)
        {
            frame = markovian_SIR_timestep(frame, context, rand_float, avg_infected, avg_recovered, prob_infection);
        }
        else if (mode == EP_MODEL_MARK_SIS)
        {
            frame = markovian_SIS_timestep(frame, context, rand_float, avg_infected, avg_recovered, prob_infection);
        }
        else
        {
            printf("No markovian mode selected.\n");
            exit(-1);
        }
        frame_array.susceptibles[i+1] = frame.susceptibles;
        frame_array.infectives[i+1] = frame.infectives;
        frame_array.removed[i+1] = frame.removed;
        if (frame.infectives < 1)
        {
            break;
        }
    }
}

void simulate_markovian_age(bin_array_t bin_array, context_t context, uint8_t mode)
{
    mpf_t avg_infected;
    mpf_init(avg_infected);
    mpf_t avg_recovered;
    mpf_init(avg_recovered);
    mpf_t prob_infection;
    mpf_init(prob_infection);

    for (int i = 0; i < bin_array.size; i++)
    {
        bin_array.array[i] = 0;
    }
    mpf_t rand_float;
    mpf_init(rand_float);

    for (int i = 0; i < context.iterations; i++)
    {
        timestep_t age = markovian_age(context, rand_float, mode, avg_infected, avg_recovered, prob_infection);
        if (bin_array.size > age)
        {
            bin_array.array[age] += 1;
        }
    }
    mpf_clear(avg_infected);
    mpf_clear(avg_recovered);
    mpf_clear(prob_infection);
    
    mpf_clear(rand_float);
}

void simulate_markovian_pop(markovian_avg_frame_array_t avg_frame_array, context_t context, uint8_t mode)
{
    mpf_t avg_infected;
    mpf_init(avg_infected);
    mpf_t avg_recovered;
    mpf_init(avg_recovered);
    mpf_t prob_infection;
    mpf_init(prob_infection);
    mpf_t rand_float;
    mpf_init(rand_float);

    markovian_frame_array_t frame_array;
    frame_array.size = avg_frame_array.size;
    frame_array.susceptibles = (pop_t*)malloc((frame_array.size+1) * sizeof(pop_t));
    frame_array.infectives = (pop_t*)malloc((frame_array.size+1) * sizeof(pop_t));
    frame_array.removed = (pop_t*)malloc((frame_array.size+1) * sizeof(pop_t));

    for (int i = 0; i < frame_array.size; i++)
    {
        avg_frame_array.susceptibles[i] = 0;
        avg_frame_array.infectives[i] = 0;
        avg_frame_array.removed[i] = 0;
    }

    for (int i = 0; i < context.iterations; i++)
    {
        markovian_pop(context, rand_float, mode, frame_array, avg_infected, avg_recovered, prob_infection);
        for (int j = 0; j <= frame_array.size; j++)
        {
            avg_frame_array.susceptibles[j] += (double)frame_array.susceptibles[j];
            avg_frame_array.infectives[j] += (double)frame_array.infectives[j];
            avg_frame_array.removed[j] += (double)frame_array.removed[j];
        }
    }

    free(frame_array.susceptibles);
    free(frame_array.infectives);
    free(frame_array.removed);

    mpf_clear(avg_infected);
    mpf_clear(avg_recovered);
    mpf_clear(prob_infection);
    mpf_clear(rand_float);

    // TODO: Should probably change to rolling avg, faster.
    for (int i = 0; i <= avg_frame_array.size; i++)
    {
        avg_frame_array.susceptibles[i] = avg_frame_array.susceptibles[i] / (double)context.iterations;
        avg_frame_array.infectives[i] = avg_frame_array.infectives[i] / (double)context.iterations;
        avg_frame_array.removed[i] = avg_frame_array.removed[i] / (double)context.iterations;
    }
}

void save_data(double* x, double* y, uint64_t size, char* name)
{
    char loc[128] = "output/";
    strcat(loc, name);
    strcat(loc, ".data");
    FILE* fp = fopen(loc, "w");
    if (fp == NULL)
    {
        printf("Cannot open data file '%s'\n", loc);
        exit(-1);
    }
    for (int i = 0; i < size ; i++)
    {
        fprintf(fp, "%f %f\n", x[i], y[i]);
    }
    fclose(fp);
}

void save_bin(bin_array_t bin_array, context_t context, char* name)
{
    uint8_t clean_size = context.time_range / 2;
    double* x = (double*)malloc(clean_size * sizeof(double));
    double* y = (double*)malloc(clean_size * sizeof(double));

    mpf_t normalised_freq;
    mpf_init(normalised_freq);
    for (int i = 0; i < clean_size; i++)
    {
        x[i] = i;
        mpf_set_ui(normalised_freq, context.iterations);
        mpf_ui_div(normalised_freq, bin_array.array[i*2+1], normalised_freq);
        y[i] = mpf_get_d(normalised_freq);
    }
    mpf_clear(normalised_freq);
    save_data(x, y, clean_size, name);
    free(x);
    free(y);
}

void save_pop(double* pop, uint16_t size, char* name)
{
    double* x = (double*)malloc((size+1) * sizeof(double));
    double* y = (double*)malloc((size+1) * sizeof(double));
    for (int i = 0; i <= size; i++)
    {
        x[i] = (double)i;
        y[i] = (double)pop[i];
    }
    save_data(x, y, size+1, name);
    free(x);
    free(y);
}

void make_graph_script(char* name)
{
    FILE* fp = fopen("output/plot_graph.p", "w");
    if (fp == NULL)
    {
        printf("Failed to create plot_graph.p file.\n");
        exit(-1);
    }
    char* project_path = realpath(".", NULL);
    char loc[128] = "output/";
    strcat(loc, name);
    strcat(loc, ".data");
    char* data_path = realpath(loc, NULL);
    fprintf(fp, "reset\n");
    fprintf(fp, "set terminal png size 600,600\n");
    fprintf(fp, "set output \"%s/output/%s_graph.png\"\n", project_path, name);
    fprintf(fp, "set title \"Markovian SIR Model Time Period\"\n");
    fprintf(fp, "set xlabel \"Time\"\n");
    fprintf(fp, "set ylabel \"Frequency\"\n");
    fprintf(fp, "set nokey\n");
    fprintf(fp, "plot \"%s\" with lines\n", data_path);
    free(project_path);
    free(data_path);
    fclose(fp);
}

void make_hist_script(char* name)
{
    // FIXME: Incorrect binning I think
    FILE* fp = fopen("output/plot_histogram.p", "w");
    if (fp == NULL)
    {
        printf("Failed to create plot_histogram.p file.\n");
        exit(-1);
    }
    char* project_path = realpath(".", NULL);
    char loc[128] = "output/";
    strcat(loc, name);
    strcat(loc, ".data");
    char* data_path = realpath(loc, NULL);
    fprintf(fp, "reset\n");
    fprintf(fp, "n=100\n");
    fprintf(fp, "max=100\n");
    fprintf(fp, "min=0\n");
    fprintf(fp, "width=(max-min)/n\n");
    fprintf(fp, "hist(x,width)=width*floor(x/width)+width/2.0\n");
    fprintf(fp, "set terminal png size 500,500\n");
    fprintf(fp, "set output \"%s/output/%s_hist.png\"\n", project_path, name);
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
    fprintf(fp, "set nokey\n");
    fprintf(fp, "plot \"%s\" using (hist($2,width)):(1.0) smooth freq with boxes lc rgb\"green\"\n", data_path);
    free(project_path);
    free(data_path);
    fclose(fp);
}

void draw_graph(char* name)
{
    make_graph_script(name);
    FILE* gnuplot = popen("gnuplot output/plot_graph.p", "r");
    fflush(gnuplot);
    fclose(gnuplot);
}

void draw_hist(char* name)
{
    make_hist_script(name);
    FILE* gnuplot = popen("gnuplot output/plot_histogram.p", "r");
    fflush(gnuplot);
    fclose(gnuplot);
}

void deterministic_pop(double* x, double* y, uint64_t size, context_t context, uint8_t mode)
{
    // TODO: Make deterministic SIR and SIS

    double step_size = (double)context.time_range / (double)size;

    x[0] = 0;

    double* S = malloc((size+1) * sizeof(double));
    double* I = y;
    double* R = malloc((size+1) * sizeof(double));
    double dI;
    double dS;
    double dR;

    S[0] = (double)context.initial_susceptibles;
    I[0] = (double)context.initial_infectives;
    R[0] = 0;

    for (int i = 1; i <= size; i++)
    {
        //  dI = -dS - dR
        //  dS = -beta * susceptible * infected
        //  dR = gamma * infected
        //  dI = beta * susceptible * infected - gamma * infected
        x[i] = (double)i * step_size;

        if (mode == EP_MODEL_MARK_SIR)
        {
            dS = -context.infection_rate_d * I[i-1] * S[i-1];
            dR = context.recovery_rate_d * I[i-1];
            dI = -dS - dR;
        }
        else if (mode == EP_MODEL_MARK_SIS)
        {
            dS = -context.infection_rate_d * I[i-1] * S[i-1] + context.recovery_rate_d * I[i-1];
            dR = 0;
            dI = -dS;
        }
        else
        {
            printf("Deterministic mode not selected.\n");
            exit(-1);
        }

        S[i] = S[i-1] + dS;
        I[i] = I[i-1] + dI;
        R[i] = R[i-1] + dR;
    }
    free(S);
    I = NULL;
    free(R);
}

// WRAPPED UP FUNCTIONS

int markovian_SIR_age(context_t context)
{
    bin_t* bins = (bin_t*)malloc(context.time_range * sizeof(bin_t));
    bin_array_t bin_array;
    bin_array.size = context.time_range;
    bin_array.array = bins;

    char* name = "stoch_mark_SIR_age";
    simulate_markovian_age(bin_array, context, EP_MODEL_MARK_SIR);
    save_bin(bin_array, context, name);
    free(bins);
    
    draw_graph(name);
    draw_hist(name);
    return 0;
}

int markovian_SIS_age(context_t context)
{
    bin_t* bins = (bin_t*)malloc(context.time_range * sizeof(bin_t));
    bin_array_t bin_array;
    bin_array.size = context.time_range;
    bin_array.array = bins;

    char* name = "stoch_mark_SIS_age";
    simulate_markovian_age(bin_array, context, EP_MODEL_MARK_SIS);
    save_bin(bin_array, context, name);
    free(bins);
   
    draw_graph(name);
    draw_hist(name);
    return 0;
}

int markovian_SIR_pop(context_t context)
{
    markovian_avg_frame_array_t avg_frame_array;
    avg_frame_array.size = context.time_range;
    avg_frame_array.susceptibles = (double*)malloc((avg_frame_array.size+1) * sizeof(double));
    avg_frame_array.infectives   = (double*)malloc((avg_frame_array.size+1) * sizeof(double));
    avg_frame_array.removed      = (double*)malloc((avg_frame_array.size+1) * sizeof(double));

    simulate_markovian_pop(avg_frame_array, context, EP_MODEL_MARK_SIR);

    save_pop(avg_frame_array.infectives, avg_frame_array.size, "stoch_mark_SIR_pop");

    free(avg_frame_array.susceptibles);
    free(avg_frame_array.infectives);
    free(avg_frame_array.removed);

    uint32_t det_precision = 25;
    double* det_x = (double*)malloc((det_precision + 1) * sizeof(double));
    double* det_y = (double*)malloc((det_precision + 1) * sizeof(double));

    deterministic_pop(det_x, det_y, det_precision, context, EP_MODEL_MARK_SIR);
    save_data(det_x, det_y, det_precision, "det_mark_SIR_pop");
    free(det_x);
    free(det_y);
    return 0;
}

int markovian_SIS_pop(context_t context)
{
    markovian_avg_frame_array_t avg_frame_array;
    avg_frame_array.size = context.time_range;
    avg_frame_array.susceptibles = (double*)malloc((avg_frame_array.size+1) * sizeof(double));
    avg_frame_array.infectives   = (double*)malloc((avg_frame_array.size+1) * sizeof(double));
    avg_frame_array.removed      = (double*)malloc((avg_frame_array.size+1) * sizeof(double));

    simulate_markovian_pop(avg_frame_array, context, EP_MODEL_MARK_SIS);

    save_pop(avg_frame_array.infectives, avg_frame_array.size, "stoch_mark_SIS_pop");

    free(avg_frame_array.susceptibles);
    free(avg_frame_array.infectives);
    free(avg_frame_array.removed);

    uint32_t det_precision = 25;
    double* det_x = (double*)malloc((det_precision + 1) * sizeof(double));
    double* det_y = (double*)malloc((det_precision + 1) * sizeof(double));

    deterministic_pop(det_x, det_y, det_precision, context, EP_MODEL_MARK_SIS);
    save_data(det_x, det_y, det_precision, "det_mark_SIS_pop");
    free(det_x);
    free(det_y);
    return 0;
}

int main(void)
{
    clock_t begin = clock();
    srand(time(NULL));

    context_t context;

    context.time_range = 1000;
    context.iterations = 100;

    context.infection_rate_d = 0.01;
    context.recovery_rate_d = 0.1;
    context.initial_susceptibles = 149;
    context.initial_infectives = 1;

    mpf_init(context.infection_rate);
    mpf_set_d(context.infection_rate, context.infection_rate_d);
    mpf_init(context.recovery_rate);
    mpf_set_d(context.recovery_rate, context.recovery_rate_d);

    markovian_SIS_age(context);

    markovian_SIS_pop(context);

    mpf_clear(context.infection_rate);
    mpf_clear(context.recovery_rate);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);

    return 0;
}
