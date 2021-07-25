#include "assemblerData.h"
#include "pass.h"


bool global_error_flag = false;
bool global_memory_flag = false;

int main(int argc, char *argv[]) {

	int i, files_amount = --argc;
	FILE *files[MAX_FILES] = {NULL}, *fp;
	char *files_name[MAX_FILES] = {NULL}, *file_name;

	if(!files_amount) {
		printf("Assembler: files were not given.\n");
	}
	else if (files_amount > MAX_FILES) {
		printf("Assembler: too many files were given. the max is: %d.\n", MAX_FILES);
	}
	else {
		
		argv++;
	
		for(i=0; i < files_amount; i++) {
			files_name[i] = argv[i];
			if (is_error(check_assembly_extension(argv[i]))){
				printf("Assembler: file %d has invalid extension.\n", i+1);
			}
			else {
				if((files[i] = fopen(argv[i], "r")) == NULL) {
					printf("Assembler: cannot open file %d.\n", i+1);
				}
			}
		}
		for(i=0; i < files_amount; i++) { 
			fp = files[i];
			file_name = files_name[i];
			global_error_flag = false;
			global_memory_flag = false;
			
			if(fp) {
				print_seperation();
				print_file_processing(i);
				pass(fp,file_name);
				printf("finished processing\n");
				free_memory();
				fclose(fp);
			}			
		}
	}
	return 0;
}

	



