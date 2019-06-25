#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "CalendarParser.h"
#include "Helper.h"
// validates properties in createCalendar
// PUTS DOUBLE DTSTART AND DTSTAMP IN PROPETY, so does main one.

char* alarmToJSON(const Alarm* alm);
char *alarmsToJSON(char *filename, int numEvent);
char *alarmListToJSON(const List* eventList);
char *createFile(char *filename, char * version, char * prodID, char * evUID, char * startDate, char *startTime);

char* propToJSON(const Property* alm);
char *propListToJSON(const List* eventList);
char *propsToJSON(char *filename, int numProp);

char *dataToEvents(char *filename, char *summary, char *uid, char *startDate, char *startTime, char *evstartDate, char *evstartTime, char*utc);

ICalErrorCode createCalendar(char* fileName, Calendar** obj) {

	if(invalidFile(fileName) != OK) {
		obj = NULL;
		return invalidFile(fileName);
	}

  FILE *fp;
  int size = 50;
  int i = 0;
	char * fileArray = calloc(size,sizeof(char));
  //char * fileArray = malloc(sizeof(char) * size); //try making this calloc.

	fp = fopen(fileName,"r");

  for(char c = getc(fp); c != EOF; c = getc(fp)) {
      if(i >= size) {
        size = size*size;
         fileArray = realloc(fileArray, size * sizeof(char));
      }
      fileArray[i] = c;
      i++;
  }

	fileArray[i] = '\0';

	/* check that fileArray has valid line endings */
	for(int f = 0; f < strlen(fileArray); f++) {
		if(fileArray[f] == '\n' && (f != 0)) { //fileArray at 0 = \n then can't check f-1
			if(fileArray[f-1] != '\r') {
				free(fileArray); //invalid but you still have to free
				return INV_FILE;
			}
		}
	}
	/* check that fileArray has valid line endings */

	char **darray = (char **)malloc(sizeof(char*) * size);

  int a = 0; //A IS HOW MANY I HAVE ALLOCATED AND HOW MANY I MUST FREE
  int b = 0;
  darray[a] = (char *)calloc(100,sizeof(char));

	//SAME CODE AS ABOVE BUT REMOVED THE || > 75 CONDITION //ALSO CHANGED CALLOC(100) TO CALLOC(200)
	for(int j = 0; j < i; j++) {
		if(j != 0 && (fileArray[j-1] == '\n')) { //just added this
			a++;
			darray[a] = (char *)calloc(200,sizeof(char)); //YES USE CALLOC because it initializes to 0
			b = 0;
		}
		darray[a][b] = fileArray[j];
		darray = (char **) realloc(darray, size * sizeof(char *));
		b++;
	}

	//darray[a++] = (char *)calloc(200,sizeof(char)); //YES USE CALLOC because it initializes to 0
	//strcpy(darray[a], "hello");

  fclose(fp);

	int darraySize = a;

	ICalErrorCode code = invalidCalendar(darray,darraySize);

	if(code != OK) {
		freeFileAndArray(fileArray,darray,a);
		obj = NULL;
		return code;
	}

	Calendar * tmpCal = malloc(sizeof(Calendar));

	tmpCal->events = initializeList(printEvent,deleteEvent,compareEvents);
	tmpCal->properties = initializeList(printProperty,deleteProperty,compareProperties); //no difference if i use &

  int start = 0;
  int end = a; //HARD CODED

  int tempVar = 0;
	i = start;

	//printf("what is end: %d\n",end);

	while(i < end) {
		char entireA[ARRSIZE];
    char firstA[100];
    char secondA[ARRSIZE]; //DON'T WANT MAGIC NUMBER

				// if(i == end) {
				// 	break;
				// }
				// if(darray[end - 1][0]== ';') {
				// 	break;
				// }

				// if(i == (end - 1)) {
				// 	if(darray[i][0]== ';') {
				// 		break;
				// 	}
				// 	//printf("ARE WE HERE??\n");
				// }

				while(darray[i][0] == ';') {
					i++;
				}

				//char entireA[10000] = "";
				int counterVar = 0;
				int tempIndex =  i + 1;
				while(darray[tempIndex][0] == ' ' || darray[tempIndex][0] == '\t') { //|| darray[tempIndex][0] == ';') {
					counterVar++;
					tempIndex++;
				}

				int newVar = 0; //don't want to read in the space or tab!
				int tempVar2 = 0;
				tempIndex = i;
				if(counterVar != 0) {

				for(int j = 0; j <= counterVar; j++) { //turn into while loop?
					while(darray[tempIndex][newVar] != '\n')  {
						entireA[tempVar2] = darray[tempIndex][newVar];
						newVar++;
						tempVar2++;

					}
					tempVar2--;
					newVar = 1;
					tempIndex++;
					i++;
				}
				i--;
				entireA[tempVar2] = '\0';
			}
			else {
				while(darray[i][tempVar] != '\n') {
					entireA[tempVar] = darray[i][tempVar];
					tempVar++;
				}
				entireA[tempVar] = '\0';
			}

				int firstVar = 0;
				int entireVar = 0;

				char tempChar = 'x';
				while(entireA[entireVar] !=  ':') {
					if(entireA[entireVar] == ';') {
						break;
					}
						tempChar = entireA[entireVar];
						firstA[firstVar] = tempChar;
						firstVar++;
						entireVar++;
				}
				firstA[firstVar] = '\0';
				int secondVar = 0;
				entireVar++; //skip ':'
				while(entireA[entireVar] !=  '\0') {
						secondA[secondVar] = entireA[entireVar];
						secondVar++;
						entireVar++;
				}
				secondA[secondVar] = '\0';
				for(int j = 0; j < strlen(secondA); j++) {
					if(secondA[j] == '\r') {
						secondA[j] = '\0';
					}
				}

    if(strcmp(firstA,"VERSION") == 0) {
      float versionNumber = atof(secondA);
      tmpCal->version = versionNumber;
    }
    else if(strcmp(firstA,"PRODID") == 0) {
      strcpy(tmpCal->prodID,secondA);
    }
    else if(strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VEVENT") == 0) {
      i = parseEvent(darray,i,&tmpCal);
    }
		else if(strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VCALENDAR") == 0) {
    	//do nothing.
    }
    else {
        Property* newProp = malloc(sizeof(Property) + 200 * sizeof(char)); //Only requires ONE FREE
        strcpy(newProp->propName,firstA);
        strcpy(newProp->propDescr,secondA);
        insertBack(tmpCal->properties, newProp); //CAN WE
    }

		i++;
    tempVar = 0; // set variables to 0
    memset(&entireA[0], 0, sizeof(entireA)); //clear array
    memset(&firstA[0], 0, sizeof(firstA)); //clear array
    memset(&secondA[0], 0, sizeof(secondA)); //clear array
  }

  *obj = tmpCal;
	int sizeofDarray = a;
	freeFileAndArray(fileArray,darray,sizeofDarray);

  return OK;
}

