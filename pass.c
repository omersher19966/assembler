#include "assemblerData.h"
#include "pass.h"

/* Code + Data Image Setup */
instruction code_image[MAX_CODE_SIZE];
directiveData data_image[MAX_DATA_SIZE];

int code_image_index = 0 , data_image_index = 0;

/* External list Ptr */ 
externalNodePtr ext_head = NULL;
entryNodePtr ent_head = NULL;

/* Assembler Counters:
	instruction counter, data counter, line counter */
int ic =  MEMORY_STARTING_LOCATION, dc = 0, lc = 0;


const command cmd[] = { 
	{"add", 0, 1, THREE_OPERANDS,INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
	{"sun", 0, 2, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
	{"and", 0, 3, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
    {"or", 0, 4, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
    {"nor", 0, 5, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
    {"move", 1, 1, TWO_OPERANDS,INSTRUCTION_R, COPY_R_INSTRUCTIONS},
    {"mvhi", 1, TWO_OPERANDS, INSTRUCTION_R, COPY_R_INSTRUCTIONS},
    {"mvlo", 1, 3, TWO_OPERANDS, INSTRUCTION_R, COPY_R_INSTRUCTIONS},
    {"addi", 10, 0, THREE_OPERANDS, INSTRUCTION_I, ARITHMETIC_I_INSTRUCTIONS},
    {"subi", 11, 0, THREE_OPERANDS, INSTRUCTION_I, ARITHMETIC_I_INSTRUCTIONS},
    {"andi", 12, 0, THREE_OPERANDS, INSTRUCTION_I, ARITHMETIC_I_INSTRUCTIONS},
    {"ori", 13, 0, THREE_OPERANDS, INSTRUCTION_I, ARITHMETIC_I_INSTRUCTIONS},
    {"nori", 14, 0, THREE_OPERANDS, INSTRUCTION_I, ARITHMETIC_I_INSTRUCTIONS},
    {"bne", 15, 0, THREE_OPERANDS, INSTRUCTION_I, BRANCHING_I_INSTRUCTIONS},
    {"beq", 16, 0, THREE_OPERANDS, INSTRUCTION_I, BRANCHING_I_INSTRUCTIONS},
    {"blt", 17, 0, THREE_OPERANDS, INSTRUCTION_I, BRANCHING_I_INSTRUCTIONS},
	{"bgt", 18, 0, THREE_OPERANDS, INSTRUCTION_I, BRANCHING_I_INSTRUCTIONS},
	{"lb", 19, 0, THREE_OPERANDS, INSTRUCTION_I, LOADING_STORING_I_INSTRUCTIONS},
	{"sb", 20, 0, THREE_OPERANDS, INSTRUCTION_I, LOADING_STORING_I_INSTRUCTIONS},
	{"lw", 21, 0, THREE_OPERANDS, INSTRUCTION_I, LOADING_STORING_I_INSTRUCTIONS},
	{"sw", 22, 0, THREE_OPERANDS, INSTRUCTION_I, LOADING_STORING_I_INSTRUCTIONS},
    {"lh", 23, 0, THREE_OPERANDS, INSTRUCTION_I, LOADING_STORING_I_INSTRUCTIONS},
    {"sh", 24, 0, THREE_OPERANDS, INSTRUCTION_I, LOADING_STORING_I_INSTRUCTIONS},
    {"jmp", 30, 0, ONE_OPERAND, INSTRUCTION_J, JMP},
    {"la", 31, 0, ONE_OPERAND, INSTRUCTION_J, LA},
    {"call", 32, 0, ONE_OPERAND, INSTRUCTION_J, CALL},
    {"stop", 63, 0, NO_OPERANDS, INSTRUCTION_J, STOP},
    {NULL}
};


const char * directive_cmd[] = {
	".db",
	".dh",
	".dw",
	".asciz",
	NULL
};

/*Declarations of static functions */

static unsigned long get_required_bits(int bytes);
static void print_the_rest(FILE *object_file, int available_bytes, int address, unsigned long temp);


/* ---------------------------------------- */

void pass(FILE *fp, char *file_name, int file_num) {
	
	int error_code = OK, icf, dcf;
	
	assembler_first_pass(fp, file_num);
				
	if(global_error_flag) {
		printf("Assembler: files have not been created because of error(s).\n");
		return;
	}

	icf = ic;
	dcf = dc;
				
	/* setup for second pass */
	rewind(fp);
	update_symbol_table(icf);
	update_data_image(icf);

	/* second pass */
	assembler_second_pass(fp, file_num);
				

	if(global_error_flag) {
		printf("Assembler: files have not been created because of error(s).\n");
		return;
	}

	remove_file_extension(file_name);
	if(is_error(error_code = create_output_files(file_name, icf, dcf))) {
		print_error(error_code, file_num, NULL);
		printf("Assembler: files have not been created because of error(s).\n");
	}

	return;

}

/* ---------------------------------------- */

void print_code_image_to_file(FILE *object_file ,int icf) {
	int index, i; 
	char *ptr;
	instructionType type;

	for(index = 0; index < code_image_index; index++) {
		fprintf(object_file, "%04d", code_image[index].value);
		type = code_image[index].type;
		if(type == INSTRUCTION_R) {
			ptr = (char *) &code_image[index].instruction_fmt.instruction_r;
		}
		else if(type == INSTRUCTION_I) {
			ptr = (char *) &code_image[index].instruction_fmt.instruction_i;
		}
		else {
			ptr = (char *) &code_image[index].instruction_fmt.instruction_j;
		}

		for(i = 0; i < MACHINE_INSTRUCTIONS_SIZE; i++) {
			fprintf(object_file, " %02X", ptr[i]);
		}
		
		fprintf(object_file, "\n"); /* new line after printing command. */
	}
}

/* ---------------------------------------- */

void print_data_image_to_file(FILE *object_file ,int icf, int dcf) {

	int index, address, address_jmp = MACHINE_INSTRUCTIONS_SIZE, bytes_to_print, available_bytes;
	unsigned long num, temp, temp2, required_bits;
	char *ptr;

	available_bytes = EMPTY;

	for(address = icf, index = 0; index < data_image_index ; address += address_jmp) {
		
		fprintf(object_file, "%04d", address);
		bytes_to_print = MACHINE_INSTRUCTIONS_SIZE;
		num = EMPTY;

		while(bytes_to_print != EMPTY && index < data_image_index) {
			
			if(available_bytes == EMPTY) {
				if(data_image[index].type == BYTE) {
					temp = data_image[index].data_fmt.byte.data;
					available_bytes = BYTE;
				}
				else if(data_image[index].type == HALF_WORD) {
					temp = data_image[index].data_fmt.half_word.data;
					available_bytes = HALF_WORD;
				}
				else {
					temp = data_image[index].data_fmt.word.data;
					available_bytes = WORD;
				}
				index++;
			}

			if (bytes_to_print < available_bytes) {
				required_bits = get_required_bits(bytes_to_print);
				temp2 = temp & required_bits;
				temp >>= (bytes_to_print * BYTE_SIZE);
				num |= (temp2 << ((MACHINE_INSTRUCTIONS_SIZE - bytes_to_print) * BYTE_SIZE));
				available_bytes -= bytes_to_print;
				bytes_to_print = EMPTY;
			}
			else {
				num |= (temp << ((MACHINE_INSTRUCTIONS_SIZE - bytes_to_print) * BYTE_SIZE));
				bytes_to_print -= available_bytes;
				available_bytes = EMPTY;	
			}

		}

		for(ptr = (char *)&num; bytes_to_print < MACHINE_INSTRUCTIONS_SIZE; bytes_to_print++) {
			fprintf(object_file, " %02X", *ptr++);
		}

		fprintf(object_file, "\n");	
	}
	/* in case temp still contains data - print the rest */
	if(available_bytes != EMPTY){
		print_the_rest(object_file, available_bytes, address, temp);
	}
	

}

/* used by print_to_data_image only */
static unsigned long get_required_bits(int bytes) {
	unsigned long required_bits;
	if (bytes == 1) {
		required_bits = 0xff;
	}
	else if(bytes == 2) {
		required_bits = 0xffff;
	}
	else if (bytes == 3) {
		required_bits = 0xffffff;
	}

	return required_bits;
}

/* used by print_to_data_image only */
static void print_the_rest(FILE *object_file, int available_bytes, int address, unsigned long temp) {
	char *ptr;
	fprintf(object_file, "%04d", address);
	for(ptr = (char *) &temp; available_bytes != EMPTY; available_bytes--) {
		fprintf(object_file, " %02X", *ptr++);
	}
}
/* ---------------------------------------- */

void free_entry_list() {
	
	entryNodePtr head = ent_head, temp = NULL;

    while (head) {
        temp = head;
        head = temp -> next;
        free(temp);
    }

	ent_head = NULL;
}

/* ---------------------------------------- */

void free_external_list() {

    externalNodePtr head = ext_head, temp = NULL;

    while (head) {
        temp = head;
        head = temp -> next;
        free(temp);
    }

	ext_head = NULL;
}

/* ---------------------------------------- */

void free_code_image() {
	int i;
	instruction empty_instruction = {0}; /* used for reseting all structures in code image.. */
	for(i=0; i < MAX_CODE_SIZE; i++) {
		code_image[i] = empty_instruction;
	}
}

/* ---------------------------------------- */

void free_data_image() {
	int i;
	directiveData empty_data = {0}; /* used for reseting all structures in data image. */
	for(i=0; i < MAX_DATA_SIZE; i++) {
		data_image[i] = empty_data;
	}
}

/* ---------------------------------------- */

void reset_counters_indexes(){
	code_image_index = 0;
	data_image_index = 0;
	lc = 0;
	ic = MEMORY_STARTING_LOCATION;
	dc = 0;
}

/* ---------------------------------------- */

void update_data_image(int icf) {
	int index;
	for(index = 0; index < data_image_index; index++) {
		data_image[index].value += icf;
	}
	return;
}


/* ---------------------------------------- */

bool is_command_word(char *word){
	command *command_ptr;
	for(command_ptr = (command *)cmd; command_ptr->commmad_name != NULL; command_ptr++) {
		if(are_strings_equal(command_ptr->commmad_name, word)) {
			return true;
		}
	}
	return false;
}

/* ---------------------------------------- */

bool is_directive_word(char *word) {
	char **ptr;
	for(ptr = (char **)directive_cmd; *ptr != NULL; ptr++) {
		if(are_strings_equal(*ptr, word)) {
			return true;
		}
	}
	return false;
}

/* ---------------------------------------- */

bool is_entry_word(char *word) {
	return are_strings_equal(word, ENTRY_DECLARATION);
}

/* ---------------------------------------- */

bool is_extern_word(char *word) {
	return are_strings_equal(word, EXTERN_DECLARATION);
}

/* ---------------------------------------- */

bool is_entry_list() {
	return ent_head != NULL ? true : false;
}

/* ---------------------------------------- */

bool is_external_list() {
	return ext_head != NULL ? true : false;
}

/* ---------------------------------------- */

bool is_valid_operands_line(char *line) {
	int last_char_index;
	remove_trailing_spaces(line);
	last_char_index = strlen(line) - 1;
	return (line[last_char_index] != COMMA_DELIMETER_CHAR) ? true : false; 
}

/* ---------------------------------------- */

int convert_to_register(char *operand) {
	return atoi(++operand);/*increasing operand for skipping the reg sing - $ */
}

/* ---------------------------------------- */

int add_instruction_to_code_image (instruction *instruction_ptr) {
	int error_code = OK;
	if (code_image_index > MAX_CODE_SIZE) {
		error_code = CODE_IMAGE_IS_FULL;
	}
	else {
		instruction_ptr->value = ic;
		ic += CODE_IMAGE_JMP;
		code_image[code_image_index++] = *(instruction_ptr);
	}
	return error_code;
}

/* ---------------------------------------- */

int add_to_data_image(long num, int jmp) {
	int error_code = OK;
	if(data_image_index > MAX_DATA_SIZE) {
		error_code = DATA_IMAGE_IS_FULL;
	}
	else {

		if(jmp == CHAR_JMP || jmp == DB_JMP) {
			data_image[data_image_index].type = BYTE;
			data_image[data_image_index].data_fmt.byte.data = num; 
		}
		else if(jmp == DH_JMP) {
			data_image[data_image_index].type = HALF_WORD;
			data_image[data_image_index].data_fmt.half_word.data = num; 
		}
		else {
			data_image[data_image_index].type = WORD;
			data_image[data_image_index].data_fmt.word.data = num; 
		}

		data_image[data_image_index].value = dc;

		data_image_index++;
		dc += jmp;
	}
	return error_code;
}

/* ---------------------------------------- */

int set_operands_list(char *line, char **operands_ptr, reqOperands operands_num) {
	
	int i, error_code = OK;
	
	for(i = 0; i < operands_num && !is_error(error_code); i++) {
        operands_ptr[i] = get_next_element(&line, COMMA_DELIMETER_STR);
        if(strlen(operands_ptr[i]) == EMPTY) {
            error_code = OPERAND_IS_EMPTY;
        }
	}

	return error_code;
	
}

/* ---------------------------------------- */

int check_line(char *line) {

	int error_code = OK, last_char_index = strlen(line) - 1;
	char *copied_line = (char *)malloc(MAX_LINE), *line_ptr, *element;
	
	if (!copied_line) {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	else {
		strcpy(copied_line, line);
		line_ptr = copied_line;
		element = get_next_element(&line_ptr, COMMA_DELIMETER_STR);
		
		if(is_empty_line(line)) {
			error_code = EMPTY_LINE;
		}
		else if (line[last_char_index] != EOL) {
			error_code = ABOVE_MAX_LINE;
		}
		else if (!is_valid_operands_line(line)) {
			error_code=  INVALID_OPERANDS_LINE;
		}
		else if(is_comment(element)) {
			error_code = COMMENT_LINE;
		}
	}
	free(copied_line);
	return error_code;
}

/* ---------------------------------------- */

int check_operands_num(char *line, reqOperands operands_num) {
	int error_code = OK, elements_amount;
	if (operands_num != NOT_CONSTANT) {
		elements_amount = get_num_of_elements(line,COMMA_DELIMETER_STR);
		if(elements_amount == operands_num) {
			error_code = OK;
		}
		else if(!elements_amount){
			error_code = NO_GIVEN_OPERANDS;
		}
		else if(elements_amount > operands_num) {
			error_code = GIVEN_OPERANDS_ARE_MORE_THAN_REQUIRED;
		}
		else {
			error_code = GIVEN_OPERANDS_ARE_LESS_THAN_REQUIRED;
		}
	}	
	return error_code;
}

/* ---------------------------------------- */

int create_output_files(char *file_name, int icf, int dcf) {
	int error_code = OK;
	FILE *object_file, *external_file, *entry_file;
	char *object_file_name = (char *)malloc(strlen(file_name) + strlen(OBJECT_FILE_EXT) + 1); /* need to see why more 1 */
	char *entry_file_name = (char *)malloc(strlen(file_name) + strlen(ENTRY_FILE_EXT));
	char *external_file_name = (char *)malloc(strlen(file_name) + strlen(EXTERN_FILE_EXT));
	bool is_entry_file_needed = is_entry_list(), is_external_file_needed = is_external_list();

	if(object_file_name == NULL || external_file_name == NULL || entry_file_name == NULL) {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	else{
			strcpy(object_file_name, file_name);
    		strcat(object_file_name,OBJECT_FILE_EXT);
			strcpy(entry_file_name, file_name);
    		strcat(entry_file_name,ENTRY_FILE_EXT);
			strcpy(external_file_name, file_name);
    		strcat(external_file_name,EXTERN_FILE_EXT);

 
			
			if((object_file = fopen(object_file_name, "w")) == NULL) {
					error_code = CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE;
			}
			else if((is_entry_file_needed) && ((entry_file = fopen(entry_file_name, "w")) == NULL)) {
					error_code = CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE;
			}
			else if((is_external_file_needed) && ((external_file = fopen(external_file_name, "w")) == NULL)) {
					error_code = CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE;
			}
			
			free(object_file_name);
			free(external_file_name);
			free(entry_file_name);

			if (error_code == OK) { 		
				
				print_object_file(object_file, icf, dcf);
				fclose(object_file);
				
				if(is_entry_file_needed) {
					print_entry_file(entry_file);
					fclose(entry_file);
				}

				if(is_external_file_needed) {
					print_external_file(external_file);
					fclose(external_file);
				}
			}
	}
	
	return error_code;
}

/* ---------------------------------------- */

void print_entry_file(FILE *entry_file) {

	entryNodePtr current_entry_ptr = NULL;

	for(current_entry_ptr = ent_head; current_entry_ptr != NULL; current_entry_ptr = current_entry_ptr -> next) {
		fprintf(entry_file,"%s %04d\n", current_entry_ptr->symbol_name, current_entry_ptr->address);
	}

}

/* ---------------------------------------- */

void print_external_file(FILE *external_file) {

	externalNodePtr current_external_ptr = NULL;
	
	for(current_external_ptr = ext_head; current_external_ptr != NULL; current_external_ptr = current_external_ptr -> next) {
		fprintf(external_file,"%s %04d\n", current_external_ptr->symbol_name, current_external_ptr->address);
	}
}


/* ---------------------------------------- */

void print_object_file(FILE *object_file, int icf, int dcf) {
	
	fprintf(object_file, "%d %d\n", icf-MEMORY_STARTING_LOCATION, dcf); /* object file headline. */
	print_code_image_to_file(object_file, icf);
	print_data_image_to_file(object_file, icf ,dcf);

}

/* ---------------------------------------- */

command *get_command(char *word) {
	command *command_ptr;
	for(command_ptr=(command *)cmd; command_ptr->commmad_name !=  NULL; command_ptr++) {
		if(!(strcmp(command_ptr->commmad_name, word))) {
			return command_ptr;
		}
	}
	return NULL;
}


/* ---------------------------------------- */
