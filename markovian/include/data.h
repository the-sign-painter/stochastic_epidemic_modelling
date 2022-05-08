#pragma once

#include <stdint.h>

#include "modelling.h"


void print_bin_array(bin_array_t bin_array);
void save_data(bin_array_t bin_array, uint64_t iterations);
void make_graph_script(void);
void make_hist_script(void);
void draw_graph(void);
void draw_hist(void);
