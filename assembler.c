#include "assemblerData.h"
#include "pass.h"


bool global_error_flag = false;
bool global_memory_flag = false;

int main(int argc, char *argv[]) {

	int index, files_amount = --argc, file_index;
	FILE *files[MAX_FILES] = {NULL}, *fp;
	char *files_name[MAX_FILES] = {NULL}, *file_name;

	if(!files_amount) {
		printf("Assembler: files were not given.\n");
	}
	else if (files_amount > MAX_FILES) {
		printf("Assembler: too many files were given. the max is: %d.\n", MAX_FILES);
	}
	else {

		for(file_index = 0 ,index = 1; index <= argc; index++) {
			if (is_error(check_assembly_extension(argv[index]))) {
				printf("Assembler: file %d has invalid extension.\n", index);
			}
			else {
				files_name[file_index] = argv[index];
				if((files[file_index] = fopen(argv[index], "r")) == NULL) {
					printf("Assembler: cannot open file %d.\n", index);
				}
				file_index++;
			}
		}
		
		for(index = 0; index < file_index;) { 
			
			fp = files[index];
			file_name = files_name[index];
			global_error_flag = false;
			global_memory_flag = false;
			index++;

			if(fp) {
				pass(fp, file_name, index);
				free_memory();
				fclose(fp);
			}			
		}
	}
	return 0;
}

	



