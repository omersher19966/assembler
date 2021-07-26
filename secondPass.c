#include "pass.h"

/* ---------------------------------------- */

void assembler_second_pass(FILE *fp, int file_num) { 

	char *allocated_line = (char *) malloc(MAX_LINE), *line , *word = NULL;
	int error_code = OK;
	
	/* reset previous indexes. */
	code_image_index = 0;
	lc = 0;
	
	if(allocated_line) {

		for(line = allocated_line; (fgets(line, MAX_LINE, fp) != NULL) && (global_memory_flag == false); line = allocated_line) {
			
            lc++;
			word = NULL;

			if(is_error(error_code = check_line(line))) {
				if(error_code == MEMORY_ALLOCATION_FAILED) {
					print_error(error_code, file_num);
				}
				else if(error_code == ABOVE_MAX_LINE) {
					start_new_line(fp);
				}
			}
			else {
				word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
				
				if(!is_error(check_label(word, true, false))) { /* skips in case first word is a label. */
					word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
				}
				
				if(!(is_guidance_word(word) || is_extern_word(word))) {
					if(is_entry_word(word)){
						if(is_error(error_code = parse_entry_sentence(line))) {
							print_error(error_code, file_num);
						}
					}
					else { /* command_sentence */
						if(is_error(error_code = complete_command_data(line, word))) {
							print_error(error_code, file_num);
						} 
					}
				}
			}
		}
	}
	else {
		print_error(MEMORY_ALLOCATION_FAILED, file_num);
	}
	free(allocated_line);
	return;
}

/* ---------------------------------------- */

int parse_entry_sentence(char *line) {
	symbolPtr symbol;
	char *operand;
	int error_code = OK;

	if(!is_error(error_code = check_operands_num(line, ONE_OPERAND))) {
        operand = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
		if(!is_error(error_code = check_label(operand, false, false))) {
			if((symbol = get_symbol_from_table(operand)) != NULL) {
				if(symbol->attributes.external) {
					error_code = SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_EXTERNAL;
				}
				else {
					symbol->attributes.entry = true;
					error_code = add_symbol_to_entry_list(symbol);
				}
			}
			else {
				error_code = SYMBOL_DOESNT_EXIST_IN_TABLE; /* need to change to more reasnoble error_code. */
			}
		}
	}

	return error_code;
}

/* ---------------------------------------- */

int complete_command_data(char *line, char *word) {
	command *command_ptr = get_command(word);
	instructionType instruction_type = command_ptr -> instruction_type;
	instructionGroupType instruction_group = command_ptr -> instruction_group;
	symbolPtr symbol;
	reqOperands operands_num = command_ptr -> operands_num;
	char *operands_list[operands_num];
	int error_code = OK;
 	 
	set_operands_list(line, operands_list, operands_num); /* should work fine because error_code was checked in the first lap. */

	if(instruction_group == BRANCHING_I_INSTRUCTIONS) {
		if((symbol = get_symbol_from_table(operands_list[THIRD_OPERAND]))) {
			if(!(symbol->attributes.external)) {
				code_image[code_image_index].instruction_fmt.instruction_i.immed = symbol->value - code_image[code_image_index].instruction_fmt.instruction_i.immed;
			}
			else {
				error_code = EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND;
			}
		}
		else {
			error_code = SYMBOL_DOESNT_EXIST_IN_TABLE;
		}
	}
	else if(instruction_type == INSTRUCTION_J && instruction_group != STOP) {
		if(code_image[code_image_index].instruction_fmt.instruction_j.reg == false) {
			if((symbol = get_symbol_from_table(operands_list[FIRST_OPERAND]))){
				if(symbol->attributes.external) {
					error_code = add_symbol_to_external_list(symbol, code_image[code_image_index].value);
				}
				code_image[code_image_index].instruction_fmt.instruction_j.address = symbol->value;	
			}
			else {
				error_code = SYMBOL_DOESNT_EXIST_IN_TABLE;
			}
		}	
	}
	
	code_image_index++;
	return error_code;
}

/* ---------------------------------------- */

int add_symbol_to_external_list(symbolPtr symbol, int address) {
	int error_code = OK;
	externalNodePtr head = ext_head;
	externalNodePtr new_ext_node_ptr = (externalNodePtr)malloc(sizeof(externalNode)), current_ext_node_ptr;

	new_ext_node_ptr->symbol_name = (char *)malloc(strlen(symbol->name));
	
	if(new_ext_node_ptr && new_ext_node_ptr->symbol_name ){
		strcpy(new_ext_node_ptr->symbol_name, symbol->name);
		new_ext_node_ptr->address = address;
		new_ext_node_ptr->next = NULL;
		
		if(head == NULL) {
			ext_head = new_ext_node_ptr;
		}
		else {
			current_ext_node_ptr = ext_head;
			while(current_ext_node_ptr->next != NULL) {
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

/* ---------------------------------------- */

int add_symbol_to_entry_list(symbolPtr symbol) {
	int error_code = OK;
	entryNodePtr head = ent_head;
	entryNodePtr new_ent_node_ptr = (entryNodePtr)malloc(sizeof(entryNode)), current_ent_node_ptr;

	new_ent_node_ptr->symbol_name = (char *)malloc(strlen(symbol->name));

	if(new_ent_node_ptr && new_ent_node_ptr->symbol_name){
		strcpy(new_ent_node_ptr->symbol_name, symbol->name);
		new_ent_node_ptr->address = symbol->value;
		new_ent_node_ptr->next = NULL;
		
		if(head == NULL) {
			ent_head = new_ent_node_ptr;
		}
		else {
			current_ent_node_ptr = ent_head;
			while(current_ent_node_ptr->next != NULL) {
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
