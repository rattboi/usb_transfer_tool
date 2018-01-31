#include "fs/fs_utils.h"
#include "fs/sd_fat_devoptab.h"
#include "background.h"
#include <gctypes.h>
#include "dynamic_libs/os_functions.h"
#include "system/memory.h"

#define TV_WIDTH 1280
#define DRC_WIDTH 854
#define TV_HEIGHT 720
#define DRC_HEIGHT 480

const int SIZES[][2] = {{TV_WIDTH, TV_HEIGHT}, {DRC_WIDTH, DRC_HEIGHT}};

u8 *picTVBuf = NULL;
u8 *picDRCBuf = NULL;

u8 *LoadPicture(char *name, u32 size)
{
    u8 *buf = NULL;
    u32 fileSize = 0;

    LoadFileToMem(name, &buf, &fileSize);
    if (fileSize < size)
    {
        free(buf);
        return NULL;
    }

    return buf;
}

void LoadPictures()
{
    picTVBuf = LoadPicture("sd://res/tvBack.tga", TV_WIDTH * TV_HEIGHT * 3 + 18);
    picDRCBuf = LoadPicture("sd://res/drcBack.tga", DRC_WIDTH * DRC_HEIGHT * 3 + 18);
}

void DrawBackground(int screen)
{
    char r = 0;            //rouge
    char g = 0;            //vert
    char b = 0;            //bleu
    const char a = 255;    //alpha ne pas changer la valeur
    unsigned int pos = 18; //saut de l'entete du header TGA

    u8 *buffer = (screen == 0 ? picTVBuf : picDRCBuf);

    if (buffer == NULL)
        return;
    //boucle d'affichage de la jaquette
    for (int y = 0; y < SIZES[screen][1]; y++)
    {
        for (int x = 0; x < SIZES[screen][0]; x++)
        {
            b = *(buffer + pos);
            g = *(buffer + pos + 1);
            r = *(buffer + pos + 2);
            pos += 3;

            uint32_t num = (r << 24) | (g << 16) | (b << 8) | a;
            OSScreenPutPixelEx(screen, x, SIZES[screen][1] - y, num);
        }
    }
}
