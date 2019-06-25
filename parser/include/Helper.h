#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "LinkedListAPI.h"

#define ARRSIZE 10000

typedef struct status {
	//index
	int i;
	//Errorcode
	ICalErrorCode code;

} Status;

char * getFirstandSecond(char **darray, int index,int size);
int length(char ** array);
void freeFileAndArray(char *fileArray,char **darray,int sizeofDarray);
int parseEvent(char **darray,int index, Calendar** obj);
int parseAlarm(char **darray,int index, Event **evt);
ICalErrorCode duplicate(char **darray,int size);
ICalErrorCode invalidFile(char* fileName);
ICalErrorCode invalidCalendar(char **darray,int size);
Status verifyEvent(char ** darray, int i, int size);
Status verifyAlarm(char ** darray, int index, int size);

ICalErrorCode verifyForFileWriting(char* fileName);
char* printEventDatetime(void *toBePrinted);
char* printEventProperties(Event* event);
char* printEventAlarms(Event* event);
char* printAlarmProperties(Alarm* alarm);

ICalErrorCode validateEvent(Event*event);
ICalErrorCode validEventProperty(char *string);
ICalErrorCode validateAlarm(Alarm *alarm);

bool comparePropertiesBool(const void* first, const void* second);

#endif
