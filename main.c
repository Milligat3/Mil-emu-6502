// Mil-emu6502

#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 0x10000
#define PROG_START 0x0600
#define VID_MEM 0x200
typedef struct 
{
	uint8_t A, X, Y, SP;
	uint16_t PC;
	union {
        uint8_t flags;
        struct {
            uint8_t C : 1;  
            uint8_t Z : 1;  
            uint8_t I : 1;  
            uint8_t D : 1;  
            uint8_t B : 1;  
            uint8_t U : 1;  
            uint8_t V : 1;  
            uint8_t N : 1; 
        };
	};
}CPU;

uint8_t memory[MEM_SIZE];
CPU cpu;

Color col_table[16] = {BLACK, WHITE, RED, (Color){0, 255, 255, 255}, PURPLE, GREEN, BLUE, YELLOW, 
						ORANGE, BROWN, (Color){255, 120, 120, 255}, (Color){100, 100, 100, 255}, GRAY, (Color){120, 255, 120, 255}, (Color){120, 120, 255, 255}, (Color){200, 200, 200, 255}};


void init_cpu(void)
{
	cpu.A = 0;
	cpu.X = 0;
	cpu.Y = 0;
	cpu.flags = 0x34;
	cpu.SP = 0xFF;
	cpu.PC = (memory[0xFFFD] << 8) | memory[0xFFFC];
}

void init_mem(void)
{
	memset(memory, 0, MEM_SIZE);
	memory[0xFFFC] = 0x00;
	memory[0xFFFD] = 0x06;
	memory[0xFFFE] = 0x00;
	memory[0xFFFF] = 0x06;
}

int load_bin(char* filename)
{
	FILE* binary = fopen(filename, "rb");
	fseek(binary, 0, SEEK_END);
	long f_size = ftell(binary);
	fseek(binary, 0, SEEK_SET);
	
	if(f_size + PROG_START > 0xC000)
	{
		fprintf(stderr, "File is too big.\n");
		fclose(binary);
		exit(1);
	}
	size_t read = fread(&memory[PROG_START], 1, f_size, binary);
	if(read != f_size)
	{
		fprintf(stderr, "Read error: expected %ld bytes, got %zu\n", f_size, read);
		fclose(binary);
		return 0;
	}
	
	printf("Loaded %ld bytes at $%04X\n", f_size, PROG_START);
	return 1;
}


void set_NZ_flags(uint8_t val)
{
	cpu.flags = (cpu.flags & 0x7D);
	if(val == 0) cpu.Z = 1;
	if(val & 0x80) cpu.N = 1;
}

#include "opcodes.c"

void exec_opcode(void)
{
	memory[0xFE] = rand() & 0xFF;
	uint8_t opcode = memory[cpu.PC++];
	op_hand[opcode]();
}

const int w_size = 640;
const int cell_size = 20;

void DrawScreen(void)
{
	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			Color val = col_table[memory[VID_MEM + i + j*32] & 0xF];
			// Color val = (Color){(memory[VID_MEM + i + j*32] % 16 * 16), 0, (memory[VID_MEM + i + j*32] % 16 * 16), 255};
			DrawRectangle(i*cell_size, j*cell_size, cell_size, cell_size, val);
		}
	}
}

void DrawDebug(void)
{
	char buf[256];
	sprintf(buf, "A: 0x%x", cpu.A);
	DrawText(buf, 0, 0, 20, WHITE);
	memset(buf, 0, 256);
	sprintf(buf, "X: 0x%x", cpu.X);
	DrawText(buf, 0, 20, 20, WHITE);
	memset(buf, 0, 256);
	sprintf(buf, "Y: 0x%x", cpu.Y);
	DrawText(buf, 0, 40, 20, WHITE);
	memset(buf, 0, 256);
	sprintf(buf, "PC: 0x%x", cpu.PC);
	DrawText(buf, 0, 60, 20, WHITE);
	memset(buf, 0, 256);
	sprintf(buf, "F: 0x%x", cpu.flags);
	DrawText(buf, 0, 80, 20, WHITE);
	memset(buf, 0, 256);
	
}

void inp_ch(void)
{
	// if(IsKeyPressed(KEY_W))
	// {
		// memory[0xFF] = 0x77;
	// }
	// if(IsKeyPressed(KEY_A))
	// {
		// memory[0xFF] = 0x61;
	// }
	// if(IsKeyPressed(KEY_S))
	// {
		// memory[0xFF] = 0x73;
	// }
	// if(IsKeyPressed(KEY_D))
	// {
		// memory[0xFF] = 0x64;
	// }
	memory[0xFF] = GetKeyPressed() + 0x20;
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Too few arguments\n");
		return -1;
	}
	op_hand_init();
	init_mem();
	init_cpu();
	
	load_bin(argv[1]);
	for (int i = 0; i < 15; i++) {
		printf("%02X ", memory[0x0600 + i]);
	}
	InitWindow(w_size, w_size, "6502-mil-emu");
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		inp_ch();
		for(int i = 0; i < 1000; i++)
		{
			// if(memory[cpu.PC] == 0x00)
				// break;
			exec_opcode();
		}
		BeginDrawing();
			ClearBackground(BLACK);
			DrawScreen();
			DrawDebug();
		EndDrawing();
	}
	
	CloseWindow();
	return 0;
}