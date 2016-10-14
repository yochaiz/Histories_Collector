#ifndef TRIE_NODE_H_
#define TRIE_NODE_H_

#include <vector>
#include <cassert>
#include <stddef.h> //NULL definitionusing std::vector;template<class T>class Node {
public:
	Node();
	~Node();
	char content();
	void setContent(char c);
	bool wordMarker();
	void setWordMarker();
	void initData();
	T* getData();
	Node<T>* findChild(char c);
	void appendChild(Node<T>* child);
	vector<Node<T>*> children();

private:
	char mContent;
	bool mMarker;
	vector<Node<T>*> mChildren;
	T* data;
};

template<class T>
Node<T>::Node() :
		mContent(' '), mMarker(false), data(NULL) {
}

template<class T>
Node<T>::~Node() {
	delete data;
}

template<class T>
Node<T>* Node<T>::findChild(char c) {
	for (int i = 0; i < (int) mChildren.size(); i++) {
		Node<T>* tmp = mChildren.at(i);
		if (tmp->content() == c) {
			return tmp;
		}
	}
	return NULL;
}

template<class T>
char Node<T>::content() {
	return mContent;
}

template<class T>
void Node<T>::setContent(char c) {
	mContent = c;
}

template<class T>
bool Node<T>::wordMarker() {
	return mMarker;
}

template<class T>
void Node<T>::setWordMarker() {
	mMarker = true;
}

template<class T>
void Node<T>::initData() {
	if (!data)
		data = new T();
}

template<class T>
T* Node<T>::getData() {
	return data;
}

template<class T>
void Node<T>::appendChild(Node<T>* child) {
	mChildren.push_back(child);
}

template<class T>
vector<Node<T>*> Node<T>::children() {
	return mChildren;
}

#endif /* TRIE_NODE_H_ */

/*
 template<class T>
 vector<T>& Node<T>::data() {
 return dataVec;
 }

 template<class T>
 vector<T>* Node<T>::dataPtr() {
 return &dataVec;
 }

 template<class T>
 int Node<T>::getDataVecSize() {
 return dataVec.size();
 }
 */
