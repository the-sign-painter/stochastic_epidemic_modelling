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


typedef struct
{
    context_t*          context;
    GObject*            sim_combo_box;
    GObject*            graph_container;
} gui_context_t;


static simulation_struct_t  simulations[] = SIMULATIONS;
static gui_context_t gui_context = {0};


static gboolean _gui_simulation_selection_cb(GtkComboBox* combo_box)
{
    /* Maybe do something? */
    return TRUE;
}


static gboolean _gui_initial_susceptibles_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->initial_susceptibles = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_initial_infectives_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->initial_infectives = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_initial_removed_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->initial_removed = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_infection_rate_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->infection_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gboolean _gui_recovery_rate_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->recovery_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gboolean _gui_time_range_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->bins.size = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_num_iterations_cb(GtkSpinButton *spin_button, void* userdata)
{
    gui_context.context->iterations = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean _gui_simulate_cb(GtkButton *button, void* userdata)
{
    int sim_index = gtk_combo_box_get_active(GTK_COMBO_BOX(gui_context.sim_combo_box));
    if (sim_index >= SIMULATIONS_COUNT)
        return FALSE;
    clock_t begin = clock();
    srand(time(NULL));

    // simulations[sim_index].cb(gui_context->context);
    modelling_simulate(gui_context.context);

    graph_set_points(gui_context.context->bins);

    //print_bin_array(bin_array);
    data_save_data(gui_context.context->bins, gui_context.context->iterations);
    data_make_graph_script();
    data_draw_graph();
    data_make_hist_script();
    data_draw_hist();

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);

    //gtk_widget_queue_draw_area(GTK_WIDGET(gui_context->graph_container), 0, 0, 10, 10);
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
    gui_context.context = context;

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

    gui_context.sim_combo_box = gtk_builder_get_object(builder, "simulation_selection_box");
    _gui_populate_sim_combo_box(gui_context.sim_combo_box);
    g_signal_connect(gui_context.sim_combo_box, "changed", G_CALLBACK(_gui_simulation_selection_cb), NULL);

    GObject* initial_susceptibles_spin_btn = gtk_builder_get_object(builder, "initial_susceptibles_spin_btn");
    g_signal_connect(initial_susceptibles_spin_btn, "changed", G_CALLBACK(_gui_initial_susceptibles_cb), NULL);

    GObject* initial_infectives_spin_btn = gtk_builder_get_object(builder, "initial_infectives_spin_btn");
    g_signal_connect(initial_infectives_spin_btn, "changed", G_CALLBACK(_gui_initial_infectives_cb), NULL);

    GObject* initial_removed_spin_btn = gtk_builder_get_object(builder, "initial_removed_spin_btn");
    g_signal_connect(initial_removed_spin_btn, "changed", G_CALLBACK(_gui_initial_removed_cb), NULL);

    GObject* infection_rate_spin_btn = gtk_builder_get_object(builder, "infection_rate_spin_btn");
    g_signal_connect(infection_rate_spin_btn, "changed", G_CALLBACK(_gui_infection_rate_cb), NULL);

    GObject* recovery_rate_spin_btn = gtk_builder_get_object(builder, "recovery_rate_spin_btn");
    g_signal_connect(recovery_rate_spin_btn, "changed", G_CALLBACK(_gui_recovery_rate_cb), NULL);

    GObject* time_range_spin_btn = gtk_builder_get_object(builder, "time_range_spin_btn");
    g_signal_connect(time_range_spin_btn, "changed", G_CALLBACK(_gui_time_range_cb), NULL);

    GObject* num_iterations_spin_btn = gtk_builder_get_object(builder, "num_iterations_spin_btn");
    g_signal_connect(num_iterations_spin_btn, "changed", G_CALLBACK(_gui_num_iterations_cb), NULL);

    GObject* simulate_btn = gtk_builder_get_object(builder, "simulate_btn");
    g_signal_connect(simulate_btn, "pressed", G_CALLBACK(_gui_simulate_cb), NULL);

    gui_context.graph_container = gtk_builder_get_object(builder, "graph_container");
    g_signal_connect(gui_context.graph_container, "draw", G_CALLBACK (graph_draw_cb), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}
