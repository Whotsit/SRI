//Query.cpp
/*
	Query handles interactions between databases. Allow databases to query each other
	for truth values of targetted facts or rules.
*/
#include<iostream>
#include<cstdio>
#include<future>
#include<thread>
#include<mutex>
//#include<async>

#include "Query.h"
#include "BoundedBuffer.h"
#include "Producer.cpp"
#include "Consumer.cpp"

using namespace std;

Query::Query(KB * knowledge, RB * rules) {
	kb = knowledge;
	rb = rules;
}

//Grandfather->vector
//[0]: [John, John]
//[1] : [Bob, Mary]

map<string, vector<vector<string>> > Query::inference(vector<string> newFact){ //(Father,bob, " ", jerry,etc)
	string relation = newFact[0];
	//int reqSize = newFact.size() - 1;
	vector<string> actors;
	//cout << "Entered Inference" << endl;
	for (int i = 1; i < newFact.size(); i++) {
		actors.push_back(newFact[i]);
		//cout << "INFERENCE: Actors:" << newFact[i] << endl;
	}

	map<string, vector<vector<string>> > output;
	//SORT OUT LOGIC OPS HERE
	vector<string> path;

	if (rb->rules.count(relation) == 1) {
		//cout << "Found in KB and RB! Evaluating assosciated Rule" << endl;
		Rule * r = rb->rules[relation];
		cout << "INFERENCE: In rulebase=" << ruleEvaluate(r,actors) << endl;
		if (ruleEvaluate(r, actors)) {
			cout << "INFERENCE: Rule Evaluated! Iterating now!" << endl;
			string temp;
			for (int i = 0; i < actors.size(); i++){
				vector<string> buffer;
				if (actors[i] != "_") buffer.push_back(actors[i]);
				else{
					for (int j = 0; j < r->components.size(); j++) {
						temp = r->components[j][0];
						cout << "INFERENCE: ITERATING" << endl;
						vector< vector<string> >path = traverse(actors[i], kb->FactMap[temp], buffer);

						cout << "INFERENCE: path.size=" << path.size() << endl;
						output[relation].insert(output[relation].end(), path.begin(), path.end());
					}
				}
			}
		}//somehow return an empty output?
	}
	cout << "Iteration Complete! Output:" << endl;
	//output = removeDoubles(output);
	for (int x = 0; x < output[relation].size(); x++){
		for (int m = 0; m < output[relation][x].size(); m++){
			//cout << "INFERENCE: Output=" << output[relation][x][m] << endl;
		}
	}
	return output;
}

bool Query::ruleEvaluate(Rule * r, vector<string> actors) {
	//cout << "Entered RuleEvaluation!" << endl;
	//bool truthValues = false;
	int ops = r->ops;
	string name = r->name;
	if (factEvaluate(actors, name)) return true;
	else if (ops == 0) return operateOR(name, actors, r);
	//else if (ops == 1) return operateAND(name, actors, r);
	else return false;//placeholder
}

bool Query::operateOR(string name, vector<string> actors, Rule * r){
	std::future<bool> finalValue;
  bool finalTruth;
	for (int i = 0; i < r->components.size(); i++) {
		vector<string> nextActor;
		for (int n = 1; n < r->components[i].size(); n++) {
			cout << "operateOR: Actor=" << actors[stoi(r->components[i][n])] << endl;
			nextActor.push_back(actors[stoi(r->components[i][n])]);
		}
    bool truthValue = false;
		cout << "Entering EvalHelper with: " << r->components[i][0] << endl;
			cout << "EVALHELPER: " << r->components[i][0] << endl;
  if (rb->rules.count(r->components[i][0]) == 1) finalValue = std::async(std::launch::async, &Query::ruleEvaluate, this, rb->rules[r->components[i][0]], actors);
	else{
		cout << "Found in KB" << endl;
		finalValue = async(std::launch::async, &Query::factEvaluate, this, actors, r->components[i][0]);
	}
    truthValue = finalValue.get();
    if(truthValue) return true;
		cout << "RULEEVAL: finalValue: " << truthValue << endl;

		cout << "operateOR: Name=" << r->components[i][0] << "| Test=" << truthValue << endl;
		finalTruth = finalTruth || truthValue;
	}
	//cout << "operateOR: finalValue: " << truthValue << endl;
	return finalTruth;
}

