#include "gtkcomponents.h"
#include "tyreese.h"

void
event_loop(EditorState *state) {
	gtk_widget_show_all(state->root);
	gtk_widget_hide(GTK_WIDGET(state->tag));
	
	gtk_main();
}

FileContainer *
read_file(char *filename) {
	FILE *input = fopen(filename, "r");
	FileContainer *fc = (FileContainer *)malloc(sizeof(FileContainer));
	fc->path = filename;
	fseek(input, 0, SEEK_END);
	if(input == NULL) {
		fc->size = 0;
		fc->data = NULL;
	} else {
		fc->size = ftell(input);
		fseek(input, 0, SEEK_SET);
		fc->data = (gchar *) malloc(fc->size);
		int bytes_read = fread(fc->data, 1, fc->size, input);
		fclose(input);
		if(bytes_read != fc->size) {
			free(fc);
			return NULL;
		}
	}
	return fc;
}

gboolean
write_file(FileContainer *fc, GtkTextView *view, GtkTextBuffer *buffer) {
	gtk_widget_set_sensitive(GTK_WIDGET(view), FALSE);
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	fc->data = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	gtk_text_buffer_set_modified(buffer, FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(view), TRUE);

	FILE *fp = fopen(fc->path, "w");
	if(fp == NULL) {
		fprintf(stderr, "Could not write to file (file pointer returned NULL)\n");
		return FALSE;
	}
	
	fprintf(fp, "%s", fc->data);
	fclose(fp);

	return TRUE;
}


void
load_css(char *style_path) {
	GtkCssProvider *css_provider = gtk_css_provider_get_default();
	GFile *css_file = g_file_new_for_path(style_path);
	GError *error = NULL;
	gtk_css_provider_load_from_file(css_provider, css_file, &error);
	g_object_unref(css_file);

	if(error != NULL) {
		printf("CSS parsing error: %s\n", error->message);
	}

	gtk_style_context_add_provider_for_screen(
		gdk_screen_get_default(), 
		(GtkStyleProvider *)css_provider, 
		GTK_STYLE_PROVIDER_PRIORITY_USER
	);
}

void
set_class(GtkWidget *widget, const gchar *class_name) {
	gtk_style_context_add_class(
		gtk_widget_get_style_context(widget), 
		class_name
	);
}

gboolean
window_key_press(GtkWidget *widget, GdkEvent *event, gpointer data) {
	EditorState *state = (EditorState *) data;
	GdkEventKey key_pressed = event->key;
	if(key_pressed.state & GDK_CONTROL_MASK) {
		switch(key_pressed.keyval) { 
			case GDK_KEY_q:
				gtk_main_quit();
				return TRUE;
				
			case GDK_KEY_s:
				write_file(
					state->fc, 
					(GtkTextView *) state->view, 
					(GtkTextBuffer *) state->buffer
				);
				gtk_widget_grab_focus(GTK_WIDGET(state->view));
				return TRUE;
			
			case GDK_KEY_t:
				toggle_tag(state);
				return TRUE;
			default:
				break;
		}
	} else if (state->mode == MODE_SEARCH) {
		printf("search mode ACTIVE\n");
	} else if (state->mode == MODE_TYPING) {
		if(key_pressed.state & GDK_CONTROL_MASK) {
			switch(key_pressed.keyval) {
				case GDK_KEY_t:
					state->mode = MODE_SEARCH;
					return TRUE;
			}
		} else {
			//state->search.func(key_pressed.keyval);
			return TRUE;
		}
	}
	return FALSE;
}

gboolean
execute_cmd(EditorState *state) {
	const gchar *cmd = gtk_entry_get_text(state->tag);
	if(strlen(cmd) < 0)
		return FALSE;
	
	if(cmd[0] == '/') {
		engage_search_mode(state, cmd);
	} else {
		go_to_line(state, cmd);
	}
	
	return TRUE;
}


int
main(int argc, char *argv[]) {
	EditorState *state = initialize(&argc, &argv);
	if(state) {
		event_loop(state);
	}
	return 1;
}
