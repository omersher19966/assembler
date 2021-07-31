/* 
	The file contatins general functions for the assembler program. 
	The file is used by all other files and all general useful functions which used by 
	most of the file are here.
*/

/* Because the File is so general all program header files are included here.*/

#include "../include/assemblerData.h"
#include "../include/pass.h"
#include "../include/symbol.h"

/* Assembly Keywords Table */
const char *assembly_keywords[] = {
	"add",
	"sub",
	"and",
	"or",
	"nor",
	"move",
	"mvhi",
	"mvlo",
	"addi",
	"subi",
	"andi",
	"ori",
	"nori",
	"bne",
	"beq",
	"blt",
	"bgt",
	"lb",
	"sb",
	"lw",
	"sw",
	"lh",
	"sh",
	"jmp",
	"la",
	"call",
	"stop",
	"db",
	"dh",
	"dw",
	"asciz",
	NULL};

/* ------------------------------------------
Function: print_error()
	Print a related error message based on the given error code.
	Print memory errors, syntax errors and invalid operands.
	Most of errors have the same format and extera info can be given by the paramter word.
Params:
	error_code - the error code to print a related message error.
	file_name - the file name where the error was detected.
	word - extra info to print, if not necessary give NULL. 
------------------------------------------- */

void print_error(int error_code, char *file_name, char *word) {

	if (global_error_flag == false) {
		printf("\n------------- %s - Errors -------------\n", file_name); /* in case is the first error in the file nice header is printed */
	}

	if (error_code == MEMORY_ALLOCATION_FAILED || error_code == DATA_IMAGE_IS_FULL || error_code == CODE_IMAGE_IS_FULL) {
		global_memory_flag = true;
	} /* in case memory error code is detected the global_memory_flag is up which will cause to the program
	to stop scanning the assembly file */

	global_error_flag = true; /* used to detect in the end of the scanning if error was occurred or not */

	switch (error_code) { /* Print the right error message based on the error code */
		/* Assembler Error Code Messages */
		case MEMORY_ALLOCATION_FAILED:
			printf("Error: stopped processing the file due to lack of memory\n");
			break;
		case DATA_IMAGE_IS_FULL:
			printf("Error: stopped processing the file because data image is full.\n");
			break;
		case CODE_IMAGE_IS_FULL:
			printf("Error : stopped processing the file because code image is full.\n");
			break;
		case INVALID_NUMBER:
			printf("Error: line %d - the given operand is not a valid number.\n", lc);
			break;
		case INVALID_STRING:
			printf("Error: line %d - the given operand is not a valid string.\n", lc);
			break;
		case INVALID_CMD:
			printf("Error: line %d - unrecognized command <%s>.\n", lc, word);
			break;
		case INVALID_REGISTER:
			printf("Error: line %d - the given operand is not a valid register.\n", lc);
			break;
		case NOT_IN_REGISTERS_RANGE:
			printf("Error: line %d - the given operand is not in the valid registers range(%d-%d).\n", lc, START_REGISTER, END_REGISTER);
			break;
		/* others */
		case ABOVE_MAX_LINE:
			printf("Error: line %d - the length of line is above max(%d).\n", lc, MAX_LINE);
			break;
		case NO_GIVEN_OPERANDS:
			printf("Error: line %d - operands were not given for the command.\n", lc);
			break;
		case GIVEN_OPERANDS_ARE_LESS_THAN_REQUIRED:
			printf("Error: line %d - given operands are less than required.\n", lc);
			break;
		case GIVEN_OPERANDS_ARE_MORE_THAN_REQUIRED:
			printf("Error: line %d - given operands are more then required.\n", lc);
			break;
		case INVALID_OPERANDS_LINE:
			printf("Error: line %d - line is not valid due to comma character in the end of the line.\n", lc);
			break;
		case INVALID_OPERAND:
			printf("Error: line %d - the given operand is not valid.\n", lc);
			break;
		case OPERAND_IS_EMPTY:
			printf("Error: line %d - empty operand in line.\n", lc);
			break;
		case EXTERNAL_SYMBOL_CANNOT_BE_USED_IN_BRANCHING_COMMAND:
			printf("Error: line %d - external label cannot be used in branching command, only entry label is allowed.\n", lc);
			break;
		case CANNOT_CREATE_OR_WRITE_TO_EXTERN_FILE:
			printf("Error: could not create or write to the externals file.\n");
			break;
		case CANNOT_CREATE_OR_WRITE_TO_ENTRY_FILE:
			printf("Error: could not create or write to the entries file.\n");
			break;
		case CANNOT_CREATE_OR_WRITE_TO_OBJECT_FILE:
			printf("Error: could not create or write to the object file.\n");
			break;
		/* Label/Symbol Error Codes */
		case INVALID_LABEL:
			printf("Error: line %d - not a valid label <%s>.\n", lc, word);
			break;
		case ABOVE_MAX_LABEL:
			printf("Error: line %d - label is above max label length(%d).\n", lc, MAX_LABEL_LENGTH);
			break;
		case LABEL_IS_ASSEMBLY_KEYWORD:
			printf("Error: line %d - label is assembly keyword, cannot be used.\n", lc);
			break;
		case LABEL_HAS_ALREADY_BEEN_USED:
			printf("Error: line %d - label <%s> has already been defined in previous lines.\n", lc, word);
			break;
		case SYMBOL_IS_NOT_DEFINED:
			printf("Error: line %d - given label is not defined.\n", lc);
			break;
		case SYMBOL_CANNOT_BE_DEFINED_AS_ENTRY_AND_EXTERNAL:
			printf("Error: line %d - given label cannot be defined as entry and external label.\n", lc);
			break;
		default:
			printf("Error: line %d - %d\n", lc, error_code);
			break;
	}
}

