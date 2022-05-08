#pragma once

#include <stdint.h>

#include "modelling.h"


void data_print_bin_array(bin_array_t bin_array);
void data_save_data(bin_array_t bin_array, uint64_t iterations);
void data_make_graph_script(void);
void data_make_hist_script(void);
void data_draw_graph(void);
void data_draw_hist(void);
