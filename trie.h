#ifndef TRIE_H_
#define TRIE_H_

#include "trie_node.h"
#include <string>
using std::string;

template<class T>
class Trie {
public:
	Trie();
	~Trie();
	Node<T>* addWord(string s);
	Node<T>* searchWord(string s);
	void deleteWord(string s);
	bool isEmpty() const;
private:
	Node<T>* root;
};

template<class T>
Trie<T>::Trie() {
	root = new Node<T>();
}

template<class T>
Trie<T>::~Trie() {
	// Free memory
	delete root;
}

template<class T>
bool Trie<T>::isEmpty() const {
	return (root->children().size() == 0);
}

template<class T>
Node<T>* Trie<T>::addWord(string s) {
	Node<T>* current = root;

	if (s.length() == 0) {
		current->setWordMarker(); // an empty word
		return current;
	}

	for (int i = 0; i < s.length(); i++) {
		Node<T>* child = current->findChild(s[i]);
		if (child != NULL) {
			current = child;
		} else {
			Node<T>* tmp = new Node<T>();
			tmp->setContent(s[i]);
			current->appendChild(tmp);
			current = tmp;
		}
		if (i == s.length() - 1) {
			current->setWordMarker();
			current->initData();
		}
	}
	return current;
}

template<class T>
Node<T>* Trie<T>::searchWord(string s) {
	Node<T>* current = root;

	while (current != NULL) {
		for (int i = 0; i < s.length(); i++) {
			Node<T>* tmp = current->findChild(s[i]);
			if (tmp == NULL)
				return NULL;
			current = tmp;
		}

		if (current->wordMarker()) {
			return current;
		} else
			return NULL;
	}
	return NULL;
}

#endif /* TRIE_H_ */
