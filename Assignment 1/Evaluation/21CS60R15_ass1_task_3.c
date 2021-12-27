// Compiling: gcc -o task_3 kd_tree.c -lm
#include<stdio.h>
#include<math.h>
#include<stdlib.h>

typedef struct 
{
    float corr[2];
    float min;
    int tower_idx;
}point;

typedef struct node{
    point p;
    struct node *left;
    struct node *right;
}node;

point median(point *X,int m,int k,int axis)
{
    point left[m],right[m];
    int left_len=0,right_len=0,i;
    
    //printf("K=%d\tCurrent Element = (%0.2f,%0.2f)\n",k,X[k-1].corr[0],X[k-1].corr[1]);
    if(m==1)
    {
        //printf("Median = (%f,%f) and axis = %d\n",X[0].corr[0],X[0].corr[1],axis);
        return X[0];
    }   
    else
    {
        for(i=0;i<m;i++)
        {
            if(i!=(k-1))
            {
            //printf("i=%d k-1=%d\n",i,k-1);
            if(X[i].corr[axis]<X[k-1].corr[axis])
            {
                left[left_len].corr[0]=X[i].corr[0];
                left[left_len++].corr[1]=X[i].corr[1]; 
                //printf("Left Set Append: (%f,%f)\n",left[left_len-1].corr[0],left[left_len-1].corr[1]);           
            } 
            else
            {
                right[right_len].corr[0]=X[i].corr[0];
                right[right_len++].corr[1]=X[i].corr[1];   
                //printf("Right Set Append: (%f,%f)\n",right[right_len-1].corr[0],right[right_len-1].corr[1]);         
            }   
            }        

        }
    }

    if(left_len==(k-1))
    {
        //printf("Median = (%f,%f) and axis = %d\n",X[k-1].corr[0],X[k-1].corr[1],axis);
        return X[k-1];
    }
    else if(left_len<(k-1))
    {
        //printf("Searching in right set\n");
        return median(right,right_len,(k-left_len-1),axis); 
        
    }
    else
    {
        //printf("Searching in left set\n");
        return median(left,left_len,k,axis);
    }
}
node *new(float x_point,float y_point)
{
    node *temp=(node *)malloc(sizeof(node));
    temp->p.corr[0]=x_point;
    temp->p.corr[1]=y_point;
    temp->left=NULL;
    temp->right=NULL;
    return temp;
}

node *kdtree(point *X,int depth,int m)
{
    if(m==0)
    {
        return NULL;
    }
    int i,k,left_len=0,right_len=0;
    int axis=depth%2;
    k=(int)ceil(m/(float)2);
    point left_arr[k],right_arr[k];
    point med=median(X,m,k,axis);
    node *root=new(med.corr[0],med.corr[1]);
    for(i=0;i<m;i++)
    {
        if(X[i].corr[axis]<med.corr[axis])
        {
            left_arr[left_len].tower_idx=X[i].tower_idx;
           // printf("Left Tower idx = %d\n",left_arr[left_len].tower_idx);
            left_arr[left_len].corr[0]=X[i].corr[0];
            left_arr[left_len++].corr[1]=X[i].corr[1];
        }
        else if(X[i].corr[axis]>med.corr[axis])
        {
            right_arr[right_len].tower_idx=X[i].tower_idx;
            //printf("Right Tower idx = %d\n",right_arr[right_len].tower_idx);
            right_arr[right_len].corr[0]=X[i].corr[0];
            right_arr[right_len++].corr[1]=X[i].corr[1];
        }
        else{
            root->p.tower_idx=X[i].tower_idx;
        }
    }
    root->left=kdtree(left_arr,depth++,left_len);
    root->right=kdtree(right_arr,depth++,right_len);
    return root;
}

float distance(point p1,point p2)
{
    return sqrt((float)pow((p2.corr[0]-p1.corr[0]),2)+(float)pow((p2.corr[1]-p1.corr[1]),2));
}

