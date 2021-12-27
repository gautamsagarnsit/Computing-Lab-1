#include<iostream>
#include<vector>
#include "togasat.hpp"

using namespace::std;

std::string problem_name = "boolean equation2.txt";
togasat::Solver solver; 

void  addInitialContraints(std::string problem_name) {   
    int vars = 0;
    int n_eq = 0; 
    vector<int> clause;
    std::string line;
    std::ifstream ifs(problem_name, std::ios_base::in);
    while(ifs.good()) {
        //cout<<"FILE GOOD\n";
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
                //cout<<val<<" ";              
            }
            //cout<<"\t--Adding Clause\n";
            solver.addClause(clause);
            
        }
      }
    }
    ifs.close();
    cout<<"All Initial Contraints read complete\n";
  }

int main(int argc, char *argv[]) {
	
    char p[1],cnf[3];
    int n_var,n_eq;
    std::vector<int> sol;   
    FILE *fpt;
    char file[25];
    addInitialContraints(problem_name);    
    togasat::lbool status = solver.solve();  // solve sat problem
    cout << status << endl ;
    sol=solver.printAnswer();  // print answer
    for(int i=0;i<sol.size();i++)
    {
        cout<<sol[i]<<" ";
    } 

}