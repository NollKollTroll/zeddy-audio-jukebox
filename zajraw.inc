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

;---------------------------------------------------------------
align 256
SAMPLE_BUF_A db 32 dup(0,0,0,0,15,15,15,15)
SAMPLE_BUF_B db 128 dup(0,15)

SAMPLE_BUF_A_HI EQU SAMPLE_BUF_A / 256
SAMPLE_BUF_B_HI EQU SAMPLE_BUF_B / 256

PRT0	EQU	$0007
PRT1	EQU	$2007
PRT2	EQU	$4007
PRT3	EQU	$6007
PRT4	EQU	$8007
PRT5	EQU	$a007
PRT6	EQU	$c007
PRT7	EQU	$e007

AY_REG	EQU	$DF
AY_DATA EQU	$0F

BlocksToPlay	dw	0
FileName	dbzx	'12345678.RAW '

;---------------------------------------------------------------
Main:
	//NMI off
	out	($FD),a
	push	ix
	push	iy
	push	bc
	push	de
	push	hl
	push	af
	exx
	ex	af,af
	push	bc
	push	de
	push	hl
	push	af


	ld	de,FileName
	call	PlayFile

	pop	af
	pop	hl
	pop	de
	pop	bc
	ex	af,af
	exx
	pop	af
	pop	hl
	pop	de
	pop	bc
	pop	iy
	pop	ix
	//NMI on
	out	($fe),a
	ret

;---------------------------------------------------------------
//input:
//  DE points to zero-terminated filename
PlayFile:
	//prepare buffer for write
	ld	bc,PRT0
	ld	a,$ff
	out	(c),a
	//send file-name characters to buffer, zero-terminated
	ld	bc,PRT2
	.NameOutLoop:
		ld	a,(de)
		out	(c),a
		inc	de
		and	a
	jp	nz,.NameOutLoop
	//open file for read
	ld	bc,PRT4
	xor	a
	out	(c),a
	//get response code
	.DelayStart:
		repeat 32
			nop
		end repeat
		in	a,(c)
	jp	m,.DelayStart
	cp	$40
	//maybe do something better for error handling...
	jp	nz,RestartStreaming

	//InitPSG
	//AY reg_0 = 0, vol A lo
	ld	a,0
	out	(AY_REG),a
	ld	a,0
	out	(AY_DATA),a
	//AY reg_1 = 0, vol A hi
	ld	a,1
	out	(AY_REG),a
	ld	a,0
	out	(AY_DATA),a
	//AY reg_7, I/O & mixer
	ld	a,7
	out	(AY_REG),a
	ld	a,%10111111
	out	(AY_DATA),a
	//pre-select reg_f
	ld	a,$0f
	out	(AY_REG),a
	ld	a,0
	out	(AY_DATA),a
	//init read statemachine
	ld	ix,RequestRead256_A
	ld	de,(BlocksToPlay)
	exx
	//init play statemachine
	ld	iy,PlayBuf_A
	ld	hl,SAMPLE_BUF_A
	//start the streaming
	jp	(ix)

RestartStreaming:
	//set red led
	ld	bc,PRT7
	ld	a,$b8
	out	(c),a
	//close file
	ld	bc,PRT4
	ld	a,$80
	out	(c),a
	//get response code
	.DelayStart:
		repeat 32
			nop
		end repeat
		in	a,(c)
	jp	m,.DelayStart
	//wait until break is not pressed
.WaitKeyOff:
	ld	b,0
.delay:
	djnz	.delay
	ld	a,$7f
	in	a,($fe)
	and	1
	jp	z,.WaitKeyOff
	//clear red led
	ld	bc,PRT7
	ld	a,$b9
	out	(c),a
	ret

;---------------------------------------------------------------
RequestRead256_A:
	exx				//4
	ld	bc,PRT5 		//10
	ld	a,0			//7
	out	(c),a			//12
	ld	ix,GetResponseCode_A	//14
	repeat 5
		nop				//4 waste cycles
	end repeat
	exx				//4
	jp	(iy)			//8 -> 79

RequestRead256_B:
	exx				//4
	ld	bc,PRT5 		//10
	ld	a,0			//7
	out	(c),a			//12
	ld	ix,GetResponseCode_B	//14
	repeat 5
		nop				//4 waste cycles
	end repeat
	exx				//4
	jp	(iy)			//8 -> 79

;--------
GetResponseCode_A:
	exx				//4
	ld	bc,PRT5 		//10
	in	a,(c)			//12
	jp	m,.TryAgain		//10/10
	cp	$40			//7
	jp	nz,RestartStreaming	//10/10 major problem, hard restart
	ld	ix,PrepareReadMagic_A	//14
	exx				//4
	jp	(iy)			//8 -> 79
.TryAgain:
	repeat 6
		nop				//4 waste cycles
	end repeat
	ld	a,0			//7 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79

GetResponseCode_B:
	exx				//4
	ld	bc,PRT5 		//10
	in	a,(c)			//12
	jp	m,.TryAgain		//10/10
	cp	$40			//7
	jp	nz,RestartStreaming	//10/10 major problem, hard restart
	ld	ix,PrepareReadMagic_B	//14
	exx				//4
	jp	(iy)			//8 -> 79
.TryAgain:
	repeat 6
		nop				//4 waste cycles
	end repeat
	ld	a,0			//7 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79

