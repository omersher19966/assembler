#include "assemblerData.h"

#ifndef SYMBOL_H
    
    #define SYMBOL_H

/* ----------------------------- */
/* Macros */
/* ----------------------------- */

    #define LABEL_SUFFIX ':' /* Sign for the label ending */
    #define MAX_SYMBOL_NAME_LENGTH	32 /* Max label length including label suffix ':' */

/* ----------------------------- */
/* External Declarations */
/* ----------------------------- */

extern int ic, dc;

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


    int					check_label(char *word, bool check_new_label, bool check_symbol_tabel);
    int    	  	check_valid_label(char *word);

    bool				is_new_label(char *word);


    /* Symbols functions */

    bool    		is_symbol_exist(char *symbol_name);

    symbolPtr   get_symbol_from_table(char *symbol_name);
    symbolPtr   create_new_symbol(char *, bool code, bool data, bool entry, bool external);

    int   			add_symbol(char *, bool code, bool data);

    int   		add_symbol_to_table(char *, bool code, bool data, bool entry, bool external);
    void    		update_symbol_table(int icf);
    void    		free_symbol_table();


#endif