#include<stdio.h>
#include<stdlib.h>
#include <sys/ipc.h>
#include<string.h>
#include<unistd.h>
int main()
{
    /*char pid="12";
    char *const parmList[] = {"ps","-au","12",NULL};
    execvp("ps", parmList);*/
    //execlp("cal","cal","2001",NULL);
    //char *command;
	
	//executing ls command
	/*strcpy(command, "ps -au| awk '{print $2 " " $3 " " $4 " " $10}' | column -t");
	system("dir");
    printf("Completed\n");*/
    char n[100],cal[3000];
    int flag=1,i,m_flag=1;  
    while(flag)
    {   
        int fd[2];
        pipe(fd);
        if (fork() == 0) 
        {            
            if(m_flag)
            {             
                close(1) ; 
                dup(fd[1]);
                close(fd[0]);
                char *const parmList[] = {"ps","-au",NULL};
                char cmd[20];
                strcpy(cmd,"echo \"START\"");
                system(cmd);
                strcpy(cmd,"ps -au");
                system(cmd);
                strcpy(cmd,"echo \"END\"");
                system(cmd);
                m_flag=0; 
            }
            //sleep(5);
            //execvp("ps", parmList);
            
            //printf("Calender of 2001\nCalender of 2001\nCalender of 2001\nCalender of 2001\n");
            //printf("%d\n",18071996);
            /*for (int i=0; i < 10; i++) {
                printf("%d\n",i);
            }*/
        
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
                char current_process[10],d[2]=" ";
                int idx=1;            
                while(strcmp(cal,"END")!=0)
                {
                    idx=1;
                    scanf(" %[^\n]",cal);
                    printf("%s\n",cal);
                    token = strtok(cal,d);
                    char pid_n[5];
                    sprintf(pid_n,"%d",12);
                    while(token!=NULL)
                    {
                        if((idx==2) && (strcmp(token,pid_n)==0))
                        {
                            printf("Process 12 is alive\n");                           
                            k++;
                        }
                        idx++;
                        token = strtok(NULL,d);
                    }       
                }
              
                if(k<1)
                {
                    flag=0;
                } 
               
            }  
            m_flag=1;    
        }
        sleep(3);
                    
    }
    return 0; 
}