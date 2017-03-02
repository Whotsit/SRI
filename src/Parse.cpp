//Parse.cpp
#include<cstdio>
#include<string>
#include<iostream>
#include<algorithm>
#include<fstream>
#include "Parse.h"
using namespace std;

Parse::Parse(KB* knowledgeBase, RB* ruleBase){
	RuleBase = ruleBase;
	KnowledgeBase = knowledgeBase;
}

//substr's second argument is how far from first character to search to, not from what char to 
//what char so this function calculates that
int Parse::searchLength(int start, int end){
	//cout << end << "-" << start << "=" << end-start <<endl;
	return end - start;
}

void Parse::ParsePred(string input,bool factMode){
	//start parsing input
	//cout << "input in Pred: " << input << endl;
	vector<string> Entries;
	string currEntry;
	int nextLen;
	bool oneArg;
	int delimiter1 = input.find("(");
	string relationship = input.substr(0, delimiter1);
	//cout << relationship << endl;
	
	int delimiter2 = input.find(",",delimiter1); //.find() sets to -1 if not found I hope
	if(delimiter2 == -1) oneArg = true;
	int delimiter3 = input.find(")",delimiter2);
	if(delimiter3 == -1) delimiter3 = input.size();
	//cout << "when " << delimiter2 << "<" << delimiter3 << endl;
	while(delimiter2 < delimiter3){
		if(delimiter2 == -1){
			if(oneArg){
				delimiter2 = delimiter1;
			}
			delimiter1+=1;
			delimiter3;
			nextLen = searchLength(delimiter1,delimiter3);
			currEntry = input.substr(delimiter1,nextLen);
			//cout << "PredLast:" << currEntry << endl;
			Entries.push_back(currEntry);
			break;
		}
		nextLen = searchLength(delimiter1,delimiter2) -1; //determine search length
		currEntry = input.substr(delimiter1+1,nextLen); //parse out actor
		//cout << "PredLoop:" << currEntry << endl;
		Entries.push_back(currEntry); //add to vector of actors
		delimiter1 = delimiter2;
		delimiter2 = input.find(",",delimiter2+1);
	}
	if(factMode){
		Fact* newFact = new Fact(relationship, Entries); //Make new fact if is fact
		AddFact(newFact);
	}else{
		Predicate* newPred = new Predicate(relationship,Entries);
		Preds.push_back(newPred);
	}
}

void Parse::ParseRule(string input){
	int numRuns = numPreds(input);
	int searchStart;
	int searchEnd = input.find(")",0);
	int nextLen;
	
	for (int i = 0; i < numRuns - 1; i++) {
		searchStart = searchEnd + 1;
		searchEnd = input.find(")", searchEnd + 1);
		if (searchEnd == -1) searchEnd = input.size();

		//Gets Logic Operator and updates searchStart past it
		if (i % 2 == 0) {
			//First Logical Operator
			if (input[searchStart] == ':') {
				if (input[searchStart + 3] == 'A') { //Need to have store as boolean in Rule Component
					Logic.push_back(1);
					searchStart += 6;
				}
				else if (input[searchStart + 3] == 'O') {
					Logic.push_back(0);
					searchStart += 5;
				}
				//If additional Logical Operator
			}
			else if (input[searchStart + 1] == 'A' || input[searchStart + 1] == 'O') {
				if (input[searchStart + 1] == 'A') {
					Logic.push_back(1);
					searchStart += 4;
				}
				else if (input[searchStart + 1] == 'O') {
					Logic.push_back(0);
					searchStart += 3;
				}
			}
		}
		nextLen = searchLength(searchStart + 1, searchEnd);
		ParsePred(input.substr(searchStart + 1, nextLen), false);
	}

	AddRule(numRuns);
	Preds.clear();
	//for(int i=0; i<Logic.size();i++) cout<<Logic.at(i)<<endl;
	Logic.clear();
}


//Lets ParseLine know how many times to run ParseFunction on input
int Parse::numPreds(string input){
	//cout << "Inside numFunctin(): "<<input << endl;
	int numOpenParens = count(input.begin(),input.end(), '(');
	return numOpenParens;	
}


