GtkWidget * init_window();
GtkBox * init_box();
GtkEntry * init_tag();
GtkScrolledWindow * init_scroll_container();
GtkSourceBuffer * init_buffer();
GtkSourceView * init_buffer_view(GtkSourceBuffer *);
EditorState * initialize(int *argc, char **argv[]);
