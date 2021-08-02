/* 
	The file is responsible to do all second pass tasks and contains all required functions to do it.
	The main function here is the assembler_second_pass() function which is responsible for the second file scanning
	line by line and uses all other functions here to do it. The function also uses program general functions
	which are not defined here but necessary here.

*/

#include "../include/pass.h"

/* ------------------------------------------
Function: assembler_second_pass()
	Parse a given file for the second time, complete the whole code image data
	and build the externals + entries lists.
	The function reads and parses each line individually until end of line is encountered.
	In addition the functions checks several paramters and in case it detects a syntax error,
	a realted messgae is printed. 
Params:
	fp - a pointer points on the given file.
	file_name - the name of the file, used for printing error messages.
------------------------------------------- */

void assembler_second_pass(FILE *fp, char* file_name) { 

	char *allocated_line = (char *)malloc(MAX_LINE * sizeof(char)), *line , *word = NULL;
	int error_code = OK;
	
	/* reset previous indexes used by assembler_first_setup */
	code_image_index = 0;
	lc = 0;
	
	if(allocated_line) {
		/* allocated line address is assinged to the line variable to save 
		the real address in the allocated line for freeing it in the end of the function */
		for(line = allocated_line; (fgets(line, MAX_LINE, fp) != NULL) && (global_memory_flag == false); line = allocated_line) {
			
            lc++;
			word = NULL;

			if(is_error(error_code = check_line(line))) { /* check several paramters in line, will not continute if error was detected */
				if(error_code == MEMORY_ALLOCATION_FAILED) {
					print_error(error_code, file_name, NULL);
				}
				else if(error_code == ABOVE_MAX_LINE) {
					start_new_line(fp);
				}
			}
			else {
				/* if the function gets here it means the line is OK for parsing it and it has at least one
					element inside it (sequense of non white space characters). */ 
				word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
				
				if(!is_error(check_label(word, true, false))) { /* skips in case first word is a label. */
					word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
				}
				
				if(!(is_directive_command(word) || is_extern_command(word))) {
					if(is_entry_command(word)){
						if(is_error(error_code = parse_entry_sentence(line))) { /* parse entry command */
							print_error(error_code, file_name, word);
						}
					}
					else { /* command_sentence */
						if(is_error(error_code = complete_command_data(line, word))) { /* complete the missing code image for this command */
							print_error(error_code, file_name,word);
						} 
					}
				}
			}
		}
	}
	else {
		print_error(MEMORY_ALLOCATION_FAILED, file_name, NULL); /* allocted line is NULL meanning memory allocation failed */
	}
	free(allocated_line);
	return;
}

/* ------------------------------------------
Function: parse_entry_sentence()
	parse entry command sentence.
	Check if one operand was given and that it's a valid label name.
	Then check if the label exists in the symbol table and if so marks it as symbol entry 
	and add it to the entry list.
Params:
	line - the rest of the entry command sentence to parse.
Return:
	OK if the entry sentence is valid, a related error code if error was detected..
------------------------------------------- */

int parse_entry_sentence(char *line) {
	symbolPtr symbol;
	char *operand;
	int error_code = OK;

	if(!is_error(error_code = check_operands_num(line, ONE_OPERAND))) { /* entry sentnce should have only one operand - a label */
        operand = get_next_element(&line, COMMA_DELIMETER_STR);
		if(!is_error(error_code = check_label(operand, false, false))) {
			if((symbol = get_symbol_from_table(operand)) != NULL) {
				/* checks if the symbol is an external symbol becuase it can't be entry and external in the same time */
				if(symbol->attributes.external) { 
					error_code = SYMBOL_CANNOT_BE_DEFINED_AS_ENTRY_AND_EXTERNAL;
				}
				else {
					symbol->attributes.entry = true;
					if(is_symbol_in_entry_list(symbol->name) == false) {
						error_code = add_symbol_to_entry_list(symbol); /* add the symbol to the entry list */
					}
				}
			}
			else {
				error_code = SYMBOL_IS_NOT_DEFINED;
			}
		}
	}

	return error_code;
}

/* ------------------------------------------
Function: complete_command_data()
	Compelete command's data in the code image which can not be done in the first pass like label address value.
	In addition the function adds external symbols used by J instructions type 
	(the only place where they can be used) to the the externals list.
Params:
	line - the rest of the command sentence to parse.
	word - the command's name.
Return:
	OK if the the function terminated successfully, a related error code if error was detected..
------------------------------------------- */


