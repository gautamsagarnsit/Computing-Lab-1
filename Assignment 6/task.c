#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include<time.h>
#include<math.h>

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
int normal_sampling(int mean) // Using Box-Muller Transform to sample from standard normal
{
    srand(10+mean+delay_2);
    double u1=(double)rand()/(double)RAND_MAX;
    double u2=(double)rand()/(double)RAND_MAX;
    srand(u1+u2);
    u1=(double)rand()/(double)RAND_MAX;
    srand(u1+u2);
    u2=(double)rand()/(double)RAND_MAX;  
    if(delay_2>0)
    {
        u2=(double)delay_2;
    }
    int delay=(int)(sqrt(-2*log(u1))*cos(2*3.14*u2))+mean;//Shifting Mean
    delay_2=(int)(sqrt(-2*log(u2))*cos(2*3.14*u1))+mean;//Shifting Mean
    return delay;
}
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
    Lift_info *current_lift=(Lift_info *)malloc(sizeof(Lift_info));
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
    Lift_info *temp_lift,*temp_lift_1,*lift;
    Floor_info *temp_floor;
   
    for(i=0;i<n_lifts+1;i++)
    {
        
        if(fork()==0)
        {
            /* Lift Initialization*/
            lift=(Lift_info *)shmat(shmid2, 0, 0);            
            //Lift_info *lift=(Lift_info *)malloc(sizeof(Lift_info));
            int people_waiting,temp=0;
            lift[i].no=i+1;
            /*
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
                lift->stopsem[j]=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
                semctl(lift->stopsem[j], 0, SETVAL, 0);
                lift->close=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
                semctl(lift->close, 0, SETVAL, 0);
            }*/
            /*lift->position=all_lifts[lift->no-1].position;
            lift->direction=all_lifts[lift->no-1].direction; 
            for(j=0;j<NFLOORS;j++)
            {             
                lift->stops[j]=all_lifts[lift->no-1].stops[j];  
                lift->stopsem[j]=all_lifts[lift->no-1].stopsem[j];
                semctl(lift->stopsem[j], 0, SETVAL, 0);
                
            }
            lift->peopleinlift=all_lifts[lift->no-1].peopleinlift;
            lift->close=all_lifts[lift->no-1].close;   
            semctl(lift->close, 0, SETVAL, 0);*/

            lift[i].position=all_lifts[lift[i].no-1].position;
            lift[i].direction=all_lifts[lift[i].no-1].direction; 
            for(j=0;j<NFLOORS;j++)
            {             
                lift[i].stops[j]=all_lifts[lift[i].no-1].stops[j];  
                lift[i].stopsem[j]=all_lifts[lift[i].no-1].stopsem[j];
                semctl(lift->stopsem[j], 0, SETVAL, 0);
                
            }
            lift[i].peopleinlift=all_lifts[lift[i].no-1].peopleinlift;
            lift[i].close=all_lifts[lift[i].no-1].close;   
            semctl(lift->close, 0, SETVAL, 0);

                     
            temp_lift=(Lift_info *)shmat(shmid1, 0, 0);            
            temp_floor=(Floor_info *)shmat(shmid, 0, 0);   
            //temp_lift_1= (Lift_info *)shmat(shmid2, 0, 0);          
            /*Starting Lift Operation*/
            while(1)
            {     
                sleep(normal_sampling(1));
                /*lift->peopleinlift=temp_lift_1[lift->no-1].peopleinlift;
                lift->close=temp_lift_1[lift->no-1].close;   
                semctl(lift->close, 0, SETVAL, 0);
                for(j=0;j<NFLOORS;j++)
                {             
                    lift->stops[j]=temp_lift_1[lift->no-1].stops[j];  
                    lift->stopsem[j]=temp_lift_1[lift->no-1].stopsem[j];             
                }    */      
                printf("\t\t\t\t\t\t\t\tLift Process: Lift: %d Floor %d:People in lift  = %d, To Deboarded = %d, Boarded =%d\n",lift->no,lift->position,lift->peopleinlift,lift->stops[lift->position],temp);                
               
                /*Drop off all passengers waiting to get out at this floor*/
                /* Dropping Passengers*/
                lift->peopleinlift-=lift->stops[lift->position];
                lift->stops[lift->position]=0;
                current_lift=lift;
                semctl(current_lift->stopsem[lift->position], 0, SETVAL, 0);
                V(current_lift->stopsem[lift->position]);  // Signal to passengers to de-board                
                sleep(normal_sampling(1));
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
                    printf("\t\t\t\t\t\t\t\tLift Process: Lift %d:People Waiting to Board: %d\n",lift->no,temp);                 
                    lift->peopleinlift+=temp_floor[lift->position].waitingtogoup; 
                    sleep(normal_sampling(1));                                
                    temp_floor[lift->position].waitingtogoup=0;                                   
                    printf("\t\t\t\t\t\t\t\tLift Process: Gate opened for people going in UP Direction\n");
                    semctl(floors[lift->position].up_arrow, 0, SETVAL, 0);
                    temp_lift->no=lift->no;
                    temp_lift->position=lift->position;
                    temp_lift->stops[lift->position]=lift->stops[lift->position];
                    temp_lift->stopsem[lift->position]=lift->stopsem[lift->position];
                    V(floors[lift->position].up_arrow); 
                    temp_lift=lift;                                
                }

                if(lift->direction==-1)
                {
                    /*pick up all passengers on this floor waiting to go down*/
                    temp=temp_floor[lift->position].waitingtogodown;
                    printf("\t\t\t\t\t\t\t\tLift Process: Lift %d:People Waiting to Board: %d\n",lift->no,temp);                                     
                    lift->peopleinlift+=temp_floor[lift->position].waitingtogodown;  
                    sleep(normal_sampling(1));                                  
                    temp_floor[lift->position].waitingtogodown=0;                                        
                    printf("\t\t\t\t\t\t\t\tLift Process: Gate opened for people going in DOWN Direction\n");
                    semctl(floors[lift->position].down_arrow, 0, SETVAL, 0);
                    temp_lift->no=lift->no;
                    temp_lift->position=lift->position;
                    temp_lift->stops[lift->position]=lift->stops[lift->position];
                    temp_lift->stopsem[lift->position]=lift->stopsem[lift->position];
                    V(floors[lift->position].down_arrow); 
                    temp_lift=lift;                         
                }                
                sleep(normal_sampling(2));                
                printf("\t\t\t\t\t\t\t\tLift Process: Boarding Complete in lift %d:People In lift: %d\n",temp_lift->no,temp_lift->peopleinlift);
                               
                lift->position+=lift->direction; //Move Lift
                printf("\t\t\t\t\t\t\t\tLift Process: Lift no. %d going in direction %d(1: UP, -1:DOWN) arrived at floor no. %d\n",lift->no, lift->direction,lift->position);
            
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
            temp_lift=(Lift_info *)shmat(shmid1, 0, 0);
            Lift_info *temp_lift=(Lift_info *)shmat(shmid1, 0, 0);  
            Lift_info *temp_lift_1=(Lift_info *)shmat(shmid2, 0, 0);
            Lift_info *local_lift;           
            temp_floor=(Floor_info *)shmat(shmid, 0, 0);         
            while(1)
            {  
                direction=(current_floor>destination)?-1:1;
                printf("Person's Process: Person %d waiting at Floor %d to go to floor %d\n",i,current_floor,destination);            
                if(direction==-1)
                {
                    temp_floor[current_floor].waitingtogodown++;
                    /* Now waiting for the lift*/                    
                    printf("Person's Process: Person %d waiting at Floor %d Pressed DOWN Button. Waiting ... \n",i,current_floor);
                    P(floors[current_floor].down_arrow);                    
                    printf("Person's Process: Lift going down Arrived at floor %d\n",current_floor);
                             
                }
                if(direction==1)
                {                   
                    temp_floor[current_floor].waitingtogoup++;
                    /* Now waiting for the lift*/                
                    printf("Person's Process: Person %d waiting at Floor %d Pressed UP Button. Waiting ... \n",i,current_floor);
                    P(floors[current_floor].up_arrow);                                          
                    printf("Person's Process: Lift going up Arrived at floor %d\n",current_floor);
                                  
                }    
                //temp_lift=current_lift;
                local_lift=temp_lift;
                printf("Person's Process: Lift No. %d is standing at floor %d\n",local_lift->no,current_floor);
                temp_lift_1->stops[destination]++;                                         
                sleep(normal_sampling(2));
                printf("Person's Process: Person %d boarded lift no. %d standing at %d and pressed button for floor no. %d\n",i,local_lift->no,local_lift->position,destination);                  
                P(local_lift->stopsem[destination]); 
                sleep(normal_sampling(1)); 
                /* Getting out of the Lift*/               
                printf("Person's Process: Person %d arrived at floor %d\n",i,destination);                
                current_floor=destination;   
                srand(555+current_floor);                 
                do
                {                    
                    destination=rand()%(NFLOORS);
                } while (current_floor==destination); 
                printf("Person's Process: Person %d starting new trip\n",i);                                          
            }     
        }
    }
    sleep(600);
    return 0;
}