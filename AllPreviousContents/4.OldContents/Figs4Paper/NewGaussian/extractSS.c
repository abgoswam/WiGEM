#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

int fill_SignalStrength(const char *signalStrength); 
int tshark_parseFile_extractSS(const char *fileName);

int main(int argc, char *argv[]) {
	int rc = 0;
	char file_name[500];
						
	if(argc!=2) {
		printf("usage: ./extract <tcpdump filename>");
		exit(1);
	}
	rc = tshark_parseFile_extractSS(argv[1]);
	if(!rc) {
		printf("Some Error In parsing file %s\n", argv[1]); fflush(stdout);
	}

	return 0;
}


int tshark_parseFile_extractSS(const char *fileName) {

	char command[100];
	char *parsed_filename = "tmp_parsed";
	FILE *input, *fp;
	int rc = 0, SignalStrength;

	char arrivalTime[500];
	char signalStrength[500], TypeSubtype[500], DS_Status[500];

	int failsafe = 0;

	printf("loading %s \n", fileName); fflush(stdout);
	sprintf( command, "tshark -Vr %s > %s", fileName, parsed_filename);
    system(command);

	input = fopen(parsed_filename,"r"); /*Open tshark parsed trace file*/
	fp = fopen("SS_Values.csv","w");	/*register SS values here*/
	
	if (!input || !fp) {
        	printf("Cannot open tshark trace file / error in fp file \n"); fflush(stdout);
        	return rc;
    }
	while (!feof(input)) {
		/*
		 * Arrival Time
		 */
       	memset(arrivalTime, 0, sizeof(arrivalTime));
       	fgets(arrivalTime, sizeof(arrivalTime), input);
       	if (!strstr( arrivalTime, "Arrival Time"))
       		continue;
		
		/*
         * SSI Signal
         */
       	failsafe = 0;
       	while(1) {
           	if(++failsafe > 1000) {
               	printf("Incomplete file \n"); fflush(stdout); remove(parsed_filename);
               	fclose(input);
                return rc;
            }
           	memset(signalStrength, 0, sizeof(signalStrength));
           	fgets(signalStrength, sizeof(signalStrength), input);
           	if (!strstr( signalStrength , "SSI Signal"))
                    continue;
           	else
              	break;
       	}

		/*
		 * Type/Subtype 
		 */
		failsafe = 0;
		while(1) {
			if(++failsafe > 1000) {
                 printf("Incomplete file \n"); fflush(stdout); remove(parsed_filename);
                 fclose(input); 
				 return rc;
			}
			memset( TypeSubtype, 0, sizeof(TypeSubtype));
			fgets( TypeSubtype, sizeof(TypeSubtype), input);
			if (!strstr( TypeSubtype , "Type/Subtype"))
            	continue;
			else 
				break;
		}
		/*
		 * NOTE: we need to make sure thay the type/subtype contains proper flags
		 * e.g.
		 * Beacon frame (0x08)
		 * QoS Data (0x28)
		 * Probe Response (0x05)
	         * Clear-to-send (0x1c)
		 * Acknowledgement (0x1d)
		 * Null function (No data) (0x24)
		 * Data (0x20)
		 */

		/*  removing this restriction in allMacFinal
		 *
		if(! strstr(TypeSubtype, "0x20"))  			
			continue;
		*/

		/*
		 * DS status
		 */
		failsafe = 0;
		while(1) {
            if(++failsafe > 1000) {
                 printf("Incomplete file \n"); fflush(stdout); remove(parsed_filename);
                 fclose(input); 
				 return rc;
            }
			memset( DS_Status, 0, sizeof(DS_Status));
			fgets( DS_Status, sizeof(DS_Status), input);
			if (!strstr( DS_Status , "DS status"))
              			continue;
			else 
				break;
		}
		/*
		 * NOTE: we need to make sure that the DS status is set properly
		 * e.g
		 * Not leaving DS or network is operating in AD-HOC mode (To DS: 0 From DS: 0) (0x00)
		 * Frame from DS to a STA via AP(To DS: 0 From DS: 1) (0x02)
		 * Frame from STA to DS via an AP (To DS: 1 From DS: 0)  (0x01)
		 */
		if(! strstr(DS_Status, "0x01"))  /* we r just using packets from STA to DS via an AP */
			continue;
		
		SignalStrength = fill_SignalStrength(signalStrength);
		fprintf(fp, "%d\n", SignalStrength);
	}
	rc = 1;  /* failsafe complete */
    fclose(input);
    remove(parsed_filename);
    return rc;
}

int fill_SignalStrength(const char *signalStrength) {
    char pseudo_signalStrength[200];
    int signal;

    memset(pseudo_signalStrength, 0, sizeof(pseudo_signalStrength));
    strcpy(pseudo_signalStrength, signalStrength);
    strtok(pseudo_signalStrength,":");
    signal = atoi (strtok(NULL," "));

    return signal;
}

