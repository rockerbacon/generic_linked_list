/*
General purpose doubly linked list
	Vitor Andrade, 2016
*/

#ifndef _LAB309_ADT_DOUBLY_LINKED_LIST_H_
#define _LAB309_ADT_DOUBLY_LINKED_LIST_H_

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include "Lab309_ADT_Container.h"

namespace lab309 {

	template<typename dataType>
	class List : public Container {

		template<typename any> friend class List;
	
		protected:
			struct Node {
				Node *previous;
				Node *next;
				dataType data;
			};

			//attributes
			Node *first;
			Node *last;

			//internal methods
			void removeNode (Node *node);
			void* getNode (size_t index) const;
			void copyChain (Node *&first, Node *&last) const throw (std::bad_alloc);
		
		public:

			class Iterator {
				friend class List<dataType>;
				private:
					Node *node;
					size_t index;

				public:
					//constructors
					Iterator (Node *node, size_t index);
					Iterator (void) : Iterator(NULL, 0) {}

					//getters
					size_t getIndex (void) const;
					dataType& getData (void);
					const dataType& getData (void) const;

					//methods
					Iterator next (void) const;
					Iterator previous (void) const;
					bool end (void) const;
					bool last (void) const;

					//operators
					Iterator& operator++ (int ignore);
					Iterator& operator-- (int ignore);
					bool operator== (const Iterator &iterator) const;
					bool operator!= (const Iterator &iterator) const { return !(*this == iterator); }
			};

			//constructors and destructors
			List (const List<dataType> &list) throw (std::bad_alloc);
			List (List<dataType> &&list);
			List (const std::initializer_list<dataType> &list) throw (std::bad_alloc);
			template<typename... dataPack> List (dataPack... data) throw (std::bad_alloc);
			~List (void);
		
			//getters
			size_t getIndex (const List<dataType> &list) const throw (std::invalid_argument);
			List<dataType> getSub (size_t beginning, size_t end) const throw (std::out_of_range, std::bad_alloc);	//[beginning, end[
			Iterator getBeginning (void) const;
			Iterator getEnd (void) const;

			//methods
			bool contains (const List<dataType> &list) const;

			void add (size_t index, const List<dataType> &list) throw (std::out_of_range, std::bad_alloc);

			size_t remove (const List<dataType> &list) throw (std::invalid_argument);
			void remove (List<dataType>::Iterator &iterator);
			List<dataType> removeIndex (List<size_t> &&indexList) throw (std::out_of_range, std::bad_alloc);
			void empty (void);

			void sort (void);	//iterative mergesort

			std::ostream& toStream (std::ostream &stream) const;

			//operators
			//assignment
			List<dataType>& operator= (const List<dataType> &list) throw (std::bad_alloc);
			List<dataType>& operator= (List<dataType> &&list);

			//relational operators
			bool operator== (const List<dataType> &list) const;
			bool operator!= (const List<dataType> &list) const;

			//array subscripting operator
			dataType& operator[] (size_t index) const throw (std::out_of_range);

	};

};

/*IMPLEMENTATION*/
/*ITERATOR*/
/*CONSTRUCTORS*/
template<typename dataType>
lab309::List<dataType>::Iterator::Iterator (Node *node, size_t index) {
	this->node = node;
	this->index = index;
}

/*GETTERS*/
template<typename dataType>
size_t lab309::List<dataType>::Iterator::getIndex (void) const {
	return this->index;
}

template<typename dataType>
dataType& lab309::List<dataType>::Iterator::getData (void) {
	return this->node->data;
}

template<typename dataType>
const dataType& lab309::List<dataType>::Iterator::getData (void) const {
	return this->node->data;
}

/*METHODS*/
template<typename dataType>
typename lab309::List<dataType>::Iterator lab309::List<dataType>::Iterator::next (void) const {
	if (this->node != NULL) {
		return Iterator(this->node->next, this->index+1);
	} else {
		return *this;
	}
}

template<typename dataType>
typename lab309::List<dataType>::Iterator lab309::List<dataType>::Iterator::previous (void) const {
	if (this->node != NULL) {
		return Iterator(this->node->previous, this->index-1);
	} else {
		return *this;
	}
}

