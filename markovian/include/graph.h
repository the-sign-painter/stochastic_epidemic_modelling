#pragma once

#include <glib/gi18n.h>
#include <gtk/gtk.h>


gboolean graph_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data);
gboolean graph_set_points(bin_array_t bins);
