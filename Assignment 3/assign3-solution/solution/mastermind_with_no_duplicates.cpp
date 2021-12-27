#include <cstdio>
#include <iostream>
#include <vector>
#include<time.h>
#include <set>

 
using namespace std;
#include "togasat.hpp"

set<vector<int> > vec;
vector<int> answer;
int hidden_code[4];
int guess[4];
int b=0,w=0;
int color_with_position[8][4];
char color[] = {'R','G','B','O','P','Y','W','K'};

void printClause(vector<int> c){
    for(int j=0; j < c.size(); j++){
        cout << c[j] << " ";
    }
    cout << endl;
}

//find the SAT assignment for the current set of clauses
vector<int> findSAT(){
    //printf("size of vector %ld \n" , vec.size());
    togasat::Solver solver;
    for(auto elem:vec){
    	solver.addClause(elem);
    }
    solver.solve();
    return solver.assigns;
}

//from the satisfying assignment decode the colour and store it in guess array
void findGuess(){
    for(int i=0;i<answer.size();i++){
        if(answer[i] == 0){
            guess[i%4] = (i/4)+1;
        }
    }
}

//compare guess and hidden code and return feedback black and white 
void codemakerFeedback(){
    int i,j;
    b = 0; w = 0;
    for(i=0;i<4;i++){
        if(guess[i] == hidden_code[i]){
            b++;
        }
    }
    for(i=0; i<4;i++){
        cout << color[guess[i]-1] << " ";
        for(j=0;j<4;j++){
            if(i!=j && guess[i] == hidden_code[j]){
                w++;
            }
        }
    }
    cout << endl;
}

