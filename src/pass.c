/* 
	The file is responsible for the whole prasing proccess of a single file and create the output files.
	The file contains and maintains all data related to parsing process like code and data image, 
	entry + external list, counters, indexes and more.
	The file uses functions from the firstPass.c and secondPass.c to complete the parsing and in the end create the output files (.ob, .ext, .ent) of a single file.
*/


/* ------ Include Headers ------ */
#include "../include/pass.h"

/* ------ External Variables ------ */

/* Code + Data Image Setup */
instruction code_image[MAX_CODE_SIZE];
directiveData data_image[MAX_DATA_SIZE];

/* global indexes */
int code_image_index = 0 , data_image_index = 0;

/* External list Ptr */ 
externalNodePtr ext_head = NULL;
entryNodePtr ent_head = NULL;

/* Assembler Pass Counters:
	instruction counter, data counter, line counter */
int ic =  MEMORY_STARTING_LOCATION, dc = 0, lc = 0;

/* Assembly instruction commands table 
   Contains Each insturction command and its static information */
const command cmd[] = { 
	{"add", 0, 1, THREE_OPERANDS,INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
	{"sub", 0, 2, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
	{"and", 0, 3, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
    {"or", 0, 4, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
    {"nor", 0, 5, THREE_OPERANDS, INSTRUCTION_R, ARITHMETIC_R_INSTRUCTIONS},
    {"move", 1, 1, TWO_OPERANDS,INSTRUCTION_R, COPY_R_INSTRUCTIONS},
    {"mvhi", 1, 2, TWO_OPERANDS, INSTRUCTION_R, COPY_R_INSTRUCTIONS},
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

/* Assembly directive commands table */
const char * directive_cmd[] = {
	".db",
	".dh",
	".dw",
	".asciz",
	NULL
};

/* Declarations of Static Functions - 
   Used only in this file */

/* Get the a number which is used to reset all bits except of the 
required bits which is base on the given bytes number */
static unsigned long get_required_bits(int bytes);
/* Print the last line in the object file in case it's needed */
static void print_the_rest(FILE *object_file, int available_bytes, int address, unsigned long temp);
/* Print code image to the given file. */
static void    print_code_image_to_file(FILE *object_file);
/* Print data image to the given file. */
static void    print_data_image_to_file(FILE *object_file ,int icf, int dcf);


/* ------------------------------------------
Function: pass().
	The main parsing function in the assembler program used to parse the whole assembly file 
	by scanning the fille twice and create the output files. 
	Search for errors after each scan and stop in case errors are detected.
Params: 
	fp - a pointer to the assembly file.
	file_name - the assembly file name.
------------------------------------------- */

void pass(FILE *fp, char *file_name) {
	
	int error_code = OK, icf, dcf; /* icf - instruction counter final, dcf - data counter final */
		/* original file name is used to keep the file name before removing the extension 
	to print the original file name in the error messages in case error is deteced in the files creation stage */
	char *original_file_name = (char *)malloc(strlen(file_name) * sizeof(char)); 

	
	if(original_file_name != NULL) {
		
		assembler_first_pass(fp, file_name); /* First scan */

		/* Search for errors after the first scan*/			
		if(global_error_flag) {
			printf("Assembler: files have not been created because of error(s).\n");
			return;
		}

		/* upadting final value of instruction and data coutners */
		icf = ic;
		dcf = dc;
		
		/* check if code image or data image are empty */
		if (icf == MEMORY_STARTING_LOCATION && dcf == EMPTY) {
			print_error(FILE_IS_EMPTY, file_name, NULL);
			printf("Assembler: files have not been created because of error(s).\n");
			return;
		}

		/* setup for second pass */
		rewind(fp);
		update_symbol_table(icf);
		update_data_image(icf);

		/* second pass */
		assembler_second_pass(fp, file_name); /*Second scan */
					
		/* Search for errors after the second scan*/	
		if(global_error_flag) {
			printf("Assembler: files have not been created because of error(s).\n");
			return;
		}

		/* save the orignal file name before removing the extensionf from file_name */
		strcpy(original_file_name,file_name);
		remove_file_extension(file_name); 
		if(is_error(error_code = create_output_files(file_name, icf, dcf))) { /* create the output files */
			print_error(error_code, original_file_name, NULL); /* the original file name should be used here and not the file name without the extension */
			printf("Assembler: files have not been created because of error(s).\n");
		}
		free(original_file_name);
	}
	else {
		print_error(MEMORY_ALLOCATION_FAILED, file_name, NULL);
	}

	return;

}

/* ------------------------------------------
Function: print_code_image_to_file().
	Print all data in the code image to the output object file according to the object file format.
	First the instruction address and then the binary machine insturction printed in hex basis.
Params: 
	object - a pointer to the object file
------------------------------------------- */

static void print_code_image_to_file(FILE *object_file) {
	int index, i; 
	unsigned char *ptr; /* char can be treated as one byte size, used to go over the data byte after byte */
	instructionType type;

	for(index = 0; index < code_image_index; index++) {
		fprintf(object_file, "%04d", code_image[index].value); /* print the address in the beginning of the line */
		type = code_image[index].type;
		/* type is used to determine which instruction format is used in the instruction structure */
		if(type == INSTRUCTION_R) {
			ptr = (unsigned char *) &code_image[index].instruction_fmt.instruction_r;
		}
		else if(type == INSTRUCTION_I) {
			ptr = (unsigned char *) &code_image[index].instruction_fmt.instruction_i;
		}
		else {
			ptr = (unsigned char *) &code_image[index].instruction_fmt.instruction_j;
		}
		/* print the machine insturcion in hex basis according the object file format byte after
		byte increasing i each time (one byte jump)*/
		for(i = 0; i < MACHINE_INSTRUCTIONS_SIZE; i++) {
			fprintf(object_file, " %02X", ptr[i]);
		}
		
		fprintf(object_file, "\n"); /* new line after printing command. */
	}
}

/* ------------------------------------------
Function: print_data_image_to_file().
	Print all data in the data image to the output object file according to the object file format.
	In each line 4 bytes in the data image are printed and first the address of the first by is printed.
	All data is printed in hex basis except the address which is printed in decimal basis.
Params: 
	object - a pointer to the object file.
	icf - the instruction counter final value.
	dcf - the instruction data final value.
------------------------------------------- */

static void print_data_image_to_file(FILE *object_file ,int icf, int dcf) {

	int index, address, bytes_to_print, available_bytes;
	unsigned long num, temp, temp2, required_bits; /* use unsinged long because it is know the long is at least 32 bits */
	unsigned char *ptr; /* use s char pointer to go over the data byte after byte*/

	available_bytes = EMPTY;
	/* address is set to icf because it's the first address to print in the data image part */
	for(address = icf, index = 0; index < data_image_index ; address += MACHINE_INSTRUCTIONS_SIZE) { /* address is increased each time by 4 becuase 4 bytes are printed */
		
		fprintf(object_file, "%04d", address); /* print the start address of the next 4 bytes */ 
		
		/* setup for cuurent iteration */
		bytes_to_print = MACHINE_INSTRUCTIONS_SIZE; /* in each ling 4 bytes are supposed to be printed */
		num = EMPTY; 
		
		/* loop until all bytes to prints are set and that there is still data to set in the data image */
		while(bytes_to_print != EMPTY && index < data_image_index) {
			/* available_bytes is used to determine the size of the current used data */
			if(available_bytes == EMPTY) {
				/* type is used to decide which data format is used in the current data node */
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
			/* check if the size of the current data is bigger than the bytes num can get */
			if (bytes_to_print < available_bytes) { /* if yes, the current data should be divide for 2 parts */
				required_bits = get_required_bits(bytes_to_print); /* bits which is used to get the part from the current data that can be added to num */
				temp2 = temp & required_bits; /* part which can be added to num */
				temp >>= (bytes_to_print * BYTE_SIZE);  /* part which will be wait for the next line */
				num |= (temp2 << ((MACHINE_INSTRUCTIONS_SIZE - bytes_to_print) * BYTE_SIZE)); /* insert the part in the right plact at num */
				/* setup for the next line */
				available_bytes -= bytes_to_print;
				bytes_to_print = EMPTY;
			}
			else { /* if no, the whole current data can be added to num in the right place */
				num |= (temp << ((MACHINE_INSTRUCTIONS_SIZE - bytes_to_print) * BYTE_SIZE)); /* shift temp to add the data in unset bytes in num */
				bytes_to_print -= available_bytes; /* the size of the data is substructed from the number bytes to print */
				available_bytes = EMPTY;	
			}

		}
		/* print the 4 bytes in hex basis according to the object file format byte after byte
		by increasing ptr (char *) */
		for(ptr = (unsigned char *)&num; bytes_to_print < MACHINE_INSTRUCTIONS_SIZE; bytes_to_print++) { 
			fprintf(object_file, " %02X", *ptr++);
		}

		fprintf(object_file, "\n");	 /* print a new line in the end of each line */
	}
	/* in case temp still contains data - print the rest */
	if(available_bytes != EMPTY){
		print_the_rest(object_file, available_bytes, address, temp);
	}
	

}

/* used by print_to_data_image only */
/* return a number which is used to reset all bits in a data except the size given in the bytes parameter */
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
/* Print last line just in case last line is not full in data and the regular method can not handle it */  
static void print_the_rest(FILE *object_file, int available_bytes, int address, unsigned long temp) {
	unsigned char *ptr;
	fprintf(object_file, "%04d", address);
	for(ptr = (unsigned char *) &temp; available_bytes != EMPTY; available_bytes--) {
		fprintf(object_file, " %02X", *ptr++);
	}
}

/* ------------------------------------------
Function: free_pass_memory()
	Relase all allocated dynamic memory used in the current file parsing like: symbol table, entry and external list and etc.
	In addition reset all code + data images, counter and indexes for the next file parsing.
------------------------------------------- */

void free_pass_memory() {
	free_symbol_table();
	free_entry_list();
	free_external_list();
	/* because code image and data image are program arrays 
	and are not dynamic memmory allocted, only reset is required */
	reset_code_image(); 
	reset_data_image();
	reset_counters_indexes();
	return;
}

/* ------------------------------------------
Function: free_entry_list().
	Relase all the allocated memory in the entry list.
	In the end the entry head pointer is pointing on null.
------------------------------------------- */

void free_entry_list() {
	
	entryNodePtr head = ent_head, temp = NULL;

    while (head) {
        temp = head;/* saving the current pointer to relase it in the end */
        head = temp -> next;/* head should move to point on the next record in the external list */
        free(temp); /* just at this point we can free temp */
    }

	ent_head = NULL;
}

/* ------------------------------------------
Function: free_external_list().
	Relase all the allocated memory in the external list.
	In the end the external head pointer is pointing on null.
------------------------------------------- */

void free_external_list() {

    externalNodePtr head = ext_head, temp = NULL;

    while (head) {
        temp = head; /* saving the current pointer to relase it in the end */
        head = temp -> next; /* head should move to point on the next record in the external list */
        free(temp); /* just at this point we can free temp */
    }

	ext_head = NULL;
}

/* ------------------------------------------
Function: reset_code_image().
	Reset the whole code image by assignment of empty data structure in each cell of the code image array.
------------------------------------------- */

void reset_code_image() {
	int i;
	instruction empty_instruction = {0}; /* used for reseting all structures in code image.. */
	for(i=0; i < MAX_CODE_SIZE; i++) {
		code_image[i] = empty_instruction;
	}
}

/* ------------------------------------------
Function: reset_data_image().
	Reset the whole data image by assignment of empty data structure in each cell of the data image array.
------------------------------------------- */

void reset_data_image() {
	int i;
	directiveData empty_data = {0}; /* used for reseting all structures in data image. */
	for(i=0; i < MAX_DATA_SIZE; i++) {
		data_image[i] = empty_data;
	}
}

/* ------------------------------------------
Function: update_data_image().
	Reset all global counter and indexes for the next file scanning.
------------------------------------------- */

void reset_counters_indexes(){
	code_image_index = 0;
	data_image_index = 0;
	lc = 0;
	ic = MEMORY_STARTING_LOCATION;
	dc = 0;
}

/* ------------------------------------------
Function: update_data_image().
	Update the data image with the final icf value after the first pass for each data node in the data image.
Params:
	icf - the icf value.
------------------------------------------- */

void update_data_image(int icf) {
	int index;
	for(index = 0; index < data_image_index; index++) {
		data_image[index].address += icf;
	}
	return;
}


/* ------------------------------------------
Function: is_command().
	Check if if the given word is an instruction command.
Params:
	word - the given word.
Return:
	TRUE if the given word is an instruction command, FALSE otherwise.
------------------------------------------- */

bool is_command(char *word){
	command *command_ptr;
	/* going over all instruction commands names in the assembly command table - cmd  */
	for(command_ptr = (command *)cmd; command_ptr->commmad_name != NULL; command_ptr++) { /* cast is needed becuase cmd is type of (const command *) and not (command *) */
		if(are_strings_equal(command_ptr->commmad_name, word)) {
			return true;
		}
	}
	return false;
}

/* ------------------------------------------
Function: is_directive_command().
	Check if if the given word is a directive command.
Params:
	word - the given word.
Return:
	TRUE if the given word is a directive command, FALSE otherwise.
------------------------------------------- */

bool is_directive_command(char *word) {
	char **ptr;
	/* going over all directive commands except extern and entry commands */
	for(ptr = (char **)directive_cmd; *ptr != NULL; ptr++) { /* cast is needed becuase directive_cmd is type (const char **) and not (char **) */
		if(are_strings_equal(*ptr, word)) {
			return true;
		}
	}
	return false;
}

/* ------------------------------------------
Function: is_entry_command().
	Check if if the given word is an entry command.
Params:
	word - the given word.
Return:
	TRUE if the given word is an entry command, FALSE otherwise.
------------------------------------------- */

bool is_entry_command(char *word) {
	return are_strings_equal(word, ENTRY_COMMAND);
}

/* ------------------------------------------
Function: is_extern_command().
	Check if if the given word is an extern command.
Params:
	word - the given word.
Return:
	TRUE if the given word is an extern command, FALSE otherwise.
------------------------------------------- */

bool is_extern_command(char *word) {
	return are_strings_equal(word, EXTERN_COMMAND);
}

/* ------------------------------------------
Function: is_entry_list().
	Check if entry list is empty or not.
Return:
	TRUE if the entry list contains some records (at least one), FALSE otherwise.
------------------------------------------- */

bool is_entry_list() {
	return ent_head != NULL ? true : false;
}

/* ------------------------------------------
Function: is_external_list().
	Check if external list is empty or not.
Return:
	TRUE if the external list contains some records (at least one), FALSE otherwise.
------------------------------------------- */

bool is_external_list() {
	return ext_head != NULL ? true : false;
}

/* ------------------------------------------
Function: is_valid_operands_line().
	Check if a given line is a valie operands line by checking if the line is end up with comma character
	or not. for example : "MAIN: add $9, $3, $22," is invalid operands line becuase of the comma character.
	In addition the function removes the white spaces character from the end of the line using another function.
Params:
	line - a given line to check if it a valid one or not.
Return:
	TRUE if the given line is a valid operands line, FALSE otherwise.
------------------------------------------- */

bool is_valid_operands_line(char *line) {
	int last_char_index;
	remove_trailing_spaces(line);
	last_char_index = strlen(line) - 1; /*the assignment can be done just after remove the trailing spaces */
	return (line[last_char_index] != COMMA_DELIMETER_CHAR) ? true : false; /* Check if the last character in the line is a comma or not */
}

/* ------------------------------------------
Function: convert_to_register().
	Convert register name to numeric value.
Params:
	operand - the register name.
Return:
	The numeric value of the given register name.
------------------------------------------- */

int convert_to_register(char *operand) {
	return atoi(++operand); /* increasing operand by 1 for skipping the register sign - $ */
}

/* ------------------------------------------
Function: add_to_data_image().
	Add new instruction to the code image. in addition increase IC for the next instruction address.
Params:
	instruction ptr - a pointer to the new instruction to add/
------------------------------------------- */

void add_instruction_to_code_image (instruction *instruction_ptr) {

	instruction_ptr->value = ic;
	code_image[code_image_index++] = *(instruction_ptr);

	ic += MACHINE_INSTRUCTIONS_SIZE;
}

/* ------------------------------------------
Function: add_to_data_image().
	Add the numeric value of the data (incase numbers or characters) to the data image, 
	determine the data size based on the given jmp paramter.
	The data is inserted to the first available cell in the data image array.
	If data image is full a related error code will be return.
Params:
	num- the given numeric value to add to the data image.
	jmp - the given jmp address size which is used to determine the data node's address and size.
Return:
	OK if data image is not full and have enough space for the new dat, a related error code if data image is full.
------------------------------------------- */

int add_to_data_image(long num, int jmp) {
	int error_code = OK;
	/* check if the data image is fulle */
	if(data_image_index > MAX_DATA_SIZE) {
		error_code = DATA_IMAGE_IS_FULL;
	}
	else {
		/* the comparison is used to determine which data format to use (byte, half word, word) 
		for storing the data. */
		if(jmp == DB_SIZE) {
			data_image[data_image_index].type = BYTE;
			data_image[data_image_index].data_fmt.byte.data = num; 
			/* although the variable types are not in the same size, the assignment in byte.data
			will take exactly the 8 low bit from num */
		}
		else if(jmp == DH_SIZE) {
			data_image[data_image_index].type = HALF_WORD;
			data_image[data_image_index].data_fmt.half_word.data = num; 
			/* although the variable types are not in the same size, the assignment in byte.data
			will take exactly the 16 low bit from num */
		}
		else {
			data_image[data_image_index].type = WORD;
			data_image[data_image_index].data_fmt.word.data = num; 
		}

		data_image[data_image_index].address = dc;

		data_image_index++;
		dc += jmp; /* increase DC by the size of the current data to the next free one */
	}
	return error_code;
}

/* ------------------------------------------
Function: set_operands_list().
	Set the given operands list (array) with operands from the given line.
Params:
	line - the given line to take from the operands.
	operands_ptr - pointer to the start of the operands list array.
	operands_num - the required operands number for the operands list taken from the assembly command table.
Return:
	OK if the set of the operand list succeeded , a related error code if error was detected.
------------------------------------------- */

int set_operands_list(char *line, char **operands_ptr, reqOperands operands_num) {
	
	int i, error_code = OK;
	/* if error was detected, stops filling the array */
	for(i = 0; i < operands_num && !is_error(error_code); i++) {
        operands_ptr[i] = get_next_element(&line, COMMA_DELIMETER_STR);
        if(strlen(operands_ptr[i]) == EMPTY) {
            error_code = OPERAND_IS_EMPTY;
        }
	}

	return error_code;
	
}

/* ------------------------------------------
Function: check_line().
	Check if the given line is a valid line based on several criteria.
Params:
	line - the given line.
Return:
	OK if the given line is a valid line, a related error code if error was detected.
------------------------------------------- */

int check_line(char *line) {

	int error_code = OK, last_char_index = strlen(line) - 1, total_line_length = strlen(line) + 1; /*total_line_length including the EOS character */
	char *copied_line = (char *)malloc(MAX_LINE * sizeof(char)), *line_ptr, *element;
	
	if (!copied_line) {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	else {
		strcpy(copied_line, line);
		line_ptr = copied_line;
		element = get_next_element(&line_ptr, COMMA_DELIMETER_STR);
		/* EMPTY_LINE and COMMENT_LINE are defined with error codes but used as indicators to detect an empty line or a comment line.
		   There are no error message for these error code */

		/* check line criteria, step by step */
		if(is_empty_line(line)) { 
			error_code = EMPTY_LINE;
		}
		else if (line[last_char_index] != EOL && total_line_length == MAX_LINE) { /* each line should finish */
			error_code = ABOVE_MAX_LINE;
		}
		else if(is_comment_line(element)) {
			error_code = COMMENT_LINE;
		}
		else if (!is_valid_operands_line(line)) { 
			error_code=  INVALID_OPERANDS_LINE;
		}

	}
	free(copied_line);
	return error_code;
}

/* ------------------------------------------
Function: check_operands_num().
	Check if the command line has the required operands number to parse it correctly.
	Do it by comparing the required number operands which can be found in the assembly command table
	with the number of operands in the given line.
Params:
	line - the rest of the line to extract the operands from.
	operands_num - the required operands number as it is define in the assembly command table.
Return:
	OK if the operands number of the current command are the same as the required number
	, a related error code if an error was detected. 
------------------------------------------- */


int check_operands_num(char *line, reqOperands operands_num) {
	int error_code = OK, elements_amount;
	
	if (operands_num != NOT_CONSTANT) {
		elements_amount = get_num_of_elements(line,COMMA_DELIMETER_STR); /* number of operands extracted from the line */
		
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

/* ------------------------------------------
Function: create_output_files().
	Create all output files (.ob, .ext, .ent) required by the assembly file.
	Create only the required files and not all off them.
	Check for errors before creating the files.
Params:
	file_name - the given assembly file name without extension to make the new files with the correct extension.
	icf - the instruction counter final value which is printed in the object file.
	dcf - the data counter final value which is printed in the object file.
Return:
	OK if the files creation succeeded without errors, a related error code if an error was detected. 
------------------------------------------- */

int create_output_files(char *file_name, int icf, int dcf) {
	int error_code = OK;
	char *entry_file_name, *external_file_name, *object_file_name;
	FILE *object_file, *external_file, *entry_file;
	bool is_entry_file_needed = is_entry_list(), is_external_file_needed = is_external_list();

	/* Memory allocation for the files name string */
	object_file_name = (char *)malloc(strlen((file_name)) + EXTENSION_SIZE * sizeof(char));
	external_file_name = (char *)malloc(strlen((file_name)) +EXTENSION_SIZE * sizeof(char));
	entry_file_name = (char *)malloc(strlen((file_name)) + EXTENSION_SIZE * sizeof(char));
	
	/* check if memory allocation failed */
	if(object_file_name == NULL || external_file_name == NULL || entry_file_name == NULL) {
		error_code = MEMORY_ALLOCATION_FAILED;
	}
	else{
			/* set the output file name with correct extension */
			strcpy(object_file_name, file_name);
    		strcat(object_file_name, OBJECT_FILE_EXT);
			strcpy(entry_file_name, file_name);
    		strcat(entry_file_name, ENTRY_FILE_EXT);
			strcpy(external_file_name, file_name);
    		strcat(external_file_name,EXTERN_FILE_EXT);
 
			/* check if the program can create and write to the output files */
			if((object_file = fopen(object_file_name, "w")) == NULL) {
					error_code = CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE;
			}
			/* open the file just if the entry list conatins some data */
			else if((is_entry_file_needed) && ((entry_file = fopen(entry_file_name, "w")) == NULL)) {
					error_code = CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE;
			}
			/* open the file just if the external list conatins some data */
			else if((is_external_file_needed) && ((external_file = fopen(external_file_name, "w")) == NULL)) {
					error_code = CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE;
			}
			

			/* if error was not detect until now, the function will print the data to required files */
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
			/* free memory allocation */
			free(object_file_name);
			free(external_file_name);
			free(entry_file_name);


	}
	
	return error_code;
}

/* ------------------------------------------
Function: print_entry_file().
	Print to entry list to the output entry file. 
Params:
	external_file - the output entry file to print the external list on.
------------------------------------------- */
void print_entry_file(FILE *entry_file) {

	entryNodePtr current_entry_ptr = NULL;
	/* going all over the entry list and print it in the required format - symbol name and address*/	
	for(current_entry_ptr = ent_head; current_entry_ptr != NULL; current_entry_ptr = current_entry_ptr -> next) {
		fprintf(entry_file,"%s %04d\n", current_entry_ptr->symbol_name, current_entry_ptr->address);
	}

}

/* ------------------------------------------
Function: print_external_file().
	Print to external list to the output external file. 
Params:
	external_file - the output external file to print the external list on.
------------------------------------------- */

void print_external_file(FILE *external_file) {

	externalNodePtr current_external_ptr = NULL;
	/* going all over the external list and print it in the required format - symbol name and address*/
	for(current_external_ptr = ext_head; current_external_ptr != NULL; current_external_ptr = current_external_ptr -> next) {
		fprintf(external_file,"%s %04d\n", current_external_ptr->symbol_name, current_external_ptr->address);
	}
}


/* ------------------------------------------
Function: print_object_file().
	Print all object file data to the output object file. 
	Use several different functions to to print different parts of the object file.
Params:
	object_file - the output object file to print the data on.
	icf - program instruction counter final value used to print the final value in the object file.
	dcf - program data counter final value used to print the final value in the object file.
------------------------------------------- */

void print_object_file(FILE *object_file, int icf, int dcf) {
	
	fprintf(object_file, "%d %d\n", icf-MEMORY_STARTING_LOCATION, dcf); /* object file headline. */
	print_code_image_to_file(object_file);
	print_data_image_to_file(object_file, icf ,dcf);

}

/* ------------------------------------------
Function: get_command().
	Return an assembly command data by searching the command in the assembly command table
	using the the given name.
	The returned command is the command with the same name as the given name.
Params:
	name - the given command name.
Return:
	pointer to the command if command is found, NULL otherwise.
------------------------------------------- */

command *get_command(char *name) {
	command *command_ptr;
	/* going over the whole assembly command table */
	for(command_ptr=(command *)cmd; command_ptr->commmad_name !=  NULL; command_ptr++) {
		if(are_strings_equal(command_ptr->commmad_name, name)) {
			return command_ptr;
		}
	}
	return NULL;
}


/* ---------------------------------------- */
