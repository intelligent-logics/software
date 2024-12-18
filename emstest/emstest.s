;
; File generated by cc65 v 2.19 - Git b993d8833
;
	.fopt		compiler,"cc65 v 2.19 - Git b993d8833"
	.setcpu		"6502"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.forceimport	__STARTUP__
	.import		_pal_bg
	.import		_ppu_off
	.import		_ppu_on_all
	.import		_music_play
	.import		_vram_adr
	.import		_vram_put
	.export		_song
	.export		_i
	.export		_text
	.export		_palette
	.export		_main

.segment	"RODATA"

_text:
	.byte	$48,$65,$6C,$6C,$6F,$20,$57,$6F,$72,$6C,$64,$21,$00
_palette:
	.byte	$0F
	.byte	$00
	.byte	$10
	.byte	$30
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00
	.byte	$00

.segment	"BSS"

_song:
	.res	1,$00
.segment	"ZEROPAGE"
_i:
	.res	1,$00

; ---------------------------------------------------------------
; void __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

.segment	"CODE"

;
; ppu_off(); // screen off
;
	jsr     _ppu_off
;
; pal_bg(palette); // load the BG palette
;
	lda     #<(_palette)
	ldx     #>(_palette)
	jsr     _pal_bg
;
; vram_adr(NTADR_A(10,14)); // screen is 32 x 30 tiles
;
	ldx     #$21
	lda     #$CA
	jsr     _vram_adr
;
; i = 0;
;
	lda     #$00
	sta     _i
;
; while(text[i]){
;
	jmp     L0004
;
; vram_put(text[i]); // this pushes 1 char to the screen
;
L0002:	ldy     _i
	lda     _text,y
	jsr     _vram_put
;
; ++i;
;
	inc     _i
;
; while(text[i]){
;
L0004:	ldy     _i
	lda     _text,y
	bne     L0002
;
; song = 1;
;
	lda     #$01
	sta     _song
;
; music_play(song);
;
	jsr     _music_play
;
; ppu_on_all(); // turn on screen
;
	jsr     _ppu_on_all
;
; while (1){
;
L000A:	jmp     L000A

.endproc

