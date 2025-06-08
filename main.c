#include <gtk/gtk.h>
#include <time.h>

// Structure to hold widgets that need to be accessed in callbacks
typedef struct {
    GtkLabel *time_label;
} AppWidgets;

// Function to update the time label
static gboolean update_time(gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    time_t current_time;
    struct tm *time_info;
    char time_string[100];

    time(&current_time);
    time_info = localtime(&current_time);

    // Format as HH:MM:SS
    strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);
    gtk_label_set_text(widgets->time_label, time_string);

    return G_SOURCE_CONTINUE; // Keep the timer running
}

// Function to apply CSS for transparency and styling
static void apply_css_styling(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        "window#time_panel_window {"
        "   background-color: red; /* Test with a solid color */"
        "}"
        "label#time_label {"
        "   background-color: yellow; /* Test with a different solid color */"
        "   color: black;"
        "   font-size: 28px;"
        "   font-weight: bold;"
        "}";
    gtk_css_provider_load_from_data(provider, css, -1);

    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(widget),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

// Activate callback for the application
static void activate_application(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkWidget *window;
    GtkWidget *center_box;
    GdkDisplay *display;
    GdkMonitor *monitor = NULL; // Initialize monitor
    GdkRectangle geometry;
    int screen_width = 300; // Test with a fixed small width
    int screen_x_offset = 50; // Test with some offset
    int screen_y_offset = 50; // Test with some offset
    const int panel_height = 100; // Test with a fixed small height

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Test Panel");
    gtk_widget_set_name(window, "time_panel_window");

    // Apply CSS for transparency and text styling
    // Connect to "realize" signal to ensure display is available for CSS provider
    g_signal_connect(window, "realize", G_CALLBACK(apply_css_styling), NULL);

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    // gtk_window_set_keep_above(GTK_WINDOW(window), TRUE); // Commented out based on user feedback

    // Determine screen geometry for positioning - All related functions commented out based on user feedback
    // display = gtk_widget_get_display(window);
    // monitor = gdk_display_get_primary_monitor(display);

    // if (!monitor) {
    //     GtkNative *native = gtk_widget_get_native(window);
    //     GdkSurface *surface = NULL;
    //     if (native) {
    //         surface = gtk_native_get_surface(native);
    //     }
    //     if (surface) {
    //         // monitor = gdk_surface_get_monitor(surface);
    //     }
    //     if (!monitor) {
    //         GListModel *monitors = gdk_display_get_monitors(display);
    //         if (monitors && g_list_model_get_n_items(monitors) > 0) {
    //             monitor = GDK_MONITOR(g_list_model_get_item(monitors, 0));
    //         }
    //     }
    // }

    // if (monitor) {
    //     gdk_monitor_get_geometry(monitor, &geometry);
    //     screen_width = geometry.width;
    //     screen_x_offset = geometry.x;
    //     screen_y_offset = geometry.y;
    // } else {
    //     g_warning("Could not determine monitor geometry. Using default test size and position.");
    //     // Using fixed test values defined above
    // }

    gtk_window_set_default_size(GTK_WINDOW(window), screen_width, panel_height);
    // gtk_window_move(GTK_WINDOW(window), screen_x_offset, screen_y_offset); // Commented out

    // Create the time label
    widgets->time_label = GTK_LABEL(gtk_label_new(NULL));
    gtk_widget_set_name(GTK_WIDGET(widgets->time_label), "time_label"); // Name for CSS targeting
    gtk_widget_set_halign(GTK_WIDGET(widgets->time_label), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(widgets->time_label), GTK_ALIGN_CENTER);

    // Use GtkCenterBox to center the label within the window
    center_box = gtk_center_box_new();
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(center_box), GTK_WIDGET(widgets->time_label));
    gtk_window_set_child(GTK_WINDOW(window), center_box);

    // Set initial time and start periodic updates every second
    update_time(widgets);
    g_timeout_add_seconds(1, update_time, widgets);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    // Allocate memory for AppWidgets structure
    AppWidgets *widgets = g_new0(AppWidgets, 1);

    app = gtk_application_new("org.wave.WPanel", G_APPLICATION_DEFAULT_FLAGS); // Changed application ID
    g_signal_connect(app, "activate", G_CALLBACK(activate_application), widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);
    g_free(widgets); // Free the allocated AppWidgets structure

    return status;
}
