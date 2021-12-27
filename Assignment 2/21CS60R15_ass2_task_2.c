// Compiling: gcc -o part_2 21CS60R15_ass2_task_2.c
// Testing: ./part_2 sample_input_file_3.txt
#include<stdio.h>
void display(float *graph,int n) //Same as in Part 1
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
void dijkstra(float *graph,float *dist,int source,int n,int m) //Same as in Part 1
{
    int i,visited[n],min_idx=-1,j;    
    float min=-1;
    for(i=0;i<n;i++)
    {
        visited[i]=0;
        dist[i]=*((graph+source*n)+i);
        
    }
   
    dist[source]=0;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if((min>0)&&(visited[j]==0))
            {
                if((dist[j]>0)&&(dist[j]<min))
                {
                    min=dist[j];
                    min_idx=j;
                    
                    
                }
            }
            else if((min<0)&&(dist[j]>0)&&(visited[j]==0))
            {
               
                min=dist[j];
                min_idx=j;
               
            }

            
        }
       
        visited[min_idx]=1;
        for(j=0;j<n;j++)
        {
            if((visited[j]==0)&&(*((graph+min_idx*n)+j)>0))
            {
                min=dist[min_idx]+*((graph+min_idx*n)+j);
               
                if((min<dist[j]) || (dist[j]<0))
                {
                    dist[j]=min;
                  
                }
            }            
        }
        min=-1;   
    }

  

}

int extract_min(float *dist,float *infection_time,int n,int target_idx)
{ //Extracting shortest node with infection time less than the target node
    int i,min_idx=-1;
    float min=-1;
    for(i=0;i<n;i++)
    {
        if((infection_time[i]<infection_time[target_idx]) && (infection_time[i]>=0))
        {//This if condition checks if current node is infected and has infection time less than the target node.
            if((min>0) && (dist[i]>0)) //Checking only for node with a path to the target node      
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
    }
    dist[min_idx]=-1;
    return min_idx;
}


int main(int argc,char **argv)
{
    int i,j,n,m,o,source,k,row,col,temp;
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


    fscanf(fpt,"%d",&o);
    
    float infection_time[o],time;
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
    
    //printf("Possible Sources\n");
    for(i=0;i<k;i++)
    {
        temp=extract_min(dist,infection_time,n,source);
        if(temp!=-1)
        {
            printf("%d\n",temp+1);
        }
    }
    fclose(fpt);
    return 0;
}