
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <gctypes.h>

u8* LoadPicture(char* name, u32 size);
void LoadPictures();
void UnloadPictures();

void DrawBackground(int screen);


#ifdef __cplusplus
}
#endif

#endif /* _FTP_H_ */
