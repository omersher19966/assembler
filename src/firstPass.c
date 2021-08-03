/* 
	The file is responsible to do all first pass tasks and contains all required functions to do it.
	The main function here is the assembler_first_pass() function which is responsible for the file scanning
	line by line and uses all other functions here to do it. The function also uses program general functions
	which are not defined here but necessary here.

*/


/* ------ Include Headers ------ */

#include "../include/pass.h"

/* ------------------------------------------
Function: assembler_first_pass()
	Parse a given file for the first time by reading each line individually until end of line is encountered.
	Store the parsed data in the the code + data images.
	In addition the functions checks several paramters and in case it detects a syntax error,
	a realted messgae is printed. 
Params:
	fp - a pointer points on the given file.
	file_name - the name of the file, used for printing error messages.
------------------------------------------- */

void assembler_first_pass(FILE *fp, char* file_name) {
	
	char *word = NULL, *label = NULL, *allocated_line = (char *)malloc(MAX_LINE * sizeof(char)), *line; /* need to handle malloc */
	bool is_label;
	int error_code = OK;
	
	if(allocated_line) { /* check memory allocation */
		/* allocated line address is assinged to the line variable to save 
		the real address in the allocated line for freeing it in the end of the function */
		for(line = allocated_line; fgets(line, MAX_LINE, fp) != NULL && global_memory_flag == false; line = allocated_line) { 
		/* check global memory flag for each line,
		in case it's up, the function stops to parse the fil */
			
			/* setup for checking new line */
			lc++; /* increase line counter for each line */
			is_label = false;
			word = NULL;
			label= NULL;
			error_code = OK;

			if((error_code = check_line(line)) != OK) {
				if(error_code != COMMENT_LINE && error_code != EMPTY_LINE) {
					if (error_code == ABOVE_MAX_LINE) {
						start_new_line(fp);
					}
					print_error(error_code, file_name, NULL);
				}
			}
			else {
				word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR); /* get first element in case line is valid */

				if((error_code = check_label(word, true, true)) != NOT_A_LABEL) { /* skips in case first word is not a label. */
					if(!is_error(error_code)) {
						is_label = true; /* the first element is a valid label */				
					}
					else {
						print_error(error_code, file_name, word);
					}
					label = word;
					word = get_next_element(&line, WHITE_SPACES_DELIMITERS_STR);
				} 
				
				error_code = OK;

				if(is_directive_command(word)) {
					if(is_label) { /* check if is label was raised meaining label should be added to the symbol table */
						error_code = add_symbol_to_table(label, false, true, false, false);
					}
					if(error_code == OK) {
						if(is_error(error_code = parse_directive_sentence(line, word))){
							print_error(error_code, file_name, NULL);
						}
					}
					else{
						print_error(error_code, file_name, NULL);
					}
				}
				else if(is_entry_command(word)){ 
					continue; /* entry command is parsed in the second pass */
				}
				else if(is_extern_command(word)){
					if (is_error(error_code = parse_extern_sentence(line))) {
						print_error(error_code, file_name, NULL);
					}
				}
				else if(is_command(word)) {
					if(is_label) { /* check if is label was raised meaining label should be added to the symbol table */
						error_code = add_symbol_to_table(label, true, false, false, false);
					}
					if(error_code == OK) {
						if (is_error(error_code = parse_command_sentence(line, word))) { 
							print_error(error_code, file_name, NULL);
						}
					}
					else {
						print_error(error_code, file_name, NULL);
					}			
				}
				else { /* meaning first word is not a valid keyword or command in the current line */
					print_error(INVALID_CMD,file_name, word);
				}
			}
		}
	}
	else {
		print_error(MEMORY_ALLOCATION_FAILED, file_name, NULL);
	}
	
	free(allocated_line);
	return;	
}

