# Zeddy Audio Jukebox

Play audio using a ZX81 and a ZXpand classic or plus.

## General info

The software plays a song as 1-bit audio on the tape output and 4-bit logarithmic audio on the soundchip on the ZXpand or an additional ZON-X.<br>
All the files are played in order and loops. Pressing BREAK jumps to the next file.<br>
**WARNING:** the sound is full of high frequency noise, DO NOT listen to this at high volumes or your hearing might be damaged! This is not a hifi reproduction, merely a curiosity.<br>
There is also a version that can use a simple R2R-DAC connected on port B of the sound chip to play raw 8-bit files. This sounds MUCH better!

## Changing the playlist

The playlist can be changed easily to accomodate your own audio files. The player code is only 2 files and builds using PokeMon's utility ZX-IDE available at http://sinclairzxworld.com/viewtopic.php?f=6&t=1064<br>
The small changes needed are in the file zaj.asm, located between the horizontal lines.

## Transcoding / ZAJ

The required audio files can be created by following this process:

1. Convert the input file test.mp3 to a raw 15700 Hz audio file, 8-bit signed, mono, named audio03.raw:<br>
sox test.mp3 -t raw -r 15700 -b 8 -e signed-integer -c 1 audio03.raw

2. Run the raw file through the python conversion utility:<br>
python3 audio_convert.py audio00.raw audio00.zaj

The converter prints the number of blocks the audio file uses, which is needed in the ZX81 playlist.

It's also possible to preview the 4-bit audio on your PC:<br>
python3 audio_convert.py -p 4 audio00.raw test.raw<br>
play -t raw -c 1 -r 15700 -b 8 -e signed-integer test.raw

Or the 1-bit audio:<br>
python3 audio_convert.py -p 1 audio00.raw test.raw<br>
play -t raw -c 1 -r 15700 -b 8 -e signed-integer test.raw

The play command is part of sox.

There are command line options for changing the dither and error diffusion in the converter, try them out for different/better results.

## Transcoding / RAW

The required audio files can be created by following this process:

1. Convert the input file test.mp3 to a raw 22109 Hz audio file, 8-bit unsigned, mono, named audio03.raw:<br>
sox test.mp3 -t raw -r 22109 -b 8 -e unsigned-integer -c 1 audio03.raw

2. Calculate the number of blocks to play by taking the file-length divided by 256. The integer part without rounding is the number of blocks to use in the playlist.

The replay rate of 22109 Hz is less than 0.27% faster than 22050 Hz, so it is OK to use that if converting sound by some other means.

## Emulators

My trusty go-to emulator EightyOne v1.29 failed to play the files, regardless of the settings I tested.<br>
If you want to listen to crappy audio, use a real ZX81!

## History

I recently released a streaming video player for the ZX81 which made me realize that there was no audio player. This is my proof of concept.

## Future

The 1-bit tape output needs to be synchronized with the automatic hsyncs on the ZX81 to sound good, which is why 15700 Hz is used. It also looks nice!<br>
A PSG-only player could play files at more than 20 kHz.<br>
The ZX80 does not have automatic hsyncs, so the sample rate could be raised on a 1-bit player as well.<br>
The quality of the converted sound files could be improved.<br>
All this is work for someone else. I have shown it is possible to stream audio on the ZX81, now make something (better) with it!

## License

The code I created for this project is licensed under Creative Commons CC0 1.0 Universal.<br>
The music was released by Kevin MacLeod, licensed under Creative Commons Attribute 3.0. See the file music.txt for details.

## Contact

Mail: <adam.klotblixt@gmail.com><br>
Github: <https://github.com/NollKollTroll/zeddy-audio-jukebox>
