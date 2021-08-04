/*	
	Assmebler Program
	This program recives assembly files (until max files value) through the command line and translates the 
	assembly commands into machine binary instructions
	The insturctions ar printed to the program output files(.ob, .ext, .ent). 
	The output files will be create in the same folder as the given assembly files.
	In case error detected while parsing the file, output files will not be created 
	and the program will print a realted error message to the stdout.

	Autor: Omer Sher
	ID: 318367703
*/

/* Include Files */

#include "../include/assemblerData.h"
#include "../include/pass.h"

/* 
	Global Error flags - 
	used for detecting errors occasions.
*/
bool global_error_flag = false;
bool global_memory_flag = false;

int main(int argc, char *argv[]) {

	int index, files_amount = --argc, file_index; /* --argc gives us the number of the given file through the command-line */
	FILE *files[MAX_FILES] = {NULL}, *fp;
	char *files_name[MAX_FILES] = {NULL}, *file_name;

	if(!files_amount) { /* Checks if any file was given to the program thorugh the command line */
		printf("\nAssembler: files were not given.\n");
	}
	else if (files_amount > MAX_FILES) { /* MAX_FILES can be changed */
		printf("\nAssembler: too many files were given. the max is: %d.\n", MAX_FILES);
	}
	else {
		/* checks assembly extnesion for each file, in case the file has valid extension, the file is added to the files[] array. */
		for(file_index = 0 ,index = 1; index <= argc; index++) {
			if (is_valid_assembly_extension(argv[index]) == false) {
				printf("\nAssembler: <%s> has invalid extension. The file will not be parsed.\n", argv[index]);
			}
			else {
				if((files[file_index] = fopen(argv[index], "r")) != NULL) {
					files_name[file_index] = argv[index];
					file_index++; /* counters how many files have valid extensions */
				}
				else {
					printf("\nAssembler: could not open <%s>. The file will not be parsed.\n", argv[index]);
				}	
			}
		}
		
		/* going over all opened files */
		for(index = 0; index < file_index; index++) { 
			
			/* setup for before the file parsing*/
			fp = files[index];
			file_name = files_name[index];
			global_error_flag = false;
			global_memory_flag = false;

			pass(fp, file_name); /* parsing the file */
			free_pass_memory(); /* free all allocated memory used in pass function*/
			fclose(fp);	/* close the current file */
		}
	}
	return 0;
}

	



