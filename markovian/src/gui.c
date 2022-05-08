#include <stdio.h>
#include <inttypes.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>


#include "gui.h"


static gint initial_susceptibles_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_susceptibles = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gint initial_infectives_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_infectives = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gint initial_removed_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->initial_removed = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gint infection_rate_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->infection_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gint recovery_rate_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->recovery_rate = gtk_spin_button_get_value(spin_button);
    return TRUE;
}


static gint time_range_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->time_range = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gint num_iterations_cb(GtkSpinButton *spin_button, context_t* context)
{
    context->iterations = gtk_spin_button_get_value_as_int(spin_button);
    return TRUE;
}


static gint simulate_cb(GtkSpinButton *spin_button, context_t* context)
{
    printf("initial_susceptibles = %"PRIu16"\n", context->initial_susceptibles);
    printf("initial_infectives = %"PRIu16"\n", context->initial_infectives);
    printf("initial_removed = %"PRIu16"\n", context->initial_removed);
    printf("infection_rate = %lf\n", context->infection_rate);
    printf("recovery_rate = %lf\n", context->recovery_rate);
    printf("time_range = %"PRIu16"\n", context->time_range);
    printf("iterations = %"PRIu64"\n", context->iterations);
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

    gtk_widget_show_all(window);
    gtk_main();
}
