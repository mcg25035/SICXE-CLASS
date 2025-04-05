/***********************************************************************/
/*  Program Name: main.c                                               */
/*  Description: This program implements Pass 1 of a SIC/XE assembler. */
/*               It identifies the location, symbol, opcode, and       */
/*               operand for each instruction. It also calculates the  */
/*               location counter (LC) and builds the symbol table.    */
/*  Author: Codingbear Chou(D1210270), Unknown...                      */
/*  Date Created: 2019.12.13                                           */
/*  Last Modified: 2025.04.06                                          */
/*  Updates:                                                           */
/*   - 2021.03.26: Fixed error: '+' prefix incorrectly used with       */
/*                 format 1 & 2 instructions.                          */
/*   - 2025.04.06: Implemented LC calculation and symbol table         */ 
/*                 building.                                           */
/***********************************************************************/
#include <string.h>
#include "2-optable.c"

/* Public variables and functions */
#define ADDR_SIMPLE 0x01
#define ADDR_IMMEDIATE 0x02
#define ADDR_INDIRECT 0x04
#define ADDR_INDEX 0x08

#define LINE_EOF (-1)
#define LINE_COMMENT (-2)
#define LINE_ERROR (0)
#define LINE_CORRECT (1)

typedef struct
{
    char symbol[LEN_SYMBOL];
    char op[LEN_SYMBOL];
    char operand1[LEN_SYMBOL];
    char operand2[LEN_SYMBOL];
    unsigned code;
    unsigned fmt;
    unsigned addressing;
} LINE;

typedef struct
{
    char symbol[LEN_SYMBOL];
    unsigned address;
} SYMREC;

#define LEN_SYMTAB (10000)
SYMREC SYMTAB[LEN_SYMTAB];
int SYMREC_count = 0; /* number of symbols in SYMTAB */

void add_symbol(char *symbol, unsigned address)
{
    if (SYMREC_count < LEN_SYMTAB)
    {
        strcpy(SYMTAB[SYMREC_count].symbol, symbol);
        SYMTAB[SYMREC_count].address = address;
        SYMREC_count++;
    }
    else
    {
        printf("SYMTAB overflow\n");
    }
}

void print_SYMTAB()
{
    int i;
    for (i = 0; i < SYMREC_count; i++)
    {
        printf("  %s: %04X\n", SYMTAB[i].symbol, SYMTAB[i].address);
    }
}

int process_line(LINE *line);
/* return LINE_EOF, LINE_COMMENT, LINE_ERROR, LINE_CORRECT and Instruction information in *line*/

/* Private variable and function */

void init_LINE(LINE *line)
{
    line->symbol[0] = '\0';
    line->op[0] = '\0';
    line->operand1[0] = '\0';
    line->operand2[0] = '\0';
    line->code = 0x0;
    line->fmt = 0x0;
    line->addressing = ADDR_SIMPLE;
}