/* ------------------------------------------
Function: parse_extern_sentence()
 	Parse extern command by parsing the rest line of the extern command (given line) and then store the given label 
	in the symbol table with the correct attributes. 
	Check if the label is a valid label and that is not already in the symbol table
	before it adds it to the symbol table.
Params:
	line - the rest line of the extern command.
Return: 
	OK if the parsing of the rest of the line succeeded, a related error code in case error was detected.
------------------------------------------- */

int parse_extern_sentence(char *line) {
	int error_code = OK;
	char *element;
	reqOperands operands_num = ONE_OPERAND; /* the required operands number for extern command */
	symbolPtr symbol = NULL;

	if((error_code = check_operands_num(line, operands_num)) == OK) { /* check if there is only one operand as it suppose to be */
		element = get_next_element(&line, COMMA_DELIMETER_STR);
		if((error_code = check_label(element, false, false)) == OK) {
			if ((symbol = get_symbol_from_table(element)) == NULL) { /* check if symbol already exists in the symbol table */
				error_code = add_symbol_to_table(element, false, true, false, true);
			}
			else {
				if ((symbol->attributes.entry == true)) { /* check if the symbol is already an entry symbol because symbol canoot be both in the same time */
					error_code = SYMBOL_CANNOT_BE_DEFINED_AS_ENTRY_AND_EXTERNAL;
				}
			}
		}
	}
	return error_code;
}

/* ------------------------------------------
Function: parse_command_sentence()
 	Parse an insturcion command according to its type(I, R, J), build a binary machine instruction 
	according the instruction type format (done by other functions) and add the instruction to the program
	code image. Check for errors before adding the instruction to the code image.
Params:
	line - the rest line of the insturction command.
	cmd_name - the given command name.
Return: 
	OK if the command parsing succeeded, a related error code in case error was detected.
------------------------------------------- */