void deleteCalendar(Calendar* obj) {
  if(obj==NULL) {
    return;
  }
  if(obj->properties == NULL || obj->events == NULL) {
    return;
  }
  clearList(obj->events); //make sure not null
  clearList(obj->properties);
  free(obj->events);
  free(obj->properties);
  free(obj);
}

/********************EVENT***********************/
void deleteEvent(void* toBeDeleted) {
  Event* event = toBeDeleted;
  clearList(event->alarms);
  clearList(event->properties);
  free(event->alarms);
  free(event->properties);
  free(event);
}

int compareEvents(const void* first, const void* second) {
	return 0;
}

/********************ALARM***********************/
void deleteAlarm(void* toBeDeleted) {
  Alarm* alarm = toBeDeleted;
  clearList(alarm->properties);
  free(alarm->properties); //just added
  free(alarm->trigger);
  free(alarm);
}

int compareAlarms(const void* first, const void* second) {
	return 0;
}

/********************PROPERTRY***********************/
void deleteProperty(void* toBeDeleted) {
  Property* property = toBeDeleted;
  free(property);
}

int compareProperties(const void* first, const void* second) {
    return 0;
}

/********************DATE***********************/
void deleteDate(void* toBeDeleted) {
  DateTime* temp = toBeDeleted;
  DateTime date = *temp;
  strcpy(date.date, "");
  strcpy(date.time, "");

}