int process_line(LINE *line)
/* return LINE_EOF, LINE_COMMENT, LINE_ERROR, LINE_CORRECT */
{
    char buf[LEN_SYMBOL];
    int c;
    int state;
    int ret;
    Instruction *op;

    c = ASM_token(buf); /* get the first token of a line */
    if (c == EOF)
        return LINE_EOF;
    else if ((c == 1) && (buf[0] == '\n')) /* blank line */
        return LINE_COMMENT;
    else if ((c == 1) && (buf[0] == '.')) /* a comment line */
    {
        do
        {
            c = ASM_token(buf);
        } while ((c != EOF) && (buf[0] != '\n'));
        return LINE_COMMENT;
    }
    else
    {
        init_LINE(line);
        ret = LINE_ERROR;
        state = 0;
        while (state < 8)
        {
            switch (state)
            {
            case 0:
            case 1:
            case 2:
                op = is_opcode(buf);
                if ((state < 2) && (buf[0] == '+')) /* + */
                {
                    line->fmt = FMT4;
                    state = 2;
                }
                else if (op != NULL) /* INSTRUCTION */
                {
                    strcpy(line->op, op->op);
                    line->code = op->code;
                    state = 3;
                    if (line->fmt != FMT4)
                    {
                        line->fmt = op->fmt & (FMT1 | FMT2 | FMT3);
                    }
                    else if ((line->fmt == FMT4) && ((op->fmt & FMT4) == 0)) /* INSTRUCTION is FMT1 or FMT 2*/
                    {                                                        /* ERROR 20210326 added */
                        printf("ERROR at token %s, %s cannot use format 4 \n", buf, buf);
                        ret = LINE_ERROR;
                        state = 7; /* skip following tokens in the line */
                    }
                }
                else if (state == 0) /* SYMBOL */
                {
                    strcpy(line->symbol, buf);
                    state = 1;
                }
                else /* ERROR */
                {
                    printf("ERROR at token %s\n", buf);
                    ret = LINE_ERROR;
                    state = 7; /* skip following tokens in the line */
                }
                break;
            case 3:
                if (line->fmt == FMT1 || line->code == 0x4C) /* no operand needed */
                {
                    if (c == EOF || buf[0] == '\n')
                    {
                        ret = LINE_CORRECT;
                        state = 8;
                    }
                    else /* COMMENT */
                    {
                        ret = LINE_CORRECT;
                        state = 7;
                    }
                }
                else
                {
                    if (c == EOF || buf[0] == '\n')
                    {
                        ret = LINE_ERROR;
                        state = 8;
                    }
                    else if (buf[0] == '@' || buf[0] == '#')
                    {
                        line->addressing = (buf[0] == '#') ? ADDR_IMMEDIATE : ADDR_INDIRECT;
                        state = 4;
                    }
                    else /* get a symbol */
                    {
                        op = is_opcode(buf);
                        if (op != NULL)
                        {
                            printf("Operand1 cannot be a reserved word\n");
                            ret = LINE_ERROR;
                            state = 7; /* skip following tokens in the line */
                        }
                        else
                        {
                            strcpy(line->operand1, buf);
                            state = 5;
                        }
                    }
                }
                break;
            case 4:
                op = is_opcode(buf);
                if (op != NULL)
                {
                    printf("Operand1 cannot be a reserved word\n");
                    ret = LINE_ERROR;
                    state = 7; /* skip following tokens in the line */
                }
                else
                {
                    strcpy(line->operand1, buf);
                    state = 5;
                }
                break;
            case 5:
                if (c == EOF || buf[0] == '\n')
                {
                    ret = LINE_CORRECT;
                    state = 8;
                }
                else if (buf[0] == ',')
                {
                    state = 6;
                }
                else /* COMMENT */
                {
                    ret = LINE_CORRECT;
                    state = 7; /* skip following tokens in the line */
                }
                break;
            case 6:
                if (c == EOF || buf[0] == '\n')
                {
                    ret = LINE_ERROR;
                    state = 8;
                }
                else /* get a symbol */
                {
                    op = is_opcode(buf);
                    if (op != NULL)
                    {
                        printf("Operand2 cannot be a reserved word\n");
                        ret = LINE_ERROR;
                        state = 7; /* skip following tokens in the line */
                    }
                    else
                    {
                        if (line->fmt == FMT2)
                        {
                            strcpy(line->operand2, buf);
                            ret = LINE_CORRECT;
                            state = 7;
                        }
                        else if ((c == 1) && (buf[0] == 'x' || buf[0] == 'X'))
                        {
                            line->addressing = line->addressing | ADDR_INDEX;
                            ret = LINE_CORRECT;
                            state = 7; /* skip following tokens in the line */
                        }
                        else
                        {
                            printf("Operand2 exists only if format 2  is used\n");
                            ret = LINE_ERROR;
                            state = 7; /* skip following tokens in the line */
                        }
                    }
                }
                break;
            case 7: /* skip tokens until '\n' || EOF */
                if (c == EOF || buf[0] == '\n')
                    state = 8;
                break;
            }
            if (state < 8)
                c = ASM_token(buf); /* get the next token */
        }
        return ret;
    }
}

