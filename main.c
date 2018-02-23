#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[]){ //program will recieve a file input that has commands in a binary structure

    //check for correct number of arguments
    if(argc != 2){
        printf("%s <trace_file>\n", argv[0]);
        exit(0);
    }
    unsigned char input[1];
    long int pc = 0;
    unsigned char ins, rs, rt, rd, out, skip = 0;
    signed char imm = 0;

    signed char $r0 = 0;
    signed char $r1 = 0;
    signed char $r2 = 0;
    signed char $r3 = 0;

    signed char regs[4] = {$r0,$r1,$r2,$r3};

    //Open the file for reading
    FILE *trace_file = fopen(argv[1], "rb");
    //fread(instruction, sizeof(instruction),1,trace_file);

    /* This file will only contain lines with instructions written in binary format, one instruction per line.
     * Once the file is open for reading, each line will be parsed to find the INS, REG, IMM, and J fields
     *
     * INS: The operation to perform (li, add, sub, beq)
     * REG: The register number to read/write
     * IMM: The immediate value to use
     * J: If the INS is beq, this will dictate the jump of 1 or 2 lines on successful beq
     *
     * Example: If a line looks like '00001000', then the instruction can be broken up like:
     *
     * 00   00  1110
     * ^    ^     ^
     * |    |     |
     * INS  REG   IMM
     *
     * In this case, INS = li, Reg = $r0, and IMM = -1; this will load -1 into $r0
     *
     * Example2: If a line looks like '01000111', then the instruction can be broken up like:
     *
     * 01   00  01  11
     * ^    ^   ^   ^
     * |    |   |   |
     * INS  rd  rs  rt
     *
     * In this case, INS = add, rd = $r0, rs = $r1, rt = $r3; This will add $r0 and $r3, and store the result in $r0.
     *
     * Example3: If a line looks like '11101110', then the instruction can be broken up like:
     *
     * 11  10 11 10
     * ^   ^  ^  ^
     * |   |  |  |
     * INS r1 r2 skip
     *
     * In this case, INS = beq, r1 = $r2, r2 = $r3, skip = 2; This instruction will compare r1 and r3, if they are equal, the PC will skip 2 lines.
     *
     * beq will have the LSB be an option bit, if set to 1, then beq will act as an output command, outputting r1 to console
     *
     *
     * */

printf("Value in $r%d is: %d\n", rd, regs[rd]);
    if(trace_file){
        while(!feof(trace_file)){

            //fseek(trace_file,pc,SEEK_SET);
            fread(&input, sizeof(input),1,trace_file);

            ins = input[0] >> 6;
            switch(ins){
            case 0: //li instruction, need rd and imm
                rd = (input[0] << 2) >> 6;
                imm = (input[0] << 4) >> 4;
                if((imm & 8) == 8){
                    imm = imm | 240;
                }
                regs[rd] = imm;

                break;
            case 1: //add instruction, need rd, rs, and rt
                rd = (input[0] << 2) >> 6;
                rs = (input[0] << 4) >> 6;
                rt = (input[0] << 6) >> 6;

                regs[rd] = regs[rs] + regs[rt];
                break;
            case 2: //sub instruction, need rd, rs, and rt
                rd = (input[0] << 2) >> 6;
                rs = (input[0] << 4) >> 6;
                rt = (input[0] << 6) >> 6;

                regs[rd] = regs[rs] - regs[rt];
                break;
            case 3: //beq and output, need rd, rs, skip, and out
                rd = (input[0] << 2) >> 6;
                rs = (input[0] << 4) >> 6;
                skip = (input[0] << 6) >> 7;
                out = input[0] & 1;

                if(out){
                    printf("Value in $r%d is: %d", rd, regs[rd]);
                }

                if(regs[rd] == regs[rs]){
                    pc += (skip+1);
                }
                break;
            default:
                break;
            }

            pc++;
        }




        printf("Reached end of While loop.\n");
        fclose(trace_file);
    }
    return 0;
}
