#ifndef text_h
#define text_h

#include <stdint.h>
#include <graphx.h>

/** Defines configuration for text output. */
struct text
{
    void (*takefunc)(const char *line);
    uint8_t background, foreground;
    uint24_t textx;
    uint8_t texty;
    gfx_region_t window;
    char data[];
};

struct text *text_alloc_context();

void text_cout(struct text *ctx, const char *text);
void text_ncsend(struct text *ctx, const char *text);
void text_ncblurb(struct text *ctx, const char *text);

void text_input(struct text *ctx, char *buf, uint8_t type, bool async); // asm

#endif