#include "symbol.h"

symbolPtr root;

/* ---------------------------------------- */

int check_label(char *word, bool check_new_label) {
	int error_code = OK;
	
	if((check_new_label) && (!is_new_label(word))) {
		error_code = NOT_A_LABEL;
	}
	else if (!is_valid_label(word)) {
		error_code =  INVALID_LABEL;
	}
	else if (is_symbol_exist(word)){
		error_code = LABEL_HAS_ALREADY_BEEN_USED;
	}
	
	return error_code;
}

/* ---------------------------------------- */

bool is_new_label(char *word) {
	
	int last_character_index = strlen(word) - 1 ;
	if(word[last_character_index] == LABEL_SUFFIX) {
		word[last_character_index] = EOS;
		return true;
	}
	
	return false;

}

/* ---------------------------------------- */

bool is_valid_label(char *word) {
	bool valid_label_flag;
	int i = 0, 	max_label_length_without_suffix = MAX_LABEL_LENGTH - 1, current_label_length = strlen(word);

	if (is_assembly_key_word(word)) {
		valid_label_flag = false;
	}
	else if (current_label_length > max_label_length_without_suffix) {
		valid_label_flag = false;
	}
	else {
		if (isalpha(word[i++])) {
			while((isalnum(word[i++])) && (i < current_label_length))
			;
			if(i == current_label_length) {
				valid_label_flag = true;
			}
			else {
				valid_label_flag = false;
			}
		}
		else {
			valid_label_flag = false;
		}
	}

	return valid_label_flag;
}

/* ---------------------------------------- */

bool is_symbol_exist(char *symbol_name) {
	symbolPtr current_symbol = root;
	while(current_symbol) {
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

/* ---------------------------------------- */

symbolPtr get_symbol_from_table(char *symbol_name) {
	symbolPtr current_symbol = root;
	while(current_symbol) {
		if(!strcmp(current_symbol->name,symbol_name)) {
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

/* ---------------------------------------- */

void add_symbol_to_table(char *symbol_name, bool data, bool code, bool entry, bool external) {
	
	symbolPtr new_symbol = create_new_symbol(symbol_name, data, code, entry, external); /* need to handle malloc */
	symbolPtr current_symbol;
	
	if(root == NULL) {
		root = new_symbol;
	}
	else {
		
		current_symbol = root;
	
		while(current_symbol) {
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
	return;
}


/* ---------------------------------------- */

symbolPtr create_new_symbol(char *symbol_name, bool data, bool code, bool entry, bool external) {
	
	symbolPtr new_symbol = (symbolPtr)malloc(sizeof(symbolNode));

	strcpy(new_symbol->name, symbol_name);
	if (external) {
		new_symbol->value = 0;
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

	return new_symbol;
}

/* ---------------------------------------- */

/* update only data symbols using recursion */
static void update_symbol_table_recursion(symbolPtr root, int icf) {
	if(root == NULL) {
		return;
	}
	if(root->attributes.data) {
		root->value += icf;
	}
	update_symbol_table_recursion(root->left, icf);
	update_symbol_table_recursion(root->right, icf);
}


void update_symbol_table(int icf) {
	update_symbol_table_recursion(root, icf); /* static recursion method to update data symbols with icf. */
}

/* ---------------------------------------- */

static void free_symbol_table_recursion(symbolPtr root) {
	if(root == NULL) {
		return;
	}
	free_symbol_table_recursion(root->left);
	free_symbol_table_recursion(root->right);
	free(root);
}

void free_symbol_table() {
	free_symbol_table_recursion(root);
}

/* ---------------------------------------- */