template<typename dataType>
bool lab309::List<dataType>::Iterator::end (void) const {
	return !this->node;
}

template<typename dataType>
bool lab309::List<dataType>::Iterator::last (void) const {
	return !this->node->next;
}

/*OPERATORS*/
template<typename dataType>
typename lab309::List<dataType>::Iterator& lab309::List<dataType>::Iterator::operator++ (int ignore) {
	if (this->node != NULL) {
		this->node = this->node->next;
		this->index++;
	}
	return *this;
}

template<typename dataType>
typename lab309::List<dataType>::Iterator& lab309::List<dataType>::Iterator::operator-- (int ignore) {
	if (this->node != NULL) {
		this->node = this->node->previous;
		this->index--;
	}
	return *this;
}

template<typename dataType>
bool lab309::List<dataType>::Iterator::operator== (const Iterator &iterator) const {
	return this->node == iterator.node;
}

/*LIST*/
/*INTERNAL METHODS*/
template<typename dataType>
void lab309::List<dataType>::removeNode (Node *node) {

	if (this->length > 1) {

		if (node == this->first) {
			node->next->previous = NULL;
			this->first = node->next;
		} else if (node == this->last) {
			node->previous->next = NULL;
			this->last = node->previous;
		} else {
			node->next->previous = node->previous;
			node->previous->next = node->next;
		}

	} else {
		this->first = NULL;
		this->last = NULL;
	}

	this->length--;

	free(node);
}

template<typename dataType>
void* lab309::List<dataType>::getNode (size_t index) const {
	Node *node;

	if (index >= this->length) {
		return NULL;
	}

	if (index < this->length / 2) {
		node = this->first;
		while (index > 0) {
			node = node->next;
			index--;
		}
	} else {
		node = this->last;
		index = this->length - index - 1;
		while (index > 0) {
			node = node->previous;
			index--;
		}
	}

	return node;
}

template<typename dataType>
void lab309::List<dataType>::copyChain (Node *&first, Node *&last) const throw (std::bad_alloc) {
	Node *cursor, *node, *previousNode;

	try {
		node = (Node*)malloc(sizeof(Node));
		if (node == NULL) {
			throw std::bad_alloc();
		}
		node->previous = NULL;
		node->data = this->first->data;
		first = node;

		previousNode = node;
		for (cursor = this->first->next; cursor != NULL; cursor = cursor->next) {
			node = (Node*)malloc(sizeof(Node));
			if (node == NULL) {
				throw std::bad_alloc();
			}
			node->previous = previousNode;
			node->data = cursor->data;
			previousNode->next = node;
			previousNode = node;
		}
		node->next = NULL;
		last = node;
	} catch (std::bad_alloc &error) {
		throw error;
	}

}

