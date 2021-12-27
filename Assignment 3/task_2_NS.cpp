#include<iostream>
#include<vector>
#include "togasat.hpp"
#include<time.h>

using namespace::std;

vector<vector<int>> vec;

std::string problem_name = "boolean equation2.txt";  //Don't Edit the file name
togasat::Solver solver;
void  add(std::string problem_name) {   
    int vars = 0;
    int n_eq = 0; 
    vector<int> clause;
    std::string line;
    std::ifstream ifs(problem_name, std::ios_base::in);
    while(ifs.good()) {
       
      getline(ifs, line);
      if (line.size() > 0) {
        if (line[0] == 'p') {
          sscanf(line.c_str(), "p cnf %d %d", &vars, &n_eq);
        } 
        else
        {
            std::stringstream ss(line);
            clause.clear();
            while (ss) 
            {
                int val;
                ss >> val;
                if (val == 0) break;
                clause.push_back(val);  
                           
            }
           
            vec.push_back(clause);
            
        }
      }
    }
    ifs.close();
    cout<<"All Initial Contraints read complete\n";
  }

void addClause(vector<int> guess,vector<int> hint)
{ // Add clauses based on the hint given by the codemaker
    int black=hint[0];
    int white=hint[1];
   

    vector<int> clause;
   
    switch(black)
    {
        
        case 0:
        //All guesses are wrong
        // Adding ~A and ~B and ~C and ~D
        clause.clear();
        clause.push_back(-guess[0]);  
       vec.push_back(clause); //Added ~A
       clause.clear();
        clause.push_back(-guess[1]);  
       vec.push_back(clause);//Added ~B
       clause.clear();
        clause.push_back(-guess[2]);  
       vec.push_back(clause);    //Added ~C
       clause.clear();
        clause.push_back(-guess[3]);  
       vec.push_back(clause);//Added ~D
        break;

        case 1:
        clause.clear();
        clause.push_back(-guess[0]); 
        clause.push_back(-guess[1]); 
        clause.push_back(-guess[2]);  
        clause.push_back(-guess[3]); 
       vec.push_back(clause); //Added ~A
       clause.clear();
        clause.push_back(-guess[1]); 
        clause.push_back(-guess[2]);  
        clause.push_back(-guess[3]); 
       vec.push_back(clause);//Added ~B
       clause.clear();
        clause.push_back(-guess[2]);
        clause.push_back(-guess[3]);   
       vec.push_back(clause);    //Added ~C
      
        break;
        
        break;

        case 3:
        clause.clear();
        clause.push_back(-guess[0]); 
         clause.push_back(-guess[1]);  
       vec.push_back(clause); //Added ~A
       clause.clear();
        clause.push_back(-guess[1]); 
         clause.push_back(-guess[2]);  
       vec.push_back(clause);//Added ~B
       clause.clear();
        clause.push_back(-guess[2]); 
         clause.push_back(-guess[3]);  
       vec.push_back(clause);    //Added ~C
       clause.clear();
        clause.push_back(-guess[3]);  
       vec.push_back(clause);//Added ~D
        break;

        case 4:
        
        clause.push_back(guess[1]);
        vec.push_back(clause); //Added A
        clause.clear();
        
        clause.push_back(guess[1]);  
        
        vec.push_back(clause);//Added B
        clause.clear();
      
        clause.push_back(guess[2]);  
        vec.push_back(clause);    //Added C
        clause.clear();
        clause.push_back(guess[3]);  
        
        vec.push_back(clause);//Added D
        clause.clear();
        
        break;
    }
    
}
vector<int> findSAT(){ //Find Satifiability conditions for constraints stored in 2D vector "vec"
    vector<int> sol;
    togasat::Solver solver;
    for(int i=0; i< vec.size(); i++){
        solver.addClause(vec[i]);
    }
    solver.solve();
    sol=solver.printAnswer();
    return sol;
}
vector<int> codebreaker(vector<int> code,togasat::Solver solver)
{
   
    std::vector<int> sol;
    vector<int> guess;
    int temp[4];
    togasat::lbool status = solver.solve();  // solve sat problem
    sol=findSAT();  // print answer
    return sol;
} 

int main()
{
    srand(time(0)); //Seed for RNG
    add(problem_name); 
    vector<int> code,temp;
    vector<int> hint;
    vector<int> guess,temp_guess;  
    int t;
    for(int i=0;i<4;i++)
    {
        code.push_back(rand()%8); 
    
    }  
    int iter=1,black=2,white=1;
    while((black!=4) && (iter<=20))
    {  
        
        black=0;
        white=0;
        guess=codebreaker(code,solver); 
        temp_guess=guess;            
        cout<<"\nIteration "<<iter++<<"     "<<guess[0]<<" "<<guess[1]<<" "<<guess[2]<<" "<<guess[3]<<"     ";        
        temp=code;
        for(int i=0;i<4;i++)
        {// Calculating Number of Black
            if(guess[i]==code[i])
            {
                black++;
                code[i]=-1;
                guess[i]=-1;
            }                              
        }        
        code=temp;
        hint.clear();
        hint.push_back(black);
        hint.push_back(white);        
        addClause(temp_guess,hint);             
    }
    if(black==4)
    {
        cout<<"\nYou Won"<<endl;
        return 0;
    }
    else{
        cout<<"\nYou Lose"<<endl;
        return 0;
    }       
}