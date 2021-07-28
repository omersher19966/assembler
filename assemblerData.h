#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "errorCodes.h"


#ifndef ASSEMBLER_H

    #define ASSEMBLER_H

/* ----------------------------- */
/* Macros */
/* ----------------------------- */

    /* File Extensions */
    #define ASSEMBLY_FILE_EXT ".as"
    #define OBJECT_FILE_EXT	".ob"
    #define ENTRY_FILE_EXT ".ent"
    #define EXTERN_FILE_EXT ".ext"

    /* Maximun prgoram values */
    #define MAX_LINE 80
    #define MAX_FILES 3 /* ???? */
    #define MAX_LABEL_LENGTH 32 /* including ':' charcter suffix */
    #define MAX_CODE_SIZE 5000
    #define MAX_DATA_SIZE 5000

    /* Machine Paramters */
    #define MEMORY_STARTING_LOCATION 100 /* Starting code image address */
    #define MACHINE_INSTRUCTIONS_SIZE 4  /*4 bytes for each instruction */
    #define START_REGISTER 0
    #define END_REGISTER 31
    
    /* Useful Macros */
    #define EMPTY 0
    #define BYTE_SIZE 8 /* 8 bits; */
    #define EOL '\n' /* Stands for End Of Line */
    #define EOS '\0' /* Stands for End Of String */
    #define NEGATIVE_SIGN '-' /* used for validating positive number */
    #define POSITIVE_SIGN '+'
    #define REG_SIGN '$'

    /* Useful Characters Macros */
    #define COMMENT_CHAR ';'
    #define COMMA_DELIMETER_CHAR ','
    #define QUTATION_MARK_CHAR '\"'
    
    /* Line delimeters for get_next_element function*/
    #define COMMA_DELIMETER_STR "," /* used for finding the next operand */
    #define WHITE_SPACES_DELIMITERS_STR " \t" /*used for finding the next word */

/* ----------------------------- */
/* New Types */
/* ----------------------------- */
   
   /* 
   typedef: bool
   --------------------
   Represents boolean enumeration.
   
   enum boolean - represents the bollean values : true = 1, false = 0; 
   */ 
   typedef enum boolean{false, true} bool; 


/* ----------------------------- */
/* External Declarations */
/* ----------------------------- */

    extern bool global_error_flag, global_memory_flag;
    extern int lc;

/* ----------------------------- */
/* Functions */
/* ----------------------------- */

    void    print_error(int error_code,char *file_name, char *word);
    void    remove_trailing_spaces (char *word);
    void    start_new_line(FILE *fp);
    void    free_memory();
    void    remove_file_extension(char *file_name);

    int     check_register(char *operand);
    int     check_assembly_extension(char *path);
    int     get_num_of_elements(char *line, char *delimeters);

    bool    are_strings_equal(char *str1, char *str2);
    bool    is_valid_number(char *operand);
    bool    is_valid_string(char *operand);
    bool    is_empty_line(char *line);
    bool    is_assembly_keyword(char *word);
    bool    is_comment(char *word);
    bool    is_error(int error_code);

    char *  get_next_element(char **line, char *delimeters) ;

#endif