int compareDates(const void* first, const void* second) {
	return 0;
}

char* printError(ICalErrorCode err) {
	//can err = NULL, or a number that's not in the enum??
 	 /*{OK, INV_FILE, INV_CAL, INV_VER, DUP_VER, INV_PRODID, DUP_PRODID,
	 INV_EVENT, INV_DT, INV_ALARM, WRITE_ERROR, OTHER_ERROR } */
	char *error = malloc(sizeof(char)*100);

	if(err == OK) {
		strcpy(error,"OK");
	}
	else if(err == INV_FILE) {
		strcpy(error, "INV_FILE");
	}
	else if(err == INV_CAL) {
		strcpy(error, "INV_CAL");
	}
	else if(err == INV_VER) {
		strcpy(error, "INV_VER");
	}
	else if(err == DUP_VER) {
		strcpy(error, "DUP_VER");
	}
	else if(err == INV_PRODID) {
		strcpy(error, "INV_PRODID");
	}
	else if(err == DUP_PRODID) {
		strcpy(error, "DUP_PRODID");
	}
	else if(err == INV_EVENT) {
		strcpy(error, "INV_EVENT");
	}
	else if(err == INV_DT) {
		strcpy(error, "INV_DT");
	}
	else if(err == INV_ALARM) {
		strcpy(error, "INV_ALARM");
	}
	else if(err == WRITE_ERROR) {
		strcpy(error, "WRITE_ERROR");
	}
	else if(err == OTHER_ERROR) {
		strcpy(error, "OTHER_ERROR");
	}
	else{
		error = NULL;
	}

	//free(error); //should be freed by where ever this function is used, main?
	return error;
}
/*****************************************************************/

/***********PRINT FUNCTIONS*************/
char* printCalendar(const Calendar* obj) {

		if(obj == NULL) {
			return NULL;
		}
		int len = 100000;
		char * string = calloc(len, sizeof(char)); //number?

		sprintf(string,"\nBEGIN:VCALENDAR\nPRODID:%s\nVERSION:%.lf",obj->prodID,obj->version);

		char * stringProp = toString(obj->properties);
		char * stringEvents = toString(obj->events);

		if(stringProp != NULL) {
			if(strlen(string) >= len) {
				len = len*len;
				string = realloc(string, len);

			}
			sprintf(string + strlen(string),"%s",stringProp);

		}
		if(stringEvents != NULL) {
			if(strlen(string) >= len) {
				len = len*len;
				string = realloc(string, len);

			}
			sprintf(string + strlen(string),"%s",stringEvents);
		}

		// strcpy(string, stringProp);
		// strcat(string, stringAlarm);

		free(stringProp);
		free(stringEvents);

		sprintf(string  + strlen(string),"\nEND:VCALENDAR");

		return string;
}

char* printEvent(void* toBePrinted) {
	Event *event = toBePrinted;
	if(toBePrinted == NULL) { //should check for null?
		return NULL;
	}
	int len = 100000;
	char * string = calloc(len, sizeof(char)); //number?

	sprintf(string,"BEGIN:VEVENT\nUID:%s",event->UID);

	char * stringProp = toString(event->properties);
	char * stringAlarm = toString(event->alarms);

	if(stringProp != NULL) {
		if(strlen(string) >= len) {
			len = len*len;
			string = realloc(string, len);

		}
		sprintf(string + strlen(string),"%s",stringProp);
	}
	if(stringAlarm != NULL) {
		if(strlen(string) >= len) {
			len = len*len;
			string = realloc(string, len);

		}
		sprintf(string + strlen(string),"%s",stringAlarm);
	}

	sprintf(string  + strlen(string),"\nEND:VEVENT");

	// strcpy(string, stringProp);
	// strcat(string, stringAlarm);

	free(stringProp);
	free(stringAlarm);
	return string;
}

