/*
1.  Compiling: gcc 21CS60R15_ass6_task_1.c -lm
    Run: ./a.out
2.  Maximum time is set to 600 seconds(in the last line) after this ctrl+c won't work to terminate the program. 
    Another File name as killprocess.c would be needed for this. Run killprocess.c to terminate the program.
3.  Please Adjust the number of lift according to your screen
4.  The flow of code is same as suggested in the assignment pdf file
5.

*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include<time.h>
#include<math.h>

#define NFLOORS 5 // Adjust number of floors here
struct sembuf pop, vop ;

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing
				   the V(s) operation */
int status;

int n_lifts=5,n_people=10; //No. of lifts and No. of persons 

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


void show_status(Lift_info *l,Floor_info *f)
{
    // Function to print the lift status
    // Number in the brackets show number of people

    P(status);    
    semctl(status, 0, SETVAL, 0);   
    system("clear");
    printf("\n");
    for(int i=0;i<30;i++)
    {
        printf("-");
    }
    printf("\n");
    
    for(int j=NFLOORS-1;j>=0;j--)
    {
        printf("Floor %d: ",j);
        
        for(int i=0;i<n_lifts;i++)
        {
            for(int k=0;k<10;k++)
            {
                printf("-");
            }
            if(l[i].position==j)
            {
                if(l[i].direction==-1)
                {
                    printf("IN(%d)-->",f[j].waitingtogodown); // Show the number of People boarding the lift
                }    
                 if(l[i].direction==1)
                {
                    printf("IN(%d)-->",f[j].waitingtogoup);// Show the number of People boarding the lift
                }             
                printf("|L%d(%d)|",l[i].no,l[i].peopleinlift);
                printf("-->OUT(%d)",l[i].stops[j]);// Show the number of People de-boarding
            }
            else
            {
                printf("|");
                for(int k=0;k<22;k++)
                {
                    printf(" ");
                } 
                printf("|");                           
            }
        }
        printf("\n\n");
    }    
    printf("\n");
    for(int i=0;i<30;i++)
    {
        printf("-");
    }
    printf("\n");
    sleep(2);
    V(status);
}

