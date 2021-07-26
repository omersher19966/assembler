#include "assemblerData.h"
#include "symbol.h"


#ifndef PASS_H

    #define PASS_H

    /* ------------ Macros ------------ */

    #define FIRST_OPERAND 0
    #define SECOND_OPERAND 1
    #define THIRD_OPERAND 2

    #define EXTERN_DECLARATION  ".extern"
    #define ENTRY_DECLARATION   ".entry"

    #define DB ".db"
    #define DH ".dh"
    #define DW ".dw"
    #define ASCIZ ".asciz"

    #define CHAR_JMP 1
    #define DB_JMP 1
    #define DH_JMP 2
    #define DW_JMP 4

    #define CODE_IMAGE_JMP 4

    /* ------------New Types ------------ */

    typedef enum{NO_OPERANDS , ONE_OPERAND, TWO_OPERANDS, THREE_OPERANDS, NOT_CONSTANT} reqOperands; /* stands for requiredOperands */

    typedef enum{INSTRUCTION_R, INSTRUCTION_I, INSTRUCTION_J} instructionType;

    typedef enum{ 
        ARITHMETIC_R_INSTRUCTIONS, 
        COPY_R_INSTRUCTIONS,
        ARITHMETIC_I_INSTRUCTIONS,
        BRANCHING_I_INSTRUCTIONS,
        LOADING_STORING_I_INSTRUCTIONS,
        JMP,
        LA,
        CALL,
        STOP} instructionGroupType;

    typedef struct instruction { 
        instructionType type;
        int value;
        union {
            struct {
                unsigned unused: 6;
                unsigned funct: 5;
                unsigned rd: 5;
                unsigned rt: 5;
                unsigned rs: 5;
                unsigned opcode: 6;

            } instruction_r;
            struct {
                unsigned immed: 16;
                unsigned rt: 5;
                unsigned rs: 5;
                unsigned opcode: 6;

            } instruction_i;
            struct {
                unsigned address: 25;
                unsigned reg: 1;
                unsigned opcode: 6;
            }instruction_j;
        } instruction_fmt;
    } instruction;

    typedef struct command {
        char *commmad_name;
        int opcode;
        int funct;
        reqOperands operands_num;
        instructionType instruction_type;
        instructionGroupType instruction_group;
        
    } command;

    typedef struct guidanceCommand * guidanceCommandPtr;

    typedef struct guidanceCommand {
        char *guidance_command_name;
    } guidanceCommand;

    typedef enum{BYTE = 1, HALF_WORD = 2, WORD = 4} dataType;

    typedef struct data {
        int value;
        dataType type;
        union {
            unsigned long byte: 8;
            unsigned long half_word: 16;
            unsigned long word: 32;
        } data;
    } data;

    typedef struct externalNode * externalNodePtr;
    
    
    typedef struct externalNode {
        int address;
        char *symbol_name;
        externalNodePtr next;
    } externalNode;


    typedef struct entryNode * entryNodePtr;
    
    
    typedef struct entryNode {
        int address;
        char *symbol_name;
        entryNodePtr next;
    } entryNode;



    /* ------------ External Variabels ------------ */

    extern int ic, dc, lc;
    extern instruction code_image[];
    extern data data_image[];
    extern int code_image_index, data_image_index;
    extern externalNodePtr ext_head;
    extern entryNodePtr ent_head;

    /* ------------ Functions ------------ */
    
    void    pass(FILE *fp, char *file_name, int file_num);

    void    print_code_image_to_file(FILE *object_file ,int icf);
    void    print_data_image_to_file(FILE *object_file ,int icf, int dcf);
    void    update_data_image(int icf);
    void    free_entry_list();
    void    free_external_list();
    void    free_code_image();
    void    free_data_image();
    void    reset_counters_indexes();

    int     check_line(char *line);
    int     check_operands_num(char *line ,reqOperands num);
    int     convert_to_register(char *operand);
    int     add_instruction_to_code_image (instruction *instruction_ptr);
    int     add_to_data_image(long num, int jmp);
    int     set_operands_list(char *line, char **operands_list, reqOperands operands_num);
    int     create_output_files(char *file_name, int icf, int dcf);
    int     create_entry_file(char *file_name);
    int     create_external_file(char *file_name);
    int     create_object_file(char *file_name, int icf, int dcf);

    bool    is_command_word(char *);
    bool    is_guidance_word(char *);
    bool    is_entry_word(char *word);
    bool    is_extern_word(char *word);
    bool    is_entry_list();
    bool    is_external_list();
    bool    is_valid_operands_line(char *line);
    bool    is_valid_asciz_line(char *line); /* checks if there is one operand in line; */

    command * get_command(char *word);

    /* ----- First pass ----- */ 
    
    void    assembler_first_pass(FILE *fp, int file_num);
    
    void    set_arithmetic_r_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register);
    void    set_copy_r_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register);
    void    set_arithmetic_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed);
    void    set_loading_storing_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed);
    void    set_branching_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register);
    void    set_jmp_instruction(instruction *instruction_ptr, command *command_ptr, int reg ,bool reg_flag);
    void    set_la_instruction(instruction *instruction_ptr, command *command_ptr);
    void    set_call_instruction(instruction *instruction_ptr, command *command_ptr);
    void    set_stop_instruction(instruction *instruction_ptr, command *command_ptr);


    int     parse_extern_sentence(char *line);
    int     parse_guidance_sentence(char *line, char *word); /* need more work. */
    int     parse_command_sentence(char *line, char *word);
    int     parse_r_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    int     parse_i_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    int     parse_j_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    int     parse_asciz_command(char *line);

    /* ----- Second Pass ----- */ 
    
    void    assembler_second_pass(FILE *fp, int file_num);

    int     parse_entry_sentence(char *line);
    int     complete_command_data(char *line, char *word);
    int     add_symbol_to_external_list(symbolPtr symbol, int command_address);
    int     add_symbol_to_entry_list(symbolPtr symbol);

#endif

