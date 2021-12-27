
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
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1 ; vop.sem_op = 1 ;
    int shmid = shmget(IPC_PRIVATE, NFLOORS*sizeof(Floor_info),0644|IPC_CREAT);
    int shmid1 = shmget(IPC_PRIVATE, sizeof(Lift_info),0644|IPC_CREAT);
    current_lift=(Lift_info *)malloc(sizeof(Lift_info));
    floors=(Floor_info *)malloc((NFLOORS)*sizeof(Floor_info));
    for(int i=0;i<NFLOORS;i++)
    {
        floors[i].waitingtogodown=0;
        floors[i].waitingtogoup=0;
        floors[i].up_arrow=semget(IPC_PRIVATE, 1, 0644|IPC_CREAT);
        floors[i].down_arrow=semget(IPC_PRIVATE, 1, 0644|IPC_CREAT);
        semctl(floors[i].up_arrow, 0, SETVAL, 0);
        semctl(floors[i].down_arrow, 0, SETVAL, 0);
    }
    //Lift_info *Plift=(Lift_info *)malloc(sizeof(Lift_info));
         
    Lift_info *Plift=(Lift_info *)shmat(shmid1, NULL, 0);        
    Plift->no=0;   
    Plift->position=NFLOORS-1;//TOP Floor
    Plift->direction=1;//For UP
    Plift->peopleinlift=0;    
    for(j=0;j<NFLOORS;j++)
    {
        Plift->stops[j]=0;
        Plift->stopsem[j]=semget(IPC_PRIVATE, 1, 0644|IPC_CREAT);
        semctl(Plift->stopsem[j], 0, SETVAL, 0);
        Plift->close=semget(IPC_PRIVATE, 1, 0644|IPC_CREAT);
        semctl(Plift->close, 0, SETVAL, 0);
    }
    
   
    for(i=0;i<n_lifts;i++)
    {
        l=fork();
        if(l==0)
        {
            /* Lift Initialization*/
            Lift_info *lift=(Lift_info *)malloc(sizeof(Lift_info));
           
            int people_waiting,temp=0;
            lift->no=i;
            srand(555+i);
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
                lift->stopsem[j]=semget(IPC_PRIVATE, 1, 0644|IPC_CREAT);
                semctl(lift->stopsem[j], 0, SETVAL, 0);
                lift->close=semget(IPC_PRIVATE, 1, 0644|IPC_CREAT);
                semctl(lift->close, 0, SETVAL, 0);
            }
            Floor_info *temp_floor=(Floor_info *)shmat(shmid, 0, 0);
            Lift_info *temp_lift=(Lift_info *)shmat(shmid1, 0, 0);
            /*Starting Lift Operation*/
            while(1)
            {
                /* 1. drop off all passengers waiting to get out at this floor*/
                /* Dropping Passengers*/
                
                sleep(1);
                if(lift->no==0)
                {
                    printf("Lift Process: Lift: %d Floor %d:People in lift  = %d, To Deboarded = %d, Boarded =%d\n",lift->no,lift->position,lift->peopleinlift,lift->stops[lift->position],temp);                
                }
               
                lift->peopleinlift-=lift->stops[lift->position];
                lift->stops[lift->position]=0;
                semctl(lift->stopsem[lift->position], 0, SETVAL, 0);
                V(lift->stopsem[lift->position]);  // Signal to passengers to de-board
                sleep(2);
                /* Now Boarding People*/
               
                if(lift->position==NFLOORS-1)
                {
                    lift->direction=-1;
                }
                if(lift->position==0)
                {
                    lift->direction=1;
                }                

                if(lift->direction==1)
                {
                    /* pick up all passengers on this floor waiting to go UP*/
                    temp= temp_floor[lift->position].waitingtogoup;
                    if(lift->no==0)
                    {
                        printf("Lift Process: Lift %d:People Waiting to Board: %d\n",lift->no,temp);
                    }
                    
                     
                    lift->peopleinlift+=temp_floor[lift->position].waitingtogoup; 
                     sleep(1);
                       
                    temp_lift=lift;                    
                    temp_floor[lift->position].waitingtogoup=0;               
                    
                    printf("Lift Process: Gate opened for people going in UP Direction\n");
                    semctl(temp_floor[lift->position].up_arrow, 0, SETVAL, 0);
                    V(temp_floor[lift->position].up_arrow);                                 
                }

                if(lift->direction==-1)
                {
                    /*pick up all passengers on this floor waiting to go down*/
                    temp=temp_floor[lift->position].waitingtogodown;
                    if(lift->no==0)
                    {
                        printf("Lift Process: Lift %d:People Waiting to Board: %d\n",lift->no,temp);
                    }
                   
                    lift->peopleinlift+=temp_floor[lift->position].waitingtogodown;  
                    sleep(1); 
                                    
                    temp_lift=lift;
                    temp_floor[lift->position].waitingtogodown=0;                     
                    
                    
                    printf("Lift Process: Gate opened for people going in DOWN Direction\n");
                    semctl(temp_floor[lift->position].down_arrow, 0, SETVAL, 0);
                    V(temp_floor[lift->position].down_arrow);   
                        
                }
                
                sleep(2);
                
                if(lift->no==0)
                {

                    printf("Lift Process: Boarding Complete in lift %d:People In lift: %d\n",temp_lift->no,temp_lift->peopleinlift);
                }
               
                lift->position+=lift->direction; //Move Lift
                if(lift->no==0)
                {
                   
                    printf("Lift Process: Lift no. %d going in direction %d(1: UP, -1:DOWN) arrived at floor no. %d\n",lift->no, lift->direction,lift->position);
                }
               
            }
        }
    }
    for(i=0;i<n_people;i++)
    {
        p=fork();
        if(p==0)
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
            Floor_info *temp_floor=(Floor_info *)shmat(shmid, 0, 0);            
            
            while(1)
            {  
                direction=(current_floor>destination)?-1:1;
                printf("Person's Process: Person %d waiting at Floor %d to go to floor %d\n",i,current_floor,destination);
            
                if(direction==-1)
                {
                    temp_floor[current_floor].waitingtogodown++;
                    /* Now waiting for the lift*/
                    
                    printf("Person's Process: Person %d waiting at Floor %d Pressed DOWN Button. Waiting ... \n",i,current_floor);
                    printf("Semaphore value = %d\n",temp_floor[current_floor].down_arrow);                   
                    P(temp_floor[current_floor].down_arrow); 
                    printf("Person's Process: Lift Arrived\n");
                             
                }
                if(direction==1)
                {                   
                    temp_floor[current_floor].waitingtogoup++;
                    /* Now waiting for the lift*/                
                    printf("Person's Process: Person %d waiting at Floor %d Pressed UP Button. Waiting ... \n",i,current_floor);
                    printf("Semaphore value = %d\n",temp_floor[current_floor].up_arrow);
                    P(temp_floor[current_floor].up_arrow);  
                    printf("Person's Process: Lift Arrived\n");    
                                  
                }       
                //printf("Person's Process: Sending Signal to Close the door\n");   
                //printf("Person's Process: People In lift for floor %d before increment: %d\n",destination,temp_lift->stops[destination]);
                temp_lift->stops[destination]++; 
                //V(temp_lift->close);
                               
                sleep(2);
                                                       
                //printf("%s",asctime(localtime(&time(NULL))));
                if(temp_lift->no==0)
                {
                    
                    printf("Person's Process: Person %d boarded lift no. %d standing at %d and pressed button for floor no. %d\n",i,temp_lift->no,temp_lift->position,destination);    
                }
                //printf("Person %d boarded lift no. %d and pressed button for floor no. %d\n",i,temp_lift->no,destination);
                
                P(temp_lift->stopsem[destination]); 
                sleep(1); 
                /* Get out of the Lift*/
               
                printf("Person's Process: Person %d arrived at floor %d\n",i,destination);                
                current_floor=destination;   
                srand(555+current_floor);                 
                do
                {                    
                    destination=rand()%(NFLOORS);
                } while (current_floor==destination);   
                                        
            }     
        }
    }
    sleep(600);
    return 0;
}