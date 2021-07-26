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
    #define INVALID_OPERAND 17
    #define OPERAND_IS_NULL 18
    #define REGISTER_NOT_IN_RANGE 19
    #define INVALID_ENTRY_SENTENCE 20
    #define EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND 21
    #define CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE 22
    #define CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE 23
    #define CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE 24


    /* Label/Symbol Error Codes */ 
    #define VALID_LABEL 40
    #define NOT_A_LABEL 41
    #define INVALID_LABEL 42
    #define LABEL_HAS_ALREADY_BEEN_USED 43
    #define ABOVE_MAX_LABEL_LENGTH 44
    #define SYMBOL_EXISTS_IN_TABLE 8
    #define SYMBOL_DOESNT_EXIST_IN_TABLE 45
    #define SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_ENTRY 46
    #define SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_EXTERNAL 47

#endif