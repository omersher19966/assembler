/*Error_Codes_H*/

#ifndef ERROR_CODES_H
   
    #define ERROR_CODES_H

/* ----------------------------- */
/* Errors Codes*/
/* ----------------------------- */

    #define OK 0
    
    /* Memory Errors 1 - 10*/

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

    /* Pass Errors 10 - 30 */
    #define FILE_IS_EMPTY 10
    #define ABOVE_MAX_LINE 11 
    #define COMMENT_LINE 12
    #define EMPTY_LINE 13
    #define NO_GIVEN_OPERANDS 14 
    #define GIVEN_OPERANDS_ARE_LESS_THAN_REQUIRED 15 
    #define GIVEN_OPERANDS_ARE_MORE_THAN_REQUIRED 16 
    #define INVALID_OPERANDS_LINE 17
    #define INVALID_OPERAND 18
    #define OPERAND_IS_EMPTY 19 
    #define EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND 20 
    #define CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE 21
    #define CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE 22
    #define CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE 23


    /* Label/Symbol Error Codes 30 - 40 */ 

    #define VALID_LABEL 30 
    #define NOT_A_LABEL 31 
    #define INVALID_LABEL 32 
    #define ABOVE_MAX_LABEL 33 
    #define LABEL_IS_ASSEMBLY_KEYWORD 34 
    #define LABEL_HAS_ALREADY_BEEN_USED 35 
    #define SYMBOL_IS_NOT_DEFINED 36 
    #define SYMBOL_CANNOT_BE_DEFINED_AS_ENTRY_AND_EXTERNAL 37

#endif