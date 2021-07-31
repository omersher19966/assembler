/* 
	The file repersents a symbol table implemented as binary search tree structure. 
	The symbol table is used to store all labels/symbols used in the assembly program.
	The symbol sorting is based on the symbol name and its place in the binary search tree is calculated by
	the lexographic order.

*/

#include "../include/symbol.h" /* includes all macros and decelrations used in the symbolTable.c file */

/* ------ Symbol Table Root Node ------ */

symbolPtr root = NULL;

/* ------ Static functions ------ */

/* Update symbol table recursively with icf final value */
static void update_symbol_table_recursion(symbolPtr root, int icf);
/* Release recursivly symbol table allocated memory*/
static void free_symbol_table_recursion(symbolPtr root);

/* ------------------------------------------
Function: check_label()
	Check for several label criterias in the given word.
Params:
	word - the given word/label.
	check_new_label - a flag for checking if the given word already exists in the symbol table.
	check_symbol_tabel - a flag for checking if the given word already exists in the symbol table.
Return: 
	OK if the given word is a valid label, a related error code otherwise. 
------------------------------------------- */

int check_label(char *word, bool check_new_label, bool check_symbol_table) {
	
	int error_code = OK;
	
	if (check_new_label && (!is_new_label(word))) { /* check if is_new_label just if check_new_label is up */
			error_code = NOT_A_LABEL;
	}

	if(!is_error(error_code)) {
		error_code = check_valid_label(word); /* label validation */
	}
	
	if ((check_symbol_table) && (!is_error(error_code))) { /* check if check_symbol_table is up and error code = OK */
		if (is_symbol_exist(word)){
			error_code = LABEL_HAS_ALREADY_BEEN_USED;
		}
	}
	
	return error_code;
}

/* ------------------------------------------
Function: is_new_label()
	Check if the given word can be considered as a new label definition with the format "label:".
Params:
	word - the given word/label.
Return: 
	TRUE if the given word is a valid new label i.e last character is colon, False otherwise. 
------------------------------------------- */

bool is_new_label(char *word) {
	
	int last_character_index = strlen(word) - 1 ;
	if(word[last_character_index] == LABEL_SUFFIX) {
		word[last_character_index] = EOS;
		return true;
	}
	
	return false;

}

/* ------------------------------------------
Function: check_valid_label()
	Check if the given word/label is a valid label.
	Check several creiteria like: assembly keyword, max label length and name convention.
Params:
	word - the given word/label.
Return: 
	OK if the given word is a valid label, a related error code otherwise. 
------------------------------------------- */

int check_valid_label(char *word) {
	int error_code = OK;
	int i = 0, 	max_label_length_without_suffix = MAX_LABEL_LENGTH - 1, current_label_length = strlen(word);

	if (is_assembly_keyword(word)) { /* check if the given word is already an assembly keyword */
		error_code = LABEL_IS_ASSEMBLY_KEYWORD;
	}
	else if (current_label_length > max_label_length_without_suffix) {
		error_code = ABOVE_MAX_LABEL;
	}
	else {
		if (isalpha(word[i++])) { /* check if the first char of the given word is alpha according the label name convention */
			for(;(isalnum(word[i])) && (i < current_label_length);i++) /* check the rest chars are alpha or num only according to the label name convention */
			;
			if(i == current_label_length) { /* meanning all chars were alpha or nums */
				error_code = OK;
			}
			else {
				error_code = INVALID_LABEL;
			}
		}
		else {
			error_code = INVALID_LABEL;
		}
	}

	return error_code;
}

/* ------------------------------------------
Function: is_symbol_exist()
	Check if there is a symbol in the symbol table with the same name as the given symbol name.
Params:
	symbol_name - the given symbol name.
Return: 
	TRUE if the given symbol_name is a name of one of the symbols in the symbol table, False otherwise. 
------------------------------------------- */

bool is_symbol_exist(char *symbol_name) {
	symbolPtr current_symbol = root;
	
	while(current_symbol) { /* Search name using lexographic order */
		if(are_strings_equal(current_symbol-> name, symbol_name)) {
			return true;
		}
		else if (strcmp(current_symbol->name, symbol_name) > 0) {
        	current_symbol = current_symbol->left;
    	}
    	else {
      		current_symbol = current_symbol->right;
    	}
	}
	return false;
}

/* ------------------------------------------
Function: get_symbol_from_table()
	Search for a symbol with same name as the given symbol name in the symbol table.
Params:
	symbol_name - the given name.
Return: 
	Pointer to the symbol if found, NULL otherwise.
------------------------------------------- */

symbolPtr get_symbol_from_table(char *symbol_name) {
	symbolPtr current_symbol = root;
	while(current_symbol) {/* Search name using lexographic order */
		if(are_strings_equal(current_symbol->name,symbol_name)) {
			return current_symbol;
		}
		else if (strcmp(current_symbol->name, symbol_name) > 0) {
        	current_symbol = current_symbol->left;
    	}
    	else {
      		current_symbol = current_symbol->right;
    	}
	}
	return NULL;

}

