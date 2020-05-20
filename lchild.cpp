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

int get_linear_result(vector <float> sample, vector < vector <float> > weights){
    float _max = ((sample[0] * weights[0][0]) + (sample[1] * weights[0][1]) + (weights[0][2]));
    int res = 0;
    for (int i = 1; i < weights.size(); i++ ){
        if (((sample[0] * weights[i][0]) + (sample[1] * weights[i][1]) + (weights[i][2])) > _max){
            _max = ((sample[0] * weights[i][0]) + (sample[1] * weights[i][1]) + (weights[i][2]));
            res = i;
        }
    }
    return res;
}

int main(int argc,char **argv){

    ifstream weightsFile,datasetFile;
    string line, word = "";
    vector < vector <float> > weights;
    vector < vector <float> > dataset;
    vector <int> results;
    string datasetaddressstr = string(argv[0]);
    datasetaddressstr += "/dataset.csv";
    string weightaddressstr = string(argv[1]);
    string index = string(argv[2]);
    weightaddressstr += "/classifier_" + index + ".csv";
    char* datasetaddress = (char *) datasetaddressstr.c_str();
    datasetFile.open(datasetaddress);
    getline(datasetFile, line);
    while(datasetFile){
        int temp = 0;
        vector <float> row;
        getline(datasetFile, line);
        stringstream s(line);
        while(getline(s, word, ',')){
            char* _word = (char *) word.c_str();
            row.push_back(atof(_word));
            temp = 1;
        }
        if(temp == 1)
            dataset.push_back(row);
    }
    datasetFile.close();
    
    ////////////////////weights vector///////////////////

    char* weightaddress = (char*) weightaddressstr.c_str();
    weightsFile.open(weightaddress);
    getline(weightsFile, line);
    while(weightsFile){
        int temp = 0;
        vector <float> row;
        getline(weightsFile, line);
        stringstream s(line);
        while(getline(s, word, ',')){
            char* _word = (char *) word.c_str();
            row.push_back(atof(_word));
            temp = 1;
        }
        if (temp == 1)
            weights.push_back(row);
    }
    weightsFile.close();

    for(int i = 0; i < dataset.size(); i++){
        int result = get_linear_result(dataset[i],weights);
        results.push_back(result);
    }

    ///////////////////make named pipe//////////////////////

    int named_pipe;
    string pipename = "namedpipe_" + index;
    named_pipe = open(pipename.c_str(),0666);
    string input_for_named_pipe = to_string(results[0]);

    for(int i = 1; i < results.size(); i++){
        input_for_named_pipe += " " + to_string(results[i]);
    }
    write(named_pipe, (char *)input_for_named_pipe.c_str(), input_for_named_pipe.length() + 1);
    close(named_pipe); 


    return 0;
}