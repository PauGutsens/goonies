#include <music.h>
void playMusic(const char* musicFile)
{
  InitAudioiDevice();
PlayMusicStream(LoadMusicStream(//Nombre del archivo de musica));
}