/* ------------------------------------------
Function: is_valid_assembly_extension()
	Check if a given file name has valid assembly extension ".as", for example - "file.as"
Params:
	file_name - the file name.
Return:
	TRUE if the given file_name has a valid assembly extension ".as", FALSE otherwise. 
------------------------------------------- */

bool is_valid_assembly_extension(char *file_name) {

	char *suffix_ptr;
	/* check if ".as" is in the file name and that the ".as" is a really extension and not part of the file name,
	meaning checks if the ".as" comes in the end of the file name */
	if ((suffix_ptr = strstr(file_name, ASSEMBLY_FILE_EXT)) && (*(suffix_ptr + strlen(ASSEMBLY_FILE_EXT)) == EOS)) {
		return true;
	}

	return false;
}

/* ------------------------------------------
Function: are_strings_equal()
	Check if the given strings: str1, str2 are equal.
Params:
	str1 - first string.
	str2 - second string.
Return:
	TRUE if the given string are equal, FALSE otherwise. 
------------------------------------------- */

bool are_strings_equal(char *str1, char *str2) {
	if (!strcmp(str1, str2)) {
		return true;
	}
	return false;
}

/* ------------------------------------------
Function: is_valid_number()
	Check if the given operand is a valid number.
	Valid number are positive or negative integers only.
Params:
	operand - given number used as an operand in the command line.
Return:
	TRUE if the given number is valid, FALSE otherwise. 
------------------------------------------- */

bool is_valid_number(char *operand) {

	for (; isspace(*operand); operand++)
	;
	if ((*operand == NEGATIVE_SIGN) || (*operand == POSITIVE_SIGN)) {
		operand++;
	}
	for (; isdigit(*operand) && *operand != EOS; operand++)
	;

	return *operand == EOS ? true : false;
}

/* ------------------------------------------
Function: check_register()
	Check if the given start operand is a valid register.
	Valid register starts with the $ and then number between 0-31 as the number of the register in the machine.
Params:
	operand - given register used as an operand in the command line.
Return:
	OK if the register is a valid register, a related error code otherwise. 
------------------------------------------- */

