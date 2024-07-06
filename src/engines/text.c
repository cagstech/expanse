#include <graphx.h>

#include "text.h"

/** Text defaults: local console, background = black, foreground = white */
struct _text text = {take_console, 0, 0};

/**
 * TAKE_CONSOLE
 * the intent here is to render text in a console style starting at the top
 * once at the bottom text, should start shifting up with newlines being appended at bottom.
 * text input routines start at the next newline and have the same shifting effect
 * if a text input should occur with a full screen, it should shift text to make room for the input
 * multi-line inputs should push up to accomodate.
 */
void text_cout(const char *text)
{
}

void text_ncsend(const char *text)
{
}