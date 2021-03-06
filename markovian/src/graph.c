#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "common.h"


#define GRAPH_XMARGIN             60
#define GRAPH_YMARGIN             60
#define GRAPH_XINTERVAL_SIZE      30
#define GRAPH_YINTERVAL_SIZE      30
#define GRAPH_XLARGETICKS         5
#define GRAPH_YLARGETICKS         5
#define GRAPH_POINTRADIUS         3
#define GRAPH_MAX_DATAPOINTS      1000
#define GRAPH_XMIN_INTERVAL_SIZE  25
#define GRAPH_YMIN_INTERVAL_SIZE  20


typedef struct
{
    float x;
    float y;
} graph_datapoint_t;


typedef struct
{
    graph_datapoint_t   datapoints[GRAPH_MAX_DATAPOINTS];
    uint16_t            size;
    float               xupper;
    float               xlower;
    float               yupper;
    float               ylower;
} graph_point_array_t;


static graph_point_array_t _graph_point_array = {0};


gboolean graph_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
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
    cairo_translate (cr, GRAPH_YMARGIN, da.height - GRAPH_XMARGIN);
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

    float maxx, maxy, xinterval, yinterval;
    {
        float minx = _graph_point_array.xlower - 1;
        maxx = _graph_point_array.xupper + 1;
        float xrange = maxx - minx;
        xinterval = (da.width - GRAPH_XMARGIN) / xrange;
    }
    {
        float miny = _graph_point_array.ylower - 1;
        maxy = _graph_point_array.yupper + 1;
        float yrange = maxy - miny;
        yinterval = (da.height - GRAPH_YMARGIN) / yrange;
    }

    /* Writing in the foreground */
    cairo_set_font_size (cr, 15);
    // cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_source_rgb (cr, 0, 0, 0);

    cairo_font_extents_t fe;
    cairo_text_extents_t te;

    cairo_font_extents(cr, &fe);

    cairo_move_to(cr, -GRAPH_YMARGIN / 3., GRAPH_XMARGIN / 3.);
    char label[] = "0";
    cairo_text_extents(cr, label, &te);
    cairo_show_text(cr, label);

    int xinterval_div;
    if (xinterval > GRAPH_XMIN_INTERVAL_SIZE)
        xinterval_div = 1;
    else if (xinterval * 2 > GRAPH_XMIN_INTERVAL_SIZE)
        xinterval_div = 2;
    else
    {
        xinterval_div = 5;
        while (xinterval * xinterval_div < GRAPH_XMIN_INTERVAL_SIZE)
            xinterval_div += 5;
    }
    for (int i = 0; i <= maxx + 2; i += xinterval_div)
    {
        if (!i)
            continue;
        cairo_move_to(cr, i * xinterval , 0);
        float ticklen = GRAPH_XMARGIN/4.5;
        if ((i/xinterval_div) % GRAPH_XLARGETICKS == 0)
            ticklen *= 1.5;
        cairo_line_to(cr, i * xinterval , ticklen);
        if (i < 0)
            continue;
        char label[11];
        snprintf(label, 11, "%d", i);
        cairo_text_extents(cr, label, &te);
        cairo_move_to(cr,
                      i * xinterval - te.x_bearing - te.width / 2,
                      GRAPH_XMARGIN/1.5        - fe.descent  + fe.height / 2);
        cairo_show_text(cr, label);
    }

    float yinterval_div;
    if (yinterval > GRAPH_YMIN_INTERVAL_SIZE)
        yinterval_div = 1;
    else if (yinterval * 2 > GRAPH_YMIN_INTERVAL_SIZE)
        yinterval_div = 2;
    else
    {
        yinterval_div = 5;
        while (yinterval * yinterval_div < GRAPH_YMIN_INTERVAL_SIZE)
            yinterval_div += 5;
    }
    for (int j = 0; j <= maxy + 2; j += yinterval_div)
    {
        if (!j)
            continue;
        cairo_move_to(cr, 0,            -j * yinterval);
        float ticklen = GRAPH_YMARGIN/4.5;
        if ((int)(j/yinterval_div) % GRAPH_YLARGETICKS == 0)
            ticklen *= 1.5;
        cairo_line_to(cr, -ticklen,     -j * yinterval);
        if (j < 0)
            continue;
        char label[11];
        snprintf(label, 11, "%d", j);
        cairo_text_extents(cr, label, &te);
        cairo_move_to(cr,
                      -GRAPH_YMARGIN/1.5    - te.x_bearing - te.width / 2,
                      -j * yinterval        - fe.descent  + fe.height / 2);
        cairo_show_text(cr, label);
    }
    cairo_stroke (cr);

    for (unsigned k = 0; k < _graph_point_array.size; k++)
    {
        graph_datapoint_t* d = &_graph_point_array.datapoints[k];
        cairo_move_to(cr, xinterval*d->x, -yinterval*d->y);
        cairo_arc(cr, xinterval*d->x, -yinterval*d->y, GRAPH_POINTRADIUS, 0, 2 * M_PI);
        cairo_fill (cr);
    }

    cairo_stroke (cr);

    return TRUE;
}


gboolean graph_set_points(bin_array_t bins)
{
    if (bins.size > GRAPH_MAX_DATAPOINTS)
        return FALSE;
    _graph_point_array.size = bins.size;
    _graph_point_array.xlower = INFINITY;
    _graph_point_array.ylower = INFINITY;
    _graph_point_array.xupper = -INFINITY;
    _graph_point_array.yupper = -INFINITY;
    for (unsigned i = 0; i < bins.size; i++)
    {
        if (i < _graph_point_array.xlower)
            _graph_point_array.xlower = i;
        if (bins.array[i] < _graph_point_array.ylower)
            _graph_point_array.ylower = bins.array[i];
        if (i > _graph_point_array.xupper)
            _graph_point_array.xupper = i;
        if (bins.array[i] > _graph_point_array.yupper)
            _graph_point_array.yupper = bins.array[i];
        _graph_point_array.datapoints[i].x = i;
        _graph_point_array.datapoints[i].y = bins.array[i];
    }
    return TRUE;
}