//Oversees all parsing on a single line of input
void Parse::ParseLine(string input){
	
	bool LOAD = false;
	bool DUMP = false;
	bool FACT = false;
	bool RULE = false;
	bool INFE = false; //short for INFERENCE
	bool DROP = false;
	
	numRuns = numPreds(input);
	//cout << numRuns;
	//cout<< "input: " << input <<endl;
	int searchStart = 0;
	int searchEnd = input.find(")");
	int nextLen = searchLength(searchStart, searchEnd);
	
	//Determine Command
	string command = input.substr(searchStart, 4);
	//cout << "command: " << command << endl;
	if(command == "LOAD"){
		LOAD = true;
		searchEnd = input.size()-1;
	}else if(command == "DUMP"){
		DUMP = true;
		searchEnd = input.size()-1;
	}else if(command == "FACT") FACT = true;
	else if(command == "RULE") RULE = true;
	else if(command == "INFE"){
		INFE = true;
		searchStart+=5;
	}else if(command == "DROP") DROP = true;
	searchStart += 5;
	nextLen = searchLength(searchStart, searchEnd);
	
	//Enact Command
	if(LOAD){
		ParseFile(input.substr(searchStart, nextLen));
		return;
	}else if(DUMP){
		string fileDump = KnowledgeBase->toString();
		DumpToFile(input.substr(searchStart, nextLen),fileDump);
		return;
	}else if(FACT){
		ParsePred(input.substr(searchStart, nextLen), true); 
		return;
	}else if(RULE){
		nextLen = searchLength(searchStart, input.size());
		ParseRule(input.substr(searchStart, nextLen));
		return;
	}else if(INFE){
	}else{ //DROP
		string searchingFor = input.substr(searchStart, nextLen+1);
		bool CheckFactinKB = KnowledgeBase->FactMap.count(searchingFor) != 0;
		//bool CheckRuleinRB = RuleBase->rules.count(searchingFor) != 0;
		if(CheckFactinKB){ //CheckFactinKB && CheckRuleinRB || CheckFactinKB
			KnowledgeBase->Remove(searchingFor);
		//}else if(CheckRuleinRB){
		}else{
			cout << searchingFor << " not in KB or RB to delete\n";
		}
		
	}
}

//Parses a file
void Parse::ParseFile(string fileName){
	string input;
	fstream file;
	file.open(fileName.c_str(),std::fstream::in);
	//Relationship.clear();
	//Entry.clear();
	cout<<"Inputting From File: "<< fileName << endl;
	while(!file.eof()){
	  getline(file,input);
	  ParseLine(input);
	}
	file.close();
}

//Called by DUMP command
void Parse::DumpToFile(string fileName,string input){
	fstream file;
	file.open(fileName.c_str(),std::fstream::out);
	file.write(input.c_str(),input.size());
	file.close();
}

//Parses single line of input from terminal
void Parse::ParseTerminalInput(){
	cout << "Enter 'q' to quit\n";
	string quit = "q";
	while(true){
		string input;
		getline(cin, input);
		//cout << input << endl;
		if(input.compare(quit) == 0) break;
		ParseLine(input);
	}
}

//Add Fact to KB once function is built
void Parse::AddFact(Fact* f){
	KnowledgeBase->Add(f);
}

//Creates rule from FactVector, Logic, and Relationship and puts it into the RB
void Parse::AddRule(int numFcns) {
	vector<Predicate*> tempPreds;
	vector<bool> tempLogic;
	int i;
	//vector<string> enactVars = Preds.at(0)->components;
	string fcnName = Preds.at(0)->name;
	
	for (i = 1; i < Preds.size(); i++) {
		tempPreds.push_back(Preds[i]);
	}

	for (i = 0; i < Logic.size(); i++) {
		tempLogic.push_back(Logic[i]);
	}

	Rule * newRule = new Rule(fcnName, tempLogic, tempPreds); //enactVars
	RuleBase->Add(newRule);

}

main(){
	KB* kb = new KB();
	RB* rb = new RB();
	Parse Parser = Parse(kb, rb);
	Parser.ParseFile("Dummy.SRL");
	string input = "FACT Alive(Roger)";
	string input2 = "FACT Father(Roger,John)";
	string input3 = "FACT Triplets(Roger,John,Jake)";
	string input4 = "FACT Quadruplets(Roger,John,Jake,Peter)";
	Parser.ParseLine(input);
	Parser.ParseLine(input2);
	Parser.ParseLine(input3);
	Parser.ParseLine(input4);
	Parser.ParseTerminalInput();
	cout << kb->toString();
	//for(int i=0; i<argc; i++) cout << "["<< i << "]: " << argv[i] << " ";
	cout << endl;
}
