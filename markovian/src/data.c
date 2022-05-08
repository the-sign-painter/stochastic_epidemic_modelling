#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <gmp.h>
#include <sys/stat.h>
#include <errno.h>

#include "data.h"


static void _data_create_DATA_DIR(void)
{
    errno = 0;
    int ret = mkdir(DATA_DIR, S_IRWXU);
    if (ret == -1) {
        switch (errno) {
            case EACCES :
                exit(EXIT_FAILURE);
            case EEXIST:
                return;
            case ENAMETOOLONG:
                exit(EXIT_FAILURE);
            default:
                return;
        }
    }
}


void data_print_bin_array(bin_array_t bin_array)
{
    for (int i = 0; i < bin_array.size; i++)
    {
        printf("%u: %u\n", i, bin_array.array[i]);
    }
}


void data_save_data(bin_array_t bin_array, uint64_t iterations)
{
    _data_create_DATA_DIR();
    FILE* fp = fopen(DATA_DIR"/data", "w");
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


void data_make_graph_script(void)
{
    _data_create_DATA_DIR();
    FILE *fp = fopen(DATA_DIR"/plot_graph.p", "w");
    if (fp == NULL)
    {
        printf("Failed to create plot_graph.p file.\n");
        exit(-1);
    }
    char* project_path = realpath(".", NULL);
    char* data_path = realpath(DATA_DIR"/data", NULL);
    fprintf(fp, "reset\n");
    fprintf(fp, "set terminal png size 500,500\n");
    fprintf(fp, "set output \"%s/"DATA_DIR"/graph.png\"\n", project_path);
    fprintf(fp, "set title \"Markovian SIR Model Time Period\"\n");
    fprintf(fp, "set xlabel \"Time\"\n");
    fprintf(fp, "set ylabel \"Frequency\"\n");
    fprintf(fp, "plot \"%s\"\n", data_path);
    fclose(fp);
}


void data_make_hist_script(void)
{
    _data_create_DATA_DIR();
    FILE *fp = fopen(DATA_DIR"/plot_histogram.p", "w");
    if (fp == NULL)
    {
        printf("Failed to create plot_histogram.p file.\n");
        exit(-1);
    }
    char* project_path = realpath(".", NULL);
    char* data_path = realpath(DATA_DIR"/data", NULL);
    fprintf(fp, "reset\n");
    fprintf(fp, "n=100\n");
    fprintf(fp, "max=100\n");
    fprintf(fp, "min=0\n");
    fprintf(fp, "width=(max-min)/n\n");
    fprintf(fp, "hist(x,width)=width*floor(x/width)+width/2.0\n");
    fprintf(fp, "set terminal png size 500,500\n");
    fprintf(fp, "set output \"%s/"DATA_DIR"/hist.png\"\n", project_path);
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


void data_draw_graph(void)
{
    FILE *gnuplot = popen("gnuplot "DATA_DIR"/plot_graph.p", "r");
    fflush(gnuplot);
}


void data_draw_hist(void)
{
    FILE* gnuplot = popen("gnuplot "DATA_DIR"/plot_histogram.p", "r");
    fflush(gnuplot);
}
