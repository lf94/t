#include "gtkcomponents.h"
#include "tyreese.h"

void
toggle_tag(EditorState *state) {
	if(gtk_widget_is_focus(GTK_WIDGET(state->tag)) == FALSE) {
		gtk_widget_show(GTK_WIDGET(state->tag));
		gtk_widget_grab_focus(GTK_WIDGET(state->tag));
	} else {
		gtk_widget_hide(GTK_WIDGET(state->tag));
		gtk_widget_grab_focus(GTK_WIDGET(state->view));
	}
}

void
select_range(EditorState *state, GtkTextIter *start, GtkTextIter *end) {
	gtk_text_buffer_select_range(GTK_TEXT_BUFFER(state->buffer), start, end);
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(state->view), gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(state->buffer)), 0.0, TRUE, 0.0, 0.17);
	
}

void
next_occurrence(EditorState *state) {
	GtkTextIter start, end;
	gboolean found = gtk_source_search_context_forward(state->search.context, &state->search.range.end, &start, &end);
	
	if(found) {
		select_range(state, &start, &end);
		state->search.range.start = start;
		state->search.range.end = end;
	} else {
	}
}

void
change_occurrence(EditorState *state) {
	state->mode = MODE_TYPING;
}

void
append_to_occurrence(EditorState *state) {
	state->mode = MODE_TYPING;
}

void
insert_before_occurrence(EditorState *state) {
	state->mode = MODE_TYPING;
}

void
delete_occurrence(EditorState *state) {
}

void
disengage_search_mode(EditorState *state) {
	gtk_source_search_context_set_highlight(state->search.context, FALSE);
	state->mode = NO_MODE;
}

void
execute_command(const gchar cmd, EditorState *state) {
	if(state->search.times == 0)
		state->search.times = 1;
	unsigned int i = 0;
	for(i = 0; i < state->search.times; i++) {
		switch(cmd) {
			case 'n':
				next_occurrence(state);
				break;
			case 'a':
			case 'i':
			case 'd':
			case 'c':
				break;
			default:
				break;
		}
	}
	state->search.times = 0;
}

void
engage_search_mode(EditorState *state, const gchar *regexp) {
	gtk_source_search_context_set_highlight(state->search.context, TRUE);
	gtk_source_search_settings_set_search_text (state->search.settings, &regexp[1]);
	GtkSourceSearchContext *search_context = gtk_source_search_context_new(state->buffer, state->search.settings);
	GtkTextIter current_position;
	GtkTextMark *insert_mark = gtk_text_buffer_get_mark(GTK_TEXT_BUFFER(state->buffer), "insert");
	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(state->buffer), &current_position, insert_mark);
	state->search.context = search_context;
	state->search.range.end = current_position;
	state->mode = MODE_SEARCH;
	state->search.times = 0;
}


void
go_to_line(EditorState *state, const gchar *linenumber) {
	GtkTextIter start;
	gint line_number = g_ascii_strtoll(&linenumber[1], NULL, 10);
	gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(state->buffer), &start, line_number);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(state->view), &start, 0.0, FALSE, 0.0, 0.5);
	select_range(state, &start, &start);
}


gboolean
execute_os_cmd(EditorState *state) {
	const gchar *cmd = gtk_entry_get_text(state->tag);
	if(strlen(cmd) < 0)
		return FALSE;
	
	if(cmd[0] == '/') {
		engage_search_mode(state, cmd);
	} else if(cmd[0] == ':') {
		go_to_line(state, cmd);
	}
	
	return TRUE;
}




gboolean
tag_key_press(GtkWidget *widget, GdkEvent *event, gpointer data) {
	EditorState *state = (EditorState *) data;
	GdkEventKey key_pressed = event->key;
	switch(key_pressed.keyval) { 
		case GDK_KEY_Return:
			if(execute_cmd(state)) {
				toggle_tag(state);
			}
			return TRUE;
		default:
			break;
	}
	return FALSE;
}