//add constraints after feedback
void addConstraints(){
    int i,j,k;
    vector<int> clause;
    if(b == 0){
        if(w==0){
            for(i=0;i<4;i++){
                for(k=0; k<4;k++){
                    clause.push_back(-color_with_position[guess[i]-1][k]);
                    vec.insert(clause);
                    clause.clear();
                }
            }
        }else if(w==4){
            for(int x=0;x<8;x++){//reject all other colours
                int flag = 0;
                for(int y = 0; y<4;y++){ //check if present in the codemaker
                    if (guess[y] == (x+1)){ //if yes then break
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0){ //means x is not there so negate all the combinations
                    for(int z=0; z<4;z++){
                    	clause.clear();
                        clause.push_back(-color_with_position[x][z]); 
                        vec.insert(clause);
                    }
                }
            }
            //meaning all the colours are present in wrong position
            for(i=0;i<4;i++){
                clause.clear();
                for(j=0;j<4;j++){
                    if(i==j){
                        clause.push_back(-color_with_position[guess[i]-1][j]);
                    }else{
                        clause.push_back(color_with_position[guess[i]-1][j]);
                    }
                }
                vec.insert(clause);
            }
        }else if (w == 3){
            clause.clear();
            for(i=0;i<4;i++){
                clause.push_back(-color_with_position[guess[i]-1][i]); 
            }
            vec.insert(clause);
        }else{
            for(i=0;i<4;i++){ //if the colour is present then it is in the wrong position
                clause.clear();
                clause.push_back(-color_with_position[guess[i]-1][i]); 
                for(j=0;j<4;j++){
                    if(i!=j)
                        clause.push_back(color_with_position[guess[i]-1][j]); 
                }
                vec.insert(clause);
            }
        }
    }
    if(b == 1){
        //then one of the colour is in the correct position
        for(i=0;i<4;i++){ //means one of the colour is atleast there
             clause.push_back(color_with_position[guess[i]-1][i]);
        }
        vec.insert(clause);
        clause.clear();
        
        if(w == 0){ //if white is 0 means that all other colors are not there
            for(i=0;i<4;i++){
                for(j=0; j<4; j++){
                    if(i!=j){
                        for(k=0;k<4;k++){
                            clause.push_back(-color_with_position[guess[i]-1][i]); 
                            clause.push_back(-color_with_position[guess[j]-1][k]);
                            vec.insert(clause);
                            clause.clear();
                        }
                    }
                }
            }
        }else { //atmost one of them is present in the correct position
            for(i=0;i<4;i++){
                for(j=i+1; j<4;j++){
                    clause.push_back(-color_with_position[guess[i]-1][i]); 
                    clause.push_back(-color_with_position[guess[j]-1][j]);
                    vec.insert(clause);
                    clause.clear();
                }
            }
            if(w == 3){
                for(int x=0;x<8;x++){//for all colours
                    int flag = 0;
                    for(int y = 0; y<4;y++){ //check if present in the codemaker
                        if (guess[y] == (x+1)){ //if yes then break
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0){ //means the other colour (x) is not there
                        for(int z=0; z<4;z++){
                            clause.clear();
                            clause.push_back(-color_with_position[x][z]); 
                            vec.insert(clause);
                        }
                    }
                }
                for(i=0;i<4;i++){ //for picking one colour from code breaker at a time
                    for(j=0;j<4;j++){ //now if the colour at pos i received a black peg then color at pos j should not be in location i and j
                        if(i!=j){
                            clause.clear();
                            clause.push_back(-color_with_position[guess[i]-1][i]);
                            for(k=0;k<4;k++){
                                if(k!=j && k!=i){
                                    clause.push_back(color_with_position[guess[j]-1][k]);
                                }
                            }
                            vec.insert(clause);
                        }
                    }
                }
            }
        }
    }
    if(b==2){
    	for(i=0;i<4;i++){ //for picking one colour from code breaker at a time
            for(j=i+1;j<4;j++){ //now if the second colour color is also present at pos j
                clause.clear();
                clause.push_back(-color_with_position[guess[i]-1][i]);
                clause.push_back(-color_with_position[guess[j]-1][j]);
                for(k=0; k<4; k++){ // then the colours at position !i and !j are wrong
                	if(k!=i && k!=j){
                		clause.push_back(-color_with_position[guess[k]-1][k]);
                		vec.insert(clause);
                		clause.pop_back();
                	}
                }
            }
        }
        //here i could have added more clauses for specific cases where w=0, w=1 and w=2.
        //As given b=2, w should be <=2 as b+w <=4
    }
    if(b==3){ //if black is 3 then white cannot be 0 so the fourth coulour is wrong
    	clause.clear();
        for(i=0;i<4;i++){ //means if three colours is there then the fourth one is worong. We could have also negated all position of colour as well
            clause.push_back(-color_with_position[guess[i]-1][i]); 
        }
        vec.insert(clause);
    }
}

void add_initial_clauses(){
    int i,j,k;
    vector<int> clause;

    //every position gets atleast 1 colour
    for(i=0; i<4; i++){
        for(j=0; j<8; j++){
            clause.push_back(color_with_position[j][i]);
        }
        vec.insert(clause);
        clause.clear();
    }
    
    //if a colour is present then it is present in only one position
    for(i=0;i<8;i++){
        for(j=0;j<4;j++){
            for(k=j+1;k<4;k++){
                clause.push_back(-color_with_position[i][j]);
                clause.push_back(-color_with_position[i][k]);
                vec.insert(clause);
                clause.clear();
            }
        }
    }

    //if a colour is present in a position then other colours are not present in that position
    for(i=0;i<4;i++){
        for(j=0;j<8;j++){
            for(k=j+1;k<8;k++){
                clause.push_back(-color_with_position[j][i]);
                clause.push_back(-color_with_position[k][i]);
                vec.insert(clause);
                clause.clear();
            }
        }
    }
}

//this function returns the initial code set by the codemaker without duplicates
void get_hidden_code_without_duplicates(){
    int i=0,j,x;
    srand(time(0));
    while(i < 4){
        int flag = 0;
        x=rand()%8;
        for(j = 0; j<i; j++){
            if(hidden_code[j] == (x+1)){
                flag = 1;
                continue;
            }
        }
        if(!flag)
            hidden_code[i++] = x+1;
    }
}

int main(void)
{
    int i = 0,j=0,k=1;
    for(i=0; i < 8; i++){
        for(j=0;j<4;j++){
            color_with_position[i][j] = k;
            k++;
        }
    }
    get_hidden_code_without_duplicates();
    //hidden_code[0] = 1; hidden_code[1] = 2; hidden_code[2] = 4; hidden_code[3] = 3; 
    cout << "The code that needs to broken is" << endl ; 
    for(i=0;i<4;i++){
        cout << color[hidden_code[i]-1] << " ";
    }
    cout << endl << "---------" << endl;
    add_initial_clauses();
    answer = findSAT();
    findGuess();
    codemakerFeedback();
    int count = 1;
    cout << count << "--> " << "black :" << b << " white : " << w << endl;
    while(b!=4){
    	count++;
        addConstraints();
        answer = findSAT();
        findGuess();
        codemakerFeedback();
        cout << count << "--> " << "black :" << b << " white : " << w << endl;
    }
}