int parse_command_sentence(char *line, char *cmd_name) {
	
	command *command_ptr = get_command(cmd_name); /* gives command's information to select how to parse the current command */ 
	instruction *new_instruction = (instruction *) malloc(sizeof(instruction));
	reqOperands operands_number = command_ptr -> operands_num;
	int error_code = OK;

	if(code_image_index > MAX_CODE_SIZE) { /* check if the code image is full or not */
		error_code = CODE_IMAGE_IS_FULL;
	}
	else if (!(new_instruction)){
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	else if((error_code = check_operands_num(line ,operands_number)) == OK) {
		/* if number of operands is valid, select which parsing method to used based on the instruction type*/
		if(command_ptr->instruction_type == INSTRUCTION_R) {
			error_code = parse_r_instruction(new_instruction, command_ptr, line, operands_number); /* used to parse R instructions type and to set the binary machine instruction */
		}
		else if(command_ptr->instruction_type == INSTRUCTION_I) {
			error_code = parse_i_instruction(new_instruction, command_ptr, line, operands_number); /* used to parse I instructions type and to set the binary machine instruction */
		}
		else {
			error_code = parse_j_instruction(new_instruction, command_ptr, line, operands_number); /* used to parse J instructions type and to set the binary machine instruction */
		}
	}
	
	if(!is_error(error_code)) { /* if no error was detected, the instruction will be added to the code image */
		add_instruction_to_code_image(new_instruction); 
	}
	
	free(new_instruction);
	return error_code;
}

/* ------------------------------------------
Function: parse_directive_sentence()
 	Parse an directive command, store the parsed directive data in the program data image.
	The parsing is done by reading the the rest of directive command line and storing the valid operands in the data image.
	Each directive data possess additional data like size and address based on global variables and the given paramters.
	The function checks for errors before adding the data to the data image.
Params:
	line - the rest line of the directive command.
	word - the given directive command name. used to select how to parse the data.
Return: 
	OK if the directive command parsing succeeded, a related error code in case error was detected.
------------------------------------------- */

int parse_directive_sentence(char *line, char *word) {
	
	char operand[MAX_LINE], *element; /* operand - used to save one operand at a time, cant be bigger than the max line length */ 
	int error_code = OK, jmp;
	long num; /* long is used because its known its size at least 32 bits */

	if (is_empty_line(line)) { /* check line before parsing */
		error_code = NO_GIVEN_OPERANDS;
	}
	else if(!(are_strings_equal(word, ASCIZ_COMMAND))){ /* parse db,dh,dw directive commands. */
		/* the given command is used to determine the data size in the data image */
		if(are_strings_equal(word, DB_COMMAND)) { 
			jmp = DB_SIZE; 
		}
		else if(are_strings_equal(word, DH_COMMAND)) {
			jmp = DH_SIZE;
		}
		else {
			jmp = DW_SIZE;
		}
		/* because the operands number is not known the process continues until NULL element is encounterd */
		for(element = get_next_element(&line, COMMA_DELIMETER_STR); element != NULL && !is_error(error_code); element = get_next_element(&line, COMMA_DELIMETER_STR)) {
			/* operand setup */
			strcpy(operand, element);
			remove_trailing_spaces(operand);
			if(strlen(operand) != EMPTY) {
				if(is_valid_number(operand)) {
					num = atol(operand); /* atol instead of atoi because num is a long type */
					error_code = add_to_data_image(num,jmp);
				}
				else {
					error_code = INVALID_NUMBER;
				}
			}
			else {
				error_code = OPERAND_IS_EMPTY;
			}
		}
	}
	else { /* parse asciz directive command. */
		error_code = parse_asciz_command(line); /* a different parsing method is needed */
	}
	return error_code;
}


/* ------------------------------------------
Function: parse_r_instruction()
 	Parse R instructions type and set the given binary machine instruction according to the given parameters
	and the R instructions type format.
	The function checks command operands before and stores them in array 
	which is used after to set the binary machine instruction.
	The function checks for errors before seting the binary machine instruction.
Params:
	instruction_ptr - the given instruction to set pointed by a pointer.
	command_ptr - a command pointed by a pointer which is used to set binary machhine instruction fields.
	line - the rest of the command line, contains the command operands.
	operands_number - the required operands number to find in the given line.
Return: 
	OK if the the line parsing and the instrcution setting succeeded, a related error code in case error was detected.
------------------------------------------- */

int parse_r_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num) {
	char **operands_list = (char **)malloc(operands_num * sizeof(char *));
	int error_code = OK, i, *registers = (int *)malloc(operands_num * sizeof(int));

	if(operands_list != NULL && registers != NULL) { /* check memory allocation */
	
		error_code = set_operands_list(line, operands_list, operands_num); /* takes out the operands from the line */
		
		if(!is_error(error_code)) {
			for(i = 0; i < operands_num && !is_error(error_code); i++) {
				/* all operands in R instruction are registers, check if they are valid before converting them to numeric value */
				if (!is_error(error_code = check_register(operands_list[i]))) {
					registers[i] = convert_to_register(operands_list[i]);
				}	
			}
		}
		/* if no error was detected, set the binary machine instruction based on the instruction group type */
		if(!is_error(error_code)) {
			if (command_ptr -> instruction_group == ARITHMETIC_R_INSTRUCTIONS) {
				set_arithmetic_r_instruction(instruction_ptr, command_ptr, registers);
			}
			else{ 
				set_copy_r_instruction(instruction_ptr, command_ptr, registers);
			}
		}

		/* free memory allocation */
		free(operands_list);
		free(registers);
	}
	else {
		error_code = MEMORY_ALLOCATION_FAILED;
	}

	return error_code;
}

