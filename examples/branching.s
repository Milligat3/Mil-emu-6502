; спрайт
LDA #$00
STA $1000
STA $1001
LDA #$66
STA $1002
STA $1003
LDA #$18
STA $1004
LDA #$3C
STA $1005
STA $1006

LDA #$24
STA $1007


LDA #$00
STA $F0 
LDA #$02
STA $F1
LDX #$00

LOOP:
LDY #$00
LDA $1000, X
STA $A0
IN_LOOP:
ASL $A0
BCC skip
LDA #$6F
STA ($F0), Y
skip:
INY
CPY #$08
BNE IN_LOOP

INX
CLC
LDA $F0
ADC #$20
STA $F0
LDA $F1
ADC #$00
STA $F1
CPX #$08
BNE LOOP

BRK

ResX:
LDX #$00
JMP LOOP