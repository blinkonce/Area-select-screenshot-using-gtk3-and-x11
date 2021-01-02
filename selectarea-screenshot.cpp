#include<gtk/gtk.h>
#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>


static void buttonclick(int *x, int *y, int *width, int *height){
	int init_x, init_y, fin_x, fin_y;
	Display *xdisplay = XOpenDisplay(0);
	XEvent xevent;
	Window xroot = DefaultRootWindow(xdisplay);

	XGrabPointer(xdisplay, xroot, 1, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	
	printf("select an area\n");
	while(1){

		XNextEvent(xdisplay, &xevent);

		if(xevent.type == ButtonPress){
			init_x = xevent.xmotion.x_root;
			init_y = xevent.xmotion.y_root;
			printf("drawing area: init_x = %d, init_y =%d\n",init_x,init_y);
		}
		else if(xevent.type == ButtonRelease){
			fin_x = xevent.xmotion.x_root;
			fin_y = xevent.xmotion.y_root;
			XFlush(xdisplay);
			XCloseDisplay(xdisplay);
			printf("drawing limit: fin_x = %d, fin_y = %d\n",fin_x,fin_y);
			break;
		}	
	}

	*x = init_x;
	*y = init_y;
	*width = fin_x - init_x;
	*height = fin_y - init_y;

	printf("height, %d and width, %d\n",*height,*width);
	if(*width <0 || *height < 0){	
		*x = fin_x;
		*y = fin_y;
		*width = init_x - fin_x;
		*height = init_x -fin_x;
		printf("inverted\n");
	}else{
		printf("uninverted\n");}
}

static void take_pic(){
	GdkWindow *root_win;
	GdkPixbuf *screen;
	int x, y, width, height;

	buttonclick(&x, &y, &width, &height);
	printf("topleft x = %d, topleft y = %d, width = %d, height = %d", x, y, width, height);
	root_win = gdk_get_default_root_window();
	screen = gdk_pixbuf_get_from_window(root_win, x, y, width, height);
	gdk_pixbuf_save(screen, "saved_img/out.jpeg", "jpeg", NULL, "quality", "100", NULL);
	g_print("clicked");
}


static void activate(GtkApplication* app, gpointer user_data){
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *buttonbox;

	window = gtk_application_window_new(app);
//	gtk_window_set_keep_above(GTK_WINDOW(window), 1);
	gtk_window_set_title(GTK_WINDOW(window), "Translator");
	gtk_window_set_default_size(GTK_WINDOW(window),500,300);

	button = gtk_button_new_with_label("take screenshot");
	buttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(buttonbox), button);
	gtk_container_add(GTK_CONTAINER(window),buttonbox);
	g_signal_connect(button,"clicked",G_CALLBACK(take_pic),NULL);
	gtk_widget_show_all(window);
}

int main (int argc, char **argv){
	GtkApplication *app;
	int status;
	
	app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}
