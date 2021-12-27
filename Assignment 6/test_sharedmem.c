#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include<time.h>


#define NFLOORS 5
struct sembuf pop, vop ;
struct sembuf UP1 = {0, 1, 0};
struct sembuf DOWN1 = {0, -1, 0};
#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing
				   the V(s) operation */
int n_lifts=1,n_people=1;
typedef struct {
int waitingtogoup; /* the number of people waiting to go up */
int waitingtogodown; /* the number of people waiting to go down */
int up_arrow; /* people going up wait on this */
int down_arrow;/* people going down wait on this */
} Floor_info;
Floor_info *floors;
int delay_2=-1000;

typedef struct {
int no; /* which lift is it */
int position; /* which floor it is on */
int direction; /* going UP or DOWN */
int peopleinlift; /* number of people in the lift */
int stops[NFLOORS]; /* for each stop how many people are waiting to get off */
int stopsem[NFLOORS]; /* people in the lift wait on one of these */
int close;
} Lift_info;
Lift_info *current_lift;

int main()
{   
    int i,j,a,b;
    pid_t l,p;
    time_t ltime;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = SEM_UNDO;
    pop.sem_op = -1 ; vop.sem_op = 1 ;
    int shmid = shmget(IPC_PRIVATE, NFLOORS*sizeof(Floor_info), 0777|IPC_CREAT);
    int shmid1 = shmget(IPC_PRIVATE, sizeof(Lift_info),0777|IPC_CREAT);
    int shmid2 = shmget(IPC_PRIVATE, n_lifts*sizeof(Lift_info),0777|IPC_CREAT);
    Lift_info *all_lifts=(Lift_info *)malloc(n_lifts*sizeof(Lift_info));
    Floor_info *floors=(Floor_info *)malloc((NFLOORS)*sizeof(Floor_info));        
    for(int i=0;i<NFLOORS;i++)
    {
       // floors[i].waitingtogodown=i*NFLOORS+rand()%NFLOORS;
        //floors[i].waitingtogoup=(i+5)*NFLOORS+rand()%NFLOORS;
        floors[i].waitingtogodown=0;
        floors[i].waitingtogoup=0;
        floors[i].up_arrow=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        floors[i].down_arrow=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        semctl(floors[i].up_arrow, 0, SETVAL, 0);
        semctl(floors[i].down_arrow, 0, SETVAL, 0);
    }
    for(int i=0;i<n_lifts;i++)
    {
        all_lifts[i].no=i+1;        
        srand(555+i);
        all_lifts[i].position=rand()%(NFLOORS);
        j=rand()%2;
        if(j==0)
        {
            all_lifts[i].direction=-1;//For Down
        }
        if(j==1)
        {
            all_lifts[i].direction=1;//For UP
        }
        all_lifts[i].peopleinlift=0;
        
        for(j=0;j<NFLOORS;j++)
        {
            all_lifts[i].stops[j]=0;
            all_lifts[i].stopsem[j]=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
            semctl(all_lifts[i].stopsem[j], 0, SETVAL, 0);
            all_lifts[i].close=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
            semctl(all_lifts[i].close, 0, SETVAL, 0);
        }
    }
    Lift_info *temp_lift,*temp_lift_1;
    Floor_info *temp_floor;
    printf("Starting the program\n");
    for(i=1;i<n_lifts+1;i++)
    {
        
        if(fork()==0)
        {
            /* Lift Initialization*/

            Lift_info *lift=(Lift_info *)malloc(sizeof(Lift_info));
            int people_waiting,temp=0;
            lift->no=i;
            /*srand(555+i);
            lift->position=rand()%(NFLOORS);
            j=rand()%2;
            if(j==0)
            {
                lift->direction=-1;//For Down
            }
            if(j==1)
            {
                lift->direction=1;//For UP
            }
            lift->peopleinlift=0;
            
            for(j=0;j<NFLOORS;j++)
            {
                lift->stops[j]=0;
                lift->stopsem[j]=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
                semctl(lift->stopsem[j], 0, SETVAL, 0);
                lift->close=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
                semctl(lift->close, 0, SETVAL, 0);
            }*/
            
            lift->position=all_lifts[lift->no-1].position;
            lift->direction=all_lifts[lift->no-1].direction; 
            for(j=0;j<NFLOORS;j++)
            {             
                lift->stops[j]=all_lifts[lift->no-1].stops[j];  
                lift->stopsem[j]=all_lifts[lift->no-1].stopsem[j];
                semctl(lift->stopsem[j], 0, SETVAL, 0);
                
            }
            lift->peopleinlift=all_lifts[lift->no-1].peopleinlift;
            lift->close=all_lifts[lift->no-1].close;   
            semctl(lift->close, 0, SETVAL, 0);
                     
            temp_lift=(Lift_info *)shmat(shmid1, 0, 0);            
            temp_floor=(Floor_info *)shmat(shmid, 0, 0);   
            temp_lift_1= (Lift_info *)shmat(shmid2, 0, 0);   
            printf("Starting Lift Process\n");
            while(1)
            { 
                
                
                lift->peopleinlift=temp_lift_1[lift->no-1].peopleinlift;
                lift->close=temp_lift_1[lift->no-1].close;   
                semctl(lift->close, 0, SETVAL, 0);   

                temp_floor[0].waitingtogodown++;
                temp_lift->no=lift->no;
                temp_lift->stops[0]=lift->stops[0];
                temp_lift->stopsem[0]=lift->stopsem[0];
                temp_lift->stopsem[1]=lift->stopsem[1];                
                
                printf("\t\t\t\t\t\t\t\tLift's Process: Lift No = %d\n",lift->no);
                printf("\t\t\t\t\t\t\t\tLift's Process: Lift stop = %d\n",lift->stops[0]);
                printf("\t\t\t\t\t\t\t\tLift's Process: Floor info= %d\n",temp_floor[0].waitingtogodown);
                lift->no++;
               
                sleep(2);
                V(temp_lift->stopsem[0]);
                
                for(j=0;j<NFLOORS;j++)
                {             
                    lift->stops[j]=temp_lift_1[lift->no-1].stops[j];  
                    lift->stopsem[j]=temp_lift_1[lift->no-1].stopsem[j];             
                }
                P(lift->stopsem[1]);
               

            }        
        }
    }
     for(i=1;i<n_people+1;i++)
    {
        if(fork()==0)
        {
            srand(555+i);
            int current_floor,destination,direction;
            do
            {
                current_floor=rand()%(NFLOORS);
                srand(555+current_floor);
                destination=rand()%(NFLOORS);
            } while (current_floor==destination);
            Lift_info *temp_lift=(Lift_info *)shmat(shmid1, 0, 0);  
            Lift_info *temp_lift_1=(Lift_info *)shmat(shmid2, 0, 0);
            Lift_info *local_lift;           
            temp_floor=(Floor_info *)shmat(shmid, 0, 0);  
            
            printf("Starting Person Process\n");
            while(1)
            {
                local_lift=temp_lift;
                P((*local_lift).stopsem[0]);
                (*local_lift).stops[0]++;
                temp_lift_1[local_lift->no].stops[0]=local_lift->stops[0];
                printf("Person's Process: Lift No = %d\n",(*local_lift).no);
                printf("Person's Process: Lift stop = %d\n",(*local_lift).stops[0]);
                printf("Person's Process: Floor info= %d\n",temp_floor[0].waitingtogodown);  

                sleep(2);
                V((*temp_lift_1).stopsem[1]);
            }        
        }
        
    }
    Lift_info *lift=(Lift_info *)shmat(shmid1, 0, 0);    
    lift->stops[0]=0;
    V(lift->stops[0]);
    lift->stops[0]=0;
    sleep(600);


}