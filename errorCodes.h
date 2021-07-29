/*Error_Codes_H*/

#ifndef ERROR_CODES_H
   
    #define ERROR_CODES_H

/* ----------------------------- */
/* Errors Codes*/
/* ----------------------------- */

    #define OK 0
    
    /* Memory Errors */
    #define MEMORY_ALLOCATION_FAILED 1 
    #define DATA_IMAGE_IS_FULL 2 
    #define CODE_IMAGE_IS_FULL 3 
  
    /* Assembler Errors */
    #define INVALID_EXTENSION 4
    #define INVALID_NUMBER 5 
    #define INVALID_STRING 6 
    #define INVALID_CMD 7 
    #define INVALID_REGISTER 8 
    #define NOT_IN_REGISTERS_RANGE 9

    /* Pass Errors */
    #define ABOVE_MAX_LINE 10  
    #define COMMENT_LINE 11 
    #define EMPTY_LINE 12 
    #define NO_GIVEN_OPERANDS 13 
    #define GIVEN_OPERANDS_ARE_LESS_THAN_REQUIRED 14 
    #define GIVEN_OPERANDS_ARE_MORE_THAN_REQUIRED 15 
    #define INVALID_OPERANDS_LINE 16 
    #define INVALID_OPERAND 17 
    #define OPERAND_IS_EMPTY 18 
    #define EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND 19 
    #define CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE 22 
    #define CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE 23 
    #define CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE 24 


    /* Label/Symbol Error Codes */ 
    #define VALID_LABEL 30 
    #define NOT_A_LABEL 31 
    #define INVALID_LABEL 32 
    #define ABOVE_MAX_LABEL 33 
    #define LABEL_IS_ASSEMBLY_KEYWORD 34 
    #define LABEL_HAS_ALREADY_BEEN_USED 35 
    #define SYMBOL_IS_NOT_DEFINED 36 
    #define SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_ENTRY 37
    #define SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_EXTERNAL 38

#endif