/* ------------------------------------------
Function: parse_i_instruction()
 	Parse I instructions type and set the given binary machine instruction according 
	to the given parameters and the I instructions type format.
	The function checks command operands before and stores them in array which is used after to set the binary machine instruction.
	The function checks for errors before seting the binary machine instruction.
Params:
	instruction_ptr - the given instruction to set pointed by a pointer.
	command_ptr - a command pointed by a pointer which is used to set binary machhine instruction fields.
	line - the rest of the command line, contains the command operands.
	operands_number - the required operands number to find in the given line.
Return: 
	OK if the line parsing and the instrcution setting succeeded, a related error code in case error was detected.
------------------------------------------- */
int parse_i_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num) {
	char **operands_list = (char **)malloc(operands_num * sizeof(char *)); /* used to store all operands */
	int *registers = (int *)malloc((operands_num-1) * (sizeof(int))),error_code = OK, immed = 0, i=0; 
	instructionGroupType instruction_group = command_ptr -> instruction_group;
	void (*func)(instruction *, command *, int *, int); /* used to assign and execute the correct set instruction function */

	if (registers != NULL && operands_list != NULL) { /* check memory allocation */
		if ((error_code = set_operands_list(line, operands_list, operands_num)) == OK) { /* takes the operands from the given line */
			/* both instruction groups use the same operands order and because of it can be handled in the same manner */
			if(instruction_group == ARITHMETIC_I_INSTRUCTIONS || instruction_group == LOADING_STORING_I_INSTRUCTIONS) {
				/* first and third operand should be registers in those instruction groups type */
				if(is_error(error_code = check_register(operands_list[FIRST_OPERAND])) || is_error(error_code = check_register(operands_list[THIRD_OPERAND]))) {
					return error_code;
				}
				registers[i++] = convert_to_register(operands_list[FIRST_OPERAND]); /* skipping the dolar sign by using ++ */
				registers[i++] = convert_to_register(operands_list[THIRD_OPERAND]); /* skipping the dolar sign by using ++ */
				
				if(is_valid_number(operands_list[SECOND_OPERAND])) { /* second operand should be a valid number */
					immed = atoi(operands_list[SECOND_OPERAND]);
				}
				else {
					error_code = INVALID_NUMBER;
					return error_code;
				}
				/* set the right function address in func, then it used to set binary machine instruction */
				func = (instruction_group == ARITHMETIC_I_INSTRUCTIONS) ? set_arithmetic_i_instruction : set_loading_storing_i_instruction;
				func(instruction_ptr, command_ptr, registers, immed);
			}
			else { /* handeles branching i instructions group */
				/* first and second operands should be valid registers */
				if(is_error(error_code = check_register(operands_list[FIRST_OPERAND])) || is_error(check_register(operands_list[SECOND_OPERAND]))) {
					return error_code;
				}
				
				registers[i++] = convert_to_register(operands_list[FIRST_OPERAND]);
				registers[i++] = convert_to_register(operands_list[SECOND_OPERAND]);
				/* check if the third operand is a valid label to used(*/
				if(is_error(error_code = check_label(operands_list[THIRD_OPERAND], false, false))) {
					return error_code;
				}
				/* set the binary machine instruction */
				set_branching_i_instruction(instruction_ptr ,command_ptr, registers);
				
			}
			/* free memory allocation */
			free(operands_list);
			free(registers);

		}
	}
	else {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	return error_code;

}

/* ------------------------------------------
Function: parse_j_instruction()
 	Parse J instructions type and set the given binary machine instruction according 
	to the given parameters and the J instructions type format.
	The function checks for errors before seting the binary machine instruction.
Params:
	instruction_ptr - the given instruction to set pointed by a pointer.
	command_ptr - a command pointed by a pointer which is used to set binary machhine instruction fields.
	line - the rest of the command line, contains the command operands.
	operands_number - the required operands number to find in the given line.
Return: 
	OK if the parsing&setting of the instruction succeeded, a related error code in case error was detected.
------------------------------------------- */

int parse_j_instruction(instruction *instruction_ptr,command *command_ptr, char *line, reqOperands operands_num)  {
	char *operand;
	int error_code = OK, reg = EMPTY; /* jmp can use a register so variable is needed to save register numeric value in case it is used */
	instructionGroupType current_instruction_group = command_ptr ->instruction_group;
	/* Stop command doesn't use any operands */
	if(current_instruction_group != STOP) {
		operand = get_next_element(&line, COMMA_DELIMETER_STR);
		if(current_instruction_group == JMP) { /*jmp command can use register or label therefore 2 checks are needed - label and register */
			if(!is_error(check_label(operand, false, false))) { /* check first if the given operand is a label */
				set_jmp_instruction(instruction_ptr, command_ptr, reg ,false); /* if so, reg flag is set to false - 0 */
			}
			else if(!is_error(check_register(operand))){ /* check if the given operand is a register */
				reg = convert_to_register(operand);
				set_jmp_instruction(instruction_ptr, command_ptr, reg ,true); /* reg flag is true and numeric value is sent */
			}
			else {
				error_code = INVALID_OPERAND;
			}	
		}
		else { /* other commands don't use register so only label check is needed */
			if(!is_error(error_code = check_label(operand, false, false))) {
				if(current_instruction_group == LA)
					set_la_instruction(instruction_ptr, command_ptr);
				else {
					set_call_instruction(instruction_ptr, command_ptr);
				}
			}
			else {
				error_code = INVALID_LABEL;
			}
		}
	}
	else {
		set_stop_instruction(instruction_ptr, command_ptr);
	}
	
	return error_code;
}


/* ------------------------------------------
Function: parse_asciz_command()
 	Parse asciz commands, store the operand string as characters in the data image 
	one character after another including EOS (End Of String) in the end of the sequence.
	The function checks for errors before adding characters as data to the data image.
Params:
	line - the rest line of the asciz command.
Return: 
	OK if the asciz command parsing succeeded, a related error code in case error was detected.
------------------------------------------- */


int parse_asciz_command(char *line) { 
 	
	const reqOperands asciz_operands_num = ONE_OPERAND; /* asciz command should have only one operand - a valid string */
	int error_code = OK, len, index;
	char *element, operand[MAX_LINE];

	if((error_code = check_operands_num(line, asciz_operands_num)) == OK) {
		/* operand setup for string validation */
		element = get_next_element(&line, COMMA_DELIMETER_STR);
		strcpy(operand, element);
		if(is_valid_string(operand)){
			len = strlen(operand);
			for(index = 0; (index < len) && (!is_error(error_code)); index++) { /* loop until string is end or error was detected */
				if(operand[index] != QUOTATION_MARK_CHAR) { /* add each character to data image excepts the first and the last qutation marks */
					error_code = add_to_data_image(operand[index], DB_SIZE);
				}
			}
			if(!is_error(error_code)) {
				error_code = add_to_data_image(EOS, DB_SIZE); /* add EOS to the end of the character sequence */
			}
		}
		else {
			error_code = INVALID_STRING;
		}
	}
	
	return error_code;
}

/* ------------------------------------------
Function: set_arithmetic_r_instruction()
	Set binary machine insturction for arithmetic R instructions group type based on several given paramters.
Params:
	instruction_ptr - instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting part of the binary machine instruction.
	current_register - a pointer to array of registers used to set the the binary machine instruction.
------------------------------------------- */

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

/* ------------------------------------------
Function: set_copy_r_instruction()
	Set binary machine insturction for copy R instructions group type based on several given paramters.
Params:
	instruction_ptr - instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting part of the binary machine instruction.
	current_register - a pointer to array of registers used to set the the binary machine instruction.
------------------------------------------- */

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

/* ------------------------------------------
Function: set_arithmetic_i_instruction()
	Set binary machine insturction for arithmetic I instructions group type based on several given paramters.
Params:
	instruction_ptr - instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting part of the binary machine instruction.
	current_register - a pointer to array of registers used to set the the binary machine instruction.
	immed - a given immed value to set the immed filed in the binary machine instruction.
------------------------------------------- */

void set_arithmetic_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed) {	
	instruction_ptr->type = INSTRUCTION_I;
	instruction_ptr->instruction_fmt.instruction_i.immed = immed;
	instruction_ptr->instruction_fmt.instruction_i.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_i.rt = *current_register;
	instruction_ptr->instruction_fmt.instruction_i.opcode = command_ptr->opcode;
	return;
}

