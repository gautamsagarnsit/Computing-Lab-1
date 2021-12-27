#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>

// Matrix multiplication is being done as a memory and CPU intensive application for all 3 child processes
const int row1=10,col1=10,col2=10;
void matmul(double M[row1][col1],double N[col1][col2],int row1,int col1,int col2)
{
   
    for(int i=0;i<row1;i++)
    {
        for(int k=0;k<col1;k++)
        {
            M[i][k]=(double)(rand()%10);
           
        }
    }
    for(int i=0;i<col1;i++)
    {
        for(int k=0;k<col2;k++)
        {
            N[i][k]=(double)(rand()%10);
           
        }
    }
    double temp[row1][col2],sum=0;
    for(int i=0;i<row1;i++)
    {
        for(int j=0;j<col2;j++)
        {
            sum=0;
            for(int k=0;k<col1;k++)
            {
                sum+=M[i][k]*N[k][j];
            }
            temp[i][j]=sum;
        }
    }

}
int main()
{
    double M[row1][col1],N[col1][col2];  
    pid_t pid,pid1,pid2,pid3;
    int i,j;
    time_t ltime;
    char n[100],cal[300],temp_cal[300];
    int flag=1,m_flag=1;  
    if ((pid1 = fork()) == 0) 
    {
		/* First child process */
		for(i=0;i<5;i++) 
        {   
            printf("First Child Process Running\n"); 
            for(i=0;i<5;i++) 
            {
                matmul(M,N,row1,col1,col2);
                sleep(1);
            }                
           
        }
       
	}
	else
    {
        if((pid2=fork())==0)
        {
            /* Second Child Process*/
            for(i=0;i<5;i++) 
            {   
                printf("Second Child Process Running\n");       
                for(i=0;i<5;i++) 
                {
                    matmul(M,N,row1,col1,col2);
                    sleep(1);
                }               
                
            }
        }
        else
        {
            if((pid3=fork())==0)
            {
                /*Third Child Process*/
                for(i=0;i<5;i++) 
                {   
                    printf("Third Child Process Running\n");       
                    for(i=0;i<5;i++) 
                    {
                        matmul(M,N,row1,col1,col2);
                        sleep(1);
                    }                    
                    
                }
            }
            else
            {
                /* Parent Process*/
                while(flag)
                {   
                    int fd[2];
                    pipe(fd);
                    if (pid=(fork()) == 0) 
                    {   
                        //m_flag = 1 indicates that the parent process is ready to read status of running processes         
                        if(m_flag)
                        {             
                            close(1) ; 
                            dup(fd[1]);
                            close(fd[0]);
                            char *const parmList[] = {"ps","-au",NULL}; // To use use exec function(Commented below)
                            char cmd[20];
                            strcpy(cmd,"echo \"START\"");// Tells parent to start reading
                            system(cmd);
                            strcpy(cmd,"ps -au");
                            system(cmd);
                            strcpy(cmd,"echo \"END\"");//Tells Parent to stop Reading
                            system(cmd);
                            //execvp("ps", parmList);
                            m_flag=0; 
                        }                   
                    }
                    else 
                    {
                       
                        m_flag=0;
                        close(0) ;
                        dup(fd[0]) ; 
                        close(fd[1]);
                        int k=0;
                        scanf(" %[^\n]",cal);
                        if(strcmp(cal,"START")==0)
                        {             
                            scanf(" %[^\n]",cal);
                            printf("%s\n",cal);	                           
                            
                            char* token;
                            char d[2]=" ";
                            int idx=1;            
                            while(strcmp(cal,"END")!=0)
                            {
                                idx=1;
                                scanf(" %[^\n]",cal);
                                strcpy(temp_cal,cal);
                                token = strtok(cal,d);
                                char pid_n1[5],pid_n2[5],pid_n3[5];
                                sprintf(pid_n1,"%d",pid1);
                                sprintf(pid_n2,"%d",pid2);
                                sprintf(pid_n2,"%d",pid3);
                                while(token!=NULL)
                                {
                                    if((idx==2) && ((strcmp(token,pid_n1)==0)|(strcmp(token,pid_n2)==0)|(strcmp(token,pid_n3)==0)))
                                    {
                                        //Print Time Stamp only when Running Chil Process is found.
                                        ltime=time(NULL);                                         
                                        printf("Child Process Found at\t");
                                        printf("%s",asctime(localtime(&ltime))); 
                                        printf("%s\n",temp_cal);                                                                                                          
                                        k++;
                                    }
                                    idx++;
                                    token = strtok(NULL,d);
                                }                              
                                       
                            }
                        
                            if(k<1) //  "All children have terminated"
                            {
                                flag=0;
                                printf("All children have terminated");
                            }                            
                        
                        }  
                        m_flag=1;    
                    }
                    sleep(6);
                                
                }
            }
                                
            
        }
    }

}