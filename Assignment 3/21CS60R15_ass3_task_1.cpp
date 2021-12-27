#include<iostream>
#include<vector>
#include "togasat.hpp"
#include<time.h>

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
std::string problem_name = "boolean equation.txt"; 
togasat::Solver solver;
void  addInitialContraints(std::string problem_name) {   
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
        // One Guess is correct
        // Adding (~A or ~B) and(~A or ~C) and (~A or ~D) and (~B or ~C) and (~B or ~D) and (~C or ~D)
        clause.clear();
        clause.push_back(-guess[0]);
        clause.push_back(-guess[1]);  
       vec.push_back(clause);  // Added ~A or ~B
       clause.clear();
        clause.push_back(-guess[0]);  
        clause.push_back(-guess[2]);
       vec.push_back(clause);
       clause.clear();
        clause.push_back(-guess[0]);
        clause.push_back(-guess[3]);  
       vec.push_back(clause); 
       clause.clear();   
        clause.push_back(-guess[1]);  
        clause.push_back(-guess[2]);
       vec.push_back(clause);
        clause.push_back(-guess[1]);  
        clause.push_back(-guess[3]);
       vec.push_back(clause);
       clause.clear();
        clause.push_back(-guess[3]);  
        clause.push_back(-guess[2]);
       vec.push_back(clause);        
        break;

        case 2:
          // 2 Guesses are correct
        clause.push_back(-guess[0]);
        clause.push_back(-guess[1]); 
        clause.push_back(-guess[2]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(-guess[0]);
        clause.push_back(-guess[1]); 
        clause.push_back(-guess[3]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(-guess[0]);
        clause.push_back(-guess[2]); 
        clause.push_back(-guess[3]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(guess[0]);
        clause.push_back(guess[1]); 
        clause.push_back(guess[2]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(guess[0]);
        clause.push_back(guess[1]); 
        clause.push_back(guess[3]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(-guess[0]);
        clause.push_back(-guess[2]); 
        clause.push_back(-guess[3]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(-guess[3]);
        clause.push_back(-guess[1]); 
        clause.push_back(-guess[2]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(guess[3]);
        clause.push_back(guess[1]); 
        clause.push_back(guess[2]);  
       vec.push_back(clause);
       clause.clear();
                
        break;

        case 3:
        //3 Guesses are correct
        clause.push_back(-guess[0]);
        clause.push_back(-guess[1]);
        clause.push_back(-guess[2]);
        clause.push_back(-guess[3]);
       vec.push_back(clause); 
       clause.clear();

        clause.push_back(guess[1]); 
        clause.push_back(guess[0]); 
       vec.push_back(clause);
       clause.clear();

        clause.push_back(guess[0]);
        clause.push_back(guess[2]);  
       vec.push_back(clause);  
       clause.clear();

        clause.push_back(guess[0]);  
        clause.push_back(guess[3]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(guess[1]);  
        clause.push_back(guess[2]);  
       vec.push_back(clause);
       clause.clear();
        

        clause.push_back(guess[1]);  
        clause.push_back(guess[3]);  
       vec.push_back(clause);
       clause.clear();

        clause.push_back(guess[2]);  
        clause.push_back(guess[3]);  
       vec.push_back(clause);
       clause.clear();
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
vector<int> codebreaker(vector<int> code,togasat::Solver solver)
{
   
    std::vector<int> sol;
    vector<int> guess;
    int temp[4];
    togasat::lbool status = solver.solve();  // solve sat problem
    sol=findSAT();  // print answer
    for(int i=0;i<sol.size();i++)
    {
        //cout<<sol[i]<<" ";
    }
    for(int i=0;(i<32) && (guess.size()<=4);i++)
    {
        if(sol[i]>0)        
        {
            
            if(sol[i]%4==1)
            {
                temp[0]=sol[i];
            }
            else if(sol[i]%4==2)
            {
                temp[1]=sol[i];
            }
            else if(sol[i]%4==3)
            {
                temp[2]=sol[i];
            }
            else if(sol[i]%4==0)
            {
                temp[3]=sol[i];
            }
        }
        
    } 
    for(int i=0;i<4;i++)
    {
        guess.push_back(temp[i]);
    }

    return guess;
} 
bool check(vector<int> guess,vector<int> code)
{ // Checks If guess is correct or not
    int flag=1;
    if(guess.size()!=code.size())
    {
        printf("Size Not Equal\n");
        return 0;
    }
    for(int i=0;i<guess.size();i++)
    {
        printf("Guess: %d, Code: %d\n",guess[i],code[i]);

        if(guess[i]!=code[i])
        {
            flag=0;
        }
    }
    return flag;
}
int main()
{
    srand(time(0)); //Seed for RNG
    addInitialContraints(problem_name);   // This function reads Initial constraints given in DIMACS format.       
    vector<int> code,temp;
    vector<int> hint;
    vector<int> guess,temp_guess;
    vector<const char*>color,color_enc;
    int t;
    color.push_back("R");//1-4
    color.push_back("B");//5-8
    color.push_back("G");//9-12
    color.push_back("Y");//13-16
    color.push_back("O");//17-20
    color.push_back("P");//21-24
    color.push_back("W");//25-28
    color.push_back("K");//29-32
    //eg. BYWO - 5 14 27 20

    cout<<"                ";
    // Please Enter input in below four line satisfying the numbering used for position encoding
    // Eg. For Red Red Red Red => 1 2 3 4
    // Colour Code in position has remainder 1 when divided by 4
    // Colour Code in position has remainder 2 when divided by 4
    // Colour Code in position has remainder 3 when divided by 4
    // Colour Code in position has remainder 0 when divided by 4
    code.push_back(17);
    code.push_back(22);
    code.push_back(31);
    code.push_back(24);
    for(int i=0;i<4;i++)
    {
        if(code[i]%4==0)
        {
            t=code[i];
            t--;
            cout<<color[(t)/4]<<" ";
        }
        else{
            cout<<color[code[i]/4]<<" ";
        }
    }  
    //cout<<color[code[0]/4]<<" "<<color[code[1]/4]<<" "<<color[code[2]/4]<<" "<<color[code[3]/4];



    int iter=1,black=2,white=1;
    while((black!=4) && (iter<=20))
    {  
        
        black=0;
        white=0;
        guess=codebreaker(code,solver); 
        temp_guess=guess;    
        color_enc.clear();   
        for(int i=0;i<4;i++)
        {
            // To map position encoded number to its colour.
            
            if(guess[i]%4==0)
            {
                t=guess[i];
                t--;
                color_enc.push_back(color[(t/4)]);
            }
            else{
                color_enc.push_back(color[(guess[i]/4)]);
            }
            
        }
        
        cout<<"\nIteration "<<iter++<<"     "<<color_enc[0]<<" "<<color_enc[1]<<" "<<color_enc[2]<<" "<<color_enc[3]<<"     ";
        //cout<<"\nIteration "<<iter++<<"     "<<guess[0]<<" "<<guess[1]<<" "<<guess[2]<<" "<<guess[3]<<"     ";
        temp=code;
        for(int i=0;i<4;i++)
        { // Calculating Number of Black
            if(guess[i]==code[i])
            {
                black++;
                code[i]=-1;
                guess[i]=-1;
            }                              
        }
        //printf("Checking White:\t");
        for(int i =0;i<4;i++)
        {
            for(int j=0;(j<4);j++)
            {
                if((guess[i]>0) && (code[j]>0))
                {
                  
                   if(guess[i]==code[j])
                   {
                       white++;
                       code[j]=-1;
                       j=4;
                   }                  
                }                
            }
        }
        code=temp;        
        cout<<"B-"<<black<<" ";
        cout<<"W-"<<white;
        hint.clear();
        hint.push_back(black);
        hint.push_back(white);
        cout<<"\n";
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