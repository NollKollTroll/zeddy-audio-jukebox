//Written in 2022 by Adam Klotblixt (adam.klotblixt@gmail.com)
//
//To the extent possible under law, the author have dedicated all
//copyright and related and neighboring rights to this software to the
//public domain worldwide.
//This software is distributed without any warranty.
//
//You should have received a copy of the CC0 Public Domain Dedication
//along with this software. If not, see
//<http://creativecommons.org/publicdomain/zero/1.0/>.

format zx81
;labelusenumeric
;LISTOFF
	// hardware options to be set and change defaults in ZX81DEF.INC
	MEMAVL	   EQU	   MEM_16K	   // can be MEM_1K, MEM_2K, MEM_4K, MEM_8K, MEM_16K, MEM_32K, MEM_48K
	STARTMODE  EQU	   SLOW_MODE	   // SLOW or FAST
	DFILETYPE  EQU	   EXPANDED	   // COLLAPSED or EXPANDED or AUTO
	include '..\..\SINCL-ZX\ZX81.INC'  // definitions of constants
;LISTON
AUTOLINE 0
	REM RAW_AUDIO_JUKEBOX_V0.2
	REM RUN_TO_LISTEN
	REM SPACE_->_NEXT_SONG
	REM _noedit _asm
	include 'zajraw.inc'
	END _asm

AUTOLINE 10
100	REM playlist begin
	LET N$ = "OCEAN2.RAW "	//The filename, DO NOT leave out the trailing space
	LET B = 22658		//Number of 256-sample blocks to play, max 65535
	GOSUB 1000		//Prepare and play the song
	LET N$ = "MONTY.RAW "
	LET B = 30822
	GOSUB 1000
998	REM playlist end
999	GOTO 100		//Repeat the playlist

1000	SCROLL
	PRINT AT 21,0; N$, B
	//Move the info about the file to the assembler domain before playing
	LET C = INT(B / 256)
	LET D = B - (C * 256)
	POKE #BlocksToPlay + 1, C
	POKE #BlocksToPlay, D
	FOR I = 1 TO LEN N$
	LET J = (#FileName - 1) + I
	POKE J,CODE N$(I)
	NEXT I
	//Make sure there is no key pressed before continuing
WAIT_NO_KEY:
	IF INKEY$<>"" THEN GOTO #WAIT_NO_KEY#
	//Finally play it
	RAND USR #Main
	RETURN

//include D_FILE and needed memory areas
include '..\..\SINCL-ZX\ZX81DISP.INC'

VARS_ADDR:
	db 80h //DO NOT REMOVE!!!

WORKSPACE:

assert ($-MEMST)<MEMAVL
// end of program
