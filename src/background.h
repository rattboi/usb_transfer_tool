
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <gctypes.h>

extern u8 *picTVBuf;
extern u8 *picDRCBuf;

u8* LoadPicture(char* name, u32 size);
int LoadPictures();

void DrawBackground(int screen);


#ifdef __cplusplus
}
#endif

#endif /* _FTP_H_ */
