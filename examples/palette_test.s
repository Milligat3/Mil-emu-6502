; Palette preview
LDX #$00
LDY #$00
LDA #$00

MAIN:
STA $0200, Y
CLC
ADC #$1
INY
JMP MAIN
