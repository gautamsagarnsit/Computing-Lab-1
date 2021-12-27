#include<stdio.h>
#include<string.h>

int main(int argc,char **argv)
{
    int n,m,N,p,i,j,row,col,temp;
    int ia[1+1000], ja[1+1000];
    double ar[1+1000], z;
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
    printf("Number of Students = %d, Number of courses = %d\n",n,m);
    int M[m],x[n][m];
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
    double a[n][m];
    for(i=0;i<n;i++)
    {
        for(j=0;j<m;j++)
        {
            a[i][j]=0;
        }
    }
    for(i=0;i<n*p;i++)
    {
        //printf("Scanning...\n");
        fscanf(fpt,"%d,%d",&row,&col);
        printf("%d %d\n",row,col);
        //printf("Scanned...\n");
        a[row][col]=1.0;
        //printf("Assigned...\n");
        
    }

    
    fscanf(fpt,"%d",&temp);
    printf("\nEND DETECTED = %d",temp);

    

    fclose(fpt);
    return 0;
}