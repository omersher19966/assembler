#include "assemblerData.h"
#include "pass.h"

/* Global Error flags - 
   used for detecting errors occasions.
*/
bool global_error_flag = false;
bool global_memory_flag = false;

int main(int argc, char *argv[]) {

	int index, files_amount = --argc, file_index;
	FILE *files[MAX_FILES] = {NULL}, *fp;
	char *files_name[MAX_FILES] = {NULL}, *file_name;

	if(!files_amount) {
		printf("\nAssembler: files were not given.\n");
	}
	else if (files_amount > MAX_FILES) {
		printf("\nAssembler: too many files were given. the max is: %d.\n", MAX_FILES);
	}
	else {

		for(file_index = 0 ,index = 1; index <= argc; index++) {
			if (is_valid_assembly_extension(argv[index]) == false) {
				printf("\nAssembler: %s has invalid extension. the file will not be parsed.\n", argv[index]);
			}
			else {
				files_name[file_index] = argv[index];
				if((files[file_index] = fopen(argv[index], "r")) == NULL) {
					printf("\nAssembler: coudltn't open the file - %s.\n", argv[index]);
				}
				file_index++;
			}
		}
		
		for(index = 0; index < file_index; index++) { 
			
			fp = files[index];
			file_name = files_name[index];
			global_error_flag = false;
			global_memory_flag = false;

			if(fp) {
				pass(fp, file_name);
				free_memory();
				fclose(fp);
			}			
		}
	}
	return 0;
}

	



