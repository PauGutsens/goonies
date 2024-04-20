#include <music.h>
void playMusic(const char* musicFile)
{
  InitAudioiDevice();
PlayMusicStream(LoadMusicStream(musicFile));
}
