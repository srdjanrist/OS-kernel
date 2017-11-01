#ifndef _LOCK_H_
#define _LOCK_H_


extern volatile unsigned int lockFlag;

#define lock lockFlag = 0;

#define unlock lockFlag = 1;

#endif
