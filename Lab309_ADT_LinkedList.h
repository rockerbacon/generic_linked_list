/*
General purpose simply linked list
	Vitor Andrade dos Santos, 2016
*/

#ifndef _LAB309_ADT_LINKED_LIST_H_
#define _LAB309_ADT_LINKED_LIST_H_

#include "Lab309_ADT_Container.h"
#include <stdlib.h>

namespace lab309 {
	template<typename dataType>
	class LList : public Container {

		template<typename any> friend class LList;

		protected:
			struct Node {
				Node *next;
				dataType data;
			};

			//attributes
			Node *first;

			//internal methods
			void copyChain (Node *&first, Node **last) const throw (std::bad_alloc);

		public:

			class Iterator {
				private:
					Node *cursor;

				public:
					Iterator (void) = default;
					Iterator (Node *cursor) {
						this->cursor = cursor;
					}
					
					inline dataType& getData (void) {
						return this->cursor->data;
					}
					inline Iterator next (void) {
						return Iterator(this->cursor->next);
					}
					inline bool end (void) {
						return this->cursor == NULL;
					}

					//assignment operator
					Iterator& operator= (Node *node) {
						this->cursor = node;
						return *this;
					}

					Iterator& operator= (const Iterator &iterator) {
						this->cursor = iterator.cursor;
						return *this;
					}
					
					Iterator& operator++ (void) {
						this->cursor = this->cursor->next;
						return *this;
					}
			};

			//constructors and destructor
			LList (const LList<dataType> &list) throw (std::bad_alloc);
			LList (LList<dataType> &&list);
			LList (const std::initializer_list<dataType> &list) throw (std::bad_alloc);
			template<typename... dataPack> LList (dataPack... data) throw (std::bad_alloc);
			~LList (void);

			//getters
			size_t getIndex (dataType data) const throw (std::invalid_argument);
			inline Node* beginning (void) { return this->first; }

			//methods
			bool contains (const LList<dataType> &list) const;

			void add (size_t index, const LList<dataType> &list) throw (std::out_of_range, std::bad_alloc);

			void remove (const LList<dataType> &list) throw (std::invalid_argument);
			LList<dataType> removeIndex (LList<size_t> &&index) throw (std::invalid_argument, std::out_of_range, std::bad_alloc);
			void empty (void);

			void sort (void);	//iterative mergesort

			std::ostream& toStream (std::ostream &stream) const;

			//operators
			//assignment
			LList<dataType>& operator= (const LList<dataType> &list) throw (std::bad_alloc);
			LList<dataType>& operator= (LList<dataType> &&list);

			//relational operators
			bool operator== (const LList<dataType> &list) const;
			bool operator!= (const LList<dataType> &list) const;

			//array subscripting
			dataType& operator[] (size_t index) const throw (std::out_of_range);


	};
}

