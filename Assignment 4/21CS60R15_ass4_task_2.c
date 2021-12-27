/*
Compiling: gcc -c 21CS60R15_ass4_task_2.c
Linking: gcc 21CS60R15_ass4_task_2.o -lglpk -lm
Running: ./a.out
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<glpk.h>

int combination(int *comb,int n, int r,int C)
{
    if(*((comb+(n-1)*C)+(r-1))>0)
    {
        //printf("%dC%d = %d\n",n,r,*((comb+(n-1)*C)+(r-1)));
        return *((comb+(n-1)*C)+(r-1));
    }
    if(n==r)
    {
        return 1;
    }
    else if(r==1)
    {
        return n;
    }
    else
    {
        *((comb+(n-1)*C)+(r-1))=combination(comb,n-1,r,C)+combination(comb,n-1,r-1,C);
        return *((comb+(n-1)*C)+(r-1));
    }
}
int main(int argc,char **argv)
{
    int n,m,t,N,p,i,j,row,col,temp;
    FILE *fpt;
    char file[25];
    if(argc==2)
    {
        fpt=fopen(argv[1],"r");
    }
    else{
        //scanf(" %s",file);
        //fpt=fopen(file,"r");
        fpt=fopen("testcase1.txt","r");
    }
    if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;   
    }
    
    fscanf(fpt,"%d,%d,%d",&n,&m,&t);
    printf("Number of Students = %d, Number of courses = %d, Number of Time slots = %d\n",n,m,t);
   
    double z;
    int M[m];
    fscanf(fpt,"%d",&N);
    printf("Max No. of course per student = %d\n",N);
    
    
    printf("Max No. of students per course:\t");
   
    for(i=0;i<m;i++)
    {
        fscanf(fpt,"%d,",&M[i]);
        printf("%d ",M[i]);
    }


    fscanf(fpt,"%d",&p);
    printf("\nNumber of student preferences = %d\n",p);
    //printf("Student preferences:\n");
    int a[m][n],x[m][n],y[m][t],lambda=-1,yct[m][t];
    for(i=0;i<m;i++)
    {
        for(j=0;j<t;j++)
        {
            yct[i][j]=0;
        }
    }
    for(i=0;i<m;i++)
    {
        for(j=0;j<n;j++)
        {
            a[i][j]=0;
            x[i][j]=0;
        }
    }
    for(i=0;i<m;i++)
    {
        for(j=0;j<t;j++)
        {
            y[i][j]=0;
        }
    }
    for(i=0;i<p;i++)
    {
        //printf("Scanning...\n");
        fscanf(fpt,"%d,%d",&col,&row);
        printf("a[%d][%d] = 1\n",row-1,col-1);
        //printf("Scanned...\n");
        a[row-1][col-1]=1;
        //printf("Assigned...\n");        
    }
    fclose(fpt);
    int count=1;
    int comb[m][2];
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<2;j++)
        {
            comb[i][j]=0;
        }
    }
    int col2 = n*(m+combination((int *)comb,m,2,2));
    int row2 = m + n+n*combination((int *)comb,m,2,2);
    int col1 = t*(m+combination((int *)comb,m,2,2));
    int row1 = m+combination((int *)comb,m,2,2);
    printf("Row1, col1 = %d,%d\n",row1,col1);
    printf("Row2,col2=%d,%d\n",row2,col2);
    int dim=(row2*col2>row1*col1)?row2*col2:row1*col1;
     printf("dim =%d\n",dim);
    int ia[1+dim], ja[1+dim];
    printf("ia,ja Assigned\n");
    double ar[1+dim];    
    printf("Staring LP\n");
    while(lambda<3)
    {
        glp_prob *lp1,*lp2;
        lp2 = glp_create_prob();
     
        glp_set_obj_dir(lp2, GLP_MIN);
        glp_add_rows(lp2, row2);
        glp_add_cols(lp2, col2);
        int ia[1+row2*col2], ja[1+row2*col2];
        double ar[1+row2*col2];

        
        lambda++;
        for(i=0;i<m;i++)
        {
        
            glp_set_row_bnds(lp2, i+1, GLP_DB, 0.0, (double)M[i]);         
            
        }
        for(i=0;i<n;i++)
        {
            
            glp_set_row_bnds(lp2,m+1+i, GLP_FX,(double) N,(double)N);
        }   
        for(i=0;i<(row2-m-n);i++)
        {
            
            glp_set_row_bnds(lp2,m+n+1+i, GLP_UP,1.0,1.0);
        }
        count=1; 
        for(i=0;i<row2;i++)
        {
            for(j=0;j<col2;j++)
            {
                glp_set_col_bnds(lp2, j+1, GLP_DB, 0.0, 1.0);
                ia[count] = (i+1), ja[count] = j+1, ar[count] = 0.0;
                count++;
            }
        }
        int idx,coeff;
        for(i=0;i<m;i++)
        {
            int col=1;
            //printf("Course %d: ",i+1);
            for(j=i*n;j<n*(i+1);j++)
            {
                idx=i*col2+j+1;
                glp_set_obj_coef(lp2,j+1, -1*(double)a[i][(int)j%n]);
        
                ia[idx] = (i+1), ja[idx] = j+1, ar[idx] =(double)a[i][(int)j%n];
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
            for(j=i;j<m*n;j+=n)
            {  
                idx=(i+m)*col2+j+1;          
                glp_set_col_bnds(lp2, j+1, GLP_DB, 0.0, 1.0);
                ia[idx] = (i+1+m), ja[idx] = (j+1), ar[idx] = (double)a[(int)j%m][i];
            /* if(a[(int)j%m][i]>0)
                {
                    printf("%d ",(int)j%m+1);
                }*/
            
            }    
            //printf("\n");   
        }
        for(i=0;i<n;i++)
        {
           /* for(j=m*n;j<col2;j++)
            {
                idx=(i+m+n)*col2+j+1;  
                glp_set_obj_coef(lp2,j+1, lambda*(double)yct[(int)j%m][i]);
                glp_set_col_bnds(lp2, j+1, GLP_DB, 0.0, 1.0);
                ia[idx] = (i+1+m+n), ja[idx] = (j+1), ar[idx] = lambda*(double)a[(int)j%m][i];
            }*/
            int idx=m*n+(i+m+n)*col2+1;
            int col_idx=m*n;
            for(j=0;j<m-1;j++)
            {
                for(int k=j+1;k<m;k++)
                {
                    coeff=0;
                    for(int l=0;l<t;l++)
                    {                        
                        if(yct[k][l]*yct[j][l])
                        {
                            coeff=1;
                        }                                         
                    } 
                    glp_set_obj_coef(lp2,j+1, (double)lambda*coeff);   
                    ia[idx] = (i+1+m+n), ja[idx] = (m*n+col_idx+1), ar[idx] = -1*(double)a[k][i]*(double)a[j][i];

                    ia[i*(k+1)+(i+m+n)*col2+1] = (i+1+m+n), ja[i*(k+1)+(i+m+n)*col2+1] = (m*n+col_idx+1), ar[i*(k+1)+(i+m+n)*col2+1] = (double)a[k][i];

                    ia[i*(j+1)+(i+m+n)*col2+1] = (i+1+m+n), ja[i*(j+1)+(i+m+n)*col2+1] = (m*n+col_idx+1), ar[i*(j+1)+(i+m+n)*col2+1] = (double)a[j][i];
                    idx++;
                }
            }
        }
       
        count--;
        glp_load_matrix(lp2, count, ia, ja, ar);
        glp_simplex(lp2, NULL);
        z = glp_get_obj_val(lp2);
        //printf("\nMaximum No. of course allocated = %g\n",z);
        double xsc[n][m];
        int temp_col_idx;
        for(i=0;i<n;i++)
        {            
            for(j=0;j<m;j++)
            {
                xsc[i][j]=0.0;
            }
        }
        for(i=0;i<n;i++)
        {            
            for(j=0;j<m;j++)
            {
               temp_col_idx=glp_get_col_prim(lp2,i*m+j+1); 
                x[i][(int)temp_col_idx-1] =1.0;          
            }
        }
        glp_delete_prob(lp2);
        // LP2 ENDS HERE
        // LP1 STARTS
       
        lp1= glp_create_prob();
        glp_set_obj_dir(lp1, GLP_MIN);
        glp_add_rows(lp1, row1);
        glp_add_cols(lp1, col1);
        
        for(i=0;i<row1;i++)
        {
            
            glp_set_row_bnds(lp1,m+1+i, GLP_FX,1.0,1.0);
        }
        count=1; 
        for(i=0;i<row1;i++)
        {
            for(j=0;j<col1;j++)
            {
                glp_set_col_bnds(lp1, j+1, GLP_DB, 0.0, 1.0);
                ia[count] = (i+1), ja[count] = j+1, ar[count] = 0.0;
                count++;
            }
        }
        idx=1;
        for(i=0;i<m;i++)
        {
            for(j=0;j<m*t;j+=t)
            {
                ia[idx] = (i+1), ja[idx] = j+1, ar[idx] = 1.0;
                idx++;
            }
            
        }
        
        for(i=0;i<t;i++)
        {
            int temp_idx=0;
            idx=m*t+(i+m)*col1+1;
            for(j=0;j<m-1;j++)
            {
                for(int k=j+1;k<m;k++)
                {
                    coeff=0;
                    for(int l=0;l<n;l++)
                    {
                        if(xsc[l][j]*xsc[l][k])
                        {
                            coeff=1;
                        }
                    }                
                    glp_set_obj_coef(lp1,m*n+temp_idx+1, (double)coeff); 
                    ia[idx] = (i+1), ja[idx] = m*n+temp_idx+1, ar[idx] = -1.0;

                    ia[j*t+(i+1)*col1+1] = (i+1), ja[j*t+(i+1)*col1+1] = m*n+temp_idx+1, ar[j*t+(i+1)*col1+1] = 1.0;

                    ia[k*t+(i+1)*col1+1] = (i+1), ja[k*t+(i+1)*col1+1] = m*n+temp_idx+1, ar[k*t+(i+1)*col1+1] = 1.0;

                    temp_idx++;
                    idx++;
                }
            }
        }
        count--;
        glp_load_matrix(lp1, count, ia, ja, ar);
        glp_simplex(lp1, NULL);
        z = glp_get_obj_val(lp1);
        //printf("\nMaximum No. of course allocated = %g\n",z);
        for(i=0;i<m;i++)
        {            
            for(j=0;j<t;j++)
            {
                yct[i][j]=0.0;
            }
        }
        for(i=0;i<m;i++)
        {            
            for(j=0;j<t;j++)
            {
                temp_col_idx=glp_get_col_prim(lp1,i*m+j+1); 
                yct[i][j] =temp_col_idx;          
            }
        }
        glp_delete_prob(lp1);
        
    }

    return 0;
}   