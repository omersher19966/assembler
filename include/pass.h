#include "../include/assemblerData.h"
#include "../include/symbol.h"

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
        char symbol_name[MAX_SYMBOL_NAME_LENGTH];
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
        char symbol_name[MAX_SYMBOL_NAME_LENGTH];
        entryNodePtr next;
    } entryNode;


/* -------------------------------- */
/* External Variabels Declarations */
/* -------------------------------- */

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
    
    /* Scan the given file, parses its assembly code and then creates the program output files, 
    print error to stdout in case error was detected. . */
    void    pass(FILE *fp, char *file_name);

    /* Print data image with the final icf value after the first pass. */
    void    update_data_image(int icf);
    /* Relase all allocated memory for the file parsing.*/
    void    free_pass_memory();
    /* Relase the entry list allocated memory. */
    void    free_entry_list();
     /* Relase the external list allocated memory. */
    void    free_external_list();
    /* Reset code image array. */
    void    reset_code_image();
    /* Reset data image array. */
    void    reset_data_image();
    /* Reset global counters and indexes like: code and data image indexes + line, instruction and data counters . */
    void    reset_counters_indexes();
    /* Print entry list to the entries file. */
    void    print_entry_file(FILE *entry_file);
    /* Print external list to the externals file. */
    void    print_external_file(FILE *external_file);
    /* Print code+data images to the object file. */
    void    print_object_file(FILE *object_file, int icf, int dcf);
    /* Adds a given instruction to the code image. */
    void    add_instruction_to_code_image (instruction *instruction_ptr);

    /* Check for line errors in the given line, return an appropiate error code in case error was detected.*/
    int     check_line(char *line);
    /* Check if the given line has the same operands number as the given given required operands, 
    return an appropiate error code in case error was detected.*/
    int     check_operands_num(char *line ,reqOperands num);
    /* Convert a given register operand to register number and return it. */
    int     convert_to_register(char *operand);
    /* Add a new data to the program data image if it's not full, return error code in case data image is full*/
    int     add_to_data_image(long num, int jmp);
    /* Set a given operands list array with operands from the given line, 
    return an appropiate error code in case error was detected. */
    int     set_operands_list(char *line, char **operands_list, reqOperands operands_num);
    /* Create program's output files (object, ent, ext), return an appropiate error code in case error was detected. */
    int     create_output_files(char *file_name, int icf, int dcf);

    /* Return TRUE if the given word is an assembly command, FALSE otherwise */
    bool    is_command(char *);
    /* Return TRUE if the given word is an assembly directive command, FALSE otherwise */
    bool    is_directive_command(char *);
    /* Return TRUE if the given word is an entry command, FALSE otherwise */
    bool    is_entry_command(char *word);
    /* Return TRUE if the given word is an entry command, FALSE otherwise */
    bool    is_extern_command(char *word);
    /* Return TRUE if the entry list is not empty, FALSE otherwise */
    bool    is_entry_list();
    /* Return TRUE if the external list is not empty, FALSE otherwise */
    bool    is_external_list();
    /* Return TRUE if the given line is a valid operands line (last char is not a comma), FALSE otherwise */
    bool    is_valid_operands_line(char *line);
    
    /* Searche for an assembly command with the same name as the given given namem, 
    return a pointer to the command if it finds it, NULL otherwise */
    command * get_command(char *name);

    /* ----- First pass ----- */ 
    
    /* Scans and Parses the given assembly file for the first time, Stored the data in the right structures 
    which are used after for the output files, print error to stdout in case error was detected.. */
    void    assembler_first_pass(FILE *fp, char *file_name);
    
    /* Set binary machine insturction for arithmetic R instructions group */
    void    set_arithmetic_r_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register);
    /* Set binary machine insturction for copy R instructions group */
    void    set_copy_r_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register);
    /* Set binary machine insturction for arithmetic I instructions group */
    void    set_arithmetic_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed);
    /* Set binary machine insturction for loading & storing I instructions group, 
    Store the instruction in the code image */
    void    set_loading_storing_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed);
    /* Set binary machine insturction for branching I instructions group */
    void    set_branching_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register);
    /* Set binary machine insturction for jmp instruction */
    void    set_jmp_instruction(instruction *instruction_ptr, command *command_ptr, int reg ,bool reg_flag);
    /* Set binary machine insturction for la instruction */
    void    set_la_instruction(instruction *instruction_ptr, command *command_ptr);
    /* Set binary machine insturction for call instruction */
    void    set_call_instruction(instruction *instruction_ptr, command *command_ptr);
    /* Set binary machine insturction for stop instruction */
    void    set_stop_instruction(instruction *instruction_ptr, command *command_ptr);

    /* Parse extern commands, store the given label in the symbol tabel with the correct attributes.
    return error code in case error was detected. */
    int     parse_extern_sentence(char *line);
    /* Parses directive commands, stores the given operands in the data image.
    return error code in case error was detected. */
    int     parse_directive_sentence(char *line, char *word);
    /* Parses instructions commands, stores the given operands in the right program structures.
    return error code in case error was detected. */
    int     parse_command_sentence(char *line, char *cmd_name);
    /* Parses R instructions type, creates binary machine instruction and then adds it to the code image.
    return error code in case error was detected. */
    int     parse_r_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    /* Parses I instructions type, creates binary machine instruction and then adds it to the code image.
    return error code in case error was detected. */
    int     parse_i_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    /* Parses J instructions type, creates binary machine instruction and then adds it to the code image.
    return error code in case error was detected. */
    int     parse_j_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num);
    /* Parses given asciz command line, stores the given ascis operand in the data image.
    return error code in case error was detected. */
    int     parse_asciz_command(char *line);

    /* ----- Second Pass ----- */ 
    
    /* Scans and Parses the given assembly file for the Second time, Complete full code and data images which 
    are used after for the output files, print error to stdout in case error was detected.. */
    void    assembler_second_pass(FILE *fp, char *file_name);
    /* Parses entry commands, stores the given label in the symbol tabel with the correct attributes.
    return error code in case error was detected. */
    int     parse_entry_sentence(char *line);
    /* Complete the comannd's data in the code image if it's needed, return error code in case error was detected. */
    int     complete_command_data(char *line, char *word);
    /* Add the given symbol to the external list with the given addrress, return error code in case error was detected. */
    int     add_symbol_to_external_list(symbolPtr symbol, int command_address);
    /* Add the given symbol to the entry list, return error code in case error was detected. */
    int     add_symbol_to_entry_list(symbolPtr symbol);

    bool    is_symbol_in_entry_list(char *symbol_name);

#endif

