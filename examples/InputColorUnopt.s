
LDA #$00
STA $F0
LDA #$02
STA $F1
LDY #$00

LOOP:
JMP Chck_Inp
BODY:

LDA $FE
STA ($F0), Y
JMP LOOP

Chck_Inp:
LDA $FF
CMP #$77
BEQ SetW
CMP #$61
BEQ SetA
CMP #$73
BEQ SetS
CMP #$64
BEQ SetD
JMP BODY


Dec_Page:
DEC $F1
RTS

Inc_Page:
INC $F1
RTS

Clear_Input:
LDA #$00
STA $FF
JMP BODY

SetW:
LDX #$00
W_L:
DEY
CPY #$FF
BNE skip_w
JSR Dec_Page
skip_w:
INX
CPX #$20
BNE W_L
JMP Clear_Input


SetA:
DEY
CPY #$FF
BNE Clear_Input
JSR Dec_Page
JMP Clear_Input

SetS:
LDX #$00
S_L:
INY
BNE skip_d
JSR Inc_Page
skip_d:
INX
CPX #$20
BNE S_L
JMP Clear_Input

SetD:
INY
BNE Clear_Input
JSR Inc_Page
JMP Clear_Input