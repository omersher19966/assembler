#include "assemblerData.h"
#include "symbol.h"

#ifndef PASS_H

    #define PASS_H

/* ----------------------------- */
/*  Macros  */
/* ----------------------------- */
    
    /* Directive Commands */
    
    #define DB_COMMAND ".db"
    #define DH_COMMAND ".dh"
    #define DW_COMMAND ".dw"
    #define ASCIZ_COMMAND ".asciz"
    #define EXTERN_COMMAND  ".extern" /* Used for detecting extern command */
    #define ENTRY_COMMAND   ".entry"/* Used for detecting entry command */


    /* Data Size For Directive Commands */
    #define DB_SIZE 1 /* 1 Byte Size */
    #define DH_SIZE 2 /* 2 Byte Size */
    #define DW_SIZE 4 /* 4 Byte Size */

    
    /* Available Operands For Instruction Commands */

    #define FIRST_OPERAND 0
    #define SECOND_OPERAND 1
    #define THIRD_OPERAND 2

/* ----------------------------- */
/* New Types */
/* ----------------------------- */

   /* 
   typedef: reqOperands
   --------------------
   Represents the required operands number data for each instruction command.
   */

    typedef enum{NO_OPERANDS , ONE_OPERAND, TWO_OPERANDS, THREE_OPERANDS, NOT_CONSTANT} reqOperands; /* stands for requiredOperands */

   /* 
   typedef: insturctionType.
   --------------------
   Represents instructionType enumeration.
   
   enum instructinoGroupType - represents all different instruction types (r, i, j).
   */ 
    
    typedef enum{INSTRUCTION_R, INSTRUCTION_I, INSTRUCTION_J} instructionType;

   /* 
   typedef: instructionGroupType
   --------------------
   Represents instructionGroupType enumeration.
   
   enum instructinoGroupType - represents the instruction group type for all instruction different types.
   */ 
    
    typedef enum instructionGroupType{ 
        ARITHMETIC_R_INSTRUCTIONS, 
        COPY_R_INSTRUCTIONS,
        ARITHMETIC_I_INSTRUCTIONS,
        BRANCHING_I_INSTRUCTIONS,
        LOADING_STORING_I_INSTRUCTIONS,
        JMP,
        LA,
        CALL,
        STOP} instructionGroupType;
   
   /* 
   typedef: dataType
   --------------------
   Represents the stored data size in directiveData struct.
   */
    
    typedef enum{BYTE = 1, HALF_WORD = 2, WORD = 4} dataType;

   /* 
   typedef: instruction
   --------------------
   Represents instruction structure.
    
   instruction struct - represents a binary machine insturction.
   value: instruction address.
   type: instruction type (r, i, j).
   instruction_fmt: conatin the binary instruction according to the insturction type format.
   */
    
    typedef struct instruction { 
        instructionType type;
        int value;
        union {
            struct { /* instruction r format */
                unsigned unused: 6;
                unsigned funct: 5;
                unsigned rd: 5;
                unsigned rt: 5;
                unsigned rs: 5;
                unsigned opcode: 6;

            } instruction_r;
            struct { /* instruction i format */
                unsigned immed: 16;
                unsigned rt: 5;
                unsigned rs: 5;
                unsigned opcode: 6;

            } instruction_i;
            struct { /* instruction j format */
                unsigned address: 25;
                unsigned reg: 1;
                unsigned opcode: 6;
            }instruction_j;
        } instruction_fmt;
    } instruction;

   /* 
   typedef: command
   --------------------
   Represents command structure.
    
   command struct - represents an instruction command in Assembly.
   opcode: command's opcode value.
   funct: command's funct value.
   operands_num: command's required operands number.
   instruction_type - instruction type (r, i, j).
   instruction_group - the instructions group which the command is part of.
   */
    
    typedef struct command {
        char *commmad_name;
        int opcode;
        int funct;
        reqOperands operands_num;
        instructionType instruction_type;
        instructionGroupType instruction_group;
        
    } command;

   /* 
   typedef: directiveData
   --------------------
   Represents directiveData structure.
    
   directiveData struct - represents values stored in the data image.
   address: the address where the value was defined.
   type: the value size in bytes. 
   data_fmt: the actual value stored according to the value size (byte, half-word, word).
   next - pointers to the next entry symbol in the entry list.
   */

    typedef struct directiveData {
        int address;
        dataType type;
        union {
            struct {
                unsigned int data: 8;
            } byte;
            struct {
                unsigned int data: 16;
            } half_word;
            struct {
                unsigned int data: 32;
            } word;
        } data_fmt;
    } directiveData;

   /* 
   typedef: externalNodePtr
   --------------------
   Represents externalNode structure ptr.
   */ 

    typedef struct externalNode * externalNodePtr;
    
   /* 
   typedef: externalNode
   --------------------
   Represents externalNode structure in the external symbols list(linked list).
    
   externalNode struct - represents an external symbol in the external list.
   address: entry symbol address.
   symbol_name: symbol name.
   next - pointers to the next entry symbol in the entry list.
   */

    typedef struct externalNode {
        int address;
        char *symbol_name;
        externalNodePtr next;
    } externalNode;

   /* 
   typedef: entryNodePtr
   --------------------
   Represents entryNode structure ptr.
   */ 
    typedef struct entryNode * entryNodePtr;
    
   /* 
   typedef: entryNode
   --------------------
   Represents entryNode structure in the entry symbols list(linked list).
    
   entryNode struct - represents an entry symbol in the entry list.
   address: entry symbol address.
   symbol_name: symbol name.
   next - pointers to the next entry symbol in the entry list.
   */

    typedef struct entryNode {
        int address;
        char *symbol_name;
        entryNodePtr next;
    } entryNode;


