
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


Inc_Y:
LDY #$1F
RTS

Dec_Y:
LDY #$00
RTS

Clear_Input:
LDA #$00
STA $FF
JMP BODY

Under_F1:
LDA #$05
RTS

Over_F1:
LDA #$02
RTS

SetW:
SEC
LDA $F0
SBC #$20
STA $F0
LDA $F1
SBC #$00
CMP #$01
BNE skip_w
JSR Under_F1
skip_w: 
STA $F1
JMP Clear_Input


SetA:
DEY
CPY #$ff
BNE Clear_Input
JSR Inc_Y
JMP Clear_Input

SetS:
CLC
LDA $F0
ADC #$20
STA $F0
LDA $F1
ADC #$00
CMP #$06
BNE skip_d
JSR Over_F1
skip_d:
STA $F1
JMP Clear_Input

SetD:
INY
CPY #$20
BNE Clear_Input
JSR Dec_Y
JMP Clear_Input