int main()
{   
    int i,j,a,b;
    pid_t l,p;
    time_t ltime;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = SEM_UNDO;
    pop.sem_op = -1 ; vop.sem_op = 1 ;

    status=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); // Semaphore for printing the lift status
    semctl(status, 0, SETVAL, 1);

    int shmid = shmget(IPC_PRIVATE, NFLOORS*sizeof(Floor_info), 0777|IPC_CREAT); // Shared Memory for floors
    int shmid1 = shmget(IPC_PRIVATE, sizeof(Lift_info),0777|IPC_CREAT); //Shared memory for one Lift. Will be used to pass current lift info to passengers boarding.
    int shmid2 = shmget(IPC_PRIVATE, n_lifts*sizeof(Lift_info),0777|IPC_CREAT); // Shared Memory for all lifts
    
    Lift_info *all_lifts=(Lift_info *)malloc(n_lifts*sizeof(Lift_info));
    Lift_info *current_lift=(Lift_info *)malloc(sizeof(Lift_info));
    Floor_info *floors=(Floor_info *)malloc((NFLOORS)*sizeof(Floor_info));   

    for(int k=0;k<NFLOORS;k++)
    { //Floor Initialization. Will be used in shared memory to update values related to floors
        floors[k].waitingtogodown=0;
        floors[k].waitingtogoup=0;
        floors[k].down_arrow=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        floors[k].up_arrow=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);        
        semctl(floors[k].down_arrow, 0, SETVAL, 0);
        semctl(floors[k].up_arrow, 0, SETVAL, 0);        
    }

    for(int i=0;i<n_lifts;i++)
    { //Lift initialization. Will be used in shared memory to update values related to lifts.
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
   
    for(i=0;i<n_lifts;i++)
    {
        if(fork()==0)
        {
            /* Lift Initialization*/
            lift=(Lift_info *)shmat(shmid2, 0, 0);   
            temp_lift=(Lift_info *)shmat(shmid1, 0, 0);            
            temp_floor=(Floor_info *)shmat(shmid, 0, 0); 
            if(i==0)
            {
                for(int k=0;k<NFLOORS;k++)
                {
                
                    temp_floor[k].waitingtogodown=0;
                    temp_floor[k].waitingtogoup=0;
                }
            }  
            int people_waiting,temp=0;
           
            lift[i].no=i+1; //Lift number starts from 1
            srand(555+i);
            lift[i].position=rand()%(NFLOORS); // Lift can be initialized at any floor
            j=rand()%2; // Randomly choosing the initial direction
            if(j==0)
            {
                lift[i].direction=-1;//For Down
            }
            if(j==1)
            {
                lift[i].direction=1;//For UP
            }
            lift[i].peopleinlift=0;
            
            for(j=0;j<NFLOORS;j++)
            {
                lift[i].stops[j]=0;
                lift[i].stopsem[j]=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); 
                semctl(lift[i].stopsem[j], 0, SETVAL, 0);
                lift[i].close=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
                semctl(lift[i].close, 0, SETVAL, 0);
            }
            /*Starting Lift Operation*/
           
            while(1)
            {     
                sleep(normal_sampling(2));
                show_status(lift,floors); 
               
                /*Drop off all passengers waiting to get out at this floor*/
                /* Dropping Passengers*/
                lift[i].peopleinlift-=lift[i].stops[lift[i].position];
                lift[i].stops[lift[i].position]=0;
                temp_lift->no=lift[i].no; // Sending Lift info to passengers
                temp_lift->position=lift[i].position;
                semctl(lift[i].stopsem[lift[i].position], 0, SETVAL, 0);
                V(lift[i].stopsem[lift[i].position]);  // Signal to passengers to de-board   
              
                sleep(normal_sampling(2));

                /* Now Boarding People*/               
                if(lift[i].position==NFLOORS-1) //Changing lift direction if lift is at top floor or at ground floor
                {
                    lift[i].direction=-1;
                }
                if(lift[i].position==0)
                {
                    lift[i].direction=1;
                }             

                if(lift[i].direction==1)
                {
                    /* pick up all passengers on this floor waiting to go UP*/
                    temp= temp_floor[lift[i].position].waitingtogoup;
                   
                    lift[i].peopleinlift+=temp_floor[lift[i].position].waitingtogoup; 
                    sleep(normal_sampling(2));                                
                    temp_floor[lift[i].position].waitingtogoup=0;                                   
                   
                    semctl(floors[lift[i].position].up_arrow, 0, SETVAL, 0);
                    temp_lift->no=lift[i].no; // Sending Lift info to passengers
                    temp_lift->position=lift[i].position;
                    V(floors[lift[i].position].up_arrow); // Signal to passenger to board
                                                  
                }

                if(lift[i].direction==-1)
                {
                    /*pick up all passengers on this floor waiting to go down*/
                    temp=temp_floor[lift[i].position].waitingtogodown;
                   
                    lift[i].peopleinlift+=temp_floor[lift[i].position].waitingtogodown;  
                    sleep(normal_sampling(2));                                  
                    temp_floor[lift[i].position].waitingtogodown=0;                                        
                   
                    semctl(floors[lift[i].position].down_arrow, 0, SETVAL, 0);
                    temp_lift->no=lift[i].no; // Sending Lift info to passengers
                    temp_lift->position=lift[i].position;                 
                    V(floors[lift[i].position].down_arrow); // Signal to passenger to board
                                         
                }                
                sleep(normal_sampling(2));              
                               
                lift[i].position+=lift[i].direction; //Move Lift
                
            }
        }
    }
    for(i=1;i<n_people+1;i++)
    {
        if(fork()==0)
        {
            
            srand(555*(i+1));
            int current_floor,destination,direction;
            do
            { // Randomly choosing current floor and destination
                current_floor=rand()%(NFLOORS);
                srand(555*(i+1)+current_floor);
                destination=rand()%(NFLOORS);
            } while (current_floor==destination);
            
            //Variables for shared memory
            temp_lift=(Lift_info *)shmat(shmid1, 0, 0);
            Lift_info *temp_lift_1=(Lift_info *)shmat(shmid2, 0, 0);
            Lift_info *local_lift;           
            temp_floor=(Floor_info *)shmat(shmid, 0, 0);  
                           
            while(1)
            {  
                direction=(current_floor>destination)?-1:1;
                
                if(direction==-1)
                {
                    temp_floor[current_floor].waitingtogodown++;
                    /* Now waiting for the lift*/                  
                    P(floors[current_floor].down_arrow);     
                    local_lift=temp_lift;//Passenger boarding complete. Passenger getting info of the lift.             
                }
                if(direction==1)
                {                   
                    temp_floor[current_floor].waitingtogoup++;
                    /* Now waiting for the lift*/               
                    P(floors[current_floor].up_arrow);                                          
                    local_lift=temp_lift;//Passenger Boarding Complete. Passenger getting info of the lift
                }    
                
                temp_lift_1[local_lift->no-1].stops[destination]++;   // Waiting for the stop                                   
                
                P(temp_lift_1[local_lift->no-1].stopsem[destination]); 
                do
                {
                    //This is to ensure person's process does move ahead of this point before lift reached its destination.
                }while(temp_lift_1[local_lift->no-1].position!=destination);
                sleep(normal_sampling(2)); 
                /* Getting out of the Lift*/               
               
                current_floor=destination;   
                srand(555+current_floor);                 
                do
                {           //Starting new journey         
                    destination=rand()%(NFLOORS);
                } while (current_floor==destination); 
              
            }     
        }
    }
    sleep(600);
    return 0;
}