/* ------------------------------------------
Function: set_loading_storing_i_instruction()
	Set binary machine insturction for loading & storing I instructions group type based on several given paramters.
Params:
	instruction_ptr - instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting part of the binary machine instruction.
	current_register - a pointer to array of registers used to set the the binary machine instruction.
	immed - a given immed value to set the immed filed in the binary machine instruction.
------------------------------------------- */

void set_loading_storing_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register, int immed) {    
    instruction_ptr->type = INSTRUCTION_I;
	instruction_ptr->instruction_fmt.instruction_i.immed = immed;
	instruction_ptr->instruction_fmt.instruction_i.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_i.rt = *current_register;
	instruction_ptr->instruction_fmt.instruction_i.opcode = command_ptr->opcode;
	return;
}

/* ------------------------------------------
Function: set_branching_i_instruction()
	Set a binary machine insturction for barnching I instructions group type based on several given paramters.
Params:
	instruction_ptr - instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting part of the binary machine instruction.
	current_register - a pointer to array of registers used to set the the binary machine instruction.
------------------------------------------- */

void set_branching_i_instruction(instruction *instruction_ptr, command *command_ptr, int *current_register) {	
    instruction_ptr->type = INSTRUCTION_I;
	instruction_ptr->instruction_fmt.instruction_i.immed = ic; /* subtruction of the IC value from the label value will bein the second pass. */
	instruction_ptr->instruction_fmt.instruction_i.rs = *current_register++;
	instruction_ptr->instruction_fmt.instruction_i.rt = *current_register;
	instruction_ptr->instruction_fmt.instruction_i.opcode = command_ptr->opcode;
	return;
}