#if 0  
/* code from allmacFinal2.c */

/*
 * usage: ./mac <directory containing all the dates folder>
 * e.g ./mac fullDays
 */
int tshark_parseFile(const char *fileName, const char *date);
void process_filename( const char *fileName, char *HrMin);
int getSnifferId (const char *sniffer);
int getDateId (const char *time);

#define TIME_DIVISIONS 72
#define NUM_SNIFFERS 9
#define NUM_FULL_DAYS 20 

typedef struct macaddressNode {
	char macId[18];
	int presence[NUM_FULL_DAYS][TIME_DIVISIONS];
	struct macaddressNode *next;
} macNode;

macNode *headNode = NULL;

int processMacEntry(char *macAddress, int dateid, int timeid) ;
int InsertMacEntry(char *macAddress, int dateid, int timeid) ;
int UpdateMacEntry(macNode *entry, int dateid, int timeid) ;
macNode *FindMacEntry(char *macAddress);

char Time[TIME_DIVISIONS][10] = { 	"12:0", "12:10", "12:20", "12:30", "12:40", "12:50",
					"13:0", "13:10", "13:20", "13:30", "13:40", "13:50",
					"14:0", "14:10", "14:20", "14:30", "14:40", "14:50",
					"15:0", "15:10", "15:20", "15:30", "15:40", "15:50",
					"16:0", "16:10", "16:20", "16:30", "16:40", "16:50",
					"17:0", "17:10", "17:20", "17:30", "17:40", "17:50",
					"18:0", "18:10", "18:20", "18:30", "18:40", "18:50",
					"19:0", "19:10", "19:20", "19:30", "19:40", "19:50",
					"20:0", "20:10", "20:20", "20:30", "20:40", "20:50",
					"21:0", "21:10", "21:20", "21:30", "21:40", "21:50",
					"22:0", "22:10", "22:20", "22:30", "22:40", "22:50",
					"23:0", "23:10", "23:20", "23:30", "23:40", "23:50" };

char Sniffer[NUM_SNIFFERS][10] = { "152", "153", "154", "155", "156", "157", "158", "159", "160" };

char Dates[NUM_FULL_DAYS][11] = { 				    "2010-04-07", "2010-04-08", "2010-04-09", 
						    		    "2010-04-14", "2010-04-15", "2010-04-16", 
					"2010-04-19", "2010-04-20", "2010-04-21", "2010-04-22", 
					"2010-04-26", "2010-04-27", "2010-04-28", "2010-04-29", "2010-04-30", 
					"2010-05-03", "2010-05-04", "2010-05-05", "2010-05-06", "2010-05-07"	};
int SignalStrengthValues[TIME_DIVISIONS][NUM_SNIFFERS];
int count[TIME_DIVISIONS][NUM_SNIFFERS];

int main (int argc, char *argv[]) {
	macNode *tmp;
	DIR *daysDir, *dateDir;
        struct dirent *days_entry;
	DIR *snifferDir;
        struct dirent *dir_entry, *file_entry;
	int t, s, count, rc;
	char *directoryName = argv[1];
        char days_dir_name[200], dir_name[200], file_name[200];
	FILE *f_output;
	
	if (!(daysDir = opendir(directoryName))) {
        	printf("Cannot open Days directory for reading: %s\n", directoryName);
        	exit(1);
        }
    	while (days_entry = readdir(daysDir)) {
          	if (strcmp(days_entry->d_name, ".") && strcmp(days_entry->d_name, "..")) {
			memset( days_dir_name, 0, sizeof(days_dir_name));
            		sprintf( days_dir_name, "%s/%s", directoryName, days_entry->d_name);
		    	if (!(dateDir = opendir(days_dir_name))) {
        			printf("Cannot open date directory for reading: %s\n", days_dir_name);
        			exit(1);
            		}
       			while (dir_entry = readdir(dateDir)) {
          			if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {
            				memset( dir_name, 0, sizeof(dir_name));
            				sprintf( dir_name, "%s/%s", days_dir_name, dir_entry->d_name);
	    				if (!(snifferDir = opendir(dir_name))) {
        					printf("Cannot open sniffer directory for reading: %s\n", dir_name);
        					exit(1);
            				}
	    				while ( file_entry = readdir(snifferDir)) {
                				if (strcmp(file_entry->d_name, ".") && strcmp(file_entry->d_name, "..")) {
							memset( file_name, 0, sizeof(file_name));
                        				sprintf(file_name, "%s/%s", dir_name, file_entry->d_name);
							rc = tshark_parseFile(file_name, days_entry->d_name);
							if(!rc) {
								printf("Some Error In parsing file %s\n", file_name); fflush(stdout);
							}
						}
	    				}
	    				closedir(snifferDir);
	 			}
      			}
      			closedir(dateDir);
		}
	}
	closedir(daysDir);

	f_output = fopen ("mac-list.txt", "w");
	if ( !f_output ) {
		printf ("Error Creating output file. Exiting");
		exit(1);
	}
	tmp = headNode;
	while(tmp) {
		count = 0;
		fprintf (f_output, "%20s -", tmp->macId);
		for ( t = 0 ; t < NUM_FULL_DAYS; t++) 
			for ( s = 0 ; s < TIME_DIVISIONS; s++)
				count += tmp->presence[t][s];

		fprintf (f_output, "%5d\n", count);
		tmp = tmp->next;
	}

	while(headNode) {
		tmp = headNode->next;
		free(headNode);
		headNode = tmp;
	}
		
      	fclose(f_output);	
      	return 0;
}

