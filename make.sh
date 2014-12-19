#!/bin/sh
#
# No need for a complex Makefile.
#
gcc -Wall -g tyreese.c initialize.c tag.c `pkg-config --cflags gtk+-3.0 gtksourceview-3.0 pango` `pkg-config --libs gtk+-3.0 pango` -lgtksourceview-3.0 -o tyreese 
