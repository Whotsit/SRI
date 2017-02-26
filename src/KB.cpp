//knowledge base//
//#include<map>
#include<algorithm>

#ifndef PREDICATE_H
#define PREDICATE_H
#endif

#include "KB.h"

using namespace std;

//Constructor, initializes as empty
KB::KB(){
	map<string, vector<Fact*> > * FactMap = new map<string, vector<Fact*> >();
}

//Destructor
KB::~KB(){
	FactMap.clear();
}

//Takes in a fact pointer (fact must be created outside of add) and adds it to
//the KB if it doesn't already exist
void KB::Add(Fact * fact){
	//checking if relationship exists in the KB
	if(FactMap.find(fact->Relationship) == FactMap.end()){ //If it doesn't exist
		vector<Fact*> v;
		v.push_back(fact);
		FactMap[fact->Relationship] = v; //May or may not be null cuz temporary variable
	}else{ //If it does exist
		for(Fact* f : FactMap[fact->Relationship]){
			if(f->actors == fact->actors){//check if fact already exists
				std::cout<< "Fact already exists" << std::endl;
				return;
			}
		}
		//after traversing the vector and finding no matches
		FactMap[fact->Relationship].push_back(fact); //add the fact to the end of the vector
	}
}

Fact* KB::Fetch(string r, vector<string> a) {
	for(Fact* f : FactMap[r]){
		if(f->actors == a) return f;
	}
	return NULL; //was nullptr (probably should still be)
}

Fact* KB::Remove(Fact * fact){
	vector<Fact*>::iterator pos = find(FactMap[fact->Relationship].begin(), FactMap[fact->Relationship].end(), fact);
	Fact * temp = fact;
	FactMap[fact->Relationship].erase(pos);

	return temp;
}

vector<Fact*>* KB::Find(string findKey){
	vector<Fact*>* ptr = &FactMap[findKey];
	return ptr;
}

string KB::toString(){
	string output = "";
	map<string, vector<Fact*> > ::iterator it = FactMap.begin();
	for(; it!= FactMap.end(); it++){
		for(int i=0; i< it->second.size();i++) output += it->second.at(i)->toString();
	}
	return output;
}
