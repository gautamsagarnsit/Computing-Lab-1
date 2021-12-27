// Compiling: gcc -o part_1 21CS60R15_ass2_task_1.c
// Testing: ./part_1 sample_input_file_1.txt
#include<stdio.h>
void display(float *graph,int n) //Function to Display the graph in matrix form
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
void dijkstra(float *graph,float *dist,int source,int n,int m) //Implementation of Dijkstra's Algo
{
    int i,visited[n],min_idx=-1,j;    
    float min=-1;
    for(i=0;i<n;i++) //Initialisation
    {
        visited[i]=0;
        dist[i]=*((graph+source*n)+i);
        
    }
  
    dist[source]=0;
    for(i=0;i<n;i++) //This loops find the vertex with minimum distance
    {
        for(j=0;j<n;j++)
        {
            if((min>0)&&(visited[j]==0))
            { //Checking for shortest distance node
                if((dist[j]>0)&&(dist[j]<min))
                {
                    min=dist[j];
                    min_idx=j;                 
                    
                }
            }
            else if((min<0)&&(dist[j]>0)&&(visited[j]==0))
            {
              // This else condition runs only for one time when the program encounters an edge for the first time
                min=dist[j];
                min_idx=j;
               
            }

            
        }
        
        visited[min_idx]=1; //min_idx stores the index for nearest node found.
        for(j=0;j<n;j++) // For each nearest node all its neighbour will be examined.
        {
            if((visited[j]==0)&&(*((graph+min_idx*n)+j)>0))
            {
                min=dist[min_idx]+*((graph+min_idx*n)+j);
                
                if((min<dist[j]) || (dist[j]<0))
                {
                    dist[j]=min; //Assigning shortest distance to a node found till now
                 
                }
            }            
        }
        min=-1;   
    }

   
}
int extract_min(float *dist,int n) //Function for extracting shortest node
{
    int i,min_idx;
    float min=-1;
    for(i=0;i<n;i++)
    {
        if((min>0) && (dist[i]>0))
        {
            if(dist[i]<min)
            {
                min=dist[i];
                min_idx=i;
            }
        }
        else if(dist[i]>0)
        {
            min=dist[i];
            min_idx=i;
        }
    }
    dist[min_idx]=-1; //Deleting the shortest node.
    return min_idx;
}
int main(int argc,char **argv)
{
    int i,j,n,m,source,k,row,col,temp;
    FILE *fpt;
    char file[25];
    if(argc==2)
    {
        fpt=fopen(argv[1],"r");
    }
    else{
        scanf(" %s",file);
        fpt=fopen(file,"r");
        //fpt=fopen("sample_part1_1.txt","r");
    }
    if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;     
        
    }
    
    fscanf(fpt,"%d,%d,%d,%d",&n,&m,&source,&k);
    source--;
  
    float graph[n][n],w_temp,dist[n];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            graph[i][j]=-1;
        }
    }
    for(i=0;i<m;i++)
    {
        fscanf(fpt,"%d,%d,%f",&row,&col,&w_temp);
        row--;
        col--;
        graph[row][col]=w_temp;
        graph[col][row]=w_temp;
    }
    
    dijkstra((float *)graph,dist,source,n,m);
    //display((float *)graph,n); //Uncomment this this display Graph matrix with shortest distances
    //printf("Possible Targets\n");
    for(i=0;i<k;i++)
    {
        temp=extract_min(dist,n);
        if(temp!=-1)
        {
            printf("%d\n",temp+1);
        }        
    }
    fclose(fpt);
    return 0;
    
       
}