int check_register(char *operand)
{

	int error_code = OK, register_num;

	if (*operand++ == REG_SIGN)
	{ /* incresing operand to get the number after it in case it's a '$' sign.*/
		if (is_valid_number(operand))
		{
			register_num = atoi(operand);
			if ((register_num < START_REGISTER) || (register_num > END_REGISTER))
			{ /* check if the register is in the range 0-31 */
				error_code = NOT_IN_REGISTERS_RANGE;
			}
		}
		else
		{
			error_code = INVALID_REGISTER;
		}
	}
	else
	{
		error_code = INVALID_REGISTER;
	}

	return error_code;
}

/* ------------------------------------------
Function: void remove_trailing_spaces()
	Remove the trailing white spaces from a given str.
	Start looking for white spaces from then until it encounters a non white space and then sets the place
	as EOS.
Params:
	str - the given string.
------------------------------------------- */

void remove_trailing_spaces(char *str)
{
	int len;
	for (len = strlen(str) - 1; isspace(str[len]); len--)/* starts from the end */
	; 
	str[++len] = EOS; /*sets the last white space as EOS */
	return;
}

/* ------------------------------------------
Function: is_empty_line()
	Check if the given line is an empty or not.
	Empty line can be also a line with only white spaces characters.
Params:
	str - the given line to check.
Return:
	TRUE if the line is empty, FALSE otherwise.
------------------------------------------- */

bool is_empty_line(char *line) {

	char *ch;
	/* loop until it encounters the first character that is not a white space, 
	EOS is also not a white space char */
	for (ch = line; isspace(*ch); ch++)
	;
	return *ch == EOS ? true : false;
}

/* ------------------------------------------
Function: is_assembly_keyword()
	Check if the given word is an assembly keyword.
	The function compare between the given word to the keyword table it maintains.
Params:
	word - the given word.
Return:
	TRUE if the given word is an assembly keywork, FALSE otherwise.
------------------------------------------- */

bool is_assembly_keyword(char *word) {
	char **keyword;
	/* need to do cast in the assembly_keyword assignment because it's defined as (const char **) */
	for (keyword = (char **)assembly_keywords; *keyword != NULL; keyword++) {
		if (are_strings_equal(*keyword, word)) {
			return true;
		}
	}
	return false;
}

/* ------------------------------------------
Function: start_new_line()
	Start a new line in the file pointer by the give file pointer.
	The function is used when above max line is detected, moving forward to the next line.
Params:
	fp - the file pointer.
------------------------------------------- */

void start_new_line(FILE *fp) {
	while ((getc(fp) != EOL) && (getc(fp) != EOF)) /* go over until End of line is detected meaning until the new line */
		;
}

/* ------------------------------------------
Function: get_next_element()
	Find the next element in the given line separated by one of the given delimeters.
	The function skips white spaces and remove trailing white spaces to give the exact element.
	Important note :the given line is assigned to point on the rest of the line after it found the element.
Params:
	line - the address of the pointer is pointing to the line (char **).
		   The address is required because the change should be seen also in the caller function.
	delimeters - the given delimeters seprated the element from the rest of the line.	
Return:
	Pointer to the element, NULL in case it's empty.
------------------------------------------- */

char *get_next_element(char **line, char *delimeters) {
	
	char *element, *ch; /* ch is the pointer which is used to go over the given ling */
	int i, element_size = EMPTY;
	bool is_delimeter_found = false; /* a flag used to detect if one of the given delimeters was found in the line */

	for (ch = *line; isspace(*ch); ch++) /* skip spaces */
		;
	*line = ch; /* Make the line to point on the first non white space char */

	while (is_delimeter_found == false && (*ch) != EOS) {
		for (i = 0; delimeters[i] != EOS && is_delimeter_found == false; i++) {
			if (*ch == delimeters[i]) {
				is_delimeter_found = true; /* raising the flag to break out from the loop */
			}
		}
		element_size++;
		if (is_delimeter_found == false) {
			ch++;
		}
	}

	if (element_size == EMPTY) { /* happens only if the line contains only white spaces or was in the end of line already */
		*line = ch;
		element = NULL;
	}
	else if (is_delimeter_found) {
		/* Convert the delimeter character to EOS, making the element as a string.
		in addition increase ch to point on the rest of the line*/
		*ch++ = EOS;
		element = *line; /* after converting the delimeter, line is now pointing on the element string */
		*line = ch;		 /*assign the rest of line back to line */
	}
	else if (*ch == EOS) { /* happens in case it's the last element without delimeter but contains some characters*/
		element = *line;
		*line = ch;
	}

	if (element) {
		remove_trailing_spaces(element);
	}
	return element;
}