char* printAlarm(void* toBePrinted) {

	Alarm*alarm = toBePrinted;
	if(toBePrinted == NULL) { //should check for null?
		return NULL;
	}
	int len = 100000;
	char * string = calloc(100000, sizeof(char)); //number?

	sprintf(string,"BEGIN:VALARM\nTRIGGER:%s\nACTION:%s",alarm->trigger,alarm->action);

	char * stringProp = toString(alarm->properties);

	if(stringProp != NULL) {
		if(strlen(string) >= len) {
			len = len*len;
			string = realloc(string, len);

		}
		sprintf(string + strlen(string),"%s",stringProp);
	}

	sprintf(string  + strlen(string),"\nEND:VALARM");

	// strcpy(string, stringProp);
	// strcat(string, stringAlarm);

	free(stringProp);
	return string;
}

char* printProperty(void* toBePrinted) {
	Property* property = toBePrinted;
	char* buffer = calloc(10000, sizeof(char));

	sprintf(buffer,"%s:%s",property->propName,property->propDescr);

	return buffer;
 }

char* printDate(void* toBePrinted) {
	DateTime* date = toBePrinted;
	char* buffer = malloc(sizeof(DateTime));

	if(date->UTC == false) {
		sprintf(buffer, "%sT%s",date->date,date->time);
	}
	else {
		sprintf(buffer, "%sT%sZ",date->date,date->time);
	}
	return buffer; //JUST ADDED
}
/***********PRINT FUNCTIONS*************/

/**********FUNCTIONS FOR WRITE CALENDAR***********/
ICalErrorCode writeCalendar(char* fileName, const Calendar* obj) {
	FILE *fp;

	if(obj == NULL) {
		return WRITE_ERROR;
	}

	//printf("WHAT IS FILENAME: %s\n",fileName);

	if(verifyForFileWriting(fileName) != OK) {
		return verifyForFileWriting(fileName);
	}

	fp = fopen(fileName,"w");

	int len = 100000;
	char *buffer = calloc(len, sizeof(char));

	sprintf(buffer, "BEGIN:VCALENDAR\r\nVERSION:%f\r\nPRODID:%s\r\n",obj->version,obj->prodID);

	void *elem;
	ListIterator iter = createIterator(obj->properties);

	while((elem = nextElement(&iter)) != NULL) {
		Property *prop = elem; //don't need this...
		if(strlen(buffer) >= len) {
			len = len*len;
			buffer = realloc(buffer,len);
		}
		sprintf(buffer + strlen(buffer), "%s:%s\r\n",prop->propName,prop->propDescr);

	}

	iter = createIterator(obj->events);

	while((elem = nextElement(&iter)) != NULL) {
		Event*event = elem;
		sprintf(buffer + strlen(buffer), "BEGIN:VEVENT\r\nUID:%s\r\n",event->UID);

		char *strStartDateTime    = printEventDatetime((void*)&(event->startDateTime));
		char *strCreationDateTime = printEventDatetime((void*)&(event->creationDateTime));
		//char *strStartDateTime    = printEventDatetime((void*)&(event->startDateTime));
		char *strProp  =  printEventProperties(event);
		char *strAlarm =  printEventAlarms(event);

		if(strlen(buffer) >= len) {
			len = len*len;
			buffer = realloc(buffer,len);
		}

		sprintf(buffer + strlen(buffer), "DTSTAMP:%s",strCreationDateTime);
		sprintf(buffer + strlen(buffer), "DTSTART:%s",strStartDateTime);

		sprintf(buffer + strlen(buffer), "%s",strProp);
		sprintf(buffer + strlen(buffer), "%s",strAlarm);

		sprintf(buffer + strlen(buffer), "END:VEVENT\r\n");

		free(strStartDateTime);
		free(strCreationDateTime);
		free(strProp);
		free(strAlarm);
	}

	sprintf(buffer + strlen(buffer), "END:VCALENDAR\r\n"); //NEWLINE BEFOREHAND?

	fputs(buffer, fp);
	free(buffer);
	fclose(fp);
	return OK;
}