node *search_nearest(node *root,point P,int m, int depth)
{
    if(root==NULL)
    {
        //printf("NULL reached\n");
        return NULL;
    }
   // printf("Searching with root = (%0.2f,%0.2f)\n",root->p.corr[0],root->p.corr[1]);

    int k;
    float temp_dist;
    P.min=distance(P,root->p);
    //printf("Distance between (%0.2f,%0.2f) and (%0.2f,%0.2f) = %f\n",P.corr[0],P.corr[1],root->p.corr[0],root->p.corr[1],P.min);
    node *min_tower=root;
    k=(int)ceil(m/(float)2);
    
    if(P.corr[depth%k]>root->p.corr[depth%k])
    {
        node *temp=search_nearest(root->right,P,m,depth++);
        if(temp!=NULL)
        {
        temp_dist=distance(P,temp->p);
        if(temp_dist<P.min)
        {
            P.min=temp_dist;
            min_tower=temp;
        }
        // Now checking if boundary of other partition intersects the circle made with centre at P and radius = P.min
        temp_dist=P.corr[depth%k]-root->p.corr[depth%k];
        temp_dist*=(temp_dist>=0)?1:-1;

        if(P.min>temp_dist)
        {
            temp=search_nearest(root->left,P,m,depth++);
            if(temp!=NULL)
            {
            temp_dist=distance(P,temp->p);
            if(temp_dist<P.min)
            {
                P.min=temp_dist;
                min_tower=temp;
            }
            }
        }
        }
    }
    else{
        node *temp=search_nearest(root->left,P,m,depth++);
        if(temp!=NULL)
        {
        temp_dist=distance(P,temp->p);
        if(temp_dist<P.min)
        {
            P.min=temp_dist;
            min_tower=temp;
        }
        // Now checking if boundary of other partition intersects the circle made with centre at P and radius = P.min
        temp_dist=P.corr[depth%k]-root->p.corr[depth%k];
        temp_dist*=(temp_dist>=0)?1:-1;

        if(P.min>temp_dist)
        {
            temp=search_nearest(root->right,P,m,depth++);
            if(temp!=NULL)
            {
            temp_dist=distance(P,temp->p);
            if(temp_dist<P.min)
            {
                P.min=temp_dist;
                min_tower=temp;
            }
            }
        }
        }
    }

    
    return min_tower;
}


void display(node *root)
{
    if(root!=NULL)
    {
        display(root->left);
        printf("%d: (%0.2f,%0.2f)\n",root->p.tower_idx,root->p.corr[0],root->p.corr[1]);
        display(root->right);
    }
    else
    {
        return;
    }
}
int main(int argc,char **argv)
{
    

    FILE *fpt;
    char file[25];
    int m,n,i;
    if(argc==2)
     {
         fpt=fopen(argv[1],"r");
     }
     else{
         scanf(" %s",file);
         fpt=fopen(file,"r");
         //fpt=fopen("sample_input_file_1.txt","r");
     }
     if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;     
        
    }
    fscanf(fpt,"%d",&m);
    //printf("%d\n",m);
    point towers[m];
    
    for(i=0;i<m;i++)
    {
        fscanf(fpt,"%f,%f",&towers[i].corr[0],&towers[i].corr[1]);
        towers[i].tower_idx=i;
       // printf("(%0.2f,%0.2f)\n",towers[i].corr[0],towers[i].corr[1]);
    }

    fscanf(fpt,"%d",&n);
    //printf("%d\n",n);
    point mobiles[n];
    

    for(i=0;i<n;i++)
    {
        fscanf(fpt,"%f,%f",&mobiles[i].corr[0],&mobiles[i].corr[1]);
       // printf("(%0.2f,%0.2f)\n",mobiles[i].corr[0],mobiles[i].corr[1]);
    }
    //int k;
    //k=(int)ceil((n/(float)2));
    //printf("Calling Median with k=%d\n",k);
    /*point temp=median(mobiles,n,k,0);
    printf("Median fom axis 0 = (%f,%f)\n",temp.corr[0],temp.corr[1]);
    temp=median(mobiles,n,k,1);
    printf("Median from axis 1 = (%f,%f)\n",temp.corr[0],temp.corr[1]);*/
    node *root=kdtree(towers,0,m);
    node *min_tower;
    //printf("Display KD TREE\n");
    //display(root);
    //printf("Nearest Towers:\n");
    for(i=0;i<n;i++)
    {
        min_tower=search_nearest(root,mobiles[i],n,0);
        //printf("Nearest Tower to (%0.2f,%0.2f) is ",mobiles[i].corr[0],mobiles[i].corr[1]);
        //printf("(%0.2f,%0.2f)\n",min_tower->p.corr[0],min_tower->p.corr[1]);
        printf("[%d] --> [%d]\n",i+1,min_tower->p.tower_idx+1);
    }
    

    fclose(fpt);
    return 0;  
}