#include <iostream>
#include <fcntl.h> 
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <vector> 
#include <bits/stdc++.h> 
#include <iomanip>


using namespace std;

vector<char *> removeDupWord(char str[]) 
{   
    vector <char *> words;
    char *token = strtok(str, " "); 
    while (token != NULL) 
    { 
      words.push_back(token); 
      token = strtok(NULL, " "); 
    }
    return words; 
} 

vector<int> removeDupWord2(char str[]) 
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


int main(int argc,char **argv)
{
  string weightvectorsaddr = string(argv[2]);
  string validationaddr = string(argv[1]);
  int mainparent = getpid();
  int num = 0;
  int fd,pid;
  string str = "";
  while(true){
    str = "classifier_" + to_string(num) + ".csv";
    string s = weightvectorsaddr + "/" + str;
    fd = open(s.c_str(),O_RDONLY);
    if(fd > 0)
      num = num + 1;
    else
      break;
  }
  for(int i = 0; i < num; i++){
    string pipename = "namedpipe_" + to_string(i);
    char * myfifo = (char *) pipename.c_str();
    int fd = open(pipename.c_str(), O_CREAT | O_RDWR, S_IRWXU);
    close(fd);
    mkfifo(pipename.c_str(),0666);
  }
  int p[num][2];
  for(int i = 0; i < num; i++){
    pipe(p[i]);
    string input_str = validationaddr + " " + weightvectorsaddr + " " + to_string(i);
    pid = fork();
    if(pid > 0){
      close(p[i][0]);
      write(p[i][1], (char *)input_str.c_str(), input_str.length() + 1);
      close(p[i][1]);
    }
    if(pid == 0){
      vector <char *> args2;
      char read_str[100];
      close(p[i][1]);
      int eof = read(p[i][0], read_str, 100);
      read_str[eof] = '\0';
      args2 = removeDupWord(read_str);
      args2.push_back(NULL);
      char *args[] = {args2[0],args2[1],args2[2],args2[3]};
      execv("./lchild",args);
    }
      
  }
  while (wait(NULL) > 0);
  /////////////////////////voter/////////////////////////
  string voterFile = "pipevoter";
  int fd2 = open(voterFile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
  close(fd2);
  mkfifo(voterFile.c_str(), 0666);
  ////////////////////////////////////////////////////////////////////////size///////////////////////////////////////////////////////////////////////

ifstream F1;
    string line1, word1 = "";
    string addr1 = "";
    addr1 += string(argv[2])+ "/classifier_0" + ".csv";
    char* addrr1 = (char*) addr1.c_str();
    F1.open(addrr1);
    vector <vector <int> > f3; 
    getline(F1, line1);
    while(F1){
        int temp1 = 0;
        vector <int> row1;
        getline(F1, line1);
        stringstream s(line1);
        while(getline(s, word1, ',')){
            char* _word1 = (char *) word1.c_str();
            row1.push_back(atoi(_word1));
            temp1 = 1;
        }
        if(temp1 == 1)
            f3.push_back(row1);
    }

    string t = to_string(f3.size());
    F1.close();
///////////////////////////////////////////////////////////////////////////////////////
  int voter = fork();
  if(voter == 0){
    string numofnamedpipes = to_string(num);
    char *args[] = {(char *) voterFile.c_str(), (char *) numofnamedpipes.c_str(),(char *) t.c_str(),NULL};
    execv("./voter",args);
  }
  while (wait(NULL) > 0);
/////////////////////////////////calc result//////////////

    char buffer [100000];
    vector <int> pipefinalresult;
    int voterp;
    char* myfifo = (char *) voterFile.c_str();
    voterp = open(myfifo,O_RDONLY);
    int eof = read(voterp, buffer, 100000);
    buffer[eof] = '\0';
    pipefinalresult = removeDupWord2(buffer);

    ifstream F;
    string line, word = "";
    string addr = "";
    addr += string(argv[1])+ "/labels" + ".csv";
    char* addrr = (char*) addr.c_str();
    F.open(addrr);
    vector <vector <int> > f2; 
    getline(F, line);
    while(F){
        int temp = 0;
        vector <int> row;
        getline(F, line);
        stringstream s(line);
        while(getline(s, word, ',')){
            char* _word = (char *) word.c_str();
            row.push_back(atoi(_word));
            temp = 1;
        }
        if(temp == 1)
            f2.push_back(row);
    }
    F.close();
    int r = 0;
    for(int i = 0; i < f2.size(); i++){
      if(f2[i][0] == pipefinalresult[i]){
        r ++;
      }
    }
    float natije;
    natije = float(r) / float(f2.size()) * float(100);
    cout << fixed;
    cout << setprecision(2);
    cout << "Accuracy:" << " " << natije << "%" << endl;


  for(int i = 0; i < num; i++){
    string pipename = "namedpipe_" + to_string(i);
    char * myfifo = (char *) pipename.c_str();
    unlink(myfifo);
  }
  unlink(voterFile.c_str());

  return 0;
}