/**********FUNCTIONS FOR VALIDATE CALENDAR***********/
//prop names cannot be "", neither can propDescr"",
//test by creating your own drivers.
ICalErrorCode validateCalendar(const Calendar* obj) {

	if(obj == NULL) {
		return INV_CAL;
	}

	if(obj->events == NULL || obj->properties == NULL){
		 return INV_CAL;
 }
 //		 calscale / method / --> if these occur they can only occur once.

	void *elem;
	ListIterator iter = createIterator(obj->properties);
	int numCalscale = 0;
	int numMethod   = 0;
	while((elem = nextElement(&iter)) != NULL) {
		Property *prop = elem; //don't need this...
			if(strcmp(prop->propName, "BEGIN") == 0) {
				//printf("are we herer???\n");
				return INV_CAL;
			}
			else if(strcmp(prop->propName,"") == 0 || strcmp(prop->propDescr,"") == 0) {
				return INV_CAL;
			}
			else if(strcmp(prop->propName, "END") == 0) {
				return INV_CAL;
			}
			else if(strcmp(prop->propName, "CALSCALE") == 0) {
				if(numCalscale >= 1) {
					return INV_CAL;
				}
				numCalscale++;
			}
			else if(strcmp(prop->propName, "METHOD") == 0) {
				if(numMethod >= 1) {
					return INV_CAL;
				}
				numMethod++;
			}
			else{
				//empty
				//unrecognized property
				//can proably get rid of the begin/end if statements
				return INV_CAL;
			}
	}

	//events list in cal
	ICalErrorCode eventCode; //not initlized?
	iter = createIterator(obj->events);
	while((elem = nextElement(&iter)) != NULL) {
		Event *event = elem; //don't need this...
		eventCode    = validateEvent(event);
		if(eventCode != OK) {
			return eventCode;
		}
	}

	//event list cannot be null or empty
	// for(int i = 0; i < getLength(obj->events); i++) {
	// 	ListIterator iter = createIterator(obj->events);
	// 	validateEvent()
	//   elem = nextElement(&iter));
	//}

	//what's valid for a cal struct, read API header and get properties from spec
	//create necessay variables/counter
	//iterate through the properties list

	//check if the event/properties exist and are not null, event should have
	//at least one struct although is that not already accounted for?

	//call separate function from events that iterates through the event list and
	//returns an appropriate error code, check the value of the error code before
	//continuing

	return OK;
}

/*JSON FUNCTIONS*/

char* dtToJSON(DateTime prop) {

	char *string = calloc(100,sizeof(char));

	if(prop.UTC == true) {
		sprintf(string, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":true}",prop.date, prop.time);
	}
	else {
		sprintf(string, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":false}",prop.date, prop.time);
	}

	return string;
}

char* eventToJSON(const Event* event) {

	char *string = calloc(10000,sizeof(char));

	if(event == NULL) {
		sprintf(string,"{}");
		return string;
	}

	int numProps  = 3 + getLength(event->properties);
	int numAlarms = 0 + getLength(event->alarms);

	void *elem = findElement(event->properties, comparePropertiesBool, "SUMMARY");

	Property* property = (Property *)elem;

	//printf("%s\n",property->propName);
	char *dtString = dtToJSON(event->startDateTime);
	if(property == NULL) {
		sprintf(string, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"\"}",dtString,numProps, \
																																		 numAlarms);
	}
	else{
		sprintf(string, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\"}",dtString,numProps, \
																																		 numAlarms,property->propDescr);
	}
	free(dtString);
	return string;
}

char* eventListToJSON(const List* eventList) {
	int size = 10000;
	char *string = calloc(size,sizeof(char));

	if(eventList == NULL) {
		sprintf(string,"[]");
		return string;
	}

	List* list = (List*)eventList;
  Event *eventHead = getFromFront(list);
	char *evString = eventToJSON(eventHead);
  sprintf(string,"[%s]",evString);
	free(evString);

if(getLength(list) > 1) {

	//printf("getLength: %d\n",getLength(list));
  void *elem;
	ListIterator iter = createIterator(list);
	elem = nextElement(&iter); //skip the first one which we've already gotten.
	for(int i = 1; i < getLength(list); i++) {
			elem = nextElement(&iter);
			Event *event = elem;

			if(strlen(string) > size) {
				size = size*size;
				string = realloc(string, size);
			}
			char *eventString = eventToJSON(event);
			sprintf(string + strlen(string) - 1, ",%s]",eventString);
			free(eventString);
	}
}
	return string;
}

