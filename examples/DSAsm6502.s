; === Zero Page Регистры ===
SPR_PTR    = $A0   ; Указатель на данные спрайта (2 байта)
VID_PTR    = $F0   ; Указатель видеопамяти (2 байта) 
SPR_ID     = $A2   ; ID текущего спрайта
SPR_X      = $A3   ; X-координата
SPR_Y      = $A4   ; Y-координата
TEMP       = $A5   ; Временный байт

; === Память спрайтов ===
; Каждый спрайт 8x8 = 8 байт

 MAIN:
 ; Отрисовать крипера в (10,5)
LDA #0          ; SPR_CREEPER = 0
STA SPR_ID
LDA #10         ; X=10
STA SPR_X
LDA #5          ; Y=5
STA SPR_Y
JSR DRAW_SPRITE

; Отрисовать игрока в (15,8)
LDA #1          ; SPR_PLAYER = 1
STA SPR_ID
LDA #15
STA SPR_X
LDA #8
STA SPR_Y
JSR DRAW_SPRITE

LDA #2
STA SPR_ID
LDA #0
STA SPR_X
LDA #128
STA SPR_Y
JSR DRAW_SPRITE


JMP MAIN
 ; Входные параметры:
;   SPR_ID - номер спрайта (0,1,2...)
;   SPR_X  - координата X (0-31)
;   SPR_Y  - координата Y (0-23)
DRAW_SPRITE:
  ; 1. Рассчитать адрес спрайта
  LDA SPR_ID
  ASL A           ; x2
  ASL A           ; x4
  ASL A           ; x8 (каждый спрайт = 8 байт)
  CLC
  ADC #<SPRITES   ; Младший байт
  STA SPR_PTR
  LDA #>SPRITES   ; Старший байт
  ADC #$00        ; Учесть перенос
  STA SPR_PTR+1

  ; 2. Рассчитать адрес видеопамяти
  LDA SPR_Y
  ASL A           ; x32 через сдвиги
  ASL A
  ASL A
  ASL A
  ASL A
  CLC
  ADC SPR_X       ; + X координата
  STA VID_PTR
  LDA #$02        ; Видеобуфер $0200
  ADC #$00        ; Учесть перенос
  STA VID_PTR+1

  ; 3. Отображение (твой код с оптимизациями)
  LDX #$00        ; Счетчик строк
@loop:
  LDY #$00
  LDA (SPR_PTR),Y ; Загружаем байт спрайта
  STA TEMP        ; Сохраняем для сдвига
  
  ; Увеличиваем указатель спрайта
  INC SPR_PTR
  BNE @no_carry
  INC SPR_PTR+1
@no_carry:

  ; Цикл по битам
  LDY #$07        ; От 7 до 0!
@bit_loop:
  ASL TEMP        ; Сдвигаем влево
  BCC @skip       ; Если 0 - пропускаем
  
  ; Рисование пикселя
  LDA #$6F        ; Цвет пикселя
  STA (VID_PTR),Y ; Y = горизонтальное смещение
  
@skip:
  DEY
  BPL @bit_loop   ; Пока Y >= 0

  ; Переход на следующую строку (+32)
  CLC
  LDA VID_PTR
  ADC #$20        ; +32 байт
  STA VID_PTR
  LDA VID_PTR+1
  ADC #$00
  STA VID_PTR+1

  INX
  CPX #$08
  BNE @loop
  RTS
  
  
  
SPRITES:
SPR_CREEPER:  ; Твой крипер
  .byte $00, $00, $66, $66, $18, $3C, $3C, $24
SPR_PLAYER:   ; Персонаж
  .byte $3C, $42, $A5, $81, $A5, $99, $42, $3C
SPR_TREE:     ; Дерево
  .byte $18, $3C, $7E, $FF, $FF, $7E, $3C, $18