/* ----------------------------- */
/* External Variabels */
/* ----------------------------- */

    extern int ic, dc, lc;
    extern instruction code_image[];
    extern directiveData data_image[];
    extern int code_image_index, data_image_index;
    extern externalNodePtr ext_head;
    extern entryNodePtr ent_head;

/* ----------------------------- */
/* Functions */
/* ----------------------------- */

    /* ----- Pass ----- */ 
    void    pass(FILE *fp, char *file_name);

    void    print_code_image_to_file(FILE *object_file ,int icf);
    void    print_data_image_to_file(FILE *object_file ,int icf, int dcf);
    void    update_data_image(int icf);
    void    free_entry_list();
    void    free_external_list();
    void    free_code_image();
    void    free_data_image();
    void    reset_counters_indexes();
    void    print_entry_file(FILE *entry_file);
    void    print_external_file(FILE *external_file);
    void    print_object_file(FILE *object_file, int icf, int dcf);

    int     check_line(char *line);
    int     check_operands_num(char *line ,reqOperands num);
    int     convert_to_register(char *operand);
    int     add_instruction_to_code_image (instruction *instruction_ptr);
    int     add_to_data_image(long num, int jmp);
    int     set_operands_list(char *line, char **operands_list, reqOperands operands_num);
    int     create_output_files(char *file_name, int icf, int dcf);


    bool    is_command_word(char *);
    bool    is_directive_word(char *);
    bool    is_entry_word(char *word);
    bool    is_extern_word(char *word);
    bool    is_entry_list();
    bool    is_external_list();
    bool    is_valid_operands_line(char *line);
    bool    is_valid_asciz_line(char *line); /* checks if there is one operand in line; */

    command * get_command(char *word);

    /* ----- First pass ----- */ 
    
    void    assembler_first_pass(FILE *fp, char *file_name);
    
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
    int     parse_directive_sentence(char *line, char *word); /* need more work. */
    int     parse_command_sentence(char *line, char *word);
    int     parse_r_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    int     parse_i_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    int     parse_j_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    int     parse_asciz_command(char *line);

    /* ----- Second Pass ----- */ 
    
    void    assembler_second_pass(FILE *fp, char *file_name);

    int     parse_entry_sentence(char *line);
    int     complete_command_data(char *line, char *word);
    int     add_symbol_to_external_list(symbolPtr symbol, int command_address);
    int     add_symbol_to_entry_list(symbolPtr symbol);

#endif

