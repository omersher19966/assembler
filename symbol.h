#include "assemblerData.h"

/* -------- Macros ---------- */

#define LABEL_SUFFIX ':'
#define MAX_SYMBOL_NAME_LENGTH	32

/* -------- External Declarations ---------- */

extern int ic, dc;

/* -------- New Types ---------- */

/* symbol node pointer */
typedef struct symbolNode * symbolPtr;

/* symbol node */

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

/* -------- Functions ---------- */

/* Labels functions, label - a word before insertion to the symbol table */

int					check_label(char *word, bool check_new_label);

bool				is_new_label(char *word);
bool    		is_valid_label(char *word);

/* Symbols functions, symbol - a word after insertion to the symbol table. */


bool    		is_symbol_exist(char *symbol_name);

symbolPtr   get_symbol_from_table(char *symbol_name);
symbolPtr   create_new_symbol(char *, bool code, bool data, bool entry, bool external);

int   			add_symbol(char *, bool code, bool data);

void    		add_symbol_to_table(char *, bool code, bool data, bool entry, bool external);
void    		update_symbol_table(int icf);
void    		free_symbol_table();