/*IMPLEMENTATION*/
/*INTERNAL METHODS*/
template<typename dataType>
void lab309::LList<dataType>::copyChain (Node *&first, Node **last) const throw (std::bad_alloc) {
	Node *cursor, *node, *previousNode;

	try {
		node = (Node*)malloc(sizeof(Node));
		if (node == NULL) {
			throw std::bad_alloc();
		}
		node->data = this->first->data;
		first = node;

		for (cursor = this->first->next; cursor != NULL; cursor = cursor->next) {
			previousNode = node;
			node = (Node*)malloc(sizeof(Node));
			if (node == NULL) {
				throw std::bad_alloc();
			}
			node->data = cursor->data;
			previousNode->next = node;
		}
		node->next = NULL;
		if (last != NULL) {
			*last = node;
		}
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

/*CONSTRUCTORS AND DESTRUCTOR*/
template<typename dataType>
lab309::LList<dataType>::LList (const LList<dataType> &list) throw (std::bad_alloc) {
	try {
		if (list.length > 0) {
			list.copyChain (this->first, NULL); //possible std::bad_alloc throw
			this->length = list.length;
		} else {
			this->first = NULL;
		}
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType>
lab309::LList<dataType>::LList (LList<dataType> &&list) {
	this->first = list.first;
	this->length = list.length;

	list.first = NULL;
}

template<typename dataType>
lab309::LList<dataType>::LList (const std::initializer_list<dataType> &list) throw (std::bad_alloc) {
	const size_t argc = list.size();
	const dataType *args = list.begin();
	Node *node, *previousNode;
	size_t i;

	try {
		if (argc > 0) {
			node = (Node*)malloc(sizeof(Node));
			if (node == NULL) {
				throw std::bad_alloc();
			}
			node->data = args[0];
			this->first = node;

			for (i = 1; i < argc; i++) {
				previousNode = node;
				node = (Node*)malloc(sizeof(Node));
				if (node == NULL) {
					throw std::bad_alloc();
				}
				node->data = args[i];
				previousNode->next = node;
			}
			node->next = NULL;

			this->length = argc;
		} else {
			this->first = NULL;
		}
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType> template<typename... dataPack>
lab309::LList<dataType>::LList (dataPack... optData) throw (std::bad_alloc) : LList<dataType> ({optData...}) {}

template<typename dataType>
lab309::LList<dataType>::~LList (void) {
	Node *remove, *next;
	for (remove = this->first; remove != NULL; remove = next) {
		next = remove->next;
		free(remove);
	}
}

/*GETTERS*/
template<typename dataType>
size_t lab309::LList<dataType>::getIndex (dataType data) const throw (std::invalid_argument) {
	Node *cursor;
	size_t index;
	try {
		for (cursor = this->first, index = 0; cursor != NULL; cursor = cursor->next, index++) {
			if (cursor->data == data) {
				return index;
			}
		}
		throw std::invalid_argument ("Error on LList<dataType>::getIndex(dataType data): list does not contain data");
	} catch (std::invalid_argument &error) {
		throw error;
	}
}

/*METHODS*/
template<typename dataType>
bool lab309::LList<dataType>::contains (const LList<dataType> &list) const {
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
void lab309::LList<dataType>::add (size_t index, const LList<dataType> &list) throw (std::out_of_range, std::bad_alloc) {
	Node *previous, *next, *first, *last;

	try {
		if (index > this->length) {
			throw std::out_of_range("Error on LList<dataType>::add(size_t index, const LList<dataType> &list): index exceeds list's boundaries");
		}

		list.copyChain(first, &last);	//possible std::bad_alloc throw
		for (previous = NULL, next = this->first; index > 0; index--) {
			previous = next;
			next = next->next;
		}

		if (previous != NULL) {
			previous->next = first;
		} else {
			this->first = first;
		}
		last->next = next;

		this->length += list.length;
	} catch (std::invalid_argument &error) {
		throw error;
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType>
void lab309::LList<dataType>::remove (const LList<dataType> &list) throw (std::invalid_argument) {
	Node *cursor, *removePrevious, *remove;
	try {
		if (list.length > this->length) {
			throw std::invalid_argument("Error on LList<dataType>::removeData (const LList<dataType> &list): attempted to remove more data than this list contains");
		}
		//for each data in list
		for (cursor = list.first; cursor != NULL; cursor = cursor->next) {
			//find node that contains data
			for (removePrevious = NULL, remove = this->first; remove != NULL; removePrevious = remove, remove = remove->next) {
				if (cursor->data == remove->data) {
					//remove node that contains data
					if (removePrevious != NULL) {
						removePrevious->next = remove->next;
					} else {
						this->first = remove->next;
					}
					free(remove);
					break;
				}
			}

			if (remove == NULL) {
				throw std::invalid_argument("Error on LList<dataType>::removeData(const LList<dataType> &list): list does not contain data in list");
			}
		}

		this->length -= list.length;
	} catch (std::invalid_argument &error) {
		throw error;
	}
}

template<typename dataType>
lab309::LList<dataType> lab309::LList<dataType>::removeIndex (LList<size_t> &&indexList) throw (std::invalid_argument, std::out_of_range, std::bad_alloc) {
	Node *remove, *removePrevious;
	LList<size_t>::Node *indexCursor, *indexCursorAux;
	LList<dataType> removed;
	size_t i;

	try {
		if (indexList.length > this->length) {
			throw std::invalid_argument ("Error on LList<dataType>::removeIndex(LList<size_t> &&indexList): attempted to remove more data than this list contains");
		}
		//for each index in indexList
		for (indexCursor = indexList.first; indexCursor != NULL; indexCursor = indexCursor->next) {
			if (indexCursor->data >= this->length) {
				throw std::out_of_range("Error on List<dataType>::remove(DlList<size_t> &&indexList): index exceeds boundaries");
			}

			//find node to be removed based on index
			removePrevious = NULL;
			remove = this->first;
			for (i = 0; i < indexCursor->data; i++) {
				removePrevious = remove;
				remove = remove->next;
			}

			//add to list of removed data and remove index
			removed.add(removed.length, remove->data);	//possible std::bad_alloc throw
			if (removePrevious != NULL) {
				removePrevious->next = remove->next;
			} else {
				this->first = remove->next;
			}
			this->length--;
			free(remove);

			//ajust indexes of list based on last removed index
			for (indexCursorAux = indexCursor->next; indexCursorAux != NULL; indexCursorAux = indexCursorAux->next) {
				if (indexCursorAux->data > indexCursor->data) {
					indexCursorAux->data--;
				}
			}

		}

		return removed;

	} catch (std::out_of_range &error) {
		throw error;
	}
}

template<typename dataType>
void lab309::LList<dataType>::empty (void) {
	Node *remove, *next;
	for (remove = this->first; remove != NULL; remove = next) {
		next = remove->next;
		free(remove);
	}
	this->first = NULL;
	this->length = 0;
}

template<typename dataType>
void lab309::LList<dataType>::sort (void) {

	size_t i, j;
	size_t partitionSize;
	Node *insertionPoint, *insertionPointPrevious;
	Node **insert, **insertPrevious, *insertNext;
	Node *cursorA, *cursorAPrevious;
	Node *cursorB, *cursorBPrevious;

	//repeat until partition size is bigger than list's size
	for (partitionSize = 1; partitionSize < this->length; partitionSize<<=1) {

		//set first two adjacent partitions
		insertionPoint = this->first;
		insertionPointPrevious = NULL;
		cursorA = this->first;
		cursorAPrevious = NULL;
		cursorB = this->first->next;
		cursorBPrevious = this->first;
		i = 1;	
		while (i < partitionSize && cursorB != NULL) {
			i++;
			cursorBPrevious = cursorB;
			cursorB = cursorB->next;
		}

		//for each pair of adjacent partitions
		while (cursorB != NULL) {

			//merge partitions
			i = j = 0;
			while (true) {

				//determine which data of adjacent partitions is to be inserted next
				if (cursorB != NULL && j < partitionSize) {
					if (i == partitionSize || cursorB->data < cursorA->data) {
						insert = &cursorB;
						insertPrevious = &cursorBPrevious;
						j++;
					} else {
						insert = &cursorA;
						insertPrevious = &cursorAPrevious;
						i++;
					}
				} else if (i < partitionSize) {
					insert = &cursorA;
					insertPrevious = &cursorAPrevious;
					i++;
				} else {
					break;
				}
				insertNext = (*insert)->next;

				//insert data at insertionPoint
				if (*insert != insertionPoint) {

					//remove node from its current position
					if (*insertPrevious != NULL) {
						(*insertPrevious)->next = (*insert)->next;
					} else {
						this->first = (*insert)->next;	
					}

					//insert node before insertionPoint
					if (insertionPointPrevious != NULL) {
						insertionPointPrevious->next = *insert;
					} else {
						this->first = *insert;
					}
					(*insert)->next = insertionPoint;

				} else {
					insertionPoint = insertionPoint->next;
				}

				insertionPointPrevious = *insert;
				*insert = insertNext;
			}

			//set beginning of next partitions
			cursorA = cursorB;
			cursorAPrevious = cursorBPrevious;
			for (i = 0; i < partitionSize && cursorB != NULL; i++, cursorBPrevious = cursorB, cursorB = cursorB->next);
		}
	}
}

template<typename dataType>
std::ostream& lab309::LList<dataType>::toStream (std::ostream &stream) const {
	Node *cursor;

	stream << '[';
	for (cursor = this->first; cursor != NULL; cursor = cursor->next) {
		stream << cursor->data;
		if (cursor->next != NULL) {
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
lab309::LList<dataType>& lab309::LList<dataType>::operator= (const LList<dataType> &list) throw (std::bad_alloc) {
	Node *nodeAux;

	try {
		if (this->first != list.first) {
			LList<dataType> copy(list);	//possible std::bad_alloc throw

			nodeAux = this->first;
			this->first = copy.first;
			copy.first = nodeAux;

			this->length = copy.length;
		}

		return *this;
	} catch (std::bad_alloc &error) {
		throw error;
	}
}

template<typename dataType>
lab309::LList<dataType>& lab309::LList<dataType>::operator= (LList<dataType> &&list) {
	Node *aux;

	if (this->first != list.first) {
		aux = this->first;
		this->first = list.first;
		list.first = aux;

		this->length = list.length;
	}

	return *this;
}

//EQUALITY
template<typename dataType>
bool lab309::LList<dataType>::operator== (const LList<dataType> &list) const {
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
bool lab309::LList<dataType>::operator!= (const LList<dataType> &list) const {
	return !(*this == list);
}

//ARRAY SUBSCRIPTING
template<typename dataType>
dataType& lab309::LList<dataType>::operator[] (size_t index) const throw (std::out_of_range) {
	Node *cursor;
	try {
		if (index >= this->length) {
			throw std::out_of_range("Error on LList<dataType>::operator[](size_t index): index exceeds list's boundaries");
		}
		for (cursor = this->first; index > 0; index--, cursor = cursor->next);

		return cursor->data;
	} catch (std::out_of_range &error) {
		throw error;
	}
}

#endif