/*bool Query::operateAND(string name, vector<string> actors, Rule * r){
	bool finalValue = true;
	for (int i = 0; i < r->components.size(); i++) {
		vector<string> nextActor;
		for (int n = 1; n < r->components[i].size(); n++) {
			cout << "operateAND: Actor=" << actors[stoi(r->components[i][n])] << endl;
			nextActor.push_back(actors[stoi(r->components[i][n])]);
		}
		cout << "Entering EvalHelper with: " << r->components[i][0] << endl;
    cout << "EVALHELPER: " << name << endl;
		if (factEvaluate(actors, name)) {
			cout << "Found in KB" << endl;
			return true;
		}else {
			if (rb->rules.count(name) == 1) return ruleEvaluate(rb->rules[name],actors);
			else return false;
		}
		cout << "RULEEVAL: finalValue: " << truthValue << endl;
		cout << "operateAND: Name=" << r->components[i][0] << "| Test=" << test << endl;
		if (test == false) return false;
		else finalValue = finalValue && test;
	}
	cout << "RULEEVAL: finalValue: " << finalValue << endl;
	return finalValue;
}*/



vector< vector<string>> Query::traverse(string actors, vector< vector<string> > actorList, vector<string> buffer) {
	vector< vector<string> > result;

	cout << "TRAVERSE: Actorlist.size=" << actorList.size() << endl;
	cout << "TRAVERSE: Actorlist[0].size=" << actorList[0].size() << endl;
	//cout << "TRAVERSE: actors.size=" << actors.size() << endl;

	int initSize = actorList[0].size();
	bool invalid = false;
	vector<string> path = buffer;
	for (int i = 0; i < initSize; i++) {
		for (int j = 0; j < actorList.size(); j++) {
			cout << "TRAVERSE: actorList[j][i]=" << actorList[j][i] << endl;
			cout << "TRAVERSE: actor=" << actors << endl;
			if (actorList[j].size() < initSize) {
				invalid = true;
				break;
			}
			else if (actors == "_") {
				path.push_back(actorList[j][i]);
				cout << "TRAVERSE: pushback " << actorList[j][i] << endl;
			}
			else {
				invalid = true;
				path.clear();
				break;
			}
		}
		if (invalid == true)break;
		cout << "TRAVERSE: path.size()=" << path.size() << "|actors.size()=" << actors.size() << endl;
		result.push_back(path);
	}
	return result;
}

bool Query::factEvaluate(vector<string> actors, string name) {
	vector< vector<string> > result;
	vector< vector<string> > actorList;
	bool isValid = false;
	//cout << "FACTEVAL: " << name << endl;
	if (kb->FactMap.count(name) >= 1) {
		//cout << "FACTEVAL: FOUND IN KB" << endl;
		actorList = kb->FactMap[name];
		int initSize = actorList[0].size();
		//cout << "FACTEVAL: InitSize = " << initSize << endl;
		bool broken = false;
		for (int i = 0; i < initSize; i++) {
			vector<string> path;
			bool matchFound = false;
			for (int j = 0; j < actorList.size(); j++) {
				//cout << "FACTEVAL: ActorList: " << actorList[j][i] << " :: Actors: " << actors[i] << endl;
				if (actorList[j].size() < initSize) {
					broken = true;
					break;
				}
				else if (actorList[j][i] == actors[j]) {
					//cout << "FACTEVAL: ActorList: "<<actorList[j][i]<<" :: Actors: "<< actors[i] << endl;
					matchFound = true;
					break;
				}
				else if(actors[j] == "_"){
					matchFound = true;
					break;
				}
				else {
					matchFound = false;
					break;
				}
				//isValid = matchFound;
			}
			isValid = matchFound;
			if (broken == true)break;
		}
	}
	//cout << "FACTEVAL: isValid " << isValid << endl;
	return isValid;
}

//Prints the results of the query to terminal
//Input: void
//Output: void
//void Query::printResults() {
//	for (int i = 0; i < Results.size(); i++) {
//		cout << Results.at(i)->toString();
//	}
//}

map<string, vector<string>> Query::removeDoubles(map<string, vector<string>>  target) {
	map<string, vector<string>> output;
	//placeholder method
	output = target;
	return output;
}
