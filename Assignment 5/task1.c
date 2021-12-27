#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/resource.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
long get_mem_usage()
{
    struct rusage myusage;
    printf("Getting memory usage...\n");
    getrusage(RUSAGE_SELF,&myusage);
    printf("Integral Shared Memory Size: %ld\n",myusage.ru_ixrss);
    printf("Integral Unshared Data Size: %ld\n",myusage.ru_idrss);
    printf("Integral Unshared Stack Size: %ld\n",myusage.ru_isrss);
}
const int row1=10,col1=10,col2=10;
void cpu_util(int pid)
{
    char str[100];
    char d[2]=" ";
    char* token;
    int i = 0;
    long int sum = 0, idle, lastSum = 0,lastIdle = 0;
    long double idleFraction;
    char filename[20];
    sprintf(filename,"/proc/%d/stat",pid);
    FILE* fp = fopen(filename,"r");
    fgets(str,100,fp);
    fclose(fp);
    token = strtok(str,d);
    sum = 0;
    while(token!=NULL){
        token = strtok(NULL,d);
        if(token!=NULL){
            sum += atoi(token);

        if(i==3)
            idle = atoi(token);

        i++;
            }
        }
        idleFraction = 100 - (idle-lastIdle)*100.0/(sum-lastSum);
        printf("CPU UTLIZATION : %.2Lf%%\n", idleFraction);
}
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
	int i,x = 10,fd[2] ;
	pid_t pid,pid1, pid2, pid3;
    pipe(fd);
    if ((pid1 = fork()) == 0) 
    {
		/* First child process */
		for(i=0;i<5;i++) 
        {
            matmul(M,N,row1,col1,col2);
            
            //sleep(1);
            //printf("Memory Usage for process %d\n",pid1);
            //get_mem_usage();
            
        }
       
	}
	else {


		/* Create another child process */
		if ((pid2 = fork()) == 0) {

		    /* Second child process */
            for(i=0;i<5;i++) 
            {
               matmul(M,N,row1,col1,col2);
               //sleep(1);
               //printf("Memory Usage for process %d\n",pid2);
               //get_mem_usage();
              
            }
          
		}
		else 
        {
            if ((pid3 = fork()) == 0)
            {
                /* Third child process */
                for(i=0;i<5;i++) 
                {
                    matmul(M,N,row1,col1,col2);
                   // sleep(1);
                    //printf("Memory Usage for process %d\n",pid3);
                    //get_mem_usage();
                }
               
		    }
            else
            { 
                int flag=1;
                while(flag)
                {
                    if (fork() == 0) //Child Process M
                    { 
                        close(1);
                        dup(fd[1]);
                        close(fd[0]);
                        char *const parmList[] = {"ps","-o","pid,ppid,time",pid1,NULL};
                        execvp("/bin/ps", parmList);

                    }
                    else
                    {
                        close(0) ;
                        dup(fd[0]) ;
                        close(fd[1]);
                        char *n;
                        for (i=0; i < 10; i++)
                        {
                            scanf(" %s",n); 
                            printf("n = %s\n",n);
                        }
                    }
                    sleep(10);
                }
                

            }			
		}
	}
    return 0;
}
