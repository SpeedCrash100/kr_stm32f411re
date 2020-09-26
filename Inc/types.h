#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef enum Boolean {
	FALSE = 0,
	TRUE
} Boolean;


typedef enum MainStates {
	Stopped,
	Waiting,
	Started,
	Stopping
} MainStates;



#endif /* TYPES_H_ */
