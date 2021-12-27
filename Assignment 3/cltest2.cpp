#include<iostream>
#include<vector>
#include "togasat.hpp"
#include<time.h>

int graph[6][6];
/*std::vector<int> printAnswer() {
    std::vector<int> vec;
    if (answer == 0) {
      //std::cout << "SAT" << std::endl;
      for (int i = 0; i < assigns.size(); i++) {
        if (assigns[i] == 0) {
          //std::cout << (i + 1) << " ";
          vec.push_back(i+1);
        } else {
         // std::cout << -(i + 1) << " ";
          vec.push_back(-(i+1));
        }
      }
      //std::cout << "0" << std::endl;
    } else {
      //std::cout << "UNSAT" << std::endl;
    }
    return vec;
    }
  */

using namespace::std;

vector<vector<int>> vec;
vector<int> findSAT(){ //Find Satifiability conditions for constraints stored in 2D vector "vec"
    vector<int> sol;
    togasat::Solver solver;
    for(int i=0; i< vec.size(); i++)
    {
        solver.addClause(vec[i]);
    }
    solver.solve();
    sol=solver.printAnswer();
    return sol;
}

togasat::Solver solver;
void  addContraints()
 {   
    vector<int> clause;
    //Constriants the atleast one colour assigned to every node
    clause.clear();
    clause.push_back(1);
    clause.push_back(7);
    clause.push_back(13); 
    vec.push_back(clause);  

    clause.clear();
    clause.push_back(1);
    clause.push_back(7);
    clause.push_back(13); 
    vec.push_back(clause); 

    clause.clear();
    clause.push_back(2);
    clause.push_back(8);
    clause.push_back(14); 
    vec.push_back(clause);  

    clause.clear();
    clause.push_back(3);
    clause.push_back(9);
    clause.push_back(15); 
    vec.push_back(clause);  

    clause.clear();
    clause.push_back(4);
    clause.push_back(10);
    clause.push_back(16); 
    vec.push_back(clause);  

    clause.clear();
    clause.push_back(5);
    clause.push_back(11);
    clause.push_back(17); 
    vec.push_back(clause); 

    clause.clear();
    clause.push_back(6);
    clause.push_back(12);
    clause.push_back(18); 
    vec.push_back(clause);
    //---------------------------------------------
    // Constraints for Atmost one colour to one node
    clause.clear();
    clause.push_back(-1);
    clause.push_back(-7);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-1);
    clause.push_back(-13);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-1);
    clause.push_back(-7);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-7);
    clause.push_back(-13);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-13);
    clause.push_back(-7);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-1);
    clause.push_back(-13);
    vec.push_back(clause);
    //----
    clause.clear();
    clause.push_back(-2);
    clause.push_back(-8);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-2);
    clause.push_back(-14);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-2);
    clause.push_back(-8);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-8);
    clause.push_back(-14);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-14);
    clause.push_back(-8);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-2);
    clause.push_back(-14);
    vec.push_back(clause);
    //---
    clause.clear();
    clause.push_back(-3);
    clause.push_back(-9);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-3);
    clause.push_back(-15);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-3);
    clause.push_back(-9);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-9);
    clause.push_back(-15);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-15);
    clause.push_back(-9);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-3);
    clause.push_back(-15);
    vec.push_back(clause);
    //----
    clause.clear();
    clause.push_back(-4);
    clause.push_back(-10);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-4);
    clause.push_back(-16);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-4);
    clause.push_back(-10);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-10);
    clause.push_back(-16);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-16);
    clause.push_back(-10);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-4);
    clause.push_back(-16);
    vec.push_back(clause);
    //---
    clause.clear();
    clause.push_back(-5);
    clause.push_back(-11);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-5);
    clause.push_back(-17);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-5);
    clause.push_back(-11);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-11);
    clause.push_back(-17);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-17);
    clause.push_back(-11);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-5);
    clause.push_back(-17);
    vec.push_back(clause);
    //---
    clause.clear();
    clause.push_back(-6);
    clause.push_back(-12);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-6);
    clause.push_back(-18);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-6);
    clause.push_back(-12);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-12);
    clause.push_back(-18);
    vec.push_back(clause);

    clause.clear();
    clause.push_back(-18);
    clause.push_back(-12);
    vec.push_back(clause);
    clause.clear();
    clause.push_back(-6);
    clause.push_back(-18);
    vec.push_back(clause);
    //------------------------------------------
    //Now Adding constrints for connected nodes on oppsite side of the same edge must have different colors
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<6;j++)
        {
            if(graph[i][j]==1)
            {
                // This portion not added in the actual submission.
                clause.clear();
                clause.push_back(-i);
                clause.push_back(-j);
                vec.push_back(clause);
                clause.clear();
                clause.push_back(-(i+6));
                clause.push_back(-(j+6));
                vec.push_back(clause);
                clause.clear();
                clause.push_back(-(i+12));
                clause.push_back(-(j+12));
                vec.push_back(clause);
            }
        }
    }
}
vector<int> check(vector<int> code,togasat::Solver solver)
{
   
    std::vector<int> sol;
    vector<int> guess;
    togasat::lbool status = solver.solve();  // solve sat problem
    sol=findSAT();  // print answer
    for(int i=0;i<sol.size();i++)
    {
       if(sol[i]>0)
       {
            printf("Guess= %d",sol[i]);
            guess.push_back(sol[i]);
       }
    }

    return guess;
} 

int main(int argc,char **argv)
{
    int i,j,m;
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
    
    fscanf(fpt,"%d",&m);
    int u,v;
    for(i=0;i<6;i++)
    {
        for(j=0;j<6;j++)
        {
            graph[i][j]=0;
        }
    }
    for(i=0;i<m;i++)
    {
        fscanf(fpt,"%d,%d",&u,&v);
        graph[u][v]=1;
    }

    addContraints();       
    vector<int> code,temp,guess,temp_guess;
    vector<const char*>color;
    color.push_back("R");
    color.push_back("G");
    color.push_back("B");
    guess=check(code,solver); 
       
}