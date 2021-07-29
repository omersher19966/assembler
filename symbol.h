#include "assemblerData.h"

#ifndef SYMBOL_H
    
    #define SYMBOL_H

/* ----------------------------- */
/* Macros */
/* ----------------------------- */

    #define LABEL_SUFFIX ':' /* Sign for the label ending */
    #define MAX_SYMBOL_NAME_LENGTH	32 /* Max label length including label suffix ':' */

/* -------------------------------- */
/* External Variabels Declarations */
/* -------------------------------- */

  extern int ic, dc; /* used in symbolTable to determine the symbol address */

/* ----------------------------- */
/* New Types */
/* ----------------------------- */
   
   /* 
   typedef: symbolPtr
   --------------------
   Represents symbolNode (defined as typedef) ptr.
   */ 
    typedef struct symbolNode * symbolPtr;

   /* 
   typedef: symbolNode
   --------------------
   Represents symbolNode structure.
    
   symbolNode structure - represents a symbol which is used in the symbol table.
   name: symbol name.
   value: symbol address.
   attributes - symbol attributes(code, data, external, entry)
   right, left - pointers to the next symbol nodes in the symbol table.
   */

    typedef struct symbolNode{
      char name[MAX_SYMBOL_NAME_LENGTH];
      int value;
      struct {
        bool code;
        bool data;
        bool external;
        bool entry;
      }attributes;
      symbolPtr right;
      symbolPtr left;

    } symbolNode;

/* ----------------------------- */
/* Functions  */
/* ----------------------------- */

    /* ----------- Label Functions ----------- */

    /* Check if the label is valid. return an error code in case error occured */
    int					check_label(char *word, bool check_new_label, bool check_symbol_tabel);
    /* Check if a given label is a valid label. return an error code in case error occured */
    int    	  	check_valid_label(char *word);
    
    /* Return TRUE if label is terminates with colon, FALSE otherwise */
    bool				is_new_label(char *word);


    /* ----------- Symbols Functions ----------- */
    
    /* Update all data symbols with the final icf value.*/
    void    		update_symbol_table(int icf);
    /* Relase the memory allocation of all symbol in the symbol table.*/
    void    		free_symbol_table();

    /* Adds a given label data as a symbol to the symbol table , return appropiate error code in case error occured.*/
    int   		  add_symbol_to_table(char *, bool code, bool data, bool entry, bool external);
    
    /* Return TRUE if given symbol name exists in the symbol table, FALSE otherwise */
    bool    		is_symbol_exist(char *symbol_name);

    /* Return pointer to a symbol with the given symbol name , return NULL if the symbol doesn't exist.*/
    symbolPtr   get_symbol_from_table(char *symbol_name);
    /* Return pointer to a new symbol if memory allocation succeeded , return NULL otherwise.*/
    symbolPtr   create_new_symbol(char *, bool code, bool data, bool entry, bool external);

#endif