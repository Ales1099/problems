#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "file.h"

static pid_t pid;
char ** a;
int i;
int check_process(int __n);

int main(int args, char *argv[]){

	int n;

	//Создание n файлов
	//В каждом файле записывается по n вещественных чисел

	fprintf(stderr, "Enter the number n = ");
	scanf("%d",&n);

	a = (char**)malloc(n * sizeof(char*));
	for(i = 0; i < n; i++){
		a[i] = (char*)malloc(10 * sizeof(char));
	}

	for(i = 0; i< n; ++i){
		sprintf(a[i], "a%d.txt",i);
	}

	srand(time(NULL));
	for(i = 0; i < n; i++){

		FILE * file = fopen((const char*)a[i],"w+");

		if(file == NULL){
			fprintf(stderr, "Cannot file - %s\n",a[i]);
			return 1;
		}

		for(int j = 0; j < n; j++){
			fprintf(file, "%f\n", (float)(rand()%10));
		}

		fclose(file);
	}

	pid = fork();

	if(pid == -1){
		fprintf(stderr, "Cannot fork!!!\n");
		return 2;
	} else if(pid == 0){
		return 0;
	}

	pid = fork();

	if(pid == -1){
		fprintf(stderr, "Cannot fork!!!\n");
		return 3;
	} else if(pid == 0){
		return 0;
	}

	return check_process(n);
}

int check_process(int __n){

	pid_t res;
	int status;

	int semid;

	if((semid = semget(SEM_INDEX,1,PERMS | IPC_CREAT)) < 0){
		fprintf(stderr, "Cannot sematphore!!!\n");
		return 6;
	}

	semctl(semid,0,SETVAL,0);

	for(int i = 0; i < __n; i++){

		pid = fork();

		if(pid == -1){
			fprintf(stderr, "Cannot fork!!!\n");
			return 4;
		} else if(pid == 0){
			errno = execl("./daemon","./daemon",a[i],NULL);
			fprintf(stderr, "Cannot execv errno = %d!!!\n",errno);
			return 5;
		}

		while(((res = waitpid(pid, &status, 0))<=0)&&(errno == EINTR));

		if(res != pid){
			fprintf(stderr, "Child process cannot waiting!!!\n");
			return 6;
		}

		if(WIFEXITED(status)){
			fprintf(stderr, "Child process %d exited with code %d\n",pid,WEXITSTATUS(status) );
		} else if(WIFSIGNALED(status)){
			fprintf(stderr, "Child process %d killed by signal %d\n", pid, WTERMSIG(status));
		} else{
			fprintf(stderr, "Child process %d terminating by unknown reason\n", pid);
		}
	}

	if(semid = semctl (semid, 0, IPC_RMID, (struct semid_ds *) 0) < 0){
		fprintf(stderr, "Cannot remove semaphore!!!\n");
		return 7;
	}

	return 0;
}