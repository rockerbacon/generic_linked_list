/*
Basic definition of abstract class List
	Vitor Andrade, 2016
*/

#ifndef _LAB309_ADT_CONTAINER_H_
#define _LAB309_ADT_CONTAINER_H_

#include <iostream>

/*DEFINITION*/
class Container {
	
	//attributes
	protected:
		size_t length;
	
	public:
		//constructors and destructors
		Container (void) {
			this->length = 0;
		}
		
		//getters
		inline size_t getLength (void) const {
			return this->length;
		}

		inline bool isEmpty (void) const {
			return this->length == 0;
		}

		//methods
		virtual std::ostream& toStream (std::ostream &stream) const = 0;
		virtual void empty (void) = 0;

};

//stream operator
inline std::ostream& operator<< (std::ostream &stream, const Container &list) {
	return 	list.toStream(stream);
}

#endif