char* calendarToJSON(const Calendar* cal) {
	//if NULL what?? cal->events?
	int size = 1000;
	char *string = calloc(size,sizeof(char));

	if(cal->events == NULL) {
		sprintf(string,"{}");
		return string;
	}

	int numProps  = 2 + getLength(cal->properties);
	int numEvents = getLength(cal->events);

	int verInt = (int)cal->version;
	sprintf(string, "{\"version\":%d,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}",verInt, cal->prodID, \
                                                                                            numProps,numEvents);

	return string;
}

Calendar* JSONtoCalendar(const char* str) {

	if(str == NULL) {
		return NULL;
	}

	Calendar * cal = malloc(sizeof(Calendar));

	cal->events = initializeList(printEvent,deleteEvent,compareEvents);
	cal->properties = initializeList(printProperty,deleteProperty,compareProperties);

	char *pVer;
	pVer = strstr (str,"version");

	if(pVer == NULL) {
		return NULL;
	}

	char *p2 = calloc(strlen(str) + 1,sizeof(char));
	strcpy(p2,str);
  //p2 = strdup(str);

	char *token = strtok(p2,",");

	int loc = 0;
	for(int i = 0; i < strlen(token); i++) {
		loc++;
		if(token[i] == ':') {
			break;
		}
	}

	char ver[100];
	int x = 0;
	for(int j = loc; j < strlen(token); j++) {
		ver[x] = token[j];
		x++;
	}

	ver[x] = '\0';

	float numVer = atof(ver);
	cal->version = numVer;

	//printf("NUMVER: %f\n",numVer);

	free(p2);

	//printf("%f\n",numVer);

	//char *pid;
  //pid = strdup(str);
	 char *p;
   p = strstr (str,"prodID");

	 if(p == NULL) {
		 return NULL;
	 }

	 //char *pid;
	 char *pid = calloc(strlen(p) + 1,sizeof(char));
	 strcpy(pid,p);
   //pid = strdup(p);
	 //char *pID = calloc(1000,sizeof(char));
	 //strcpy(pID, p);
	 //printf("%s\n",p);
	 //printf("%s\n",pid);
	int locP = 0;
 	for(int i = 0; i < strlen(pid); i++) {
 		locP++;
 		if(pid[i] == ':') {
 			break;
 		}
 	}
	locP++; //to skip ':'
	//printf("%c\n",pid[locP]);
 	char verP[1000];
 	int xP = 0;
 	for(int j = locP; j < strlen(pid); j++) {
		// if(pid[j] == '\"' ) {
		// 	break;
		// }
		if(pid[j] == '}') {
			//printf("Everhere??\n");
 			break;
 		}
 		verP[xP] = pid[j];
 		xP++;
 	}
	verP[xP - 1] = '\0';
	//printf("%s\n",verP);
	strcpy(cal->prodID,verP);

	free (pid);

	return cal;
}

Event* JSONtoEvent(const char* str) {

	if(str == NULL) {
		return NULL;
	}
	int loc = 0;
	int i = 0;
	for(; i < strlen(str); i++) {
		if(str[i] == ':') {
			break;
		}
		loc++;
	}
	if(loc == strlen(str)) {
		return NULL;
	}

	// char *p2;
	// p2 = strdup(str);
	//
	// //char *token = strtok(p2,":");
	loc++; //skip ':'
	loc++; //skip '"'
	char id[1000];
	int j = 0;
	for(int i = loc; i < strlen(str);i++) {
		if(str[i] == '\"') {
			break;
		}
		id[j] = str[i];
		j++;
	}
	id[j] = '\0';

	//printf("%s\n",id);
	Event *new = malloc(sizeof(Event));
	new->properties = initializeList(printProperty,deleteProperty,compareProperties);
  new->alarms     = initializeList(printAlarm,deleteAlarm,compareAlarms);

	strcpy(new->UID,id);

	//printf("new->uID: %s\n",new->UID);
	return new;
}

void addEvent(Calendar* cal, Event* toBeAdded) {

	if(cal == NULL || toBeAdded == NULL) {
		return;
	}

	insertBack(cal->events, toBeAdded);

	return;
}

