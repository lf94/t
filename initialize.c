#include "gtkcomponents.h"
#include "tyreese.h"

GtkWidget *
init_window(FileContainer *file_container) {
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), file_container->path);
	set_class(window, "window");
	return window;
}

GtkBox *
init_box(int GTK_ORIENTATION, gboolean homogeneous) {
	GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION, 0));
	gtk_box_set_homogeneous(box, homogeneous);
	return box;
}

GtkEntry *
init_tag() {
	GtkWidget *tag = gtk_entry_new();
	set_class(tag, "tag");
	return GTK_ENTRY(tag);
}


GtkScrolledWindow *
init_scroll_container() {
	GtkWidget *scroll_container = gtk_scrolled_window_new(NULL, NULL);
	set_class(scroll_container, "editor-window");
	gtk_scrolled_window_set_policy(
		GTK_SCROLLED_WINDOW(scroll_container), 
		GTK_POLICY_NEVER, 
		GTK_POLICY_ALWAYS
	);
	
	return GTK_SCROLLED_WINDOW(scroll_container);
}

GtkSourceBuffer *
init_buffer(FileContainer *file_container) {
	GtkSourceBuffer *source_buffer = gtk_source_buffer_new(NULL);
	gtk_source_buffer_set_highlight_matching_brackets(source_buffer, FALSE);
	
	GtkTextBuffer *text_buffer = GTK_TEXT_BUFFER(source_buffer);
	gtk_text_buffer_set_text(
		text_buffer, 
		file_container->data, 
		file_container->size
	);
	GtkTextIter start_of_buffer;
	gtk_text_buffer_get_start_iter(text_buffer, &start_of_buffer);
	gtk_text_buffer_place_cursor(text_buffer, &start_of_buffer);
	gtk_text_buffer_set_modified(text_buffer, FALSE);
	
	return source_buffer;
}

GtkSourceView *
init_buffer_view(GtkSourceBuffer *source_buffer) {
	GtkSourceView *source_view = GTK_SOURCE_VIEW(gtk_source_view_new_with_buffer(source_buffer));
	set_class(GTK_WIDGET(source_view), "editor");
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(source_view), 4);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(source_view), GTK_WRAP_CHAR);
	gtk_source_view_set_draw_spaces(source_view, GTK_SOURCE_DRAW_SPACES_TAB);
	gtk_source_view_set_auto_indent(source_view, TRUE);
	gtk_source_view_set_indent_on_tab(source_view, TRUE);
	//gtk_source_view_set_show_line_numbers(source_view, TRUE);
	
	return source_view;
}

EditorState *
initialize(int *argc, char **argv[]) {
	gtk_init(argc, argv);

	FileContainer* file_container = read_file((*argv)[1]);

	load_css("/home/lee/dev/src/tyreese/styles/style.css");

	/* INITIALIZE COMPONENTS */
	GtkWidget *main_window = init_window(file_container);
	GtkBox *horizontal_box = init_box(GTK_ORIENTATION_HORIZONTAL, TRUE);
	GtkBox *vertical_box = init_box(GTK_ORIENTATION_VERTICAL, FALSE);
	
	GtkEntry *tag = init_tag();

	GtkContainer *scroll_container = GTK_CONTAINER(init_scroll_container());
	GtkSourceBuffer *source_buffer = init_buffer(file_container);
	GtkSourceView *source_view = init_buffer_view(source_buffer);
	GtkSourceSearchSettings *search_settings = gtk_source_search_settings_new();
	gtk_source_search_settings_set_regex_enabled (search_settings, TRUE);

	/* PACK COMPONENTS */
	gtk_container_add(scroll_container, GTK_WIDGET(source_view));
	gtk_box_pack_start(vertical_box, GTK_WIDGET(tag), FALSE, FALSE, 0);
	gtk_box_pack_start(vertical_box, GTK_WIDGET(scroll_container), TRUE, TRUE, 0);
	gtk_widget_set_size_request(GTK_WIDGET(main_window), 480, 768-24);
	gtk_box_pack_start(horizontal_box, GTK_WIDGET(vertical_box), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(horizontal_box));
	gtk_widget_grab_focus(GTK_WIDGET(source_view));

	/* INITIALIZE EDITOR STATE */
	EditorState *editor_state = malloc(sizeof(EditorState));
	editor_state->root = main_window;
	editor_state->view = source_view;
	editor_state->buffer = source_buffer;
	editor_state->search.settings = search_settings;
	editor_state->search.context = NULL;
	editor_state->mode = NO_MODE;
	editor_state->tag = tag;
	editor_state->fc = file_container;
	
	/* BIND EVENTS */
	g_signal_connect(editor_state->root, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(editor_state->root, "key-press-event", G_CALLBACK(window_key_press), editor_state);
	g_signal_connect(editor_state->tag, "key-press-event", G_CALLBACK(tag_key_press), editor_state);
	
	return editor_state;
}