int complete_command_data(char *line, char *word) {
	
	command *command_ptr = get_command(word);
	instructionType instruction_type = command_ptr -> instruction_type;
	instructionGroupType instruction_group = command_ptr -> instruction_group;
	symbolPtr symbol;
	reqOperands operands_num = command_ptr -> operands_num;
	char **operands_list = (char **)malloc(operands_num * sizeof(char *)); /* command operands are set in array */
	int error_code = OK;
 	 
	if (operands_list != NULL) {
		set_operands_list(line, operands_list, operands_num); /* should work fine because operands number was checked in the first lap. */

		if(instruction_group == BRANCHING_I_INSTRUCTIONS) {
			if((symbol = get_symbol_from_table(operands_list[THIRD_OPERAND]))) {
				if(!(symbol->attributes.external)) { /* external symbols can not be used in branching command */
					code_image[code_image_index].instruction_fmt.instruction_i.immed = symbol->value - code_image[code_image_index].instruction_fmt.instruction_i.immed;
				}
				else {
					error_code = EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND;
				}
			}
			else {
				error_code = SYMBOL_IS_NOT_DEFINED;
			}
		}
		else if(instruction_type == INSTRUCTION_J && instruction_group != STOP) {
			if(code_image[code_image_index].instruction_fmt.instruction_j.reg == false) {
				if((symbol = get_symbol_from_table(operands_list[FIRST_OPERAND]))){
					if(symbol->attributes.external) { /* external symbol can be used only in J instructions type */
						error_code = add_symbol_to_external_list(symbol, code_image[code_image_index].value);
					}
					code_image[code_image_index].instruction_fmt.instruction_j.address = symbol->value;	
				}
				else {
					error_code = SYMBOL_IS_NOT_DEFINED;
				}
			}	
		}
		free(operands_list);
	}
	else {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	/* Increase code image no matter which commands is used becuase 
	if this function is used meaning the current sentence is a command sentence */
	code_image_index++; 
	return error_code;
}

/* ------------------------------------------
Function: add_symbol_to_external_list()
	Add a given symbol as external record to the external list pointed by the global pointer - ext_head.
	The function creates a new entry record and concatenates it to the end of the list.
Params:
	symbil - the given symbol used to make the a new external record.
Return:
	OK if the addition succceeded or a related error code if error was detected..
------------------------------------------- */

int add_symbol_to_external_list(symbolPtr symbol, int address) {
	int error_code = OK;
	externalNodePtr new_ext_node_ptr = (externalNodePtr)malloc(sizeof(externalNode)), current_ext_node_ptr;
	/* new entry record creation */
	if(new_ext_node_ptr){
		strcpy(new_ext_node_ptr->symbol_name, symbol->name);
		new_ext_node_ptr->address = address;
		new_ext_node_ptr->next = NULL;
		
		/* Check if the list is empty, avoid using of a null pointr */	
		if(ext_head == NULL) {
			ext_head = new_ext_node_ptr;
		}
		else {
			current_ext_node_ptr = ext_head;
			while(current_ext_node_ptr->next != NULL) { /* going to the end of the list */
				current_ext_node_ptr = current_ext_node_ptr -> next;
			}
			current_ext_node_ptr->next = new_ext_node_ptr;
		}
	}
	else {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	return error_code;
}

/* ------------------------------------------
Function: add_symbol_to_entry_list()
	Add a given symbol as entry record to the entry list pointed by the global pointer - ent_head.
	The function created a new entry record and concatenate it to the end of the list.
Params:
	symbil - the given symbol used to make a new entry record.
Return:
	OK if the addition succceeded or a related error code if error was detected..
------------------------------------------- */

int add_symbol_to_entry_list(symbolPtr symbol) {
	int error_code = OK;
	entryNodePtr new_ent_node_ptr = (entryNodePtr)malloc(sizeof(entryNode)), current_ent_node_ptr;
	/* new entry record creation */
	if(new_ent_node_ptr){
		strcpy(new_ent_node_ptr->symbol_name, symbol->name);
		new_ent_node_ptr->address = symbol->value;
		new_ent_node_ptr->next = NULL;

	/* Check if the list is empty, avoid using of a null pointr */	
		if(ent_head == NULL) {
			ent_head = new_ent_node_ptr;
		}
		else {
			current_ent_node_ptr = ent_head;
			
			while(current_ent_node_ptr->next != NULL) { /* going to the end of the list */
				current_ent_node_ptr = current_ent_node_ptr -> next;
			}
			
			current_ent_node_ptr->next = new_ent_node_ptr;
		}
	}
	else {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	return error_code;
}


/* ------------------------------------------
Function: is_symbol_in_entry_list()
	Check if symbol is already in the entry list by checking the given symbol name
	with all other symbols names in the entry list.
Params:
	symbol_name - the given symbol name.
Return:
	TRUE if symbol is already in the entry list, FALSE otherwise.
------------------------------------------- */

bool is_symbol_in_entry_list(char *symbol_name) {
	entryNodePtr current_ent = ent_head;

	while(current_ent != NULL) { /* going over all the list unless it encounters the symbol in the list */
		if(are_strings_equal(symbol_name, current_ent->symbol_name)) {
			return true;
		}
		current_ent =  current_ent -> next;
	}
	
	return false;
}