char *objectToJSON(char *filename) {
	char*dir = malloc(200);

	strcpy(dir,"./uploads/");
	strcat(dir,filename);

	Calendar *cal = NULL;

	ICalErrorCode err = createCalendar(dir, &cal);

	if(err != OK) {
		return "{}";
	}

	ICalErrorCode val = validateCalendar(cal);

	if(val != OK) {
		return "{}";
	}

	return calendarToJSON(cal);
}

char *eventsToJSON(char *filename) {
	char*dir = malloc(200);

	strcpy(dir,"./uploads/");
	strcat(dir,filename);

	Calendar *cal = NULL;

	ICalErrorCode err = createCalendar(dir, &cal);

	if(err != OK) {
		return "{}";
	}


	char* eventList = eventListToJSON((List*)cal->events);

	return eventList;
}

char *dataToEvents(char *filename, char *summary, char *uid, char *startDate, char *startTime, char *evstartDate, char *evstartTime, char*utc) {
	Calendar *cal = NULL;
	char*dir = malloc(200);
	strcpy(dir,"./uploads/");
	strcat(dir,filename);

	ICalErrorCode err = createCalendar(dir, &cal);

	if(err != OK) {
		return "{}";
	}

	Event *new = malloc(sizeof(Event));
	new->properties = initializeList(printProperty,deleteProperty,compareProperties);
	new->alarms     = initializeList(printAlarm,deleteAlarm,compareAlarms);

	if(strcmp(summary,"DNE") != 0) {
		Property *prop = malloc(sizeof(Property));
		strcpy(prop->propName,"SUMMARY");
		strcpy(prop->propDescr,summary);
		insertBack(new->properties, prop);
	}

	strcpy(new->UID,uid);
	// DateTime 	creationDateTime;
	//
	// //Event start date-time.
	// DateTime     startDateTime;
	strcpy(new->creationDateTime.date,startDate);
	strcpy(new->creationDateTime.time,startTime);

	strcpy(new->startDateTime.date,evstartDate);
	strcpy(new->startDateTime.time,evstartTime);

	if(strcmp(utc,"true") == 0) {
		new->startDateTime.UTC = true;
	}
	else{
		new->startDateTime.UTC = false;
	}

	addEvent(cal, new);

	ICalErrorCode errWrite = writeCalendar(dir, cal);

	if(errWrite != OK) {
		return "{}";
	}

	// err = createCalendar(dir, &cal);
 //
 // if(err != OK) {
	//  return "{}";
 // }

 char* string = eventToJSON(new);
 return string;
}


char *alarmListToJSON(const List* eventList) {
	int size = 10000;
  char *string = calloc(size,sizeof(char));

 if(eventList == NULL) {
	 sprintf(string,"[]");
	 return string;
 }

 List* list = (List*)eventList;
 Alarm*eventHead = getFromFront(list);
 char *evString = alarmToJSON(eventHead);
	sprintf(string,"[%s]",evString);
	//printf("%s\n",string);
 free(evString);

if(getLength(list) > 1) {

 //printf("getLength: %d\n",getLength(list));
 void *elem;
 ListIterator iter = createIterator(list);
 elem = nextElement(&iter); //skip the first one which we've already gotten.
 for(int i = 1; i < getLength(list); i++) {
		 elem = nextElement(&iter);
		 Alarm *event = elem;

		 if(strlen(string) > size) {
			 size = size*size;
			 string = realloc(string, size);
		 }
		 char *eventString = alarmToJSON(event);
		 sprintf(string + strlen(string) - 1, ",%s]",eventString);
		 free(eventString);
 }
}
 return string;
}

char* alarmToJSON(const Alarm* alm) {

	char *string = calloc(10000,sizeof(char));

	if(alm == NULL) {
		sprintf(string,"{}");
		return string;
	}
	int numProps  = 2 + getLength(alm->properties);

	sprintf(string, "{\"action\":\"%s\",\"trigger\":\"%s\",\"numProps\":%d}",alm->action,alm->trigger,numProps);

	return string;
}

