#include "pass.h"

/* ---------------------------------------- */

void assembler_first_pass(FILE *fp) {
	
	char *word = NULL, *label = NULL, *line = (char *) malloc(MAX_LINE); /* need to handle malloc */
	bool is_label;
	int error_code = OK;
	
	if(line) {

		while(fgets(line, MAX_LINE, fp) != NULL && global_memory_flag == false) { /* global_error_flag == false */
			
			lc++;
			is_label = false;
			word = NULL;
			error_code = OK;

			if((error_code = check_line(line)) != OK) {
				if(error_code != COMMENT_LINE && error_code != EMPTY_LINE) {
					if (error_code == ABOVE_MAX_LINE) {
						start_new_line(fp);
					}
					print_error(error_code);
				}
			}
			else {
				
				word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);

				if((error_code = check_label(word, true, true)) != NOT_A_LABEL) { /* skips in case first word is a label. */
					if(!is_error(error_code)) {
						is_label = true;					}
					else {
						print_error(error_code);
					}
					label = word;
					word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
				} 
				
				error_code = OK;

				if(is_guidance_word(word)) {
					if(is_label) {
						error_code = (!(is_symbol_exist(label))) ? OK : SYMBOL_EXISTS_IN_TABLE;
						if(!is_error(error_code)) {
							add_symbol_to_table(label, false, true, false, false);
						}
						else {
							print_error(error_code);
						}
					}
					error_code = parse_guidance_sentence(line, word);
					if(is_error(error_code)){
						print_error(error_code);
					}
				}
				else if(is_entry_word(word)){
					continue;
				}
				else if(is_extern_word(word)){
					error_code = parse_extern_sentence(line);
					if (is_error(error_code)) {
						print_error(error_code);
					}
				}
				else if(is_command_word(word)) {
					if(is_label) {
						error_code = (!(is_symbol_exist(label))) ? OK : SYMBOL_EXISTS_IN_TABLE;
						if(!is_error(error_code)) {
							add_symbol_to_table(label, true, false, false, false);
						}
						else {
							print_error(error_code);
						}
					}
					error_code = parse_command_sentence(line, word);
					if (is_error(error_code)) {
						print_error(error_code);
					}
					
				}
				else {
					print_error(error_code = INVALID_CMD);
				}
			}
		}
	}
	else {
		print_error(MEMORY_ALLOCATION_FAILED);
	}

	return;	
}

/* ---------------------------------------- */

int parse_extern_sentence(char *line) {
	int error_code = OK;
	char *operand, *element;
	reqOperands operands_num = ONE_OPERAND;
	symbolPtr symbol = NULL;

	if((error_code = check_operands_num(line, operands_num)) == OK) {
		element = get_next_element(&line, COMMA_DELIMETER_STR);
		if((error_code = check_label(element, false, false)) == OK) {
			if ((symbol = get_symbol_from_table(element)) == NULL) {
				add_symbol_to_table(element, false, true, false, true);
			}
			else {
				if (symbol->attributes.entry = true) {
					error_code = SYMBOL_HAS_ALREADY_BEEN_DEFINED_AS_ENTRY;
				}
			}
		}
	}
	return error_code;
}

/* ---------------------------------------- */

