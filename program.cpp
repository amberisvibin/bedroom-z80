#include <iostream>
#include <string>

#include <gtk/gtk.h>

/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;

static void clear_surface(void)
{
  cairo_t *cr;

  cr = cairo_create(surface);
  
  //set to black
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_paint(cr);

  cairo_destroy(cr);
}

/* Create a new surface of the appropriate size to store our scribbles */
static gboolean configure_event_cb(GtkWidget         *widget,
                    GdkEventConfigure *event,
                    gpointer           data)
{
  if(surface)
  {
    cairo_surface_destroy(surface);
}

  surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width(widget),
                                               gtk_widget_get_allocated_height(widget));

  /* Initialize the surface to white */
  clear_surface();

  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean
draw_cb(GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);

  return FALSE;
}

/* Draw a rectangle on the surface at the given position */
static void
draw_pixel(GtkWidget *widget,
            gdouble    x,
            gdouble    y,
            int        size)
{
  cairo_t *cr;

  /* Paint to the surface, where we store our state */
  cr = cairo_create(surface);

  cairo_set_source_rgb(cr, 1, 1, 1);

  cairo_rectangle(cr, x, y, size, size);
  cairo_fill(cr);

  cairo_destroy(cr);

  /* Now invalidate the affected region of the drawing area. */
  gtk_widget_queue_draw_area(widget, x, y, size, size);
}



static void
close_window(void)
{
  if(surface)
    cairo_surface_destroy(surface);
}



static void
activate(GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *frame;
  GtkWidget *drawing_area;

  //define menubar things
  GtkWidget *menubar;
  GtkWidget *fileMenu;
  GtkWidget *fileMi;
  GtkWidget *quitMi;


  int zoom = 2;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "z80 screen test");
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);


  gtk_container_set_border_width(GTK_CONTAINER(window), 0);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window), box);

  
  menubar = gtk_menu_bar_new();
  fileMenu = gtk_menu_new();

  fileMi = gtk_menu_item_new_with_label("File");
  quitMi = gtk_menu_item_new_with_label("Quit");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);
  gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);

   g_signal_connect(G_OBJECT(quitMi), "activate",
        G_CALLBACK(gtk_widget_destroy(window)), NULL);


  drawing_area = gtk_drawing_area_new();
  /* set a minimum size */
  gtk_widget_set_size_request(drawing_area, 320 * zoom, 200 * zoom);

  gtk_box_pack_start(GTK_BOX(box), drawing_area, FALSE, FALSE, 0);

  /* Signals used to handle the backing surface */
  g_signal_connect(drawing_area, "draw",
                    G_CALLBACK(draw_cb), NULL);
  g_signal_connect(drawing_area,"configure-event",
                    G_CALLBACK(configure_event_cb), NULL);


 
  gtk_widget_show_all(window);


  draw_pixel(drawing_area, 0, 0, zoom);
  
}

int
main(int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new("com.github.amberisvibin.z80screentest", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