int get_BYTE_length(LINE *line)
{
    int i, len = 0;
    char *p = line->operand1;
    int is_last_slash = 0;
    int is_in_string = 0;

    if (p[0] == 'C')
    {
        p++;
        while (*p != '\0')
        {
            if (!is_in_string && *p != '\'')
            {
                printf("ERROR: Invalid operand for BYTE directive: \n");
                return -1;
            }

            if (is_in_string && *p == '\'' && !is_last_slash)
            {
                is_in_string = 0;
                break;
            }

            if (*p == '\\')
            {
                is_last_slash = 1;
                continue;
            }

            if (is_last_slash)
            {
                is_last_slash = 0;
                len++;
                continue;
            }

            p++;
            len++;
            continue;
        }
        if (is_in_string)
        {
            printf("ERROR: Invalid operand for BYTE directive\n");
            return -1;
        }
        return len;
    }

    if (p[0] == 'X')
    {
        p++;
        if (*p != '\'')
        {
            printf("ERROR: Invalid operand for BYTE directive\n");
            return -1;
        }
        p++;
        is_in_string = 1;
        while (*p != '\0')
        {
            if (is_in_string && *p == '\'')
            {
                is_in_string = 0;
                break;
            }

            char allowed_chars[] = "0123456789ABCDEFabcdef";

            if (is_in_string && !strchr(allowed_chars, *p))
            {
                printf("ERROR: Invalid operand for BYTE directive\n");
                return -1;
            }

            p++;
            len++;
            continue;
        }
        if (is_in_string)
        {
            printf("ERROR: Invalid operand for BYTE directive\n");
            return -1;
        }
        return (len + 1) / 2; // Each two hex digits represent one byte
    }

    printf("ERROR: Invalid operand for BYTE directive\n");
    return -1; // Invalid operand for BYTE directive
}

int get_length(LINE line)
{
    if (strcmp(line.op, "WORD") == 0)
    {
        return 3;
    }
    if (strcmp(line.op, "RESW") == 0)
    {
        return 3 * atoi(line.operand1);
    }
    if (strcmp(line.op, "RESB") == 0)
    {
        return atoi(line.operand1);
    }
    if (strcmp(line.op, "BYTE") == 0)
    {
        int byte_length = get_BYTE_length(&line);
        if (byte_length < 0)
        {
            printf("ERROR: Invalid operand for BYTE directive\n");
            return -1;
        }
        return byte_length;
    }

    if (line.fmt == FMT0)
    {
        return 0;
    }
    if (line.fmt == FMT1)
    {
        return 1;
    }
    if (line.fmt == FMT2)
    {
        return 2;
    }
    if (line.fmt == FMT3)
    {
        return 3;
    }
    if (line.fmt == FMT4)
    {
        return 4;
    }

    printf("ERROR: Invalid format\n");
    return -1; // Invalid format
}

int main(int argc, char *argv[])
{
    int i, c, line_count;
    char buf[LEN_SYMBOL];
    LINE line;
    int locataion_counter = 0;

    if (argc < 2)
    {
        printf("Usage: %s fname.asm\n", argv[0]);
    }
    else
    {
        if (ASM_open(argv[1]) == NULL)
            printf("File not found!!\n");
        else
        {
            for (line_count = 1; (c = process_line(&line)) != LINE_EOF; line_count++)
            {
                if (c == LINE_ERROR)
                    printf("%03d : Error\n", line_count);
                else if (c == LINE_COMMENT)
                    printf("%03d : Comment line\n", line_count);
                else
                {
                    // printf("%03d : %12s %12s %12s,%12s (FMT=%X, ADDR=%X)\n", line_count, line.symbol, line.op, line.operand1, line.operand2, line.fmt, line.addressing);

                    if (line.operand2 == NULL || line.operand2[0] == '\0') {
                        printf("%06X %12s %12s %12s\n", locataion_counter, line.symbol, line.op, line.operand1);    
                    }
                    else{
                        printf("%06X %12s %12s %12s, %s\n", locataion_counter, line.symbol, line.op, line.operand1, line.operand2);
                    }
                    
                    

                    if (line.symbol[0] != '\0')
                    {
                        add_symbol(line.symbol, locataion_counter);
                    }

                    int instruction_length = get_length(line);
                    if (instruction_length < 0)
                    {
                        return -1;
                    }
                    locataion_counter += instruction_length;

                    
                }
            }
            printf("Program length = %d\n", locataion_counter);
            printf("Symbol table below:\n");
            print_SYMTAB();
            ASM_close();
        }
    }
}