int parse_command_sentence(char *line, char *word) {
	
	command *command_ptr = get_command(word);
	instruction *new_instruction = (instruction *) malloc(sizeof(instruction));
	reqOperands operands_number = command_ptr -> operands_num;
	int error_code = OK;

	if(code_image_index > MAX_CODE_SIZE) {
		error_code = CODE_IMAGE_IS_FULL;
	}
	else if (!(new_instruction)){
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	else if((error_code = check_operands_num(line ,operands_number)) == OK) {
		if(command_ptr->instruction_type == INSTRUCTION_R) {
			error_code = parse_r_instruction(new_instruction, command_ptr, line, operands_number);
		}
		else if(command_ptr->instruction_type == INSTRUCTION_I) {
			error_code = parse_i_instruction(new_instruction, command_ptr, line, operands_number);
		}
		else {
			error_code = parse_j_instruction(new_instruction, command_ptr, line, operands_number);
		}
	}
	
	if(!is_error(error_code)) {
		add_instruction_to_code_image(new_instruction);
	}
	
	free(new_instruction);
	return error_code;
}

/* ---------------------------------------- */

int parse_guidance_sentence(char *line, char *word) {
	
	char operand[MAX_LINE], *element;
	int error_code = OK, jmp;
	long num;

	if (is_empty_line(line)) {
		error_code = INVALID_OPERANDS_LINE;
	}
	else if(!(are_strings_equal(word, ASCIZ))){ /* parse db,dh,dw guidance commands. */
		if(are_strings_equal(word, DB)) { 
			jmp = DB_JMP; 
		}
		else if(are_strings_equal(word, DH)) {
			jmp = DH_JMP;
		}
		else {
			jmp = DW_JMP;
		}
		for(element = get_next_element(&line, COMMA_DELIMETER_STR); element != NULL && !is_error(error_code); element = get_next_element(&line, COMMA_DELIMETER_STR)) {
			strcpy(operand, element);
			remove_trailing_spaces(operand);
			if(is_valid_number(operand)) {
				num = atol(operand);
				error_code = add_to_data_image(num,jmp);
			}
			else {
				error_code = INVALID_NUMBER;
			}
		}
	}
	else { /* parse asciz guidance command. */
			int len, index;
			if(is_valid_asciz_line(line)){
				element = get_next_element(&line, COMMA_DELIMETER_STR);
				strcpy(operand, element);
				remove_trailing_spaces(operand);
				if(is_valid_string(operand)) {
					len = strlen(operand) - 1;
					for(index = 1; index < len; index++) {
						add_to_data_image(element[index], CHAR_JMP);
					}
					add_to_data_image(EOS, CHAR_JMP);
				}
				else {
					error_code = INVALID_STRING;
				}
			}
	}
	return error_code;
}


/* ---------------------------------------- */

int parse_r_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num) {
	char *operands_list[operands_num];
	int error_code = OK, i, registers[operands_num];

	error_code = set_operands_list(line, operands_list, operands_num);
	
	if(!is_error(error_code)) {
		for(i = 0; i < operands_num && !is_error(error_code); i++) {
			error_code = check_register(operands_list[i]);
			if (!is_error(error_code)) {
				operands_list[i]++;
				registers[i] = convert_to_register(operands_list[i]);
			}	
		}
	}

	if(!is_error(error_code)) {
		switch(command_ptr -> instruction_group) {
			case ARITHMETIC_R_INSTRUCTIONS:
				set_arithmetic_r_instruction(instruction_ptr, command_ptr, registers);
				break;
			case COPY_R_INSTRUCTIONS:
				set_copy_r_instruction(instruction_ptr, command_ptr, registers);
				break;
		}
	}

	return error_code;
}

/* ---------------------------------------- */
int parse_i_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num) {
	char *operands_list[operands_num];
	int registers[operands_num-1],error_code = OK, immed = 0, i=0;  
	instructionGroupType instruction_group = command_ptr -> instruction_group;
	void (*func)(instruction *, command *, int *, int);

	if ((error_code = set_operands_list(line, operands_list, operands_num)) == OK) {
		if(instruction_group == ARITHMETIC_I_INSTRUCTIONS || instruction_group == LOADING_STORING_I_INSTRUCTIONS) {
			if(is_error(error_code = check_register(operands_list[FIRST_OPERAND])) || is_error(error_code = check_register(operands_list[THIRD_OPERAND]))) {
				return error_code;
			}
			registers[i++] = convert_to_register(++operands_list[FIRST_OPERAND]); /* skipping the dolar sign by using ++ */
			registers[i++] = convert_to_register(++operands_list[THIRD_OPERAND]); /* skipping the dolar sign by using ++ */
			
			if(is_valid_number(operands_list[SECOND_OPERAND])) {
				immed = atoi(operands_list[SECOND_OPERAND]);
			}
			else {
				error_code = INVALID_NUMBER;
				return error_code;
			}

			func = (instruction_group == ARITHMETIC_I_INSTRUCTIONS) ? set_arithmetic_i_instruction : set_loading_storing_i_instruction;
			func(instruction_ptr, command_ptr, registers, immed);
		}
		else { /* handeles branching i instructions group */
			if(is_error(error_code = check_register(operands_list[FIRST_OPERAND])) || is_error(check_register(operands_list[SECOND_OPERAND]))) {
				return error_code;
			}
			
			registers[i++] = convert_to_register(operands_list[FIRST_OPERAND]);
			registers[i++] = convert_to_register(operands_list[SECOND_OPERAND]);
		
			if(is_error(error_code = check_label(operands_list[THIRD_OPERAND], false, false))) {
				return error_code;
			}

			set_branching_i_instruction(instruction_ptr ,command_ptr, registers);
			
		}

	}

	return error_code;

}

/* ---------------------------------------- */

