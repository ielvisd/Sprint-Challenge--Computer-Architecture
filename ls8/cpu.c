#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, int argc, char **argv)
{
  // char data[DATA_LEN] = {
  //     // From print8.ls8
  //     0b10000010, // LDI R0,8
  //     0b00000000,
  //     0b00001000,
  //     0b01000111, // PRN R0
  //     0b00000000,
  //     0b00000001 // HLT
  // };

  // int address = 0;

  // for (int i = 0; i < DATA_LEN; i++)
  // {
  //   cpu->ram[address++] = data[i];
  // }

  if (argc != 2)
  {
    // printf("Correct usage: ./files file_name.extension\n");
    return;
  }
  FILE *fp;
  char line[256];
  fp = fopen(argv[1], "r");
  if (fp == NULL)
  {
    printf("Error opening file.\n");
    return;
  }
  int address = 0;
  while (fgets(line, 256, fp) != NULL)
  {
    char *endptr;
    unsigned char val = strtoul(line, &endptr, 2);
    if (line == endptr)
    {
      //         printf("skipping: %s", line);
      continue;
    }
    cpu->ram[address++] = val;
  }
  fclose(fp);

  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // `MUL registerA registerB`

    // Multiply the values in two registers together and store the result in registerA.

    // Machine code:
    // ```
    // 10100010 00000aaa 00000bbb
    // A2 0a 0b
    // ```

    // TODO
    // printf("cpu->PC  is: %d\n", cpu->PC );

    // regA = cpu->ram[cpu->PC + 1];
    // printf("regA is: %d\n", regA);
    // regB = cpu->ram[cpu->PC + 2];
    // printf("regB is: %d\n", regA);

    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];

    break;

    // TODO: implement more ALU ops

  case ALU_ADD:

    cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];

    break;

  case ALU_CMP:

    // printf("cpu->registers[regA]: %d\n & cpu->registers[regB]: %d\n", cpu->registers[regA], cpu->registers[regB]);

    if (cpu->registers[regA] == cpu->registers[regB])
    {


      cpu->flags[0] = 0b00000001;

    }

    else if (cpu->registers[regA] > cpu->registers[regB])
    {

      cpu->flags[0] = 0b00000010;
    }

    else if (cpu->registers[regA] < cpu->registers[regB])
    {

      cpu->flags[0] = 0b00000100;
    }

    break;
  }
}

// MAR = Memory Address Register (the address)
// MDR = Memory Data Register (the data)
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char MAR)
{
  return cpu->ram[MAR];
}

