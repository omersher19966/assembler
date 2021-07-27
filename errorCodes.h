/*error_code_h*/

#ifndef ERROR_CODES_H
   
    #define ERROR_CODES_H
   
    /* Errors */
    #define OK 0
    
    /* Memory Errors */
    #define MEMORY_ALLOCATION_FAILED 1 /* v */
    #define DATA_IMAGE_IS_FULL 2 /* v */
    #define CODE_IMAGE_IS_FULL 3 /* v */
  
    /* Assembler Errors */
    #define INVALID_EXTENSION 4 /* v - message is not needed */
    #define INVALID_NUMBER 5 /* v */
    #define INVALID_STRING 6 /* v */
    #define INVALID_CMD 7 /* v */
    #define INVALID_REGISTER 8 /* v */

    /* Pass Errors */
    #define ABOVE_MAX_LINE 10  /* v */
    #define COMMENT_LINE 11 /* v - message is not needed */
    #define EMPTY_LINE 12 /* v - message is not needed */
    #define NO_GIVEN_OPERANDS 13 /* v */
    #define GIVEN_OPERANDS_ARE_LESS_THAN_REQUIRED 14 /* v */
    #define GIVEN_OPERANDS_ARE_MORE_THAN_REQUIRED 15 /* v */
    #define INVALID_OPERANDS_LINE 16 /* v */
    #define INVALID_OPERAND 17 /* v */
    #define OPERAND_IS_EMPTY 18 /* v */
    #define EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND 19 /* v */
    #define CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE 22 /* v */
    #define CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE 23 /* v */
    #define CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE 24 /* v */


    /* Label/Symbol Error Codes */ 
    #define VALID_LABEL 30 /* v - message is not needed */
    #define NOT_A_LABEL 31 /* v - message is not neede */
    #define INVALID_LABEL 32 /* v */
    #define ABOVE_MAX_LABEL 33 /* v */
    #define LABEL_IS_ASSEMBLY_KEYWORD 34 /* v */
    #define LABEL_HAS_ALREADY_BEEN_USED 35 /* v */
    #define SYMBOL_IS_NOT_DEFINED 36 /* need to handle 36-38 after I will get an answer in the forum */
    #define SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_ENTRY 37
    #define SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_EXTERNAL 38

#endif