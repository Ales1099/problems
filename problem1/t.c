#include <stdio.h>

int main(int argc, char *argv[]){
	FILE * file = fopen((const char*)argv[1],"rb");

	float number;

	fprintf(stderr,"%f\n",number);

	fclose(file);

	return 0;
}
