#include <stdio.h>
#include <errno.h>
#include "file.h"

int main(int argc, char * argv[]){

	if(argc == 1){
		fprintf(stderr, "Cannor argumen of main function!!!\n");
		return 1;
	}

	int semid;

	pid_t pid = getpid();

	fprintf(stderr, "Process by %d started!\n", pid);

	FILE * file = fopen((const char*)argv[1],"r+");

	if(file == NULL){
		fprintf(stderr, "Cannot file - %s!\n", argv[1]);
		return 2;
	}

	float number = 0.0f, sum = 0.0f;

	while((errno = fscanf(file,"%f",&number)) != EOF){
		sum += number;
	}
	fclose(file);
	fprintf(stderr, "sum = %f\n",sum);

	//Запись суммы элементов файла в res.txt
	//Имя файла задаётся через аргументы командной строки
	//ИСПОЛЬЗОВАНИЕ СЕМАФОРА!!!

	if((semid = semget(SEM_INDEX,1,0)) < 0){
		fprintf(stderr, "Cannot daemon!!!\n");
		return 3;
	}

	while(semctl(semid,0,GETVAL,0));

	semctl(semid,0,SETVAL,1);

	file = fopen("res.txt","r+");

	if(file == NULL){

		//ЗАКРЫТИЕ СЕМАФОРА!!!
		semctl(semid,0,SETVAL,0);
		fprintf(stderr, "Cannot file - res.txt!\n");
		return 3;
	}

	float t;
	fscanf(file,"%f",&t);
	fprintf(stderr, "%f\n",t);
	t += sum;
	fprintf(stderr, "%f\n",t);
	fseek(file,0,SEEK_SET);
	fprintf(file, "%f", t);
	fclose(file);

	semctl(semid,0,SETVAL,0);

	//ЗАКРЫТИЕ СЕМАФОРА!!!

	fprintf(stderr, "Process by %d finished!\n", pid);

	return 0;
}