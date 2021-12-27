// Compiling: gcc -o part_3 21CS60R15_ass2_task_3.c
// Testing: ./part_3 sample_input_file_4.txt
#include<stdio.h>
#include<float.h>

#define INF FLT_MAX
void display(float *graph,int n) //same as in part 1
{
    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            printf("%.2f\t",*((graph+i*n)+j));
        }
        printf("\n");
    }
}
int path(int *next,int *path,int u,int v,int n)
{ //This function generates the shortest path between two given nodes and return the length/number of edges between them
    int i=0;
    
    if(*((next+u*n)+v)==-1) //If no path then return
    {
        return -1;
    }
    
    
    if(u==v)
    {
       //if same node then distance =0
        return 0;
    }
    
    while(u!=v)
    {
        //check for next node in the path
        u=*((next+u*n)+v);
        path[i++]=u;        
    }
    
    return i-1;
}
int main(int argc,char **argv)
{
    int i,j,n,m,k,row,col,length,o;
    FILE *fpt;
    char file[25];
    if(argc==2)
    {
        fpt=fopen(argv[1],"r");
    }
    else{
        scanf(" %s",file);
        fpt=fopen(file,"r");
        //fpt=fopen("sample.txt","r");
    }
    if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;     
        
    }
    
    fscanf(fpt,"%d,%d",&n,&m);
   
   
    float graph[n][n],w_temp;
    int next[n][n], path_idx[n];
    int infection[n];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if(i==j)
            {
                graph[i][j]=0;
                next[i][j]=j;
            }
            else{
                graph[i][j]=INF;
                next[i][j]=-1;
            }
            
            
        }
    }
    
    for(i=0;i<m;i++)
    {
        fscanf(fpt,"%d,%d,%f",&row,&col,&w_temp);
        row--;
        col--;
        graph[row][col]=w_temp;
        graph[col][row]=w_temp;
        next[row][col]=col;
        next[col][row]=row;
       
    }
    
    
    for(i=0;i<n;i++) //Implementation of floyd-warshall algorithm
    {
        for(j=0;j<n;j++)
        {
            for(k=0;k<n;k++)
            {
                
                if(graph[j][k]>(graph[j][i]+graph[i][k]))
                {
                   graph[j][k]=graph[j][i]+graph[i][k];
                   next[j][k]=next[j][i];
                   
                }
            
            }
        }
    }

    

    for(i=0;i<n;i++) 
    {
        for(j=0;j<n;j++)
        {
            if(graph[i][j]==INF)
            {
                graph[i][j]=-1; //Indicating no path between the nodes i and j
                next[i][j]=-1;
            }
        }
    }
    
    
    

    //Uncomment below 2 lines to display graph
    //printf("ALL pair shortest paths\n");
    //display((float *)graph,n);




    fscanf(fpt,"%d",&o);
    float infection_time[n],time;
    for(i=0;i<n;i++)
    {
        infection_time[i]=-1;
    }
    for(i=0;i<o;i++)
    {
        fscanf(fpt,"%d,%f",&j,&time);
        j--;
        infection_time[j]=time;
    }

    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if((infection_time[i]<infection_time[j]) && (infection_time[i]>=0))
            {
               length= path((int *)next,(int *)path_idx,i,j,n); ///Compute path between two infected node
               
               for(k=0;k<length;k++)
               {
                   
                   if(infection_time[path_idx[k]]<0)
                   {

                       infection[path_idx[k]]=1; //Mark the possible infection node.
                       //For printing a separate loop is needed as printing here would generate duplicates in the ouput
                      
                   }
                   else
                   {
                       infection[path_idx[k]]=-1;
                   }
               }
            }
        }
    } 
    //printf("Possible infection recommendation\n");
    for(i=0;i<n;i++)
    {
        if(infection[i]==1)
        {
            printf("%d\n",i+1);
        }
    }


    fclose(fpt); 
    return 0;
    
       
}