/* ------------------------------------------
Function: set_jmp_instruction()
	Set a binary machine insturction for jmp instruction based on several given paramters.
Params:
	instruction_ptr - an instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting part of the binary machine instruction.
	reg - a register number in case the jmp command uses a register.
	reg_flag - a flag used to determine if the address file should filled with a register number or not.
------------------------------------------- */

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

/* ------------------------------------------
Function: set_la_instruction()
	Set a binary machine insturction for la instruction based on a commnad pointed by a given pointer.
Params:
	instruction_ptr - an instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting the binary machine instruction.
------------------------------------------- */

void set_la_instruction(instruction *instruction_ptr, command *command_ptr) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	instruction_ptr->instruction_fmt.instruction_j.reg = EMPTY;
	instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	return;
}

/* ------------------------------------------
Function: set_call_instruction()
	Set a binary machine insturction for call instruction based on a commnad pointed by a given pointer.
Params:
	instruction_ptr - an instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting the binary machine instruction.
------------------------------------------- */

void set_call_instruction(instruction *instruction_ptr, command *command_ptr) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	instruction_ptr->instruction_fmt.instruction_j.reg = EMPTY;
	instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	return;
}

/* ------------------------------------------
Function: set_stop_instruction()
	Set a binary machine insturction for stop instruction based on a commnad pointed by a given pointer.
Params:
	instruction_ptr - an instruction to set pointed by a pointer.
	command_ptr - a given command to based on for setting the binary machine instruction.
------------------------------------------- */

void set_stop_instruction(instruction *instruction_ptr, command *command_ptr) {
	instruction_ptr->type = INSTRUCTION_J;	
	instruction_ptr->instruction_fmt.instruction_j.opcode = command_ptr->opcode;
	instruction_ptr->instruction_fmt.instruction_j.reg = EMPTY;
	instruction_ptr->instruction_fmt.instruction_j.address = EMPTY;
	return;
}

/* ---------------------------------------- */
