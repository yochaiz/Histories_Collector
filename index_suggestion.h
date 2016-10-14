#ifndef INDEX_SUGGESTION_H_
#define INDEX_SUGGESTION_H_

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
using std::string;
using std::vector;
using std::cerr;
using std::endl;

template<class T>
class IndexSuggestion {
private:
	string subName;
	vector<T>* namesVec;
public:
	IndexSuggestion(string name,vector<T>* vec);
	IndexSuggestion(const IndexSuggestion& x);
	IndexSuggestion& operator=(const IndexSuggestion& x);
	vector<T>* getNamesVec() const;
	string getSubName() const;
	template<class G>
	friend bool operator<(const IndexSuggestion<G>& item1,const IndexSuggestion<G>& item2);
	~IndexSuggestion();
};

template<class T>
IndexSuggestion<T>::IndexSuggestion(string name, vector<T>* vec) :
		subName(name), namesVec(vec) {
}

template<class T>
IndexSuggestion<T>::IndexSuggestion(const IndexSuggestion& x) :
		subName(x.subName), namesVec(x.namesVec) {
	//cerr << "using IndexSuggestion copy c'tor" << endl;
}

template<class T>
IndexSuggestion<T>& IndexSuggestion<T>::operator=(const IndexSuggestion& x) {
	cerr << "using IndexSuggestion operator=" << endl;
	return *this;
}

template<class T>
IndexSuggestion<T>::~IndexSuggestion() {
}

template<class T>
vector<T>* IndexSuggestion<T>::getNamesVec() const {
	return namesVec;
}

template<class T>
string IndexSuggestion<T>::getSubName() const {
	return subName;
}

template<class G>
bool operator<(const IndexSuggestion<G>& item1, const IndexSuggestion<G>& item2) {
	assert(item1.namesVec);
	assert(item2.namesVec);
	if (item1.namesVec->size() == item2.namesVec->size())
		return (item1.subName < item2.subName);
	else
		return (item1.namesVec->size() < item2.namesVec->size());
}

#endif /* INDEX_SUGGESTION_H_ */