int getTimeId (const char *time) {
	int k;
	for ( k = 0 ; k < TIME_DIVISIONS; k++) {
		if ( !strcmp ( Time[k], time ))
			return k;
	}
	return -1;
}	

int getSnifferId (const char *sniffer) {
	int k;
	for ( k = 0 ; k < NUM_SNIFFERS ; k++) {
		if ( !strcmp ( Sniffer[k], sniffer ))
			return k;
	}
	return -1;
}	

int getDateId (const char *date) {
	int k;
	for ( k = 0 ; k < NUM_FULL_DAYS ; k++) {
		if ( !strcmp ( Dates[k], date ))
			return k;
	}
	return -1;
}

int tshark_parseFile(const char *fileName, const char *date) {

	char command[100], HrMin[10];
	char *parsed_filename = "tmp_parsed";
	FILE *input;
	int DateId, TimeId, rc = 0;

	char arrivalTime[500];
	char getAddress[500], TypeSubtype[500], DS_Status[500];
   	char *macAddress;

	int failsafe = 0;

	printf("loading %s \n", fileName); fflush(stdout);
	sprintf( command, "tshark -Vr %s > %s", fileName, parsed_filename);
    	system(command);

	input = fopen(parsed_filename,"r"); /*Open tshark parsed trace file*/
	if (!input) {
        	printf("Cannot open tshark trace file \n"); fflush(stdout);
        	return rc;
    	}
	process_filename(fileName, HrMin);	

	TimeId = getTimeId(HrMin);
	DateId = getDateId(date);
	if ( TimeId == -1 || DateId == -1) {
		printf("Error in setting TimeId and/or DateId \n"); fflush(stdout); remove(parsed_filename); 
		fclose(input);
		return rc;
	}
	
	while (!feof(input)) {
		/*
		 * Arrival Time
		 */
        	memset(arrivalTime, 0, sizeof(arrivalTime));
        	fgets(arrivalTime, sizeof(arrivalTime), input);
        	if (!strstr( arrivalTime, "Arrival Time"))
              		continue;

		/*
		 * Type/Subtype 
		 */
		failsafe = 0;
		while(1) {
                         if(++failsafe > 1000) {
                                 printf("Incomplete file \n"); fflush(stdout); remove(parsed_filename);
                                 fclose(input); 
				 return rc;
                         }
			memset( TypeSubtype, 0, sizeof(TypeSubtype));
			fgets( TypeSubtype, sizeof(TypeSubtype), input);
			if (!strstr( TypeSubtype , "Type/Subtype"))
              			continue;
			else 
				break;
		}
		/*
		 * NOTE: we need to make sure thay the type/subtype contains proper flags
		 * e.g.
		 * Beacon frame (0x08)
		 * QoS Data (0x28)
		 * Probe Response (0x05)
	         * Clear-to-send (0x1c)
		 * Acknowledgement (0x1d)
		 * Null function (No data) (0x24)
		 * Data (0x20)
		 */

		/*  removing this restriction in allMacFinal
		 *
		if(! strstr(TypeSubtype, "0x20"))  			
			continue;
		*/

		/*
		 * DS status
		 */
		failsafe = 0;
		while(1) {
                         if(++failsafe > 1000) {
                                 printf("Incomplete file \n"); fflush(stdout); remove(parsed_filename);
                                 fclose(input); 
				 return rc;
                         }
			memset( DS_Status, 0, sizeof(DS_Status));
			fgets( DS_Status, sizeof(DS_Status), input);
			if (!strstr( DS_Status , "DS status"))
              			continue;
			else 
				break;
		}
		/*
		 * NOTE: we need to make sure that the DS status is set properly
		 * e.g
		 * Not leaving DS or network is operating in AD-HOC mode (To DS: 0 From DS: 0) (0x00)
		 * Frame from DS to a STA via AP(To DS: 0 From DS: 1) (0x02)
		 * Frame from STA to DS via an AP (To DS: 1 From DS: 0)  (0x01)
		 */
		if(! strstr(DS_Status, "0x01"))  /* we r just using packets from STA to DS via an AP */
			continue;
		
		/*
		 * Source address
		 */
		failsafe = 0;
		while(1) {
                         if(++failsafe > 1000) {
                                 printf("Incomplete file \n"); fflush(stdout); remove(parsed_filename);
                                 fclose(input); 
				return rc;
                         }
			memset(getAddress, 0, sizeof(getAddress));
                        fgets(getAddress, sizeof(getAddress), input);
			if (!strstr( getAddress , "Source address"))
				continue;
			else 
				break;
		}
		/* first find the mac */
		strtok( getAddress , "(");
		macAddress = strtok( NULL, ")"); 
		
		 /* lets do our update now */
		if(macAddress == NULL)
			continue; /* do not perform any processing in this case */

		printf("Processing for mac: %s\n", macAddress);
		rc = processMacEntry(macAddress, DateId, TimeId);
		if (!rc) {
			printf("Error in processMacEntry for %s: \n", macAddress); fflush(stdout); 
		}
	
		printf("\n\n%s", arrivalTime);
		//printf("%s", TypeSubtype);
		printf("%s", DS_Status);
		printf("%s", getAddress);
	
        }

	rc = 1;  /* failsafe complete */
    	fclose(input);
        remove(parsed_filename);
    	return rc;
}

