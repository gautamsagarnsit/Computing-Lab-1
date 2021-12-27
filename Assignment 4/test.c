#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<glpk.h>

int main(int argc,char **argv)
{   
    int n,m,k,p,i,j,row,col,temp;
    FILE *fpt;
    char file[25];
    if(argc==2)
    {
        fpt=fopen(argv[1],"r");
    }
    else{
        scanf(" %s",file);
        fpt=fopen(file,"r");
        //fpt=fopen("testcase.txt","r");
    }
    if(fpt==NULL)
    {
        printf("File not Found\n");
        return 1;   
    }
    
    fscanf(fpt,"%d,%d,%d",&n,&m,&k);
   // printf("Number of Students = %d, Number of courses = %d\n",n,m);
    int ia[1+(m+n)*m*n], ja[1+(m+n)*m*n];
    double ar[1+(m+n)*m*n], z;
    int c[n*m],d[n*m],x[n*m],temp_c,temp_d;
    for(i=0;i<n*m;i++)
    {
        fscanf(fpt,"%d,%d",&temp_c,&temp_d);
        c[i]=temp_c;
        d[i]=temp_d;
        x[i]=0;
    }

    
    int count=1;
    glp_prob *lp;
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
    glp_add_rows(lp, m*n+n+m+1);
    glp_add_cols(lp, n*m);
    int temp_count=0;
    for(i=0;i<m*n;i++)
    {
       
        glp_set_row_bnds(lp, i+1, GLP_DB, 0.0, (double)d[i]);         
        
    }
    for(i=0;i<m;i++)
    {
        
            glp_set_row_bnds(lp,m*n+1+i, GLP_FX,k);
        
    }   
     
    count=1; 
    for(i=0;i<m*n;i++)
    {
        glp_set_col_bnds(lp, i+1, GLP_DB, 0.0,d[i]);
        ia[count] = (i+1), ja[count] = i+1, ar[count] =1.0;
        glp_set_obj_coef(lp,i+1, (double)c[i]);
        count++;
    }
    for(i=0;i<n;i++)
    {
        fpr(j=i*m;j<n*m;j++)
        {
            glp_set_col_bnds(lp, i+1, GLP_DB, 0.0,d[i]);
            ia[count] = (j+1), ja[count] = j+1, ar[count] =1.0;
            count++;
        }
        
    }  
    glp_set_row_bnds(lp,m*n+1+n, GLP_FX,k*n);
    
    for(i=;i<m*n;i++)
    {
        glp_set_col_bnds(lp, i+1, GLP_DB, 0.0,1.0);
    ia[count] = (j+1), ja[count] = j+1, ar[count] =1.0;
    count++;
    }
    count--;
    
    glp_load_matrix(lp, count, ia, ja, ar);

    glp_simplex(lp, NULL);
    z = glp_get_obj_val(lp);
    printf("\nMinimum Cost = %g\n",z);
   for(i=0;i<n*m;i++)
   {
       printf("Item %d from vendor %d: ",i/m,i/n);
       double x=glp_get_col_prim(lp,i);
       printf("\n");
   }

    glp_delete_prob(lp);
    fclose(fpt);
    return 0;
}