int parse_j_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num)  {
	char *operand;
	int error_code = OK, reg = 0;
	instructionGroupType current_instruction_group = command_ptr ->instruction_group;

	if(current_instruction_group != STOP) {
		operand = get_next_element(&line, COMMA_DELIMETER_STR);
		if(current_instruction_group == JMP) {
			if(!is_error(check_label(operand, false, false))) {
				set_jmp_instruction(instruction_ptr, command_ptr, reg ,false);
			}
			else if(!is_error(check_register(operand))){
				reg = convert_to_register(operand);
				set_jmp_instruction(instruction_ptr, command_ptr, reg ,true);
			}
			else {
				error_code = INVALID_OPERAND;
			}	
		}
		else {
			if(!is_error(error_code = check_label(operand, false, false))) {
				if(current_instruction_group == LA)
					set_la_instruction(instruction_ptr, command_ptr);
				else {
					set_call_instruction(instruction_ptr, command_ptr);
				}
			}
		}
	}
	else {
		set_stop_instruction(instruction_ptr, command_ptr);
	}
	
	return error_code;
}


/* ---------------------------------------- */

void set_arithmetic_r_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register) {
	instruction_ptr->type = INSTRUCTION_R;
	instruction_ptr->instruction_fmt.instruction_r.unused = EMPTY;
	instruction_ptr->instruction_fmt.instruction_r.funct = command_ptr->funct;
	instruction_ptr->instruction_fmt.instruction_r.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_r.rt = *current_register++;
	instruction_ptr->instruction_fmt.instruction_r.rd = *current_register;
	instruction_ptr->instruction_fmt.instruction_r.opcode = command_ptr->opcode;
	return;
}

/* ---------------------------------------- */

void set_copy_r_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register) {	
	instruction_ptr->type = INSTRUCTION_R;
	instruction_ptr->instruction_fmt.instruction_r.unused = EMPTY;
	instruction_ptr->instruction_fmt.instruction_r.funct = command_ptr->funct;
	instruction_ptr->instruction_fmt.instruction_r.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_r.rd = *current_register;
	instruction_ptr->instruction_fmt.instruction_r.rt = EMPTY;
	instruction_ptr->instruction_fmt.instruction_r.opcode = command_ptr->opcode;
	return;
}

/* ---------------------------------------- */

void set_arithmetic_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed) {	
	instruction_ptr->type = INSTRUCTION_I;
	instruction_ptr->instruction_fmt.instruction_i.immed = immed;
	instruction_ptr->instruction_fmt.instruction_i.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_i.rt = *current_register;
	instruction_ptr->instruction_fmt.instruction_i.opcode = command_ptr->opcode;
	return;
}

/* ---------------------------------------- */

void set_loading_storing_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed) {    
    instruction_ptr->type = INSTRUCTION_I;
	instruction_ptr->instruction_fmt.instruction_i.immed = immed;
	instruction_ptr->instruction_fmt.instruction_i.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_i.rt = *current_register;
	instruction_ptr->instruction_fmt.instruction_i.opcode = command_ptr->opcode;
	return;
}

/* ---------------------------------------- */
/* not finished. */
void set_branching_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register) {	
    instruction_ptr->type = INSTRUCTION_I;
	instruction_ptr->instruction_fmt.instruction_i.immed = ic; /* subtruction of the IC value from the label value will bein the second pass. */
	instruction_ptr->instruction_fmt.instruction_i.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_i.rt = *current_register;
	instruction_ptr->instruction_fmt.instruction_i.opcode = command_ptr->opcode;
	return;
}

/* ---------------------------------------- */

void set_jmp_instruction(instruction *instruction_ptr, command *command_ptr, int reg ,bool reg_flag) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	if(reg_flag) {
		instruction_ptr->instruction_fmt.instruction_j.address = reg;
	} 
	else {
		instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	}
	instruction_ptr->instruction_fmt.instruction_j.reg = reg_flag;
	return;
}

/* ---------------------------------------- */

void set_la_instruction(instruction *instruction_ptr, command *command_ptr) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	instruction_ptr->instruction_fmt.instruction_j.reg = EMPTY;
	instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	return;
}

/* ---------------------------------------- */

void set_call_instruction(instruction *instruction_ptr, command *command_ptr) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	instruction_ptr->instruction_fmt.instruction_j.reg = EMPTY;
	instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	return;
}

/* ---------------------------------------- */

void set_stop_instruction(instruction *instruction_ptr, command *command_ptr) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	instruction_ptr->instruction_fmt.instruction_j.reg = EMPTY;
	instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	return;
}

/* ---------------------------------------- */