/*CONSTRUCTORS AND DESTRUCTOR*/
template<typename dataType>
lab309::List<dataType>::List (const List<dataType> &list) throw (std::bad_alloc) {

	try {
		if (list.length > 0) {
			list.copyChain(this->first, this->last);	//possible std::bad_alloc throw
			this->length = list.length;
		} else {
			this->first = NULL;
			this->last = NULL;
		}
	} catch (std::invalid_argument &error) {
		throw error;
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType>
lab309::List<dataType>::List (List<dataType> &&list) {
	Node *aux;

	this->first = list.first;
	this->last = list.last;
	this->length = list.length;

	list.first = NULL;
}

template<typename dataType>
lab309::List<dataType>::List (const std::initializer_list<dataType> &list) throw (std::bad_alloc) {
	Node *node, *previousNode;
	size_t i;
	const size_t argc = list.size();
	const dataType *args = list.begin();

	try {

		if (argc > 0) {
			node = (Node*)malloc(sizeof(Node));
			if (node == NULL) {
				throw std::bad_alloc();
			}
			node->previous = NULL;
			node->data = args[0];
			this->first = node;

			previousNode = node;
			for (i = 1; i < argc; i++) {
				node = (Node*)malloc(sizeof(Node));
				if (node == NULL) {
					throw std::bad_alloc();
				}
				node->data = args[i];
				node->previous = previousNode;
				previousNode->next = node;
				previousNode = node;
			}
			node->next = NULL;
			this->last = node;

			this->length = argc;
		} else {
			this->first = NULL;
			this->last = NULL;
		}

	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType> template<typename... dataPack>
lab309::List<dataType>::List (dataPack... data) throw (std::bad_alloc) : List<dataType> ({data...}) { }

template<typename dataType>
lab309::List<dataType>::~List (void) {	
	Node *clear, *next;
	
	for (clear = this->first; clear != NULL; clear = next) {
		next = clear->next;
		free(clear);
	}	
}

/*GETTERS*/
template <typename dataType>
size_t lab309::List<dataType>::getIndex (const List<dataType> &list) const throw (std::invalid_argument) {
	size_t i;
	Node *cursorA, *cursorB, *cursorL;

	try {

		if (list.length > this->length) {
			throw std::invalid_argument("Error on List<dataType>::getIndex(const List<dataType> &list): this list does not contain list");
		}

		cursorA = this->first;
		cursorL = list.first;
		for (i = 0; i <= this->length-list.length; i++) {
			cursorB = cursorA;
			if (cursorL != list.first) {
				cursorL = list.first;
			}
			while (cursorB->data == cursorL->data) {
				cursorL = cursorL->next;
				if (cursorL == NULL) {
					return i;
				}
				cursorB = cursorB->next;
			}
			cursorA = cursorA->next;
		}

		throw std::invalid_argument("Error on List<dataType>::getIndex(const List<dataType> &list): this list does not contain list");

	} catch (std::invalid_argument &error) {
		throw error;
	}
}

template<typename dataType>
lab309::List<dataType> lab309::List<dataType>::getSub (size_t beginning, size_t end) const throw (std::out_of_range, std::bad_alloc) {
	List<dataType> subList;
	Node *cursorBeginning, *cursorEnd;
	Node *node, *previousNode;

	try {
		if (end < beginning) {
			size_t aux = end;
			end = beginning;
			beginning = aux;
		}
		if (end > this->length) {
			throw std::out_of_range("Error on List<dataType>::getSub(size_t beginning, size_t end): end exceeds list's boundaries");
		}

		//get beginning and end of data set
		end -= beginning;
		subList.length = end;
		for (cursorBeginning = this->first; beginning > 0; beginning--, cursorBeginning = cursorBeginning->next);
		for (cursorEnd = cursorBeginning; end > 0; end--, cursorEnd = cursorEnd->next);

		//copy data set to sublist
		node = (Node*)malloc(sizeof(Node));
		if (node == NULL) {
			throw std::bad_alloc();
		}
		node->data = cursorBeginning->data;
		node->previous = NULL;
		subList.first = node;

		for (cursorBeginning = cursorBeginning->next; cursorBeginning != cursorEnd; cursorBeginning = cursorBeginning->next) {
			previousNode = node;
			node = (Node*)malloc(sizeof(Node));
			if (node == NULL) {
				throw std::bad_alloc();
			}
			previousNode->next = node;
			node->previous = previousNode;
			node->data = cursorBeginning->data;
		}
		node->next = NULL;
		subList.last = node;

		return subList;

	} catch (std::invalid_argument &error) {
		throw error;
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType>
typename lab309::List<dataType>::Iterator lab309::List<dataType>::getBeginning (void) const {
	return Iterator(this->first, 0);
}

template<typename dataType>
typename lab309::List<dataType>::Iterator lab309::List<dataType>::getEnd (void) const {
	return Iterator(this->last, this->length-1);
}

/*METHODS*/
template<typename dataType>
bool lab309::List<dataType>::contains (const List<dataType> &list) const {
	size_t i;
	Node *cursorA, *cursorB, *cursorL;

	if (list.length > this->length) {
		return false;
	}

	cursorA = this->first;
	cursorL = list.first;
	for (i = 0; i <= this->length-list.length; i++) {
		cursorB = cursorA;
		if (cursorL != list.first) {
			cursorL = list.first;
		}
		while (cursorB->data == cursorL->data) {
			cursorL = cursorL->next;
			if (cursorL == NULL) {
				return true;
			}
			cursorB = cursorB->next;
		}
		cursorA = cursorA->next;
	}

	return false;
}

template<typename dataType>
void lab309::List<dataType>::add (size_t index, const List<dataType> &list) throw (std::out_of_range, std::bad_alloc) {
	Node *move, *chainFirst, *chainLast;
	try {
		if (index > this->length) {
			throw std::out_of_range ("Error on List<dataType>::add(size_t index, const List<dataType> &list): index exceeds list's boundaries");
		} 

		if (list.length > 0) {
			list.copyChain(chainFirst, chainLast);	//possible std::bad_alloc throw

			move = (Node*)this->getNode(index);
			if (this->length == 0) {
				this->first = chainFirst;
				this->last = chainLast;
			} else {
				if (move == NULL) {
					this->last->next = chainFirst;
					chainFirst->previous = this->last;
					this->last = chainLast;
				} else {
					if (index != 0) {
						move->previous->next = chainFirst;
						chainFirst->previous = move->previous;
					} else {
						this->first = chainFirst;
					}
					move->previous = chainLast;
					chainLast->next = move;
				}
			}

			this->length += list.length;
		}
	} catch (std::invalid_argument &error) {
		throw error;
	} catch (std::bad_alloc &error) {
		throw error;
	} catch (std::out_of_range &error) {
		throw error;
	}
}

template<typename dataType>
size_t lab309::List<dataType>::remove (const List<dataType> &list) throw (std::invalid_argument) {
	size_t index;
	Node *removeFirst, *removeLast;
	Node *cursor, *next;

	try {

		if (list.length > this->length) {
			throw std::invalid_argument("Error on List<dataType>::remove(const List<dataType> &list): list contains more data than this list");
		}

		//find location where data to be removed is
		removeFirst = this->first;
		cursor = list.first;
		for (index = 0; index <= this->length-list.length; index++) {
			removeLast = removeFirst;
			if (cursor != list.first) {
				cursor = list.first;
			}
			while (removeLast->data == cursor->data) {
				cursor = cursor->next;
				removeLast = removeLast->next;
				if (cursor == NULL) {
					goto REMOVE;
				}
			}
			removeFirst = removeFirst->next;
		}

		throw std::invalid_argument("Error on List<dataType>::remove(const List<dataType> &list): this list does not contain list");

		//remove data from this list
		REMOVE:
		for (cursor = removeFirst; cursor != removeLast; cursor = next) {
			next = cursor->next;
			this->removeNode(cursor);
		}

		return index;

	} catch (std::invalid_argument &error) {
		throw error;
	}
	
}

template<typename dataType>
void lab309::List<dataType>::remove (List<dataType>::Iterator &iterator) {
	Node *remove = iterator.node;
	iterator++;
	this->removeNode(remove);
}

template<typename dataType>
lab309::List<dataType> lab309::List<dataType>::removeIndex (List<size_t> &&indexList) throw (std::out_of_range, std::bad_alloc) {
	Node *remove;
	size_t indexCursor;
	size_t index, auxIndex;
	List<dataType> removed;
	
	try {
		for (indexCursor = 0; indexCursor < indexList.getLength(); indexCursor++) {
			index = indexList[indexCursor];
			if (index >= this->length) {
				throw std::out_of_range("Error on List<dataType>::remove(List<size_t> &&indexList): index exceeds boundaries");
			}

			remove = (Node*)this->getNode(index);
			removed.add(removed.length, remove->data);	//possible std::bad_alloc throw
			if (indexCursor != indexList.getLength()-1) {
				for (auxIndex = indexCursor; auxIndex < indexList.getLength(); auxIndex++) {
					if (indexList[auxIndex] > index) {
						indexList[auxIndex]--;
					}
				}
			}

			this->removeNode(remove);
		}

		return removed;

	} catch (std::out_of_range &error) {
		throw error;
	}
}

template<typename dataType>
void lab309::List<dataType>::empty (void) {
	Node *cursor, *next;
	for (cursor = this->first; cursor != NULL; cursor = next) {
		next = cursor->next;
		free(cursor);
	}
	this->first = NULL;
	this->last = NULL;
	this->length = 0;
}

template<typename dataType>
void lab309::List<dataType>::sort (void) {

	size_t i, j;
	size_t partitionSize;
	Node *insertionPoint, **insert;
	Node *cursorA, *cursorB, *next;

	for (partitionSize = 1; partitionSize < this->length; partitionSize <<=1) {

		//set beginning of adjacent partitions
		insertionPoint = this->first;
		cursorA = this->first;
		for (i = 0, cursorB = this->first; i < partitionSize && cursorB != NULL; i++, cursorB = cursorB->next);
		while (cursorB != NULL) {

			//merge partitions
			i = j = 0;
			while (true) {

				if (cursorB != NULL && j < partitionSize) {
					if (i == partitionSize || cursorB->data < cursorA->data) {
						insert = &cursorB;
						j++;
					} else {
						insert = &cursorA;
						i++;
					}
				} else if (i < partitionSize) {
					insert = &cursorA;
					i++;
				} else {
					break;
				}

				next = (*insert)->next;
				if (*insert != insertionPoint) {

					if (*insert != this->first) {
						(*insert)->previous->next = (*insert)->next;
					} else {
						this->first = (*insert)->next;
					}
					if (*insert != this->last) {
						(*insert)->next->previous = (*insert)->previous;
					} else {
						this->last = (*insert)->previous;
					}

					(*insert)->previous = insertionPoint->previous;
					(*insert)->next = insertionPoint;

					if (insertionPoint != this->first) {
						insertionPoint->previous->next = *insert;
					} else { 
						this->first = *insert;
					}
					insertionPoint->previous = *insert;

				} else {
					insertionPoint = insertionPoint->next;
				}

				*insert = next;
			}

			//set beginning of next partitions
			cursorA = cursorB;
			for (i = 0; i < partitionSize && cursorB != NULL; i++, cursorB = cursorB->next);
		}
	}
}

template<typename dataType>
std::ostream& lab309::List<dataType>::toStream (std::ostream &stream) const {
	Node *cursor;

	stream << '[';
	for (cursor = this->first; cursor != NULL; cursor = cursor->next) {
		stream << cursor->data;
		if (cursor != this->last) {
			stream << ", ";
		} else {
			stream << ']';
		}
	}

	return stream;
}

/*OPERATORS*/
//ASSIGNMENT
template<typename dataType>
lab309::List<dataType>& lab309::List<dataType>::operator= (const List<dataType> &list) throw (std::bad_alloc) {
	Node *nodeAux;

	try {
		List<dataType> copy(list);	//possible std::bad_alloc throw

		nodeAux = this->first;
		this->first = copy.first;
		copy.first = nodeAux;

		this->last = copy.last;
		this->length = copy.length;

		return *this;
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType>
lab309::List<dataType>& lab309::List<dataType>::operator= (List<dataType> &&list) {
	Node *aux;

	aux = this->first;
	this->first = list.first;
	list.first = aux;

	this->last = list.last;
	this->length = list.length;

	return *this;
}

//EQUALITY
template<typename dataType>
bool lab309::List<dataType>::operator== (const List<dataType> &list) const {
	Node *cursorA, *cursorB;

	if (this->length != list.length) {
		return false;
	}

	cursorA = this->first;
	cursorB = list.first;
	while (cursorA != NULL) {
		if (cursorA->data != cursorB->data) {
			return false;
		}
		cursorA = cursorA->next;
		cursorB = cursorB->next;
	}

	return true;
}

template<typename dataType>
bool lab309::List<dataType>::operator!= (const List<dataType> &list) const {
	return !(*this == list);
}

//ARRAY SUBSCRIPTING
template<typename dataType>
dataType& lab309::List<dataType>::operator[] (size_t index) const throw (std::out_of_range) {
	Node *node;
	try {
		node = (Node*)this->getNode(index);
		if (node == NULL) {
			throw std::out_of_range("Error on List<dataType>::operator[](size_t index): index exceeds list's boundaries");
		}
		return node->data;
	} catch (std::out_of_range &error) {
		throw error;
	}
}

#endif
