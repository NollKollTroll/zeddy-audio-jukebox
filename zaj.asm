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
	AUTOLINE 10
	REM _hide _asm

;---------------------------------------------------------------
FileName_00	dbzx	'audio00.zaj', 0
BLOCKS_00	EQU	5277

FileName_01	dbzx	'audio01.zaj', 0
BLOCKS_01	EQU	11601  / 2

FileName_02	dbzx	'audio02.zaj', 0
BLOCKS_02	EQU	26837 / 2

FileName_03	dbzx	'audio03.zaj', 0
BLOCKS_03	EQU	21813 / 2

;---------------------------------------------------------------
Main:
	//NMI off
	out	($FD),a
MainLoop:
	ld	de,FileName_00
	ld	bc,BLOCKS_00
	call	PlayFile

	ld	de,FileName_01
	ld	bc,BLOCKS_01
	call	PlayFile

	ld	de,FileName_02
	ld	bc,BLOCKS_02
	call	PlayFile

	ld	de,FileName_03
	ld	bc,BLOCKS_03
	call	PlayFile

	jp	MainLoop

	include 'zaj.inc'

	END _asm

AUTORUN:
	RAND USR #Main

//include D_FILE and needed memory areas
include '..\..\SINCL-ZX\ZX81DISP.INC'

VARS_ADDR:
	db 80h //DO NOT REMOVE!!!

WORKSPACE:

assert ($-MEMST)<MEMAVL
// end of program
