#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>


#include "gui.h"
#include "graph.h"
#include "modelling.h"
#include "data.h"


typedef enum
{
    SIMULATION_MARKOVIAN_SIR,
    SIMULATION_MARKOVIAN_SIS,
} simulation_enum_t;


#define SIMULATIONS_COUNT                                           2
#define MAX_SIM_NAME_LEN                                            16
#define SIMULATIONS                                                    \
{                                                                      \
    { SIMULATION_MARKOVIAN_SIR, "Markovian SIR", modelling_simulate }, \
    { SIMULATION_MARKOVIAN_SIS, "Markovian SIS", modelling_simulate }, \
}


typedef struct
{
    simulation_enum_t   id;
    char                name[MAX_SIM_NAME_LEN];
    void                (*cb)(context_t* context);
} simulation_struct_t;


static simulation_struct_t  simulations[]       = SIMULATIONS;
static simulation_enum_t    active_sim_index    = 0;


static gboolean _gui_simulation_selection_cb(GtkComboBox* combo_box, context_t* context)
{
    int sim_index = gtk_combo_box_get_active(combo_box);
    if (sim_index >= SIMULATIONS_COUNT)
        return FALSE;
    active_sim_index = sim_index;
    return TRUE;
}


static gboolean _gui_initial_susceptibles_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_susceptibles = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_initial_infectives_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_infectives = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_initial_removed_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_removed = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_infection_rate_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->infection_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gboolean _gui_recovery_rate_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->recovery_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gboolean _gui_time_range_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->bins.size = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_num_iterations_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->iterations = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_simulate_cb(GtkButton *button, context_t* context)
{
    clock_t begin = clock();
    srand(time(NULL));

    simulations[active_sim_index].cb(context);

    graph_set_points(context->bins);

    //print_bin_array(bin_array);
    data_save_data(context->bins, context->iterations);
    data_make_graph_script();
    data_draw_graph();
    data_make_hist_script();
    data_draw_hist();

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);

    gtk_image_new_from_file(DATA_DIR"/graph.png");
    return TRUE;
}


static void _gui_populate_sim_combo_box(GObject* combo_box)
{
    GtkTreeIter iter;
    GtkListStore *store = gtk_list_store_new (1, G_TYPE_STRING);
    for (uint8_t i = 0; i < SIMULATIONS_COUNT; i++)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, simulations[i].name, -1);
    }
    gtk_combo_box_set_model(GTK_COMBO_BOX(combo_box), GTK_TREE_MODEL(store));
    GtkCellRenderer* cell = gtk_cell_renderer_text_new();
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), cell, "text", 0, NULL );
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box),0);
}


void gui_init(context_t* context, int* argc, char*** argv)
{
    GtkWidget*  window;
    GtkBuilder* builder = NULL;

    gtk_init(argc , argv);

    builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder,"src/gui.glade" , NULL) == 0)
    {
        printf("gtk_builder_add_from_file FAILED\n");
        return;
    }

    window  = GTK_WIDGET(gtk_builder_get_object(builder,"window1"));

    gtk_builder_connect_signals(builder,NULL);

    GObject* simulation_selection_box = gtk_builder_get_object(builder, "simulation_selection_box");
    _gui_populate_sim_combo_box(simulation_selection_box);
    g_signal_connect(simulation_selection_box, "changed", G_CALLBACK(_gui_simulation_selection_cb), context);

    GObject* initial_susceptibles_spin_btn = gtk_builder_get_object(builder, "initial_susceptibles_spin_btn");
    g_signal_connect(initial_susceptibles_spin_btn, "changed", G_CALLBACK(_gui_initial_susceptibles_cb), context);

    GObject* initial_infectives_spin_btn = gtk_builder_get_object(builder, "initial_infectives_spin_btn");
    g_signal_connect(initial_infectives_spin_btn, "changed", G_CALLBACK(_gui_initial_infectives_cb), context);

    GObject* initial_removed_spin_btn = gtk_builder_get_object(builder, "initial_removed_spin_btn");
    g_signal_connect(initial_removed_spin_btn, "changed", G_CALLBACK(_gui_initial_removed_cb), context);

    GObject* infection_rate_spin_btn = gtk_builder_get_object(builder, "infection_rate_spin_btn");
    g_signal_connect(infection_rate_spin_btn, "changed", G_CALLBACK(_gui_infection_rate_cb), context);

    GObject* recovery_rate_spin_btn = gtk_builder_get_object(builder, "recovery_rate_spin_btn");
    g_signal_connect(recovery_rate_spin_btn, "changed", G_CALLBACK(_gui_recovery_rate_cb), context);

    GObject* time_range_spin_btn = gtk_builder_get_object(builder, "time_range_spin_btn");
    g_signal_connect(time_range_spin_btn, "changed", G_CALLBACK(_gui_time_range_cb), context);

    GObject* num_iterations_spin_btn = gtk_builder_get_object(builder, "num_iterations_spin_btn");
    g_signal_connect(num_iterations_spin_btn, "changed", G_CALLBACK(_gui_num_iterations_cb), context);

    GObject* simulate_btn = gtk_builder_get_object(builder, "simulate_btn");
    g_signal_connect(simulate_btn, "pressed", G_CALLBACK(_gui_simulate_cb), context);

    GObject* graph_container = gtk_builder_get_object(builder, "graph_container");
    g_signal_connect(graph_container, "draw", G_CALLBACK (graph_draw_cb), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}
