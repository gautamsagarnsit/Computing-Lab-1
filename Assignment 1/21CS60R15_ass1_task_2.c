// Compiling: gcc -o task_2 part_2.c -lm
#include<stdio.h>
#include<math.h>

typedef struct 
{
    float x;
    float y;
    float min;
    int tower_idx;
}point;


float distance(point p1,point p2) // Same as in Part 1
{
    return sqrt((float)pow((p2.x-p1.x),2)+(float)pow((p2.y-p1.y),2));
}

float search_nearest_naive(point *X,point *X_dash,int idx, int m)
{
    float min=-1,dist,min_x,min_y;
    int i; 
    //printf("Minimum distance of (%0.2f,%0.2f) from towers: ",X_dash[idx].x,X_dash[idx].y);
    for(i=0;i<m;i++)
    {
        dist=distance(X[i],X_dash[idx]);
       // printf("%0.2f\t",dist);
        if(min>=0)
        {
           
            if(min>dist)
            {
                min=dist;   
                X_dash[idx].tower_idx=i;
            }

        }
        else
        {
            min=dist;
            X_dash[idx].tower_idx=i;
        }
    }
    
    return min;
        
    
}

void incremental_search_nearest_tower(point *X,point *X_dash,int idx, int m,int n)
{
    int i,j;
    float dist;
    for(i=0;i<n;i++)
    {
        if(X_dash[i].tower_idx!=idx)
        {
            dist=distance(X_dash[i],X[idx]);
            if(dist<=X_dash[i].min)
            {
                X_dash[i].min=dist;
                X_dash[i].tower_idx=i;
            }
        }
        else
        {
            X_dash[i].min=search_nearest_naive(X,X_dash,i,m);     
        }
    }
}
 int main(int argc,char **argv)
 {
     FILE *fpt;
     char file[25]; //Input File name should be of maximum length = 25
     int m,m_new,n_new,n,i,temp_idx;
     float temp_x,temp_y;
    
     if(argc==2)
     {
         fpt=fopen(argv[1],"r");
     }
     else{
         scanf(" %s",file);
         fpt=fopen(file,"r");
     }
     
     
    
     if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;     
        
    }
   
    fscanf(fpt,"%d",&m);
    //printf("%d\n",m);
    point towers[m];
    
    for(i=0;i<m;i++) //Reading Tower locations
    {
        fscanf(fpt,"%f,%f",&towers[i].x,&towers[i].y);
        //printf("(%0.2f,%0.2f)\n",towers[i].x,towers[i].y);
    }

    fscanf(fpt,"%d",&n);
    //printf("%d\n",n);
    point mobiles[n];

    for(i=0;i<n;i++) // Reading Mobile Locations
    {
        fscanf(fpt,"%f,%f",&mobiles[i].x,&mobiles[i].y);
        //printf("(%0.2f,%0.2f)\n",mobiles[i].x,mobiles[i].y);
    }


    //printf("Initial Minimum Distances:\n");

    for(i=0;i<n;i++)
     {
         mobiles[i].min=search_nearest_naive(towers,mobiles,i,m);
         //printf("(%0.2f,%0.2f): %0.2f\n",towers[mobiles[i].tower_idx].x,towers[mobiles[i].tower_idx].y,mobiles[i].min);
     }


    fscanf(fpt,"%d",&m_new);
    //printf("%d\n",m_new);
    
    for(i=0;i<m_new;i++) // Reading New Tower locations
    {
        
        fscanf(fpt,"%d,%f,%f",&temp_idx,&temp_x,&temp_y);
        temp_idx--; //Comment this line if indices given are 0-indexed and are not 1-indexed
        towers[temp_idx].x=temp_x;
        towers[temp_idx].y=temp_y;       
        incremental_search_nearest_tower(towers,mobiles,temp_idx,m,n);
    }

    fscanf(fpt,"%d",&n_new);
    //printf("%d\n",n_new);
   

    for(i=0;i<n_new;i++) // Reading New Mobile locations
    {
        
        fscanf(fpt,"%d,%f,%f",&temp_idx,&temp_x,&temp_y);
        temp_idx--; //Comment this line if indices given are 0-indexed and are not 1-indexed
        mobiles[temp_idx].x=temp_x;
        mobiles[temp_idx].y=temp_y;
      
        mobiles[temp_idx].min=search_nearest_naive(towers,mobiles,temp_idx,m);
    }
    
    fclose(fpt);   

    //printf("Updated Minimum Distances:\n");

    for(i=0;i<n;i++)
     {
        // printf("(%0.2f,%0.2f)\n",towers[mobiles[i].tower_idx].x,towers[mobiles[i].tower_idx].y);
         printf("[%d] --> [%d]\n",i+1,mobiles[i].tower_idx+1);
     }  
    
     return 0;  
 }