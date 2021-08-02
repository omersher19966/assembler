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
    #define EXTENSION_SIZE 20 /* extension characters size for the output file */

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
    #define QUOTATION_MARK_CHAR '\"'
    
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


/* -------------------------------- */
/* External Variabels Declarations */
/* -------------------------------- */

    extern bool global_error_flag, global_memory_flag; /* global error flags used to detect if error was detected. */

/* ----------------------------- */
/* Functions */
/* ----------------------------- */
    
    /* Print an appropiate error message based on the given error code and then raises the global error flag.*/
    void    print_error(int error_code,char *file_name, char *word);
    /* Remove trailing space characters from the given string. */
    void    remove_trailing_spaces (char *str);
    /* Move forward to the next line in the file pointed by the given file pointer (fp).*/
    void    start_new_line(FILE *fp);
    /* Relase all allocated program memory.*/
    void    free_memory();
    /* Remove assembly file extension ".as" from the given file name*/
    void    remove_file_extension(char *file_name);
    
    /* Checks if a given operand is a valid register, return an appropiate error code in case it's not */
    int     check_register(char *operand);
    /* Return the number of the elements in a given line seprated by one of the given delimeters */
    int     get_num_of_elements(char *line, char *delimeters);
     
    /* Return TRUE if string are equal, FALSE otherwise */
    bool    are_strings_equal(char *str1, char *str2);
    /* Return TRUE if the given operand is a valid number, FALSE otherwise */
    bool    is_valid_number(char *operand);
    /* Return TRUE if the given operand is a valid string, FALSE otherwise */
    bool    is_valid_string(char *operand);
    /* Return TRUE if the given line is an empty line, FALSE otherwise */
    bool    is_empty_line(char *line);
    /* Return TRUE if the given path has a valid assembly extension ".as", FALSE otherwise */
    bool    is_valid_assembly_extension(char *file_name);
    /* Return TRUE if the given word is a save assembly keyword, FALSE otherwise */
    bool    is_assembly_keyword(char *word);
    /* Return TRUE if the line is a comment line based on the first given word in the line, FALSE otherwise */
    bool    is_comment_line(char *word);
    /* Return TRUE if error was detected , FALSE otherwise */
    bool    is_error(int error_code);
    
    /* Return a pointer to the next element in the line speradted by one of the given delimeters, return NULL in case element is empty*/
    char *  get_next_element(char **line, char *delimeters) ;

#endif