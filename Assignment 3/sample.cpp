#include "togasat.hpp"
#include <iostream>

using namespace::std;
int main(int argc, char *argv[]) {
	togasat::Solver solver;
    std::vector<int> sol;

	std::vector<int> clause;
    clause.push_back(1);  // not x1
    solver.addClause(clause);  // add (not x1 v x2)

    clause.clear();
	clause.push_back(-1);  // not x1
    clause.push_back(3);  // x3
    solver.addClause(clause); // add (not x1 v x3)

    clause.clear();
    clause.push_back(1);  // x1
    clause.push_back(2);  // x2
    clause.push_back(3);  // x3
    solver.addClause(clause); // add (x1 v x2 v x3)
    
    //std::string problem_name = "boolean equation2.txt";   //read a CNF file in DIMACS format
    //solver.parseDimacsProblem(problem_name);  // parse problem
    togasat::lbool status = solver.solve();  // solve sat problem   
    cout << status << endl ;
    sol=solver.printAnswer();  // print answer
    for(int i=0;i<sol.size();i++)
    {
        cout<<sol[i]<<" ";
    }
    clause.clear();
    clause.push_back(-1);  // x1
    clause.push_back(2);  // x2
    solver.addClause(clause); // add (x1 v x2 v x3)

    status = solver.solve();  // solve sat problem
    cout<<"\n";
    cout << status << endl ;
    sol=solver.printAnswer();  // print answer
    for(int i=0;i<sol.size();i++)
    {
        cout<<sol[i]<<" ";
    }

}