void process_filename( const char *fileName, char *HrMin) {

	char pseudo_fileName[200];
	char *hr, *min, hrString[10], minString[10];
	int hrInt, minInt, oneTenth;

	memset( pseudo_fileName, 0, sizeof(pseudo_fileName));
	strcpy ( pseudo_fileName, fileName);
	
	strtok( pseudo_fileName,"_");
	hr = strtok ( NULL, ":");
	min = strtok ( NULL, ":");

	hrInt = atoi(hr);
	minInt = atoi(min);

	//printf("%d %d\n", hrInt, minInt);	

	oneTenth = minInt % 10;
	if ( oneTenth < 5 )
		minInt -= oneTenth;
	else 
		minInt += (10 - oneTenth);
			
	//printf("%d\n", minInt);
	memset( minString, 0, sizeof(minString));
	if(minInt == 60) {
		hrInt += 1;
		sprintf( minString, "0");
	} else
		sprintf( minString, "%d", minInt);

	sprintf( hrString, "%d", hrInt);
	strcpy ( HrMin, hrString);
	strcat ( HrMin, ":");
	strcat ( HrMin, minString);

	//printf( "%s\n", HrMin);
	return;
}

macNode *FindMacEntry(char *macAddress) {
	macNode *tmp = headNode;
	while(tmp) {
		//printf("%18s\n", tmp->macId);
		if(!strcmp( tmp->macId, macAddress))
			break;

		tmp = tmp->next;
	}
	return tmp;
}
int InsertMacEntry(char *macAddress, int dateid, int timeid) {
	int i, j;
	macNode *newNode;

	/* initializing new node */	
	newNode = (macNode *)malloc(sizeof(macNode));
	if(!newNode) {
		printf("Error allocating new node. "); fflush(stdout);
		return 0;
	}
	strcpy(newNode->macId, macAddress);
	for ( i = 0; i < NUM_FULL_DAYS ; i++) 
		for (j = 0; j < TIME_DIVISIONS; j++)
			newNode->presence[i][j] = 0;

	newNode->presence[dateid][timeid] = 1;
	newNode->next = headNode;
	headNode = newNode;

	return 1;
}
int UpdateMacEntry(macNode *entry, int dateid, int timeid) { 
	if(!entry) 
		return 0;

	if(entry->presence[dateid][timeid] != 1)
		entry->presence[dateid][timeid] = 1;
	
	return 1;
}
/* return : 0 in case of error
	    1 otherwise
 */
int processMacEntry(char *macAddress, int dateid, int timeid) {
	macNode *entry = NULL;
	int rc;
	if(!macAddress)
		return 0;

	entry = FindMacEntry(macAddress);
	if(!entry) {
		/* mac address not present in link list */
		/* insert and update */
		//printf("mac: %s not present in the link list. Inserting\n", macAddress); fflush(stdout);
		rc = InsertMacEntry(macAddress, dateid, timeid);
		if(!rc) {
			printf("Error inserting mac: %s @(%d, %d)\n", macAddress, dateid, timeid); fflush(stdout);
			return 0;
		}
	} else {
		//printf("mac: %s present in the link list. Updating\n", macAddress); fflush(stdout); 
		rc = UpdateMacEntry(entry, dateid, timeid); 
		if(!rc) {
			printf("Error updating mac: %s @(%d, %d)\n", macAddress, dateid, timeid); fflush(stdout);
			return 0;
		}
	}
	return 1;
}
#endif
