// Compiling: gcc -o part_1 part_1.c -lm

#include<stdio.h>
#include<math.h>

typedef struct 
{
    float x;
    float y;
    float min;
    int tower_idx;

}point;

float distance(float x1,float y1,float x2, float y2)  // Returns the euclidean distance between two points.
{
    return sqrt((float)pow((x2-x1),2)+(float)pow((y2-y1),2));
}

float search_nearest(point *X,point *X_dash,int idx, int m) //Implements Nearest search Naive Algorithm
{
    float min=-1,dist,min_x,min_y;
    int i; 
  
    for(i=0;i<m;i++)
    {
        dist=distance(X[i].x,X[i].y,X_dash[idx].x,X_dash[idx].y);
       
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

 int main(int argc,char **argv)
 {
     FILE *fpt;
     char file[25];
     if(argc==2)
     {
         fpt=fopen(argv[1],"r");
     }
     else{
         scanf(" %s",file);
         fpt=fopen(file,"r");
     }
     
     int m,mobile_new_index,tower_new_index,n,i;
     
     if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;     
        
    }
   
    fscanf(fpt,"%d",&m);
  
    point towers[m];
    
    for(i=0;i<m;i++)
    {
    fscanf(fpt,"%f,%f",&towers[i].x,&towers[i].y);
  
    }
    fscanf(fpt,"%d",&n);
   
    point mobiles[n];
    for(i=0;i<n;i++)
    {
    fscanf(fpt,"%f,%f",&mobiles[i].x,&mobiles[i].y);
   
    }
    
    fclose(fpt);

    for(i=0;i<n;i++)
     {
         mobiles[i].min=search_nearest(towers,mobiles,i,m);
         //printf("(%0.2f,%0.2f)\n",mobiles[i].tower_x,mobiles[i].tower_y);
         printf("[%d] --> [%d]\n",i+1,mobiles[i].tower_idx+1);
     }
  
     return 0;
     
    
 }
 