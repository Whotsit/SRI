//knowledge base//
//#include<map>
#include<algorithm>
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
	//cout << fact->Relationship;
	//if(FactMap.find(fact->Relationship) == FactMap.end()){
	if(FactMap.count(fact->Relationship) == 0){ //If it doesn't exist
		vector<Fact*> v;
		v.push_back(fact);
		FactMap[fact->Relationship] = v; //May or may not be null cuz temporary variable
	}else{ //If it does exist
		vector<bool> diffChecker;			
		//if(f->actors == fact->actors){//check if fact already exists
		//	
			//return;
		//} 		
		int temp = FactMap[fact->Relationship].size();
		if(FactMap[fact->Relationship].size()!= 0){
				for(int j=0; j<FactMap[fact->Relationship].size(); j++){ //Iterating through Vector
					if(FactMap[fact->Relationship].at(j)->actors.size() == fact->actors.size()){ //if they have the same amount of actors
						diffChecker.assign(fact->actors.size(),false); //Mark no difference
						for(int k = 0; k<FactMap[fact->Relationship].at(j)->actors.size();k++){ //Iterate through actors
							if(FactMap[fact->Relationship].at(j)->actors.at(k) != fact->actors.at(k)){ //If actor pair is different
								diffChecker.at(k) = true; //mark difference
								break;
							}
						}
						for(int k =0; k<diffChecker.size(); k++){ //Iterate through diffChecker
							if(diffChecker.at(k)==true){ //If there is a difference
								FactMap[fact->Relationship].push_back(fact); //Add fact
								break;
							}
						}
					}
				}
				if(diffChecker.size()==0) FactMap[fact->Relationship].push_back(fact);
				diffChecker.clear();
		}else{
			FactMap[fact->Relationship].push_back(fact);
		}
		if(temp == FactMap[fact->Relationship].size()){
			cout<<"Fact already exists"<<endl;
		}
	}	
}

Fact* KB::Fetch(string r, vector<string> a) {
	for(Fact* f : FactMap[r]){
		if(f->actors == a) return f;
	}
	return NULL; //was nullptr (probably should still be)
} 

/*Fact* KB::Remove(Fact * fact){
	ptrdiff_t pos = find(FactMap[fact->Relationship].begin(), FactMap[fact->Relationship].begin(), fact) - FactMap[fact->Relationship].begin();
	Fact * temp = fact;
	FactMap[fact->Relationship].erase(pos);

	return temp;
}*/

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
