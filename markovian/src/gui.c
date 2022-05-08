#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>


#include "gui.h"
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

    simulate(context);

    //print_bin_array(bin_array);
    save_data(context->bins, context->iterations);
    make_graph_script();
    draw_graph();
    make_hist_script();
    draw_hist();

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);

    gtk_image_new_from_file(DATA_DIR"/graph.png");
    return TRUE;
}


gfloat f (gfloat x)
{
    return 0.03 * pow (x, 3);
}


#define XMARGIN             60
#define YMARGIN             60
#define XINTERVAL_SIZE      30
#define YINTERVAL_SIZE      30


static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    GdkRectangle da;            /* GtkDrawingArea size */
    gdouble dx = 1.0, dy = 1.0; /* Pixels between each point */
    gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 10.0, clip_y2 = 10.0;
    GdkWindow *window = gtk_widget_get_window(widget);

    /* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (window,
            &da.x,
            &da.y,
            &da.width,
            &da.height);

    /* Draw on a black background */
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_paint (cr);

    /* Change the transformation matrix */
    cairo_translate (cr, YMARGIN, da.height - XMARGIN);
    cairo_scale (cr, 1., 1.);

    /* Determine the data points to calculate (ie. those in the clipping zone */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);

    /* Draws x and y axis */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_move_to (cr, clip_x1, 0.0);
    cairo_line_to (cr, clip_x2, 0.0);
    cairo_move_to (cr, 0.0, clip_y1);
    cairo_line_to (cr, 0.0, clip_y2);
    cairo_stroke (cr);

    /* Writing in the foreground */
    cairo_set_font_size (cr, 15);
    // cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_source_rgb (cr, 0, 0, 0);

    for (int i = 0; i < da.width / XINTERVAL_SIZE; i++)
    {
        cairo_move_to(cr, i * XINTERVAL_SIZE , 0);
        cairo_line_to(cr, i * XINTERVAL_SIZE , XMARGIN/3.);
        cairo_move_to(cr, i * XINTERVAL_SIZE , XMARGIN/1.5);
        char label[10];
        snprintf(label, 10, "%d", i);
        cairo_show_text(cr, label);
    }

    for (int j = 0; j < da.height / YINTERVAL_SIZE; j++)
    {
        cairo_move_to(cr, 0,            -j * YINTERVAL_SIZE);
        cairo_line_to(cr, -YMARGIN/3.,  -j * YINTERVAL_SIZE);
        cairo_move_to(cr, -YMARGIN/1.5, -j * YINTERVAL_SIZE);
        char label[10];
        snprintf(label, 10, "%d", j);
        cairo_show_text(cr, label);
    }
    cairo_stroke (cr);

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
