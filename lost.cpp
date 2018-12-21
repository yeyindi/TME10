/*
 * lost.cpp
 *
 *  Created on: 21 déc. 2018
 *      Author: 3874115
 */

#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;
int i,n = 6;
int pid;
sem_t * sread1;
sem_t * sread2;
sem_t * swrite;

void send(char * buf){
	const char *msg[]={"je","suis","un","vilain","programme","\n"};
	//cout << getpid() << " send() : " << endl;
	for (i = 0; i < n; i++) {
		//cout << " send() : dans le for ,boucle :" << i << endl;
		sem_wait(swrite);
		sem_wait(swrite);
		strcpy(buf,msg[i]);
		cout << "  send() : " << msg[i] << " sent" << endl;
		cout << buf << endl;
		sem_post(sread1);
		sem_post(sread2);
	}
	exit(0);
}

void receive(char * buf){
	if((pid=fork())==-1){
		perror("fork");
		exit(1);
	}
	//cerr <<  getpid() << " void receive()"  << endl;
	for (i = 0; i < (n/2); i++) {
		if(pid==0)
			sem_wait(sread1);
		else
			sem_wait(sread2);
		//cout<<getpid()<<endl;
		printf("%s ", buf);
		sem_post(swrite);
	}
	//sem_post(swrite);
}

int main(int argc, char **argv) {

	char * shared = (char*) mmap(NULL, 50*sizeof(char),
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			-1 , 0);

	sread1 = sem_open("/readsem1", O_CREAT | O_RDWR, 0666, 0);
	sread2 = sem_open("/readsem2", O_CREAT | O_RDWR, 0666, 0);
	swrite = sem_open("/readsem3", O_CREAT | O_RDWR, 0666, 2);


	//cout << "sem begin" << endl;
	//sem_init(sread1, 1, 0);

	//cout << "sem ok" << endl;
	//sem_init(sread2, 1, 0);
	//sem_init(swrite, 1, 2);
	cout << "sem ok" << endl;

	if((pid=fork())==-1){
		perror("fork");
		exit(1);
	}
	else
		cout << "fork ok" << endl;



	if(pid==0){
		cout << "main() fork send: " << endl;
		//printf("%s", "bonjour");
		send(shared);
	}
	else
		cout << "main() fork receive: " << endl;
		receive(shared);

	if(pid==0)
		exit(0);

	for (i = 0; i < 2; ++i) {
		wait(0);
	}
	printf("fin du programme\n");
	/*sem_destroy(sread1);
	sem_destroy(sread2);
	sem_destroy(swrite);*/
	sem_close(sread1);
	sem_close(sread2);
	sem_close(swrite);
	sem_unlink("/readsem1");
	sem_unlink("/readsem2");
	sem_unlink("/readsem3");
	munmap(shared,50*sizeof(char));

	return 0;
}



