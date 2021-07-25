#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "errorCodes.h"


#ifndef ASSEMBLER_H

    #define ASSEMBLER_H
    
    /* Macros */
    #define ASSEMBLY_FILE_EXT ".as"
    #define OBJECT_FILE_EXT	".ob"
    #define ENTRY_FILE_EXT ".ent"
    #define EXTERN_FILE_EXT ".ext"
    #define EXTENSION_LENGTH 3

    #define MAX_LINE 80
    #define MAX_FILES 3
    #define MAX_LABEL_LENGTH 32 /* including ':' charcter suffix */
    #define MAX_CODE_SIZE 5000
    #define MAX_DATA_SIZE 5000


    #define MEMORY_STARTING_LOCATION 100
    #define MACHINE_INSTRUCTIONS_SIZE 4 /* 4 bytes. */
    #define ADDRESS_JMP 4
    #define BYTE_SIZE 8 /* 8 bits; */

    #define LABLE_SUFFIX ':'
    #define COMMENT ';'
    #define EOL '\n'
    #define EOS '\0'
    #define NEGATIVE_SIGN '-'
    #define POSITIVE_NUMBER '+'
    #define REG_SIGN '$'
    #define START_REGISTER 0
    #define END_REGISTER 31
    #define EMPTY 0


    #define COMMA_DELIMETER_CHAR ','
    #define LABEL_DELIMETER_CHAR ':'
    #define QUTATION_MARK_CHAR '\"'

    #define COMMA_DELIMETER_STR ","
    #define LABEL_DELIMITER_STR ":"
    #define WHITE_SPACES_DELIMITERS_STR " \t"

    /* New Types */

    typedef enum boolean{false, true} bool; 

    /* External Declarations */
    
    extern bool global_error_flag, global_memory_flag;
    extern int lc;

    /* Functions - assemblerUtils */

    void    print_error(int error_code);
    void    print_file_processing(int i);
    void    print_seperation();
    void    skip_spaces(char** word);
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
    bool    is_empty_line(char *word);
    bool    is_assembly_key_word(char *word);
    bool    is_comment(char *word);
    bool    is_error(int error_code);

    char *  get_next_element(char **line, char *delimeters) ;

#endif