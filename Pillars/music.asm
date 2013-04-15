; Binary packer
; Written by Tammo 'kb' Hinrichs 2000-2008 (public domain)
; Compile with nasm or yasm
;
; Add all data to include here

bits 32
section .data

;variables

global _song0
_song0:
  incbin "../music/song0.wav"

;**********************************************************************;
; Some replacements for things VC2005 needs when compiling without CRT ;
;**********************************************************************;

%ifdef NDEBUG
global __fltused
__fltused dd 0

section .text

global __chkstk
__chkstk ret

%endif
