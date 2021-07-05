#include <stdio.h>
#include <stdlib.h>

//these are our instructions
#define PRINT_BEEJ  1
#define HALT        2
#define PRINT_NUM   3
#define PRINT_SUM   4
#define SAVE        5
#define PRINT_REGISTER  6
#define ADD         7

unsigned int registers[8];

unsigned int memory[] = {
    PRINT_BEEJ,
    PRINT_NUM,
    327,
    PRINT_SUM,
    17,
    14,
    PRINT_SUM,
    2019,
    48,
    SAVE,
    13,
    2,
    SAVE,
    36,
    3,
    ADD,
    2,
    3,
    PRINT_REGISTER,
    2,
    HALT
};

int main(void)
{
    int cur_index = 0;
    int running = 1;

    unsigned char reg_num, val;
    unsigned char reg_num0, reg_num1;

    while(running) {
        printf("Registers:\n[ ");
        for (int i = 0; i<8; i++){
            printf("%d ", registers[i]);
        }
        printf("]\n");
        unsigned char command = memory[cur_index];

        switch (command) {

            case ADD:
                reg_num0 = memory[cur_index + 1];
                reg_num1 = memory[cur_index + 2];

                registers[reg_num0] += registers[reg_num1];

                cur_index += 3;
                break;


            case SAVE:
                val = memory[cur_index + 1];
                reg_num = memory[cur_index + 2];

                registers[reg_num] = val;

                cur_index += 3;
                break;


            case PRINT_BEEJ:
                printf("Beej!\n");
                cur_index++;
                break;

            case PRINT_NUM:
                // printf("print_Num\n");
                printf("%d\n", memory[cur_index+1]);
                cur_index += 2;
                break;

            case PRINT_SUM:
                // printf("sum here\n");
                printf("%d\n", memory[cur_index+1] + memory[cur_index+2] );
                cur_index += 3;
                break;

            case PRINT_REGISTER:
                printf("%d\n", registers[memory[cur_index + 1]]);
                cur_index += 2;
                break;

            case HALT:
                running = 0;
                // cur_index++;
                break;
        
            default:
                printf("Unrecognized instruction\n");
                exit(1);
                // break;
        }

    }

    return 0;
}