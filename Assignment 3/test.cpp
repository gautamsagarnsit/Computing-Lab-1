#include <cstdio>
#include <iostream>
#include <vector>

 
using namespace std;
#include "togasat.hpp"

vector<vector<int> > vec;

void findSAT(){
    togasat::Solver solver;
    for(int i=0; i< vec.size(); i++){
        solver.addClause(vec[i]);
    }
    solver.solve();
    solver.printAnswer();
}

int main(void)
{    
    std::vector<int> clause;
    // not x1 v not x2 v x3 v not x4
    clause.push_back(-1);
    clause.push_back(-2);
    clause.push_back(3);
    clause.push_back(-4);
    vec.push_back(clause);
    findSAT();

    // x3 v x4
    clause.clear();
    clause.push_back(3);
    clause.push_back(4);
    //solver.addClause(clause);
    vec.push_back(clause);
    findSAT();
    //solver.solve();
    //solver.printAnswer();

    clause.clear();
    clause.push_back(4);
    vec.push_back(clause);
    findSAT();
}