;--------
PrepareReadMagic_A:
	exx				//4
	ld	bc,PRT0 		//10
	ld	a,42			//7
	out	(c),a			//12
	ld	ix,WaitForPlayBufDone_A //14
	repeat 5
		nop				//4 waste cycles
	end repeat
	exx				//4
	jp	(iy)			//8 -> 79

PrepareReadMagic_B:
	exx				//4
	ld	bc,PRT0 		//10
	ld	a,42			//7
	out	(c),a			//12
	ld	ix,WaitForPlayBufDone_B //14
	repeat 5
		nop				//4 waste cycles
	end repeat
	exx				//4
	jp	(iy)			//8 -> 79

;--------
WaitForPlayBufDone_A:
	ld	a,h			//4 NOTE: HL from Play statemachine
	exx				//4
	cp	SAMPLE_BUF_A_HI 	//7
	jp	nz,.NextBuffer		//10/10
.CheckBreak:
	ld	a,$7f			//7
	in	a,($fe) 		//11
	rra				//4
	jp	nc,RestartStreaming	//10/10 restart if break key pressed
	ld	bc,0			//10 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.NextBuffer:
	ld	ix,ReadDataToBuf_A	//14
	ld	hl,SAMPLE_BUF_A 	//10
	ld	bc,PRT0 		//10
	repeat 2
		nop				//4 waste cycles
	end repeat
	exx				//4
	jp	(iy)			//8 -> 79

WaitForPlayBufDone_B:
	ld	a,h			//4 NOTE: HL from Play statemachine
	exx				//4
	cp	SAMPLE_BUF_B_HI 	//7
	jp	nz,.NextBuffer		//10/10
.CheckBreak:
	ld	a,$7f			//7
	in	a,($fe) 		//11
	rra				//4
	jp	nc,RestartStreaming	//10/10 restart if break key pressed
	ld	bc,0			//10 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.NextBuffer:
	ld	ix,ReadDataToBuf_B	//14
	ld	hl,SAMPLE_BUF_B 	//10
	ld	bc,PRT0 		//10
	repeat 2
		nop				//4 waste cycles
	end repeat
	exx				//4
	jp	(iy)			//8 -> 79

;--------
ReadDataToBuf_A:
	exx				//4
	ini				//16
	ini				//16
	jp	z,.DoneReading		//10/10
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.DoneReading:
	ld	ix,CheckLoopCounter_A	//14
	ld	a,0			//7 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79

ReadDataToBuf_B:
	exx				//4
	ini				//16
	ini				//16
	jp	z,.DoneReading		//10/10
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.DoneReading:
	ld	ix,CheckLoopCounter_B	//14
	ld	a,0			//7 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79

;--------
CheckLoopCounter_A:
	exx				//4
	dec	de			//6
	ld	a,d			//4
	or	e			//4
	jp	z,.CloseFile		//10/10
	ld	ix,RequestRead256_B	//14
	ld	a,0			//7 waste cycles
	ld	a,i			//9 waste cycles
	ld	a,i			//9 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.CloseFile:
	ld	ix,WaitForLastBuffer	//14
	ld	a,0			//7 waste cycles
	ld	a,i			//9 waste cycles
	ld	a,i			//9 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79

CheckLoopCounter_B:
	exx				//4
	dec	de			//6
	ld	a,d			//4
	or	e			//4
	jp	z,.CloseFile		//10/10
	ld	ix,RequestRead256_A	//14
	ld	a,0			//7 waste cycles
	ld	a,i			//9 waste cycles
	ld	a,i			//9 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.CloseFile:
	ld	ix,WaitForLastBuffer	//14
	ld	a,0			//7 waste cycles
	ld	a,i			//9 waste cycles
	ld	a,i			//9 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79

;--------
WaitForLastBuffer:
	ld	a,l			//4 NOTE: HL from PlaySample registers!
	exx				//4
	cp	0			//7
	jp	z,.LastBuffer		//10/10
	repeat 9
		nop				//4 waste cycles
	end repeat
	inc	bc			//6 waste cycles
	exx				//4
	jp	(iy)			//8 -> 79
.LastBuffer:
	exx
	jp	RestartStreaming	//timing does not matter anymore

;---------------------------------------------------------------
PlayBuf_A:
	ld	a,(hl)			//7
	out	(AY_DATA),a		//11
	inc	l			//4
	jp	z,.NextBuf		//10/10
.NextSample:
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	nop				//4 waste cycles
	jp	(ix)			//8 -> 68
.NextBuf:
	ld	hl,SAMPLE_BUF_B 	//10
	ld	iy,PlayBuf_B		//14
	nop				//4 waste cycles
	jp	(ix)			//8 -> 68

;--------
PlayBuf_B:
	ld	a,(hl)			//7
	out	(AY_DATA),a		//11
	inc	l			//4
	jp	z,.NextBuf		//10/10
.NextSample:
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	ld	a,0			//7 waste cycles
	nop				//4 waste cycles
	jp	(ix)			//8 -> 68
.NextBuf:
	ld	hl,SAMPLE_BUF_A 	//10
	ld	iy,PlayBuf_A		//14
	nop				//4 waste cycles
	jp	(ix)			//8 -> 68

;---------------------------------------------------------------