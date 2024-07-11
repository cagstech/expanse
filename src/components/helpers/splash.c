
#include <stdbool.h>
#include <stdint.h>
#include <sys/util.h>

#include <graphx.h>

struct star
{
    uint24_t x;
    uint8_t y;
    uint8_t r;
    uint8_t hue;
};

#define SPLASH_STARFIELD_NUM 100
#define STARFIELD_XMIN 0
#define STARFIELD_YMIN 0
#define STARFIELD_XMAX 320
#define STARFIELD_YMAX 210
#define POSSIBLE_STAR_HUES 7
uint8_t star_hues[POSSIBLE_STAR_HUES] = {255, 223, 191, 255, 247, 215, 255};

void splash_RenderStarfield(void)
{
    gfx_ZeroScreen();
    static bool starfield_generated = false;
    static struct star starfield[SPLASH_STARFIELD_NUM];
    if (!starfield_generated)
    {
        for (uint24_t i = 0; i < SPLASH_STARFIELD_NUM; i++)
        {
            starfield[i].x = randInt(STARFIELD_XMIN, STARFIELD_XMAX);
            starfield[i].y = randInt(STARFIELD_YMIN, STARFIELD_YMAX);
            starfield[i].r = randInt(0, 1);
            starfield[i].hue = star_hues[randInt(1, POSSIBLE_STAR_HUES) - 1];
        }
        starfield_generated = true;
    }
    for (uint24_t i = 0; i < SPLASH_STARFIELD_NUM; i++)
    {
        gfx_SetColor(starfield[i].hue);
        gfx_FillCircle(starfield[i].x, starfield[i].y, starfield[i].r);
    }
    gfx_SetColor(10);
    gfx_Rectangle(STARFIELD_XMIN + 5, STARFIELD_YMAX + 4, STARFIELD_XMAX - 10, 26);
    gfx_SetTextScale(3, 3);
    gfx_PrintStringXY("EXPANSE", 10, 10);
    gfx_SetTextScale(1, 1);
    gfx_BlitBuffer();
}

void splash_ConsoleClearLine(void)
{
    gfx_SetColor(0);
    gfx_FillRectangle(STARFIELD_XMIN + 6, STARFIELD_YMAX + 5, STARFIELD_XMAX - 12, 24);
    gfx_BlitRectangle(gfx_buffer, STARFIELD_XMIN + 6, STARFIELD_YMAX + 5, STARFIELD_XMAX - 12, 24);
}
void splash_ConsolePrintLine(const char *line)
{
    splash_ConsoleClearLine();
    gfx_PrintStringXY(line, STARFIELD_XMIN + 10, STARFIELD_YMAX + 12);
    gfx_BlitRectangle(gfx_buffer, STARFIELD_XMIN + 6, STARFIELD_YMAX + 5, STARFIELD_XMAX - 12, 24);
}