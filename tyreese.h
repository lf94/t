/*
	Simplify the mayhem with structure.
*/
#include <stdlib.h>
#include <string.h>

enum {
	NO_MODE,
	MODE_TYPING,
	MODE_SEARCH,
	ALL_MODES
};

typedef struct {
	char *path;
	gchar *data;
	unsigned int size;
} FileContainer;

typedef struct {
	GtkTextIter start, end;
} GtkIterRange;

typedef struct {
	GtkSourceSearchSettings *settings;
	GtkSourceSearchContext *context;
	GtkIterRange range;
	unsigned int times;
} GtkSearch;

typedef struct {
	GtkWidget *root;
	GtkSourceView *view;
	GtkSourceBuffer *buffer;
	GtkSearch search;
	unsigned int mode;
	GtkEntry *tag;
	FileContainer *fc;
} EditorState;

FileContainer * read_file(char *);
gboolean write_file(FileContainer *, GtkTextView *, GtkTextBuffer *);
void load_css(char *);
void set_class(GtkWidget *, const gchar *);
gboolean window_key_press(GtkWidget *, GdkEvent *, gpointer);
gboolean execute_cmd(EditorState *);

#include "initialize.h"
#include "tag.h"
