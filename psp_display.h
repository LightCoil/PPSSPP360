#pragma once

void Display_Init();
void Display_Shutdown();
int  Display_GetWidth();
int  Display_GetHeight();
void Display_ReadPixels(uint8_t *outRGBA);