unsigned char cpu_ram_write(struct cpu *cpu, unsigned char MAR, unsigned char MDR)
{
  return cpu->ram[MAR] = MDR;
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  unsigned char reg_num, val, return_addr;

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).

    // IR = Instruction Register (current instruction)
    unsigned char IR = cpu_ram_read(cpu, cpu->PC);

    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction

    unsigned char operand0 = cpu_ram_read(cpu, cpu->PC + 1);
    unsigned char operand1 = cpu_ram_read(cpu, cpu->PC + 2);

    // printf("TRACE: cpu-PC: %d: cpu-IR: %02X    operand0: %02x operand1: %02x\n", cpu->PC, IR, operand0, operand1);

    // 4. switch() over it to decide on a course of action.

    switch (IR)
    {
    case LDI:
      // per spec LDI sets specified register to specified value
      // printf("cpu->PCstart: %d\n", cpu->PC);
      cpu->registers[operand0] = operand1;
      cpu->PC += 3;
      break;
    case PRN:
      printf("%d\n", cpu->registers[operand0]);
      cpu->PC += 2;
      break;
    case HLT:
      running = 0;
      break;
    case MUL:
      alu(cpu, ALU_MUL, operand0, operand1);
      cpu->PC += 3;
      break;

    case ADD:
      alu(cpu, ALU_ADD, operand0, operand1);
      cpu->PC += 3;
      break;

    case CMP:
      alu(cpu, ALU_CMP, operand0, operand1);
      //  printf("CMP cpu->flags[0], %d\n", cpu->flags[0] );
      cpu->PC += 3;
      break;
    case CALL:

      //save our spot where we want to go back to
      return_addr = cpu->PC + 2;
      cpu->registers[7]--;
      cpu->ram[cpu->registers[7]] = return_addr;

      //move the PC to the address of the subroutine
      reg_num = cpu->ram[cpu->PC + 1];
      cpu->PC = cpu->registers[reg_num];

      break;
    case RET:

      //pop the PC off the stack
      cpu->PC = cpu->ram[cpu->registers[7]];
      cpu->registers[7]++;
      break;
    case PUSH:
      cpu->registers[7]--; //our SP

      reg_num = cpu->ram[cpu->PC + 1];
      val = cpu->registers[reg_num];

      cpu->ram[cpu->registers[7]] = val;

      cpu->PC += 2; //our program counter
      break;
    case POP:
      reg_num = cpu->ram[cpu->PC + 1];
      cpu->registers[reg_num] = cpu->ram[cpu->registers[7]];

      cpu->registers[7]++;

      cpu->PC += 2; //our program counter
      break;

    case JMP:
      reg_num = cpu->ram[cpu->PC + 1];

        // printf("reg_num: %d\n", reg_num);
        // printf("cpu->registers[reg_num] %d\n", cpu->registers[reg_num]);

        int reg_address = cpu->registers[reg_num];

        int spotsToJump = reg_address - cpu->PC;


      cpu->PC = spotsToJump; //our program counter
      break;

    case JEQ:

      // printf("JEQ cpu->PC: %d\n", cpu->PC);

      // printf("cpu->flags[0] cpu->PC: %d\n", cpu->flags[0]);

      if (cpu->flags[0] == 00000001)
      {
        // printf("JEQ MATCH!:\n");
        reg_num = cpu->ram[cpu->PC + 1];

        // printf("reg_num: %d\n", reg_num);
        // printf("cpu->registers[reg_num] %d\n", cpu->registers[reg_num]);

        int reg_address = cpu->registers[reg_num];

        int spotsToJump = reg_address - cpu->PC;
        // printf("spotsToJump: %d\n", spotsToJump);

        // printf("cpu->registers[reg_num]: %d\n", cpu->registers[reg_num]);
        // printf("reg_address: %d\n", reg_address);

        cpu->PC += spotsToJump;
        // printf("cpu->PC: %d\n", cpu->PC);
        // cpu->PC += reg_address;
        break;
      }

      else
      {
        cpu->PC += 2;
        break;
      }

    case JNE:
      // printf("cpu->PC0: %d\n", cpu->PC);
      // printf("cpu->flags[0], %d\n", cpu->flags[0]);

      // printf("cpu->flags[0] && 00000001 , %d\n", cpu->flags[0] & 00000001);

      // int check = cpu->flags[0] & 00000001;

      // printf("check is: %d\n", check);

      if ((cpu->flags[0] & 00000001) == 0)
      {
        // printf("cpu->PC1: %d\n", cpu->PC);
        // printf("not equal!\n");
        reg_num = cpu->ram[cpu->PC + 1];

        // printf("reg_num: %d\n", reg_num);
        // printf("cpu->registers[reg_num] %d\n", cpu->registers[reg_num]);

        int reg_address = cpu->registers[reg_num];

        int spotsToJump = reg_address - cpu->PC;
        // printf("spotsToJump: %d\n", spotsToJump);

        // printf("cpu->registers[reg_num]: %d\n", cpu->registers[reg_num]);
        // printf("reg_address: %d\n", reg_address);

        // cpu->PC += 1 ;
        // printf("cpu->PC2: %d\n", cpu->PC);

        // cpu->PC = reg_address; //our program counter
        cpu->PC += spotsToJump;
        // printf("cpu->PC: %d\n", cpu->PC);
        // cpu->PC += reg_address;
        break;
      }
 

      else
      {
        // printf("Active?!\n");
        cpu->PC += 2;
        break;
      }
    // Adds default case in case it gets a case it doesn't recognize
    default:
      // Print at a field width of 2 (2), add leading zeroes if necessary (0), use capital letters for hex values (X)
      printf("unexpected instruction 0x%02X at 0x%02X\n", IR, cpu->PC);
      exit(1);
    }
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  // Init Registers per spec
  for (int i = 0; i < 6; i++)
  {
    cpu->registers[i] = 0;
  }
  cpu->registers[7] = 0xF4;

  // Init PC
  cpu->PC = 0;

  // Init RAM
  memset(cpu->ram, 0, sizeof(cpu->ram));
}
