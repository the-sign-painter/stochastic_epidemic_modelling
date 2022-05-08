#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>


#include "gui.h"
#include "graph.h"
#include "modelling.h"
#include "data.h"


static gboolean initial_susceptibles_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_susceptibles = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean initial_infectives_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_infectives = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean initial_removed_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_removed = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean infection_rate_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->infection_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gboolean recovery_rate_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->recovery_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gboolean time_range_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->bins.size = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean num_iterations_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->iterations = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gboolean simulate_cb(GtkSpinButton *spin_button, context_t* context)
{
    clock_t begin = clock();
    srand(time(NULL));

    modelling_simulate(context);

    set_points(context->bins);

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

    GObject* initial_susceptibles_spin_btn = gtk_builder_get_object(builder, "initial_susceptibles_spin_btn");
    g_signal_connect(initial_susceptibles_spin_btn, "changed", G_CALLBACK(initial_susceptibles_cb), context);

    GObject* initial_infectives_spin_btn = gtk_builder_get_object(builder, "initial_infectives_spin_btn");
    g_signal_connect(initial_infectives_spin_btn, "changed", G_CALLBACK(initial_infectives_cb), context);

    GObject* initial_removed_spin_btn = gtk_builder_get_object(builder, "initial_removed_spin_btn");
    g_signal_connect(initial_removed_spin_btn, "changed", G_CALLBACK(initial_removed_cb), context);

    GObject* infection_rate_spin_btn = gtk_builder_get_object(builder, "infection_rate_spin_btn");
    g_signal_connect(infection_rate_spin_btn, "changed", G_CALLBACK(infection_rate_cb), context);

    GObject* recovery_rate_spin_btn = gtk_builder_get_object(builder, "recovery_rate_spin_btn");
    g_signal_connect(recovery_rate_spin_btn, "changed", G_CALLBACK(recovery_rate_cb), context);

    GObject* time_range_spin_btn = gtk_builder_get_object(builder, "time_range_spin_btn");
    g_signal_connect(time_range_spin_btn, "changed", G_CALLBACK(time_range_cb), context);

    GObject* num_iterations_spin_btn = gtk_builder_get_object(builder, "num_iterations_spin_btn");
    g_signal_connect(num_iterations_spin_btn, "changed", G_CALLBACK(num_iterations_cb), context);

    GObject* simulate_btn = gtk_builder_get_object(builder, "simulate_btn");
    g_signal_connect(simulate_btn, "pressed", G_CALLBACK(simulate_cb), context);

    GObject* graph_container = gtk_builder_get_object(builder, "graph_container");
    g_signal_connect(graph_container, "draw", G_CALLBACK (draw_cb), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}
