//#include "LinkedListAPI.h"
//#include "CalendarParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "CalendarParser.h"
#include "Helper.h"
#include "LinkedListAPI.h"

void freeFileAndArray(char *fileArray,char **darray,int sizeofDarray) {
	free(fileArray);
	for(int x = sizeofDarray; x >= 0; x--) {
		free(darray[x]);
	}
	free(darray);
}

int length(char ** array) {
  int counter = 0;
  int index = 0;
  while(array[0][index] != '\n') { //SHOULD I CHANGE THIS???? IS '\N COUNDTED IN SIZE'
    index++;
    counter++;
  }
  return counter;
}

int parseEvent(char **darray,int i, Calendar** obj)  {
  int tempVar = 0;
  char entireA[ARRSIZE] = ""; //not initialized
  char firstA[100] = "";
  char secondA[ARRSIZE]= "";

	bool dStartFound = false;
	bool dStampFound = false;
	bool uIdFound    = false;

  Event *new = malloc(sizeof(Event));
	new->properties = initializeList(printProperty,deleteProperty,compareProperties);
  new->alarms     = initializeList(printAlarm,deleteAlarm,compareAlarms);

	int index = i + 1;
	while(strcmp(firstA,"END") != 0 && strcmp(secondA,"VEVENT") != 0) {

		while(darray[index][0] == ';') {
			index++;
		}

		int counterVar = 0;
		int tempIndex =  index + 1;
		while(darray[tempIndex][0] == ' ' || darray[tempIndex][0] == '\t') { //|| darray[tempIndex][0] == ';') {
			counterVar++;
			tempIndex++;
		}

		int newVar = 0; //don't want to read in the space or tab!
		int tempVar2 = 0;
		tempIndex = index;
		if(counterVar != 0) {

		for(int i = 0; i <= counterVar; i++) { //turn into while loop?
			while(darray[tempIndex][newVar] != '\n')  {
				entireA[tempVar2] = darray[tempIndex][newVar];
				newVar++;
				tempVar2++;

			}
			tempVar2--;
			newVar = 1;
			tempIndex++;
			index++;
		}
		index--;
		entireA[tempVar2] = '\0';
	}
	else {
		while(darray[index][tempVar] != '\n') {
			entireA[tempVar] = darray[index][tempVar];
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

		for(int i = 0; i < strlen(secondA); i++) {
			if(secondA[i] == '\r') {
				secondA[i] = '\0';
			}
		}

		index++; // set variables to 0
		tempVar = 0; // set variables to 0
		tempVar2 = 0;


      if((strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VALARM") == 0)) {
        index = parseAlarm(darray,index,&new);
      }
      else if((strcmp(firstA,"UID") == 0) && (uIdFound == false)) {
				uIdFound = true;
        strcpy(new->UID,secondA);
      }
      else if((strcmp(firstA,"DTSTART") == 0) && (dStartFound == false)) {
				dStartFound = true;
        char data[10] = ""; //should I make it bigger? //NO NEW LINE?
				char time[10] = ""; //should I make it bigger? //NO NEW LINE?
        data[0] = '\0';
        time[0] = '\0';
        int k = 0;
        int num = 0;
        int num2 = 0;
        bool var = false;

        new->startDateTime.UTC = false;

        while(num < 8) {
          data[num] = secondA[k];
          num++;
          k++;
        }
        data[8] = '\0';

        if(secondA[k] == 'T'){
          var = true;
        }

        k++;
        while(num2 < 6) {
          time[num2] = secondA[k];
          num2++;
          k++;
        }
        time[6] = '\0';

        if(secondA[k] == 'Z') {
          if(var) {
            new->startDateTime.UTC = true;
          }
        }
        strcpy(new->startDateTime.date,data);
        strcpy(new->startDateTime.time,time);
      }
      else if((strcmp(firstA,"DTSTAMP") == 0) && (dStampFound == false)) {
				dStampFound = true;
        char dataStamp[10] = ""; //should I make it bigger? //NO NEW LINE?
        char timeStamp[10] = ""; //should I make it bigger? //NO NEW LINE?
        dataStamp[0] = '\0';
        timeStamp[0] = '\0';
        int kStamp = 0;
        int numStamp = 0;
        int num2Stamp = 0;
        bool varStamp = false;

        new->creationDateTime.UTC = false;

        while(numStamp < 8) {
          dataStamp[numStamp] = secondA[kStamp];
          numStamp++;
          kStamp++;
        }
        dataStamp[8] = '\0';

        if(secondA[kStamp] == 'T'){
          varStamp = true;
        }

        kStamp++;
        while(num2Stamp < 6) {
          timeStamp[num2Stamp] = secondA[kStamp];
          num2Stamp++;
          kStamp++;
        }
        timeStamp[6] = '\0';

        if(secondA[kStamp] == 'Z') {
          if(varStamp) {
            new->creationDateTime.UTC = true;
          }
        }
        strcpy(new->creationDateTime.date,dataStamp);
        strcpy(new->creationDateTime.time,timeStamp);

      }
			else if(strcmp(firstA,"END") == 0 && strcmp(secondA,"VALARM") == 0) {
	    	//do nothing.
	    }
      else if(strcmp(firstA,"END") == 0 && strcmp(secondA,"VEVENT") == 0) {
        break;
      }

      else{
          Property* newProp = malloc(sizeof(Property) + 200000 * sizeof(char)); //Only requires ONE FREE
          strcpy(newProp->propName,firstA);
          strcpy(newProp->propDescr,secondA);
          insertBack(new->properties, newProp);
      }

      memset(&entireA[0], 0, sizeof(entireA)); //clear array
      memset(&firstA[0], 0, sizeof(firstA)); //clear array
      memset(&secondA[0], 0, sizeof(secondA)); //clear array

  }

  insertBack((*obj)->events, new);
  return index - 1; //-/?? CHECK AGAIN WHY YOU'RE DOING THIS

}

int parseAlarm(char **darray,int i, Event **evt) {
  int tempVar = 0;
  char entireA[ARRSIZE] = "";
  char firstA[100] = "";
  char secondA[ARRSIZE]= "";

	bool actionFound  = false;
	bool triggerFound = false;

  Alarm *new = malloc(sizeof(Alarm));
  new->properties = initializeList(printProperty,deleteProperty,compareProperties);

	int index = i; //WHY NOT I PLUS ONE??

	while(strcmp(firstA,"END") != 0 && strcmp(secondA,"VALARM") != 0) {

			while(darray[index][0] == ';') {
				index++;
			}

			int counterVar = 0;
			int tempIndex =  index + 1;
			while(darray[tempIndex][0] == ' ' || darray[tempIndex][0] == '\t') { //|| darray[tempIndex][0] == ';') {
				counterVar++;
				tempIndex++;
			}
			int newVar = 0; //don't want to read in the space or tab!
			int tempVar2 = 0;
			tempIndex = index;
			if(counterVar != 0) {

			for(int i = 0; i <= counterVar; i++) { //turn into while loop?
				while(darray[tempIndex][newVar] != '\n')  {
					entireA[tempVar2] = darray[tempIndex][newVar];
					newVar++;
					tempVar2++;

				}
				tempVar2--;
				newVar = 1;
				tempIndex++;
				index++;
			}
			index--;
			entireA[tempVar2] = '\0';
		}
		else {
			while(darray[index][tempVar] != '\n') {
				entireA[tempVar] = darray[index][tempVar];
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

			for(int i = 0; i < strlen(secondA); i++) {
				if(secondA[i] == '\r') {
					secondA[i] = '\0';
				}
			}

			index++; // set variables to 0
			tempVar = 0; // set variables to 0
			tempVar2 = 0;

      if((strcmp(firstA,"ACTION") == 0) && (actionFound == false)) {
				actionFound = true;
        strcpy(new->action,secondA);
      }
      else if((strcmp(firstA,"TRIGGER") == 0) && (triggerFound == false)) {
				triggerFound = true;
        char * string = malloc(sizeof(char)*100);// SHOULD I MAKE IT BIGGER??
        strcpy(string, secondA);
        new->trigger = string;
        //printf("TRIGGER: %s\n",new->trigger);
      }
      else if(strcmp(firstA,"END") == 0 && strcmp(secondA,"VALARM") == 0) {
        break;
      }

      else{
          Property* newProp = malloc(sizeof(Property) + 200000 * sizeof(char)); //Only requires ONE FREE
          strcpy(newProp->propName,firstA);
          strcpy(newProp->propDescr,secondA);
          insertBack(new->properties, newProp);
      }
      memset(&entireA[0], 0, sizeof(entireA)); //clear array
      memset(&firstA[0], 0, sizeof(firstA)); //clear array
      memset(&secondA[0], 0, sizeof(secondA)); //clear array

  }
  insertBack((*evt)->alarms, new);
  return index - 1;
}

/*******************ERROR HANDLING*******************/

ICalErrorCode duplicate(char **darray,int size) {

	int index = 0;
	char arr[1000]; //again magic number!
	bool varVersion = false;
	bool varID= false;

	int i = 0;
	while( i < size) {

	while((i < size) && (darray[i][0] == ' ' || darray[i][0] == '\t')) {
		i++;
	}

		char *string = getFirstandSecond(darray, i,size); //OH YOU READ THEN YOU SKIP!
		int j = 0;
			while(string[j] != ':') { //don't need to check ';' because it's stored with ':'
				arr[index] = string[j];
				index++;
				j++;
			}
			arr[index++] = '\0';
			//printf("PRINGING ARRAY: %s\n",arr);

			if(strcmp(arr,"VERSION") == 0) {
					if(varVersion == false) {
						varVersion = true;
					}
					else if(varVersion == true) {
							free(string); //need to free
						return DUP_VER;
					}
			}

				if(strcmp(arr,"PRODID") == 0) {
					if(varID == false) {
						varID = true;
					}
					else if(varID == true) {
						free(string);
						return DUP_PRODID;
					}
				}

			index = 0;
			memset(&arr[0], 0, sizeof(arr)); //clear array
			free(string);
			i++;
	}

	return OK;
}

char * getFirstandSecond(char **darray, int index, int size) {
	//int index = 0;
	int tempVar = 0;
	int tempVar2 = 0;
	//char entireA[ARRSIZE] = "\0";
	char firstA[1000] = "\0"; //again magic number!
	char secondA[ARRSIZE] = "\0"; //again magic number!

	/*FROM EVENT*/
	// while(darray[index][tempVar] != '\n') {
	// 	entireA[tempVar] = darray[index][tempVar];
	// 	tempVar++;
	// }
	// 	tempVar = 0;

	while(darray[index][tempVar] != ':') {
		if(darray[index][tempVar] == ';') {
			break;
		}
		firstA[tempVar] = darray[index][tempVar];
		tempVar++;
		}

		tempVar++;  //to skip ':'

		while(darray[index][tempVar] != '\r') { //CHANGED TO BACK SLASH R \R
			secondA[tempVar2] = darray[index][tempVar];
			tempVar++;
			tempVar2++;
		}
		secondA[tempVar2++] = '\0';

		/*from alarm for linefolding*/
		if(index != size) { //or index <size??
		int counterVar = 0;
		int tempIndex = index+1;
		while(darray[tempIndex][0] == ' ' || darray[tempIndex][0] == '\t') {
			counterVar++;
			tempIndex++;
		}

			tempVar2--;
			tempIndex = index + 1;

			int newVar = 1;
			for(int i = 0; i < counterVar; i++) {
				while(darray[tempIndex][newVar] != '\n')  {
					secondA[tempVar2] = darray[tempIndex][newVar];
					newVar++;
					tempVar2++;
				}
				newVar = 1;
				tempVar2--;;
				tempIndex++;
				index++;
			}
		}
		/*from alarm for linefolding*/

		index++; // set variables to 0
		tempVar = 0; // set variables to 0
		tempVar2 = 0;

		char *string = malloc(sizeof(char) * 1000);
		strcpy(string, firstA);
		strcat(string, ":");
		strcat(string, secondA);
		strcat(string,"\0");

		//printf("WHAT IS THE STRING: %s\n",string);
		return string;

}

ICalErrorCode invalidFile(char* fileName) {
	FILE *fp;

	if(fileName == NULL) {
		return INV_FILE;
	}
	if(strlen(fileName) == 0) {
		return INV_FILE;
	}

	if((fp = fopen(fileName,"r")) == NULL) {
		//fclose(fp); //does this leak?
		return INV_FILE; //file doesn't exist or cannot be opened?
	}
	fclose(fp);

	int p = strlen(fileName);

		if(strlen(fileName) <= 4) { //or 4?
			return INV_FILE;
		}
		else if((fileName[p-1] != 's') || (fileName[p-2] != 'c') || (fileName[p-3] != 'i') ||  (fileName[p-4] != '.')) {
			return INV_FILE;
		}
	return OK;
}

ICalErrorCode invalidCalendar(char **darray,int size) {
	int i = 0;
	int tempVar = 0;

	bool beginEventExists = false;
	bool endEventExists = false;
	bool verExists   = false;
	bool idExists    = false;

	int eventStartCounter = 0;
	int eventEndCounter = 0;

	if(darray[0][0] == '#') {
		return INV_CAL;
	}

	//printf("what is size: %d\n",size);
	//printf("at 66: %s\n",darray[66]);
	//int secondLast = size - 1;

	if(darray[size - 1][0] == ';') {
		return OK;
	}

	while(i < size) {
		char entireA[ARRSIZE];
    char firstA[100];
    char secondA[ARRSIZE]; //DON'T WANT MAGIC NUMBER
		//printf("r we here\n");
			// if(i == (size - 2)) {
			// 	//printf("r we here\n");
			// 	break;
			// 	// if(darray[i][0] == ';') {
			// 	// 	break;
			// 	// }
			// }
			//printf("WHAT IS I: %d\n",i);

			// if(i == secondLast) {
			// 	printf("55555TELLLLLIFIFFURRRHERE\n");
			// 	if(darray[i][0] == ';') {
			// 		break;
			// 	}
			// }

				while(darray[i][0] == ';') {
					i++;
				}

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
				firstA[firstVar] = '\0'; //SET FIRSTVAR TO 0????
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

				//printf("%s\n",entireA);

		 if (i == 0) {
					if((strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VCALENDAR") == 0)) {
						//
					}
					else{
						return INV_CAL; //missing opening bracket
					}
		}
    else if(strcmp(firstA,"VERSION") == 0) {
			if(verExists == false) {
					verExists = true;
			}
			else if(verExists) {
				return DUP_VER;
			}

			if(strcmp(secondA,"") == 0) {
				return INV_VER;
			}
			float check = atof(secondA);
				if(strcmp(secondA,"0.0") == 0) {
							//
				}
				else {
						if(check == 0.0) {
						return INV_VER;
				}
			}
    }
		else if(strcmp(firstA,"PRODID") == 0) {
			if(idExists == false) {
					idExists = true;
			}
			else if(idExists) {
				return DUP_PRODID;
			}

			if(strcmp(secondA,"") == 0) {
				return INV_PRODID;
			}
    }
		else if(strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VEVENT") == 0) {
			eventStartCounter++;
			beginEventExists = true;
			if(verExists == false) {
				return INV_CAL;
			}
			else if(idExists == false) {
				return INV_CAL;
			}

			Status new = {0,0};
      new = verifyEvent(darray,i,size);

			if(new.code != OK) {
				return new.code;
			}
    }
		else if(strcmp(firstA,"END") == 0 && strcmp(secondA,"VEVENT") == 0) {

			endEventExists = true;
			eventEndCounter++;
			if(eventStartCounter != eventEndCounter) {
				return INV_CAL;
			}
		}

		i++;
		tempVar = 0; // set variables to 0
		memset(&entireA[0], 0, sizeof(entireA)); //clear array
		memset(&firstA[0], 0, sizeof(firstA)); //clear array
		memset(&secondA[0], 0, sizeof(secondA)); //clear array
	}

	//printf("r wwe heeeeeeerrre\n");

	if(beginEventExists == false || endEventExists == false) {
		return INV_CAL;
	}

	if(strcmp(darray[size],"END:VCALENDAR\r\n") != 0) {
		return INV_CAL;
	}
	return OK;
}

Status verifyEvent(char ** darray, int i, int size)  {
	Status temp = {0,0};

	int tempVar = 0;
	char entireA[ARRSIZE] = ""; //not initialized
	char firstA[ARRSIZE]  = "";
	char secondA[ARRSIZE] = "";

	bool prodID = false;
	bool invalidEventStart = false;
	bool dStampExists = false;
	bool dStartExists = false;
	bool dStamptExists = false;
	bool dStarttExists = false;

	int alarmStartCounter = 0;
	int alarmEndCounter = 0;

	int index = i + 1;
	while((strcmp(firstA,"END") != 0 && strcmp(secondA,"VEVENT") != 0) && index < size) {

		while(darray[index][0] == ';') {
			index++;
		}

		int counterVar = 0;
		int tempIndex =  index + 1;
		while(darray[tempIndex][0] == ' ' || darray[tempIndex][0] == '\t') { //|| darray[tempIndex][0] == ';') {
			counterVar++;
			tempIndex++;
		}

		int newVar = 0; //don't want to read in the space or tab!
		int tempVar2 = 0;
		tempIndex = index;
		if(counterVar != 0) {

		for(int i = 0; i <= counterVar; i++) { //turn into while loop?
			while(darray[tempIndex][newVar] != '\n')  {
				entireA[tempVar2] = darray[tempIndex][newVar];
				newVar++;
				tempVar2++;

			}
			tempVar2--;
			newVar = 1;
			tempIndex++;
			index++;
		}
		index--;
		entireA[tempVar2] = '\0';
	}
	else {
		while(darray[index][tempVar] != '\n') {
			entireA[tempVar] = darray[index][tempVar];
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

		for(int i = 0; i < strlen(secondA); i++) {
			if(secondA[i] == '\r') {
				secondA[i] = '\0';
			}
		}

		index++; // set variables to 0
		tempVar = 0; // set variables to 0
		tempVar2 = 0;

			if((strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VALARM") == 0)) {
				alarmStartCounter++;
				Status new = {0,0};

				new = verifyAlarm(darray,index,size);
				if(new.code != OK) {
					return new;
				}
				index = new.i;
			}
			else if(strcmp(firstA,"UID") == 0) {
				prodID = true;
				if(strcmp(secondA,"") == 0) {
					temp.code = INV_EVENT;
					return temp;
				}
			}
			else if(strcmp(firstA,"DTSTART") == 0) {
				dStartExists = true;
				if(strcmp(secondA,"") == 0) {
					temp.code = INV_DT;
					return temp;
					}

				int numofChar = 0; //set to 0?
				for(int j = 0; j < strlen(secondA); j++) {
						if(secondA[j] == 'T') {
							dStarttExists = true;
						}
						if(isalpha(secondA[j])) {
						numofChar++;
				  }
			 }
			 if(dStarttExists == false) {
				 temp.code = INV_DT;
				 return temp;
			 }
			 if(numofChar > 2) {
				 temp.code = INV_DT;
				 return temp;
				}

				int numDate = 0;
				for(int i = 0; i < strlen(secondA); i++) {
					if(secondA[i] >= '0' && secondA[i] <= '9') {
						 numDate++;
					}
				}
				if(numDate != 14) {
					temp.code = INV_DT;
 				  return temp;
				}
			}
			else if(strcmp(firstA,"DTSTAMP") == 0) {
				dStampExists = true;
				if(strcmp(secondA,"") == 0) {
					temp.code = INV_DT;
					return temp;
					}

				int numofChar = 0; //set to 0?
				for(int j = 0; j < strlen(secondA); j++) {
						if(secondA[j] == 'T') {
							dStamptExists = true;
						}
						if(isalpha(secondA[j])) {
						numofChar++;
					}
			 }
			 if(dStamptExists == false) {
				 temp.code = INV_DT;
				 return temp;
			 }
			 if(numofChar > 2) {
				 temp.code = INV_DT;
				 return temp;
				}

				int numDate = 0;
				for(int i = 0; i < strlen(secondA); i++) {
					if(secondA[i] >= '0' && secondA[i] <= '9') {
						 numDate++;
					}
				}
				if(numDate != 14) {
					temp.code = INV_DT;
					return temp;
				}
			}
			else if((strcmp(firstA,"END") == 0) && (strcmp(secondA,"VEVENT") == 0)) {
				if(prodID == false) {
					temp.code = INV_EVENT;
				}
				else if(dStampExists == false || dStartExists == false) {
					//printf("are we here\n");
					temp.code = INV_EVENT;
				}
				else if(invalidEventStart == true) {
					temp.code = INV_EVENT;
				}
				else{
					temp.i = index - 1;
					temp.code = OK;
				}
				//printf("are we here\n");
				return temp;
			}
			else if(strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VEVENT") == 0) {
					invalidEventStart = true;
			}
			else if(strcmp(firstA,"END") == 0 && strcmp(secondA,"VALARM") == 0) { //WHY OR
				alarmEndCounter++;
					if(alarmStartCounter != alarmEndCounter) {
						temp.code = INV_EVENT;
						return temp;
					}
			}
			else if(strcmp(firstA,"") == 0 || strcmp(secondA,"") == 0) {
				temp.code = INV_EVENT;
				return temp;
			}

			memset(&entireA[0], 0, sizeof(entireA)); //clear array
			memset(&firstA[0], 0, sizeof(firstA)); //clear array
			memset(&secondA[0], 0, sizeof(secondA)); //clear array

	}
	temp.code = INV_EVENT;
	return temp;
}

Status verifyAlarm(char ** darray, int i, int size) {
	Status temp = {0,0};

	int tempVar = 0;
	char entireA[ARRSIZE] = ""; //not initialized
	char firstA[100] = "";
	char secondA[ARRSIZE]= "";

	bool actionExist  = false;
	bool triggerExist = false;

	int index = i; //WHY NOT I PLUS ONE??

	while((strcmp(firstA,"END") != 0 && strcmp(secondA,"VALARM") != 0) && index < size) {

			/*logic for handling comments*/
			while(darray[index][0] == ';') {
				index++;
			}

			int counterVar = 0;
			int tempIndex =  index + 1;
			while(darray[tempIndex][0] == ' ' || darray[tempIndex][0] == '\t') { //|| darray[tempIndex][0] == ';') {
				counterVar++;
				tempIndex++;
			}

			int newVar = 0; //don't want to read in the space or tab!
			int tempVar2 = 0;
			tempIndex = index;
			if(counterVar != 0) {

			for(int i = 0; i <= counterVar; i++) { //turn into while loop?
				while(darray[tempIndex][newVar] != '\n')  {
					entireA[tempVar2] = darray[tempIndex][newVar];
					newVar++;
					tempVar2++;

				}
				tempVar2--;
				newVar = 1;
				tempIndex++;
				index++;
			}
			index--;
			entireA[tempVar2] = '\0';
		}
		else {
			while(darray[index][tempVar] != '\n') {
				entireA[tempVar] = darray[index][tempVar];
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

			for(int i = 0; i < strlen(secondA); i++) {
				if(secondA[i] == '\r') {
					secondA[i] = '\0';
				}
			}

			index++; // set variables to 0
			tempVar = 0; // set variables to 0
			tempVar2 = 0;

			if(strcmp(firstA,"ACTION") == 0) {
				actionExist = true;

				if(strcmp(secondA,"") == 0) {
						//invalidAction = true;
						temp.code = INV_ALARM;
						return temp;
					}
			}
			else if(strcmp(firstA,"TRIGGER") == 0) {
				triggerExist = true;

				if(strcmp(secondA,"") == 0) {
						temp.code = INV_ALARM;
						return temp;
					}
			}
			else if(strcmp(firstA,"END") == 0 && strcmp(secondA,"VALARM") == 0) {
				if(triggerExist == false || actionExist == false) {
					temp.code = INV_ALARM;
				}
				else{
					temp.i = index - 1;
					temp.code = OK;
				}
				return temp;
			}
			else if(strcmp(firstA,"BEGIN") == 0 && strcmp(secondA,"VALARM") == 0) {
					temp.code = INV_ALARM;
					return temp;
			}
			else if(strcmp(firstA,"") == 0 || strcmp(secondA,"") == 0) {
				temp.code = INV_ALARM;
				return temp;
			}

			memset(&entireA[0], 0, sizeof(entireA)); //clear array
			memset(&firstA[0], 0, sizeof(firstA)); //clear array
			memset(&secondA[0], 0, sizeof(secondA)); //clear array

	}
	temp.code = INV_ALARM;
	return temp;
}

/**********FUNCTIONS FOR WRITE CALENDAR***********/

char* printEventDatetime(void *toBePrinted) {
	DateTime* date = (DateTime*)toBePrinted;
	char* buffer = calloc(1000,sizeof(char));

	if(date->UTC == false) {
		sprintf(buffer, "%sT%s\r\n",date->date,date->time);
	}
	else {
		sprintf(buffer, "%sT%sZ\r\n",date->date,date->time);
	}
	return buffer;
}

char* printEventProperties(Event* event) {
	void *elem;
	ListIterator iter = createIterator(event->properties);
	int len = 1000;
	char *buffer = calloc(len, sizeof(char));

	while((elem = nextElement(&iter)) != NULL) {
		Property *prop = elem;
		if(strlen(buffer) >= len) {
			len = len*len;
			buffer = realloc(buffer,len);
		}
		sprintf(buffer + strlen(buffer), "%s:%s\r\n",prop->propName,prop->propDescr);
	}
	return buffer;
}

char* printEventAlarms(Event* event) {
	void *elem;
	ListIterator iter = createIterator(event->alarms);

	int len = 1000;
	char *buffer = calloc(len, sizeof(char));

	while((elem = nextElement(&iter)) != NULL) {
		Alarm*alarm = elem;

		if(strlen(buffer) >= len) {
			len = len*len;
			buffer = realloc(buffer,len);
		}

		sprintf(buffer + strlen(buffer), "BEGIN:VALARM\r\nACTION:%s\r\nTRIGGER:%s\r\n",alarm->action,alarm->trigger);
		char *strProp  = printAlarmProperties(alarm);
		sprintf(buffer + strlen(buffer), "%s",strProp);
		sprintf(buffer + strlen(buffer), "END:VALARM\r\n");
		free(strProp);
	}
	return buffer;
}

char* printAlarmProperties(Alarm* alarm) {
	void *elem;
	ListIterator iter = createIterator(alarm->properties);

	int len = 1000;
	char *buffer = calloc(len, sizeof(char));

	while((elem = nextElement(&iter)) != NULL) {
		Property *prop = elem;

		if(strlen(buffer) >= len) {
			len = len*len;
			buffer = realloc(buffer,len);
		}

		sprintf(buffer + strlen(buffer), "%s:%s\r\n",prop->propName,prop->propDescr);
	}
	return buffer;
}

ICalErrorCode verifyForFileWriting(char* fileName) {
	FILE *fp;
	if(fileName == NULL) {
		return WRITE_ERROR;
	}

		if((fp = fopen(fileName,"w")) == NULL) {
			fclose(fp);
			return WRITE_ERROR; //file doesn't exist or cannot be opened?
		}
		fclose(fp);

	  int p = strlen(fileName);

		if(strlen(fileName) <= 4) { //or 4?
			return WRITE_ERROR;
		}
		else if((fileName[p-1] != 's') || (fileName[p-2] != 'c') || (fileName[p-3] != 'i') ||  (fileName[p-4] != '.')) {
			return WRITE_ERROR;
		}
	return OK;
}

/**********VALIDATE CALENDAR******************/
ICalErrorCode validateEvent(Event *event) {

	if(event == NULL) {
		return INV_EVENT;
	}

	if(event->alarms == NULL || event->properties == NULL){
		 return INV_EVENT;
 }

	DateTime *start = (DateTime*)&event->startDateTime;
	DateTime *stamp = (DateTime*)&event->creationDateTime;

	//ensuring dtstart and dtstamp don't exceed appropriate size
 if((strlen(start->date) > 8) || (strlen(start->time) > 6)) {
	 	 return INV_EVENT;
 }

 if((strlen(stamp->date) > 8) || (strlen(stamp->time) > 6)) {
		return INV_EVENT;
 }

	void *elem;
	ListIterator iter = createIterator(event->properties);
	char *arr[14] = {"CLASS","CREATED","DESCRIPTION","GEO","LAST-MODIFIED", \
                   "LOCATION","ORGANIZER","PRIORITY","SEQUENCE","STATUS", \
								    "SUMMARY","TRANSP","URL","RECURRENCE-ID"};
	/*
	; The following are OPTIONAL,
						; but MUST NOT occur more than once.
						;
						class / created / description / geo /
						last-mod / location / organizer / priority /
						seq / status / summary / transp /
						url / recurid /

						; Either 'dtend' or 'duration' MAY appear in
                  ; a 'eventprop', but 'dtend' and 'duration'
                  ; MUST NOT occur in the same 'eventprop'.
                  ;
                  dtend / duration /
	*/
	bool dtendExists    = false;
	bool durationExists = false;
	ICalErrorCode code;

	int counter = 0;
	for(int i = 0; i < 14; i++) {
		while((elem = nextElement(&iter)) != NULL) {
			Property *prop = elem;
			//printf("what is propName/descr: %s:%s\n",prop->propName,prop->propDescr);

			code = validEventProperty(prop->propName);
			if(code != OK) {
				return code;
			}

			if(strcmp(arr[i],prop->propName) == 0) {
				//printf("%s\n",arr[i]);
				counter++;
			}

			if(counter > 1) {
				return INV_EVENT;
			}

			if(strcmp("DTEND",prop->propName) == 0) {
				dtendExists = true;
				if(durationExists) {
					return INV_EVENT;
				}
			}

			if(strcmp("DURATION",prop->propName) == 0) {
				durationExists = true;
				if(dtendExists) {
					return INV_EVENT;
				}
			}
			if(strcmp(prop->propName,"") == 0 || strcmp(prop->propDescr,"") == 0) {
				return INV_EVENT;
			}

		}
		iter = createIterator(event->properties); //OR get Head function
		counter = 0;
	}

	ListIterator iterAlarm = createIterator(event->alarms);
	ICalErrorCode alarmCode;
	while((elem = nextElement(&iterAlarm)) != NULL) {
		Alarm *alarm = elem; //don't need this...
		alarmCode    = validateAlarm(alarm);
		if(alarmCode != OK) {
			return alarmCode;
		}
	}

	//check not null? in here or before calling?
	// repeat procedure from calendar, looking at variables/counters/

	//call validateAlarm, another layer which behaves the same way that event
	//behaves with calendar
	return OK;
}

ICalErrorCode validateAlarm(Alarm *alarm) {

	if(alarm == NULL) {
		return INV_ALARM;
	}

	if(alarm->properties == NULL){ //can this be null??
		 return INV_ALARM;
 }

  if(alarm->trigger == NULL){
		return INV_ALARM; //COULD TRIGGER EVER BE NULL?
  }

 bool durationExists = false;
 bool repeatExists   = false;
 int numDuration = 0;
 int numRepeat   = 0;

 int numAttach = 0;

 //action and trigger are dealt with beforehand
 //only checks for attach, duration, and repeat.
 void *elem;
 ListIterator iter = createIterator(alarm->properties);
 while((elem = nextElement(&iter)) != NULL) {
	 Property *prop = elem;

	 if(strcmp(prop->propName,"") == 0 || strcmp(prop->propDescr,"") == 0) { //empty propName dealt with in else
		 return INV_ALARM;
	 }
	 else if(strcmp(prop->propName, "ATTACH") == 0) {
	 if(numAttach >= 1) {
		 return INV_ALARM;
	 }
	 numAttach++;
  }
	 else if(strcmp(prop->propName, "DURATION") == 0) {
		durationExists = true;
	 	if(numDuration >= 1) {
	 		return INV_ALARM;
	 	}
	 	numDuration++;
	 }
	 else if(strcmp(prop->propName, "REPEAT") == 0) {
		 repeatExists = true;
	 	if(numRepeat >= 1) {
	 		return INV_ALARM;
	 	}
	 	numRepeat++;
	 }
	 else {
		 //empty
		 //unrecognized property
		 return INV_ALARM;
	 }

 }

//check too see if one exists and the other doesnt
 if(durationExists == true) {
	 if(repeatExists == false) {
		 return INV_ALARM;
	 }
 }

 if(repeatExists == true) {
	if(durationExists == false) {
		return INV_ALARM;
	}
}
 	return OK;
}

ICalErrorCode validEventProperty(char *string) {
	//printf("what is string: %s\n",string );
	char *arr[27] = {"CLASS","CREATED","DESCRIPTION","GEO","LAST-MODIFIED", \
                   "LOCATION","ORGANIZER","PRIORITY","SEQUENCE","STATUS", \
								   "SUMMARY","TRANSP","URL","RECURRENCE-ID","RRULE","DTEND", \
									 "DURATION","ATTACH","ATTENDEE","CATEGORIES","COMMENT", \
								   "CONTACT","EXDATE","RSTATUS","RELATED-TO","RESOURCES","RDATE"};

	for(int i = 0; i < 27; i++) {
		if(strcmp(arr[i],string) == 0) {
			return OK;
		}
	}
return INV_EVENT;
}

bool comparePropertiesBool(const void* first, const void* second) {
    if(first == NULL || second == NULL){
        return 0;
    }

    //dec vars
    Property* prop1 = (Property*)first;
    Property* prop2 = (Property*)second;

		if(strcmp(prop1->propName,prop2->propName) == 0) {
			return true;
		}
    return false;
}