/* ------------------------------------------
Function: is_comment_line()
	Check if the current line is a comment line by checking given first character of the first word in line. 
	Compare it with the comment sign - ';'.
Params:
	word - the first word of the line.
Return:
	TRUE if the current line is a comment line, FALSE otherwise.
------------------------------------------- */

bool is_comment_line(char *word) {
	return *word == COMMENT_CHAR ? true : false;
}

/* ------------------------------------------
Function: is_valid_string()
	Check if the given operand is a valid string.
	Do it by checking if the first and last characters are quotation marks and all other character are printable 
	and are not quotation mark.
Params:
	operand - the given operand.
Return:
	TRUE if the operand is a valid string, FALSE otherwise.
------------------------------------------- */

bool is_valid_string(char *operand) {
	int index = 0, last_char_index = strlen(operand) - 1;
	if (operand[index] == QUOTATION_MARK_CHAR && operand[last_char_index] == QUOTATION_MARK_CHAR) {	
	/* the loop check also the character are not quotation mark in the middle to avoid strings like : "abc"sd" (not valid string) */
		for (index = 1; isprint(operand[index]) && operand[index] != QUOTATION_MARK_CHAR && index < last_char_index; index++)
		;
		if (index == last_char_index) { /* If the loop went over the whole string so it means the operand is a valid string  */
			return true;
		}
	}
	return false;
}

/* ------------------------------------------
Function: get_num_of_elements()
	Conuter the number of elements in the given line seperated by one of the given delimeter.
	Use get_next_element() function to get all elements.
	The function works on a copied line for not changing the orginal one.
Params:
	line - the given line.
	delimeters - the given deliemters as string.
Return:
	The number of elements in line.
------------------------------------------- */

int get_num_of_elements(char *line, char *delimeters) {
	char copied_line[MAX_LINE], *line_ptr, *element;  /* use copied line for not changing the orignal one in the get_next_element functions */
	int counter;
	strcpy(copied_line, line);
	line_ptr = copied_line; /* the pointer which is used for working on the copied line */
	for (counter = 0; (element = get_next_element(&line_ptr, delimeters)) != NULL; counter++)
		;
	return counter;
}

/* ------------------------------------------
Function: is_error()
	Check if the given code is an error one or not. 
	OK is not an error, all others are errors.
Params:
	error_code - the given error code the check error on.
Return:
	TRUE if the error code is an error(different from OK), FALSE otherwise(OK).
------------------------------------------- */

bool is_error(int error_code) {
	if (error_code != OK) {
		return true;
	}
	return false;
}

/* ------------------------------------------
Function: free_memory()
	Relase all allocated dynamic memory for the program like : symbol table, entry and external list and etc.
	In addition reset all counter and program indexes for the next file scanning (if there is more files).
------------------------------------------- */

void free_memory() {
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
Function: is_error()
	Remove assembly file extension for the given file name.
	If the functions is used meaning the file name contains the assembly extnesion and becuase of it validation is not needed.
Params:
	file_name - the given file contains the assembly extension.
------------------------------------------- */

void remove_file_extension(char *file_name) {
	char *extension_ptr;
	extension_ptr = strstr(file_name, ASSEMBLY_FILE_EXT); /* point to the assembly extension in the file name */
	*extension_ptr = EOS; /* set EOS instead assembly extension occurance i.e remove the assembly extension */
	return;
}