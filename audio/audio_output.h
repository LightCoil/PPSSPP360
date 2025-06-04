#pragma once

void AudioOutput_Init();
void AudioOutput_Shutdown();
void AudioOutput_PlayPCM(const int16_t *pcmData, int samples);
