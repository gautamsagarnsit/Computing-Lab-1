/*
Compiling: gcc -c 21CS60R15_ass4_task_1.c
Linking: gcc 21CS60R15_ass4_task_1.o -lglpk -lm
Running: ./a.out

*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<glpk.h>

int main(int argc,char **argv)
{   
    int n,m,N,p,i,j,row,col,temp;
    FILE *fpt;
    char file[25];
    if(argc==2)
    {
        fpt=fopen(argv[1],"r");
    }
    else{
        //scanf(" %s",file);
        //fpt=fopen(file,"r");
        fpt=fopen("testcase.txt","r");
    }
    if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;   
    }
    
    fscanf(fpt,"%d,%d",&n,&m);
   // printf("Number of Students = %d, Number of courses = %d\n",n,m);
    int ia[1+(m+n)*m*n], ja[1+(m+n)*m*n];
    double ar[1+(m+n)*m*n], z;
    int M[m],x[n][m];
    fscanf(fpt,"%d",&N);
    //printf("Max No. of course per student = %d\n",N);
    
    
    //printf("Max No. of students per course:\t");
   
    for(i=0;i<m;i++)
    {
        fscanf(fpt,"%d,",&M[i]);
        //printf("%d ",M[i]);
    }


    fscanf(fpt,"%d",&p);
    //printf("\nNumber of student preferences = %d\n",p);
   
    int a[m][n];
    for(i=0;i<m;i++)
    {
        for(j=0;j<n;j++)
        {
            a[i][j]=0;
        }
    }
    for(i=0;i<p;i++)
    {
       
        fscanf(fpt,"%d,%d",&col,&row);
       // printf("a[%d][%d] = 1\n",row-1,col-1);
       
        a[row-1][col-1]=1;
       
        
    }
    int count=1;
    glp_prob *lp;
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MAX);
    glp_add_rows(lp, n+m);
    glp_add_cols(lp, n*m);
    for(i=0;i<m;i++)
    {
       
        glp_set_row_bnds(lp, i+1, GLP_DB, 0.0, (double)M[i]);         
        
    }
    for(i=0;i<n;i++)
    {
        
        glp_set_row_bnds(lp,m+1+i, GLP_FX,(double) N,(double)N);
    }   
    count=1; 
    for(i=0;i<(m+n);i++)
    {
        for(j=0;j<m*n;j++)
        {
            glp_set_col_bnds(lp, j+1, GLP_DB, 0.0, 1.0);
            ia[count] = (i+1), ja[count] = j+1, ar[count] = 0.0;
            count++;
        }
    }
   
    int idx;
    for(i=0;i<m;i++)
    {
        int col=1;
        //printf("Course %d: ",i+1);
        for(j=i;j<m*n;j+=m)
        {
            idx=i*n*m+j+1;
            glp_set_obj_coef(lp,j+1, (double)a[i][(int)j/m]);
    
            ia[idx] = (i+1), ja[idx] = j+1, ar[idx] = (double)a[i][(int)j/m];
            /*if(a[i][(int)j/m]>0)
            {
                
                printf("%d ",(int)j/m+1);
            }*/
           
        }
       // printf("\n");
    }
   
    for(i=0;i<n;i++)
    {
        //printf("Students %d: ",i+1);
        for(j=m*i;j<(m+m*i);j++)
        {  
            idx=(i+m)*n*m+j+1;          
            glp_set_col_bnds(lp, j+1, GLP_DB, 0.0, 1.0);
            ia[idx] = (i+1+m), ja[idx] = (j+1), ar[idx] = (double)a[(int)j%m][i];
           /* if(a[(int)j%m][i]>0)
            {
                printf("%d ",(int)j%m+1);
            }*/
           
        }    
        //printf("\n");   
    }
    count--;
    //printf("\nLoading Matrix...\n");
    //printf("Total elements in contraints matrix: %d\n",count);
    glp_load_matrix(lp, count, ia, ja, ar);
    //printf("Loading Complete....\nSolving LP problem...\n");
    glp_simplex(lp, NULL);
    //printf("Results:\n");
    z = glp_get_obj_val(lp);
    printf("\nMaximum No. of course allocated = %g\n",z);
    //printf("Course Allocation:\n");
   for(i=0;i<n;i++)
   {
       printf("Student %d: ",i+1);
       for(j=0;j<m;j++)
       {
           double x=glp_get_col_prim(lp,i*m+j+1);
           //printf("%.1f ",x);
           if((int)x>0)
           {
               printf("%d ",(j+1));
           }
           
       }
       printf("\n");
   }

    glp_delete_prob(lp);
    fclose(fpt);
    return 0;
}