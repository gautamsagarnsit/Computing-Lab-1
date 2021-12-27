#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>	
#include<stdlib.h>
#include <unistd.h>


#define P(s) semop(s, &pop, 1)  
#define V(s) semop(s, &vop, 1)  
#define NFLOORS 5
typedef struct 
{
    int waitingtogoup; /* the number of people waiting to go up */
    
    int up_arrow; /* people going up wait on this */
} Floor_info;
Floor_info *floors;


void main()
{
	int shmid,shmid1;
	Floor_info  *a, *b;
    int *c;
	int i,j=1;
    shmid1=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
    floors=(Floor_info *)malloc(NFLOORS*sizeof(Floor_info));
    struct sembuf pop, vop ;
    pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = SEM_UNDO;
	pop.sem_op = -1 ; vop.sem_op = 1 ;
    printf("Starting ...\n");
    int semid1=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
    semctl(semid1, 0, SETVAL, 1);
	shmid = shmget(IPC_PRIVATE, NFLOORS*sizeof(Floor_info), 0777|IPC_CREAT);
    printf("Got shared Memory segment\n");
    int sem[NFLOORS];
    for(i=0;i<NFLOORS;i++)
    {
        
        
        floors[i].up_arrow = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);	
        //sem[i]=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);	  
        printf("%d\n",i);  
       semctl(floors[i].up_arrow, 0, SETVAL, 0);
        //semctl(sem[i], 0, SETVAL, 0);
	   
    }
    printf("Aquired all semaphores\n");
    
    printf("Final Starting ...\n");
	
	if (fork() == 0) {

		/* Child Process::  Consumer */

		b = (Floor_info *) shmat(shmid, 0, 0);
        c=(int *)shmat(shmid1,0,0);
		while (1) {

			P(floors[4].up_arrow);
             b->waitingtogoup= b->waitingtogoup+ 1;
           // P(sem[4]);
            sleep(1);
            *c=*c+1;
			printf("\t\t\t\t First Process: %d \n",b->waitingtogoup);
            printf("\t\t\t\t First Process(shmid1): %d \n",*c);
			V(semid1);
		}

	}
	else {
        if(fork()==0)
        {
            /* Second Process:: Producer */

            a = (Floor_info *) shmat(shmid, 0, 0);
            //a[0] = 0; a[1] = 1; 
            a->waitingtogoup=0;
            c=(int *)shmat(shmid1,0,0);
            while (1) {	            

                P(semid1);
                a->waitingtogoup= a->waitingtogoup+ 1;
                sleep(1);		
                printf("Second Process: %d\n", a->waitingtogoup);
                *c=*c+1;
                printf("Second Process(shmid1): %d\n",*c);
                V(floors[3].up_arrow);
                //V(sem[4]);
            }
        }
        else
        {
            a = (Floor_info *) shmat(shmid, 0, 0);
            //a[0] = 0; a[1] = 1; 
            a->waitingtogoup=0;
            c=(int *)shmat(shmid1,0,0);
            *c=0;
            while (1) {	            

                P(floors[3].up_arrow);
                a->waitingtogoup= a->waitingtogoup+ 2;
                sleep(1);		
                printf("Parent Process: %d\n", a->waitingtogoup);	
                V(floors[4].up_arrow);
                //V(sem[4]);

                
            }
        }  
	}
   
   
}

