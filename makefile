output: EnsembleClassifier.o lchild.o voter.o
	g++ -std=c++11 EnsembleClassifier.o -o EnsembleClassifier.out
	g++ -std=c++11 lchild.o -o lchild
	g++ -std=c++11 voter.o -o voter

EnsembleClassifier.o: EnsembleClassifier.cpp 
		g++ -std=c++11 -c EnsembleClassifier.cpp 

voter.o: voter.cpp 
		g++ -std=c++11 -c voter.cpp 

lchild.o: lchild.cpp
		g++ -std=c++11 -c lchild.cpp




