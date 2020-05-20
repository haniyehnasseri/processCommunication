#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include<algorithm> 

using namespace std;

string change_to_string(int number){
	string string_number_temp = "";
	string string_number = "";
	while(number != 0)
	{
		string_number_temp += '0' + number % 10;
		number /= 10;
	}
	for(int i = string_number_temp.size() - 1; i >=0; i--)
		string_number += string_number_temp[i];
	if(string_number == "")
		string_number += '0';
	return string_number;
}

vector<int> removeDupWord(char str[]) 
{   
    vector <int> words;
    char *token = strtok(str, " "); 
    while (token != NULL) 
    { 
      words.push_back(atoi(token)); 
      token = strtok(NULL, " "); 
    }
    return words; 
}

bool notin(int t,vector <int> a){
    for(int i = 0; i < a.size(); i++){
        if(t == a[i]){
            return false;
        }

    }
    return true;
}

int main(int argc,char **argv){
    vector < vector <int> > finalresults;
    int numofsamples;
    string voterPipe = string(argv[0]);
    int numofpipes = atoi(argv[1]);
    int n = atoi(argv[2]);
    char buffer [100000];
    for(int i = 0; i < numofpipes; i++){
        memset(buffer, 0, sizeof(buffer));
        vector <int> resultsofeachchild;
        int namedpipe;
        string _myfifo = "namedpipe_" + to_string(i);
        char* myfifo = (char *) _myfifo.c_str();
        namedpipe = open(myfifo,O_RDONLY);
        int eof = read(namedpipe, buffer, 100000);
        buffer[eof] = '\0';
        resultsofeachchild = removeDupWord(buffer);
        numofsamples = resultsofeachchild.size();
        finalresults.push_back(resultsofeachchild);
    }


    int count [n];
    vector <int> FinalRes;
   
    for(int i = 0; i < numofsamples; i++){
        for(int j = 0; j < n;j++){
            count [j] = 0;
        }
        for(int t = 0; t < finalresults.size(); t++){
            count[finalresults[t][i]] ++;
        }
        int _max = -1;
        int ind;
        for(int k = 0; k < n; k++){
            if(count[k] > _max){
                _max = count[k];
                ind=k;
            }
        }

        FinalRes.push_back(ind);
    }

    //////////////pipe to send FinalRes//////////////
    string input_for_named_pipe = to_string(FinalRes[0]);
    for(int i = 1; i < FinalRes.size(); i++){
        input_for_named_pipe += " " + to_string(FinalRes[i]);
    }
    input_for_named_pipe[input_for_named_pipe.size()] = '\0';
    int named_pipe_2;
    named_pipe_2 = open(voterPipe.c_str(),0666);
    write(named_pipe_2, input_for_named_pipe.c_str(), input_for_named_pipe.size() + 1);
    close(named_pipe_2); 
    return 0;
}
