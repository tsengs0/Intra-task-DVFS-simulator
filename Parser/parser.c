#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int char_int(char *in);
float char_float(char *in);

int main(int argc, char **argv)
{
/*
	long i;
	FILE *fp = fopen(argv[1], "rb");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET); // rewind(fp)
	
	char *string;
	string = (char*) malloc(fsize + 1);
	fread(string, fsize, 1, fp);
	fclose(fp);

	string[fsize] = NULL;
	for(i = 0; i < fsize; i++) printf("%d:\t%c\r\n", i, string[i]);
*/
	int c, col_size = 0, row_size = 0;
	char *col_element = NULL, *token;
	const char delim[2] = " ";
	float **param_1 = NULL;
	FILE *fp;
	int i, j;
	 
	fp = fopen(argv[1], "rb");

	param_1 = (float**) malloc(sizeof(float*) * 1);
	c = fgetc(fp);
	while(c != EOF) {
		col_size = 0;
		while(c != 0x0A) {
			col_element = (char*) realloc(col_element, sizeof(char) * (col_size + 1));
			col_element[col_size] = c;
			col_size += 1;
			c = fgetc(fp);
		}	
		
		col_size = 0;	
		token = strtok(col_element, delim);
		while(token != NULL) {
			param_1[row_size] = (float*) realloc(param_1[row_size], sizeof(float) * (col_size + 1));
			param_1[row_size][col_size] = char_float(token);
			token = strtok(NULL, delim);
			col_size += 1;			
		}

		while((c = fgetc(fp)) == 0x0A);
		param_1 = (float**) realloc(param_1, sizeof(float*) * (row_size + 1));
		row_size += 1;
	}
	
	for(i = 0; i < row_size; i++) 
		for(j = 0; j < col_size; j++) 
			printf("(%d,%d):\t%.02f\r\n", i, j, param_1[i][j]);
	
	free(param_1);

	return 0;
}

int char_int(char *in) {
	int temp = 0, cnt = 0;
	
	do {
		temp *= 10; 
		temp += (in[cnt] - 48);
		cnt += 1;
	} while(in[cnt] != EOF);

	return temp;
}

float char_float(char *in) {
	float temp = 0.0;
	int cnt = 0, digit = 1, mul, i;
	
	do {
		temp *= 10; 
		temp += (in[cnt] - 48);
		cnt += 1;
	} while(in[cnt] != NULL && in[cnt] != 0x2E);
	
	if(in[cnt] == 0x2E) {
		cnt += 1;
		do {
			for(i = 0, mul = 1; i < digit; i++) mul *= 10; 
			temp += ((float) (in[cnt] - 48) / mul);
			cnt += 1; digit += 1;
		} while(in[cnt] != NULL);
	}

	return temp;
}