char *alarmsToJSON(char *filename, int numEvent) {
	char*dir = malloc(200);

	strcpy(dir,"./uploads/");
	strcat(dir,filename);

	Calendar *cal = NULL;

	ICalErrorCode err = createCalendar(dir, &cal);

	if(err != OK) {
		return "{}";
	}

	void *elem;
	ListIterator iter = createIterator(cal->events);
	//elem = nextElement(&iter); //skip the first one which we've already gotten.
	for(int i = 1; i <= numEvent ; i++) {
			elem = nextElement(&iter);
			Event *event = elem;

			if(i == numEvent) {
				//printf("UID: %s\n",event->UID);
				char *alarmString = alarmListToJSON(event->alarms);
				deleteCalendar(cal);
				return alarmString;
			}
		}
	deleteCalendar(cal);
	return NULL;
}


char *createFile(char *filename, char * version, char * prodID, char * evUID, char * startDate, char *startTime) {
	Calendar * cal = malloc(sizeof(Calendar));

	cal->events = initializeList(printEvent,deleteEvent,compareEvents);
	cal->properties = initializeList(printProperty,deleteProperty,compareProperties);

	double x = atof(version);
	cal->version = x;
	strcpy(cal->prodID,prodID);

	// printf("CAL->PRODID: %s\n",cal->prodID);
	// printf("CAL->VERSION: %f\n",cal->version);

	Event* event = malloc(sizeof(Event));
	event->alarms = initializeList(printAlarm,deleteAlarm,compareAlarms);
	event->properties = initializeList(printProperty,deleteProperty,compareProperties);
	strcpy(event->UID,evUID);

	//printf("event->UID: %s\n",event->UID);

	strcpy(event->creationDateTime.date,startDate);
	strcpy(event->creationDateTime.time,startTime);

	strcpy(event->startDateTime.date,startDate);
	strcpy(event->startDateTime.time,startTime);

	addEvent(cal, event);

	//printf("size: %d\n",getLength(cal->events));

	char*dir = malloc(200);

	strcpy(dir,"./uploads/");
	strcat(dir,filename);

	ICalErrorCode errWrite = writeCalendar(dir, cal);

	if(errWrite != OK) {
		return "{}";
	}

	ICalErrorCode err = createCalendar(dir, &cal);

	if(err != OK) {
		return "{}";
	}

	return calendarToJSON(cal);
}


char *propsToJSON(char *filename, int numEvent) {
	char*dir = malloc(200);

	strcpy(dir,"./uploads/");
	strcat(dir,filename);

	Calendar *cal = NULL;

	ICalErrorCode err = createCalendar(dir, &cal);

	if(err != OK) {
		return "{}";
	}

	void *elem;
	ListIterator iter = createIterator(cal->events);
	//elem = nextElement(&iter); //skip the first one which we've already gotten.
	for(int i = 1; i <= numEvent ; i++) {
			elem = nextElement(&iter);
			Event *event = elem;

			if(i == numEvent) {
				//printf("UID: %s\n",event->UID);
				char *alarmString = propListToJSON(event->properties);
				deleteCalendar(cal);
				return alarmString;
			}
		}
	deleteCalendar(cal);
	return NULL;
}

char *propListToJSON(const List* eventList) {
	int size = 10000;
  char *string = calloc(size,sizeof(char));

 if(eventList == NULL) {
	 sprintf(string,"[]");
	 return string;
 }

 List* list = (List*)eventList;
 Property*eventHead = getFromFront(list);
 char *evString = propToJSON(eventHead);
	sprintf(string,"[%s]",evString);
	//printf("%s\n",string);
 free(evString);

if(getLength(list) > 1) {

 //printf("getLength: %d\n",getLength(list));
 void *elem;
 ListIterator iter = createIterator(list);
 elem = nextElement(&iter); //skip the first one which we've already gotten.
 for(int i = 1; i < getLength(list); i++) {
		 elem = nextElement(&iter);
		 Property *event = elem;

		 if(strlen(string) > size) {
			 size = size*size;
			 string = realloc(string, size);
		 }
		 char *eventString = propToJSON(event);
		 sprintf(string + strlen(string) - 1, ",%s]",eventString);
		 free(eventString);
 }
}
 return string;
}

char* propToJSON(const Property* alm) {
	char *string = calloc(10000,sizeof(char));

	if(alm == NULL) {
		sprintf(string,"{}");
		return string;
	}

	sprintf(string, "{\"propName\":\"%s\",\"propDescr\":\"%s\"}",alm->propName,alm->propDescr);

	return string;
}
