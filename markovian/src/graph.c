#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "common.h"


#define XMARGIN             60
#define YMARGIN             60
#define XINTERVAL_SIZE      30
#define YINTERVAL_SIZE      30
#define XLARGETICKS         5
#define YLARGETICKS         5
#define POINTRADIUS         3
#define MAX_DATAPOINTS      1000


typedef struct
{
    float x;
    float y;
} datapoint_t;


static datapoint_t datapoints[MAX_DATAPOINTS] = {0};
static uint16_t num_datapoints = 0;


gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
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

    cairo_font_extents_t fe;
    cairo_text_extents_t te;

    cairo_font_extents(cr, &fe);

    cairo_move_to(cr, -YMARGIN / 3., XMARGIN / 3.);
    char label[] = "0";
    cairo_text_extents(cr, label, &te);
    cairo_show_text(cr, label);

    for (int i = -XMARGIN/XINTERVAL_SIZE; i < (da.width / XINTERVAL_SIZE) -1; i++)
    {
        if (!i)
            continue;
        cairo_move_to(cr, i * XINTERVAL_SIZE , 0);
        float ticklen = XMARGIN/4.5;
        if (i % XLARGETICKS == 0)
            ticklen *= 1.5;
        cairo_line_to(cr, i * XINTERVAL_SIZE , ticklen);
        if (i < 0)
            continue;
        char label[10];
        snprintf(label, 10, "%d", i);
        cairo_text_extents(cr, label, &te);
        cairo_move_to(cr,
                      i * XINTERVAL_SIZE - te.x_bearing - te.width / 2,
                      XMARGIN/1.5        - fe.descent  + fe.height / 2);
        cairo_show_text(cr, label);
    }

    for (int j = -YMARGIN/YINTERVAL_SIZE; j < (da.height / YINTERVAL_SIZE) - 1; j++)
    {
        if (!j)
            continue;
        cairo_move_to(cr, 0,            -j * YINTERVAL_SIZE);
        float ticklen = YMARGIN/4.5;
        if (j % YLARGETICKS == 0)
            ticklen *= 1.5;
        cairo_line_to(cr, -ticklen,     -j * YINTERVAL_SIZE);
        if (j < 0)
            continue;
        char label[10];
        snprintf(label, 10, "%d", j);
        cairo_text_extents(cr, label, &te);
        cairo_move_to(cr,
                      -YMARGIN/1.5        - te.x_bearing - te.width / 2,
                      -j * YINTERVAL_SIZE - fe.descent  + fe.height / 2);
        cairo_show_text(cr, label);
    }
    cairo_stroke (cr);

    for (unsigned k = 0; k < num_datapoints; k++)
    {
        datapoint_t* d = &datapoints[k];
        cairo_move_to(cr, XINTERVAL_SIZE*d->x, -YINTERVAL_SIZE*d->y);
        cairo_arc(cr, XINTERVAL_SIZE*d->x, -YINTERVAL_SIZE*d->y, POINTRADIUS, 0, 2 * M_PI);
        cairo_fill (cr);
    }

    cairo_stroke (cr);

    return TRUE;
}


gboolean set_points(bin_array_t bins)
{
    if (bins.size > MAX_DATAPOINTS)
        return FALSE;
    num_datapoints = bins.size;
    for (unsigned i = 0; i < bins.size; i++)
    {
        datapoints[i].x = i;
        datapoints[i].y = bins.array[i];
    }
    return TRUE;
}
