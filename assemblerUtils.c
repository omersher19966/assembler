#include "assemblerData.h"
#include "symbol.h"

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
	NULL
};


/* ---------------------------------------- */

void print_error(int error_code) {
	if (error_code == MEMORY_ALLOCATION_FAILED) {
		global_memory_flag = true;
	}
	global_error_flag = true;
	printf("line %d: %d\n", lc, error_code);
	/*switch(error_type) {
		case NO_GIVEN_FILES: 
			printf("NO_GIVEN_FILES\n");
			break;
		case INVALID_EXTENSION: 
			printf("INVALID_EXTENSION\n");
			break;
		default:
			printf("Default option\n");
			break;			 	      	
	}*/
}

/* ---------------------------------------- */

void print_seperation() {
	printf("-----------------------------------");
}

/* ---------------------------------------- */

void print_file_processing(int i) {
	printf("\nProccessing file %d...\n", i);
}

/* ---------------------------------------- */

void print_entry_file(FILE *ent_file) {
	symbolPtr symbol_ptr;
	
}

/* ---------------------------------------- */
/* need to change it later */
int check_assembly_extension(char *path) {
	int error_code;
	char *suffix_ptr;
	
	if ((suffix_ptr = strstr(path, ASSEMBLY_FILE_EXT)) && (*(suffix_ptr + EXTENSION_LENGTH) == EOS)){
		error_code = OK;
	}
	else {
		error_code = INVALID_EXTENSION;
	}
	return error_code;
}

/* ---------------------------------------- */

bool are_strings_equal(char *str1, char* str2) {
	if (!strcmp(str1, str2)){
    	return true;
	}  	
	return false;
}

/* ---------------------------------------- */


bool is_valid_number(char *operand) {
	
	for(;isspace(*operand); operand++)
	; 
	if ((*operand == NEGATIVE_SIGN) || (*operand == POSITIVE_NUMBER)) {
		operand++;
	}
	for(;isdigit(*operand) && *operand != EOS; operand++)
	;

	return *operand == EOS ? true : false;
}

/* ---------------------------------------- */

int check_register(char *operand) {
	int error_code = OK, register_num;
	if( (!(*operand++ == REG_SIGN)) && is_valid_number(operand)){
		error_code = NOT_A_REGISTER;
	}
	else {
		register_num = atoi(operand);
		if (!(register_num >= START_REGISTER && register_num <= END_REGISTER)) {
			error_code = REGISTER_NOT_IN_RANGE;
		}
	}
	return error_code;
}

/* ---------------------------------------- */

bool is_valid_operand (char *operand){
	if (*operand == NEGATIVE_SIGN) {
		operand++;
	}
	for(;isalnum(*operand) && *operand != EOS; operand++)
	;
	return *operand == EOS ? true : false;
}

/* ---------------------------------------- */

void remove_trailing_spaces (char *word) {
	int len;
	for(len = strlen(word) - 1; isspace(word[len]); len--) {		 
	}
	word[++len] = EOS;
	return;
}

/* ---------------------------------------- */

bool is_empty_line(char *word) {
	char *ch;
	for (ch = word; (isspace(*ch)) && (*ch != NULL); ch++)
	;
	return *word == EOS ? true : false;
}

/* ---------------------------------------- */

void skip_spaces(char **word) {
	for (; (isspace(*word)); *word++)
	;
}

/* ---------------------------------------- */

bool is_assembly_key_word(char *word) {
	 char **keyword; 
	for(keyword = (char **)assembly_keywords; *keyword != NULL; keyword++) {
		if(are_strings_equal(*keyword, word)) {
			return true;
		}
	}
	return false;
}

/* ---------------------------------------- */


/* ---------------------------------------- */

void start_new_line(FILE *fp) {
	while(getc(fp) != EOL || getc(fp) != EOF)
	;
}

/* ---------------------------------------- */

char *get_next_element(char **line, char *delimeters) {
    char *element, *ch = *line, delimeter;
    int element_size, i;
	bool is_delimeter_found = false;
    
    do {
        if (is_delimeter_found == true) {
            ch++;
        }
        for(;isspace(*ch);ch++)
        ; 
        *line = ch;
        for(element_size = 0 ; is_delimeter_found == false && (*ch) != EOS ; element_size++, ch++)
			for(i=0; delimeters[i] != EOS; i++) {
				if(*ch == delimeters[i]) {
					is_delimeter_found = true;
					delimeter = *ch--;
					break;
				}
			}
        ;
    } while ((*ch != EOS) && (!element_size && is_delimeter_found));
    

    if (!(element_size)) {
        *line = ch;
        element = NULL; 
    }
    else if(*ch == delimeter) {
        *ch++ = EOS;
        element = *line;
        *line = ch;
    }
    else if(*ch == EOS) {
        element = *line;
        *line = ch;
    }

	if(element){
        remove_trailing_spaces(element);
    }
    return element;
}

/* ---------------------------------------- */

bool is_comment(char *word) {
	return *word == COMMENT ? true : false;
}

/* ---------------------------------------- */

bool is_valid_string(char *operand) {
	int index = 0, last_char_index = strlen(operand) - 1;
	if (operand[index] == QUTATION_MARK_CHAR && operand[last_char_index] == QUTATION_MARK_CHAR) {
		for(index = 1; isprint(operand[index]) && operand[index] != QUTATION_MARK_CHAR && index < last_char_index ;index++);
		if(index == last_char_index) {
			return true;
		}
	}
	return false;
}

/* ---------------------------------------- */

int get_num_of_elements(char *line, char *delimeters) {
	char copied_line[MAX_LINE], *line_ptr, *element;
	int counter;
	strcpy(copied_line, line);
	line_ptr = copied_line;
	for(counter = 0; (element = get_next_element(&line_ptr, delimeters)) != NULL; counter++);
	return counter;
}

/* ---------------------------------------- */

bool is_error(int error_code) {
	if(error_code == OK) {
		return false;
	}
	return true;
}

/* ---------------------------------------- */

void free_memory() {
	free_symbol_table();
	free_entry_list();
	free_external_list();
	free_code_image();
	free_data_image();
	reset_counters_indexes();
	return;
}

/* ---------------------------------------- */

void remove_file_extension(char *file_name) {
	char *extension_ptr;
	extension_ptr = strstr(file_name, ASSEMBLY_FILE_EXT);
	*extension_ptr = EOS;
	return;
}