/* ------------------------------------------
Function: add_symbol_to_table()
	Add a new symbol defined by the given paramters the the symbol table(binary search tree).
	Add the symbol to the correct place in the symbol table determined by the lexographic order of its name.
Params:
	symbol_name - the given name.
	data - a flag used to determine if the the new symbol is a data type or not.
	code - a flag used to determine if the the new symbol is a code type or not.
	entry - a flag used to determine if the the new symbol is an entry symbol or not.
	external - a flag used to determine if the the new symbol is an external symbol or not.
Return: 
	OK if the new symbol was added to the symbol tabel, a related error code otherwise.
------------------------------------------- */

int add_symbol_to_table(char *symbol_name, bool code, bool data, bool entry, bool external) {
	int error_code = OK;
	symbolPtr new_symbol = create_new_symbol(symbol_name, data, code, entry, external);
	symbolPtr current_symbol;
	
	if(new_symbol != NULL) { /* check if memory allocation succeeded for the new symbol */
		if(root == NULL) { 
			root = new_symbol; /* in case symbol table is empty */
		}
		else {
			
			current_symbol = root;
		
			while(current_symbol) { /* search the right place to place the new symbol using lexographic search */
				if (strcmp(new_symbol->name, current_symbol->name) > 0) {
					if(current_symbol->right == NULL) {
						current_symbol->right = new_symbol;
						break;
					}
					else {
						current_symbol = current_symbol->right;
					}
				}
				else {
					if(current_symbol->left == NULL) {
						current_symbol->left = new_symbol;
						break;
					}
					else {
						current_symbol = current_symbol->left;
					}
				}	
			}
		}
	}
	else {
		error_code = MEMORY_ALLOCATION_FAILED;
	}

	return error_code;
}


/* ------------------------------------------
Function: get_symbol_from_table()
	Create a new symbol from the given symbol paramaters.
Params:
	symbol_name - the given name.
	data - a flag used to determine if the the new symbol is a data type or not.
	code - a flag used to determine if the the new symbol is a code type or not.
	entry - a flag used to determine if the the new symbol is an entry symbol or not.
	external - a flag used to determine if the the new symbol is an external symbol or not.
Return: 
	Pointer to the new symbol, NULL if couldn't create a new symbol.
------------------------------------------- */

symbolPtr create_new_symbol(char *symbol_name, bool data, bool code, bool entry, bool external) {
	
	symbolPtr new_symbol = (symbolPtr)malloc(sizeof(symbolNode));
	
	if(new_symbol != NULL) {
		/* set symbol attributes */
		strcpy(new_symbol->name, symbol_name);
		if (external) {
			new_symbol->value = EMPTY;
		}
		else if(code) {
			new_symbol->value = ic;
		} else {
			new_symbol->value = dc;
		}
		new_symbol->attributes.code = code;
		new_symbol->attributes.data = data;
		new_symbol->attributes.entry = entry;
		new_symbol->attributes.external = external;
		new_symbol->left = NULL;
		new_symbol->right = NULL;
	}
	
	return new_symbol;
}

/* ------------------------------------------
Function: update_symbol_table()
	Update all symbols's addresses in the symbol table with the final icf(instruction counter final).
	Use a recucresion function called update_symbol_table_recursion() to go recursively over the symbol
	table and update all symbol's addresses.
Params:
	icf - last insturction counter final value.
------------------------------------------- */

void update_symbol_table(int icf) {
	update_symbol_table_recursion(root, icf); /* static recursion method to update data symbols with icf. */
}

/* static because used by update_symbol_table only to update symbol recursively with icf.*/
static void update_symbol_table_recursion(symbolPtr root, int icf) {
	if(root == NULL) { /* recursion breakpoint */
		return;
	}
	if((root->attributes.data) && (!root->attributes.external)) {
		root->value += icf;
	}
	update_symbol_table_recursion(root->left, icf); /* update the left branch */
	update_symbol_table_recursion(root->right, icf); /* update the right branch */
}



/* ------------------------------------------
Function: free_symbol_table()
	Release all symbol allocated memomry i.e removes all symbols from the symbol table and free the memory.
	Use a recucresion function called free_symbol_table_recursion() to go recursively over the symbol
	table and free all symbols inside.
------------------------------------------- */

void free_symbol_table() {
	free_symbol_table_recursion(root);
	root = NULL;
}

/* static because used by free_symbol_table only to release symbol table allocated memory recursively.*/
static void free_symbol_table_recursion(symbolPtr root) {
	if(root == NULL) {
		return;
	}
	free_symbol_table_recursion(root->left); /* free the left branch */
	free_symbol_table_recursion(root->right); /* free the right branch */
	free(root);
}



/* ---------------------------------------- */