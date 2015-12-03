#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAXDEER 4
#define THREAD_COUNT 30
#define MAXELF 25
#define PERCENTERROR 20
#define MAXELFWAIT 3
#define ITERS 1

typedef struct{
int deerID;
}deer_params;

typedef struct{
int elfID;
}elf_params;

int deercount;
int elfWait;
sem_t deerSem, lastDeer, hohoho, end, frontDesk, fullDesk;
pthread_t  thread[THREAD_COUNT];

void *santa( void *);
void *deer( void *);
void *elf( void *);
void harness();
void spendTimeInTropics(int id);
void travelToNorthPole(int id);
void shout();
void deliverPresents();
void thanks();

void main(int argc , char *argv[] )
{
	int ii;
	ii = 0;
	for(ii = 0; ii < ITERS; ii++){ 
	int i;
	


	deer_params params[MAXDEER];
	elf_params elfparams[MAXELF];

	srand(time(NULL));
	deercount = 0;
	elfWait = 0;
	sem_init(&deerSem,0,1);

	sem_init(&lastDeer,0,0);
	sem_init(&hohoho, 0,0);
	sem_init(&end,0,0);
	sem_init(&frontDesk, 0,1);
	sem_init(&fullDesk, 0,0);


	if(pthread_create(&thread[0],NULL, santa, NULL)){
		perror("Santa Failed to Create: \n");
		exit(-1);
	}


	for(i = 1; i < THREAD_COUNT; i++){
		if(i < MAXDEER + 1){
			params[i - 1].deerID = i;
			if(pthread_create(&thread[i], NULL, deer,(void *) &params[i-1])){
				perror("Deer Failed To Create: \n");
				exit(-1);
			}	
		}
		else{
			elfparams[i - 5].elfID = i-4;
			if(pthread_create(&thread[i], NULL, elf,(void *) &elfparams[i-5])){
				perror("ELF Failed To Create: \n");
				exit(-1);
			}
		}
	}



	for(i = 0; i < THREAD_COUNT - MAXELF; i++){
		pthread_join(thread[i], NULL);
	}
	thanks();
	}
}

void *santa(void *ptr)
{
	int i;
	printf("Santa    >>> going to sleep for a while!\n");

	sem_wait(&lastDeer);
	printf("Santa    >>> I woke up!\n");
	printf("Santa    >>> Done with the Elves!\n");
	for(i = MAXDEER + 1; i < THREAD_COUNT; i++){
		pthread_cancel(thread[i]);
	}
	
	harness();
	shout();

	for(i = 1; i <= MAXDEER; i++){
		sem_post(&hohoho);
	}
	deliverPresents();

	for(i = 0; i < MAXDEER; i++){
		sem_post(&end);
	}

	sleep(rand() % 4 + 1);
	//thanks();
	pthread_exit(NULL);
}



void *deer(void *ptr)
{
	int i, id, value;
	deer_params *p;

	p = (deer_params *) ptr;
	id = p->deerID;
	
	//waiting is handled in both of these functions
	spendTimeInTropics(id);
	travelToNorthPole(id);

	sem_wait(&deerSem);
	deercount++;
	if(deercount < MAXDEER){
		printf("ReinDeer %d >>> waiting in the hut!\n", id);
		sem_post(&deerSem);
		sem_wait(&lastDeer);
	}
	else{
		sem_post(&deerSem);
		printf("ReinDeer %d >>> waking those in the Hut!\n",id);
		for(i = 0; i < MAXDEER; i++){
			sem_post(&lastDeer);

		}
		printf("ReinDeer %d >>> waking up Santa!\n", id);
	}



	sem_wait(&hohoho);

	printf("ReinDeer %d >>> flying with santa!\n", id);
	
	sleep(rand() % 4 + 1);


	sem_wait(&end);
	travelToNorthPole(id);
	sem_wait(&deerSem);
	deercount --;
	sem_post(&deerSem);
	pthread_exit(NULL);

}

void *elf(void *ptr)
{
	int i, id, value;
	elf_params *p;

	p = (elf_params *) ptr;
	id = p->elfID;
	
	//waiting is handled in both of these functions
	
	int randnum;
	printf("Elf %d     >>> Started!\n", id);
	for(i = 0; i < 6; i++){
		printf("Elf %d     >>> making present >>> %d!\n", id, (i+1));
		randnum = (rand() % 100 + 1);
		if(randnum <= PERCENTERROR){
			printf("Elf %d     >>> ERROR making present >>> %d!\n", id, i+1);
			sem_wait(&frontDesk);
			elfWait++;
			//sleep(2);
			if(elfWait < MAXELFWAIT){
				sem_post(&frontDesk);
				sem_wait(&fullDesk);
				sem_wait(&frontDesk);
			}
			else{
				for(i = 1; i < MAXELFWAIT; i++){
					sem_post(&fullDesk);
				}
				printf("     >>>Office at full capacity!<<<\n");
				printf("Elf %d     >>> waking up Santa!\n", id);

				elfWait = 0;
				//sem_post(&frontDesk);
			}
			//sem_wait(&santashelp);

			printf("Elf %d     >>> Entering Santa's Shop!\n", id);
			//sem_wait(&frontDesk);
			//sleep(1);
			printf("Elf %d     >>> Problem >>> bla bla bla!\n", id);
			//sem_wait(&frontDesk);
			//sleep(rand() % 4 + 1);
			printf("Santa      >>> Hey Elf %d.... >>> You fix the problem as follows: blah blah blah blah!\n", id);
			//elfWait--;
			sem_post(&frontDesk);
			printf("Elf %d     >>> Leaving Santa's Shop!\n", id);
		}
		else{
			sleep(rand() % 4 + 1);
		}
	}
	printf("Elf %d     >>> Terminated!\n", id);
	
	if(id == 11 || id == 23 || id == 5){
		sleep(9);
		if(elfWait > 0){
			for(i = 0; i < elfWait; i++){
				sem_post(&fullDesk);
			}

		}
	}
	pthread_exit(NULL);

}

void harness(){
	printf("Santa   >>> Harnessing all reindeer!\n");
	sleep(rand() % 4 + 1);
}

void spendTimeInTropics(int id)
{
	printf("Reindeer %d >>> going to the tropics!\n", id);
	sleep(rand() % 4 + 1);
}

void travelToNorthPole(int id)
{
	printf("Reindeer %d >>> going to the north pole\n", id);
	sleep(rand() % 4 + 1);
}

void shout()
{
	printf("Santa    >>> Ho Ho Ho Let's Distribute Toys!!!\n");
}

void deliverPresents(){
	printf("Santa    >>> Flying the sleigh!\n");
        sleep(rand() % 4 + 1);
        printf("Santa    >>> Finished delivering presents >>> Flying back to North Pole!\n");

}

void thanks(){
	printf("Santa    >>> Thanks for all your help Reindeer and elves!!!\n");
	printf("---------------------------------------------------------------------------------------------------\n");
}

