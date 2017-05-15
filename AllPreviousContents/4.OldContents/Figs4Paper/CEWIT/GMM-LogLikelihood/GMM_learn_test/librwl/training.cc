/* 
 * RICE UNIVERSITY SOFTWARE DISTRIBUTION LICENSE
 * Rice Wireless Localization Toolkit (the 'Software')
 * Copyright (C) 2005, Rice University.  All rights reserved.
 *
 * Created by Andreas Haeberlen (ahae@cs.rice.edu), Dept. of Computer Science
 * 
 * The software is available for download and use subject to the terms 
 * and conditions of this License. Access or use of the Software 
 * constitutes acceptance and agreement to the terms and conditions 
 * of this License.
 * 
 * This software, the Rice Wireless Localization Toolkit, is distributed 
 * to individuals for personal non-commercial use and to non-profit 
 * entities for non-commercial purposes only. It is licensed on a 
 * non-exclusive basis, free of charge for these uses. All parties 
 * interested in any other use of the software should contact the 
 * Rice University Office of Technology Transfer [techtran@rice.edu]. 
 * The license is subject to the following conditions:
 *
 *    1. No support will be provided by the developer or by Rice University.
 *    2. Redistribution is not permitted. Rice will maintain a copy of 
 *       the Rice Wireless Localization Toolkit as a directly downloadable
 *       file, available under the terms of license specified in this agreement.
 *    3. Neither the name of the University nor the names of its faculty, 
 *       staff, or students may be used to endorse or promote products 
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY WILLIAM MARSH RICE UNIVERSITY, HOUSTON, TX 
 * AND IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESS, IMPLIED OR STATUTORY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF ACCURACY, 
 * COMPLETENESS, NONINFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE WHICH ARE DISCLAIMED. ACCESS OR USE OF THE SOFTWARE 
 * IS ENTIRELY AT THE USER'S RISK. IN NO EVENT SHALL RICE UNIVERSITY OR 
 * ITS FACULTY, STAFF OR STUDENTS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE. THE SOFTWARE USER SHALL INDEMNIFY, DEFEND AND HOLD HARMLESS 
 * RICE UNIVERSITY AND ITS FACULTY, STAFF AND STUDENTS FROM ANY AND ALL 
 * CLAIMS, ACTIONS, DAMAGES, LOSSES, LIABILITIES, COSTS AND EXPENSES, 
 * INCLUDING ATTORNEYS' FEES AND COURT COSTS, DIRECTLY OR INDIRECTLY ARISING 
 * OUR OF OR IN CONNECTION WITH ACCESS OR USE OF THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include "eval.h"

#define dprintf(a...)

trainingDataPackage::trainingDataPackage()
{
    this->numObservations = 0;
    this->scenario = new scenarioDescriptor();
}

trainingDataPackage::trainingDataPackage(const char *placeFileName, const char *apFileName)
{
    this->numObservations = 0;
    this->scenario = new scenarioDescriptor(placeFileName, apFileName);
}

bool trainingDataPackage::readNewFile(const char *fileName)//, observationRecord *observation, int *locationIndex)
{
    struct stat statbuf;
    char linebuf[200];
    int apId, i ;//locId; 
    int signal;

    printf("\n***11***---------------%s\n", fileName);
    fflush(stdout);

    if (stat(fileName, &statbuf)<0)
        return false;

    if (S_ISDIR(statbuf.st_mode)) {
        printf("%s is a directory\n", fileName);
        return false;
    }

    printf("\n***12***\n");
    fflush(stdout);

    FILE *f = fopen(fileName, "r");
    if (!f) {
        fprintf(stderr, "Cannot open for reading: %s\n", fileName);
        return false;
    }

    printf("\n***13***\n");
    fflush(stdout);

    memset(linebuf, 0, sizeof(linebuf));
    strcpy(linebuf, fileName);

    strtok(linebuf, "/");
    apId = atoi (strtok(NULL, "\0"));

    //locId = atoi (strtok(NULL, "\0"));

    //printf("\n-------------->>>>>>>>>>>>***16.5 *** apId=%d : locId=%d\n", apId);//, locId);
    printf("\n-------------->>>>>>>>>>>>***16.5 *** apId=%d \n", apId);//, locId);
/*
    do {
        fgets(linebuf, sizeof(linebuf), f);
    } while (strcmp(linebuf, "[position]\n"));

    printf("\n***14***\n");
    fflush(stdout);

    fgets(linebuf, sizeof(linebuf), f);
    printf("\n *** Getting the x, y coordinates***\n");
    int xPos = atoi(strtok(linebuf, " "));
    int yPos = atoi(strtok(NULL, "\n"));
    printf("\n ***14.5 xPos=%d yPos=%d***\n",xPos,yPos);

    locId = scenario->justgetLocationIndex(xPos, yPos, 2);

    printf("\n***15 *** locId=%d\n", locId);
    fflush(stdout);

    do {
        fgets(linebuf, sizeof(linebuf), f);
    } while (strcmp(linebuf, "[sniffer]\n"));
    fgets(linebuf, sizeof(linebuf), f);
    
    printf("\n***16***\n");
    fflush(stdout);

    char *bssid = strtok(linebuf, " ");
    char *essid = strtok(NULL, "\r\n");
    printf("\n ***16.25 bssid=%s essid=%s***\n",bssid, essid);

    apId = scenario->getApIndex(bssid, essid);
    printf("\n***16.5 *** locId=%d : apId=%d\n", locId, apId);

    printf("\n***17***\n");
    fflush(stdout);

    printf("\n***18***\n");
    fflush(stdout);
*/

#if 0
    for ( i = 0; i < numObservations; i++ ) {
	if (observation[i].locId == locId) {
		printf("Got it! numObs = %d : i=%d : thisAp=%d\n",numObservations, i, apId);
		break;
	}
    }

    printf("\n***19 ---i=%d ***\n", i);
    fflush(stdout);

    for ( int k = 0; k < NUM_OBSERVATIONS; k++)
	locationIndex[i + k] = locId;
#endif
    for ( int k = 0; k < NUM_OBSERVATIONS; k++) {
        memset(linebuf, 0, sizeof(linebuf));
        fgets(linebuf, sizeof(linebuf), f);
        if (feof(f))
            break;
	
	signal = atoi(linebuf);
	observation[k].sir[apId] = signal;
    }

    printf("\n***20***\n");
    fflush(stdout);

    fclose(f);
    return true;
}
   
bool trainingDataPackage::readFile(const char *fileName, observationRecord *o, int *location)
{
    struct stat statbuf;
    char linebuf[200];
    int i;

    if (stat(fileName, &statbuf)<0)
        return false;
        
    if (S_ISDIR(statbuf.st_mode)) {
        printf("%s is a directory\n", fileName);
        return false;
    }

    FILE *f = fopen(fileName, "r");
    if (!f) {
        fprintf(stderr, "Cannot open for reading: %s\n", fileName);
        return false;    
    }

    do {
        fgets(linebuf, sizeof(linebuf), f);
    } while (strcmp(linebuf, "[position]\n"));

    char description[100];
    fgets(description, sizeof(description), f);
    strtok(description, "\n");
    if (strcmp(description, "Unknown")) {
        fgets(linebuf, sizeof(linebuf), f);
        int xPos = atoi(strtok(linebuf, " "));
        int yPos = atoi(strtok(NULL, " "));
        int zPos = atoi(strtok(NULL, " "));
    
        *location = scenario->getLocationIndex(description, xPos, yPos, zPos);
    } else {
        *location = -1;
    }
    
    o->zero();
    
    do {
        fgets(linebuf, sizeof(linebuf), f);
    } while (strcmp(linebuf, "[stations]\n"));

    do {
        fgets(linebuf, sizeof(linebuf), f);
        if ((linebuf[0]!='B') || feof(f))
            break;
        strtok(linebuf, "=");
        char *bssid = strtok(NULL, " ");
        strtok(NULL, "=");
        int freq = atoi(strtok(NULL, " "));
        strtok(NULL, "=");
        int mode = atoi(strtok(NULL, " "));
        strtok(NULL, "=");
        int sir = atoi(strtok(NULL, " "));
        strtok(NULL, "=");
        int snr = atoi(strtok(NULL, " "));
        strtok(NULL, "=");
        char *essid = strtok(NULL, "\r\n");

        if (mode != 3) {
            dprintf("Unknown mode in %s: %d\n", fileName, mode);
        }

        int thisAP = scenario->getApIndex(bssid, essid);
        if (thisAP>=0) {
            o->sir[thisAP] = sir;
            o->snr[thisAP] = snr;
        } else {
            dprintf("Ignoring AP %s, ESSID %s\n", bssid, essid);
        }
    } while (1);
    
    fclose(f);
    
    return true;
}

void trainingDataPackage::addTrainingDirectory(const char *directoryName)
{
    struct dirent *entry1, *entry2, *entry3;
    DIR *scanFileDir1, *scanFileDir2, *scanFileDir3  ;
    int snifferId;//, loctnId;
    char fileName1[200], fileName2[200], fileName3[200];
    char out_file[100];
    
    if (!(scanFileDir1 = opendir(directoryName))) {
        printf("Cannot open Training directory for reading: %s\n", directoryName);
        exit(1);
    }
    
    printf("\n***6***\n");
    fflush(stdout);
    
    while (entry1 = readdir(scanFileDir1)) {
        if (strcmp(entry1->d_name, ".") && strcmp(entry1->d_name, "..")) {

	    snifferId = scenario->getSnifferIndex(entry1->d_name);
            memset( fileName1, 0, sizeof(fileName1));
            sprintf(fileName1, "%s/%s", directoryName, entry1->d_name);

	    printf("%s/%s snifferId:%d", directoryName, entry1->d_name, snifferId);	
	    
    	    if ( !( scanFileDir2 = opendir(fileName1))) {
        	printf("Cannot open sniffer directory for reading: %s\n", fileName1);
        	exit(1);
    	    }
            sprintf( out_file, "Scans/%d", snifferId);

	    while (entry2 = readdir(scanFileDir2)) {
		if (strcmp(entry2->d_name, ".") && strcmp(entry2->d_name, "..")) {
			memset( fileName2, 0, sizeof(fileName2));
                	sprintf(fileName2, "%s/%s", fileName1, entry2->d_name);
                	printf("--- filename:%s\n",fileName2);
			tshark_parseFile(fileName2, out_file);
                }
            }
	    closedir(scanFileDir2);
	    addFile(out_file);
	}
    }
    closedir(scanFileDir1);
}
#if 0

			loctnId = scenario->getLoctnIndex(entry2->d_name);
            		memset( fileName2, 0, sizeof(fileName2));
                	sprintf(fileName2, "%s/%s", fileName1, entry2->d_name);
			printf("snifferId:%d loctnId:%d pathname:%s\n",snifferId, loctnId, fileName2);

           		if ( !( scanFileDir3 = opendir(fileName2))) {
                		printf("Cannot open location directory for reading: %s\n", fileName2);
                		exit(1);
           		}
                        sprintf( out_file, "Scans/%d.%d", snifferId, loctnId);
			while (entry3 = readdir(scanFileDir3)) {
                		if (strcmp(entry3->d_name, ".") && strcmp(entry3->d_name, "..")) {
            				memset( fileName3, 0, sizeof(fileName3));
                			sprintf(fileName3, "%s/%s", fileName2, entry3->d_name);
                			printf("--- filename:%s\n",fileName3);
					tshark_parseFile(fileName3, out_file);
                		}
           		}
	   		closedir(scanFileDir3);
			addFile(out_file);
	         }
             }
	     closedir(scanFileDir2);
        }
    }
    closedir(scanFileDir1);
}

#endif

int trainingDataPackage::tshark_parseFile(const char *fileName, const char *out_file)
{
    printf("loading %s\n", fileName);
   
    char linebuff[256], *tmp; 	int signal;
    char command[100];
    char *parsed_filename = "tmp_parsed";

    sprintf( command, "tshark -Vr %s > %s", fileName, parsed_filename);
    system(command);
   
    FILE *input = fopen(parsed_filename,"r"); /*Open tshark parsed trace file*/
    if (!input) {
	printf("Cannot open tshark trace file \n");
	return 0;
    }
 
    FILE *output = fopen(out_file,"a"); /*Open RWL format file for creating*/
    if (!output) {
                printf("Cannot create output file \n");
                return 0;
    }
//--------------------------------------    
    while (!feof(input)) {
 	memset(linebuff, 0, sizeof(linebuff));
	fgets(linebuff, sizeof(linebuff), input);
        if (!strstr(linebuff,"SSI Signal:"))
              continue;

        strtok(linebuff,":");
        tmp = strtok(NULL," ");
        signal = atoi(tmp);

        fprintf(output,"%d\n", signal);
    }


//----------------------------------------
    fclose(input);
    fclose(output);
    remove(parsed_filename);
    return 0;
}

void trainingDataPackage::addDirectory(const char *directoryName)
{
    struct dirent *entry;
    DIR *scanFileDir;
    
    if (!(scanFileDir = opendir(directoryName))) {
        printf("Cannot open directory for reading: %s\n", directoryName);
        exit(1);
    }
    
    printf("\n***6***\n");
    fflush(stdout);
    
    while (entry = readdir(scanFileDir)) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            char fileName[200];
            sprintf(fileName, "%s/%s", directoryName, entry->d_name);

    	    printf("\n***7*** filename: %s\n",fileName);
    	    fflush(stdout);
            addFile(fileName);
    	    printf("\n***8***\n");
    	    fflush(stdout);
        }
    }
    
    closedir(scanFileDir);
}

void trainingDataPackage::applyCorrective(float a, float b, float c)
{
    for (int i=0; i<numObservations; i++)
        observation[i].applyCorrective(a, b, c);
}

void trainingDataPackage::addFile(const char *fileName)
{
    dprintf("loading %s\n", fileName);

    printf("\n***9***\n");
    fflush(stdout);
    readNewFile(fileName);//, observation, locationIndex);
    //if (readFile(fileName, &observation[numObservations], &locationIndex[numObservations]))
    //    numObservations ++;

    printf("\n***10***\n");
    fflush(stdout);
    if (numObservations >= maxObservations) {
        printf("Too much training data: %d records\n", maxObservations);
        exit(1);
    }
}

void trainingDataPackage::dump()
{
    char buffer[256];
    
    scenario->dump();
    for (int i=0; i<numObservations; i++) {
        printf("Observation #%d at %s:\n", i, scenario->getLocation(locationIndex[i])->render(buffer));
        observation[i].dump();
    }
}

void trainingDataPackage::swapObservations(int i, int j)
{
    assert((0<=i) && (i<=numObservations));
    assert((0<=j) && (j<=numObservations));

    observationRecord hO;
    int hL;
    
    hO = observation[i];
    observation[i] = observation[j];
    observation[j] = hO;
    
    hL = locationIndex[i];
    locationIndex[i] = locationIndex[j];
    locationIndex[j] = hL;
}

void trainingDataPackage::patchIncompleteObservations()
{
    observationRecord avg[MAX_LOCATIONS];
    for (int i=0; i<MAX_LOCATIONS; i++)
        avg[i].zero();
        
    for (int i=0; i<numObservations; i++) {
        for (int j=0; j<scenario->getNumAP(); j++) {
            if (observation[i].sir[j]) {
                avg[locationIndex[i]].sir[j] += observation[i].sir[j];
                avg[locationIndex[i]].snr[j] ++;
            }
        }
    }
    
    for (int i=0; i<numObservations; i++) {
        for (int j=0; j<scenario->getNumAP(); j++) {
            if ((observation[i].sir[j] == 0) && (avg[locationIndex[i]].snr[j]>0))
                observation[i].sir[j] = avg[locationIndex[i]].sir[j]/avg[locationIndex[i]].snr[j];
        }
    }
}

locationRecord *trainingDataPackage::findAP(int apIndex)
{
    int bestSIR = 0, bestLocation = 0;
    
    assert((apIndex>=0) && (apIndex<MAX_AP));
    for (int i=0; i<numObservations; i++) {
        if (observation[i].sir[apIndex] > bestSIR) {
            bestSIR = observation[i].sir[apIndex];
            bestLocation = locationIndex[i];
        }
    }
    
    return scenario->getLocation(bestLocation);
}

int trainingDataPackage::numObservationsAtLocation(int index)
{
    int result = 0;
    
    for (int i=0; i<numObservations; i++)
        if (locationIndex[i] == index)
            result ++;
            
    return result;
}

observationRecord trainingDataPackage::removeObservation(int index)
{
    assert((index>=0) && (index<numObservations));

    observationRecord result = observation[index];
    for (int i=index; i<(numObservations-1); i++) {
        observation[i] = observation[i+1];
        locationIndex[i] = locationIndex[i+1];
    }
    
    numObservations--;
    return result;
}

int trainingDataPackage::firstObservationAt(int index)
{
    for (int i=0; i<numObservations; i++)
        if (locationIndex[i] == index)
            return i;

    return -1;
}

trainingDataPackage *trainingDataPackage::clone()
{
    trainingDataPackage *peer = new trainingDataPackage();
    
    for (int i=0; i<numObservations; i++) {
        peer->observation[i] = observation[i];
        peer->locationIndex[i] = locationIndex[i];
    }
    
    peer->numObservations = numObservations;
    peer->scenario = scenario->clone();
    
    return peer;
}

void trainingDataPackage::writeObservationsTo(const char *fileName)
{
    int numAP = scenario->getNumAP();
    FILE *f = fopen(fileName, "w+");
    
    if (!f)
        panic("Cannot open for writing: %s", fileName);
        
    fprintf(f, "%d\n", numObservations);
    for (int i=0; i<numObservations; i++) {
        fprintf(f, "%d", locationIndex[i]);
        for (int j=0; j<numAP; j++) 
            fprintf(f, " %d", observation[i].sir[j]);
            //fprintf(f, " %d:%d", observation[i].sir[j], observation[i].snr[j]);
        fprintf(f, "\n");
    }
    
    fclose(f);
}

void trainingDataPackage::readObservationsFrom(const char *fileName, int M)
{
    FILE *f = fopen(fileName, "r");
    char linebuf[256];
    
    if (!f)
        panic("Cannot open for reading: %s", fileName);
        
    fgets(linebuf, sizeof(linebuf)-1, f);
	//numObservations = atoi(linebuf);
	numObservations = M;
	printf("No of observations to read from file %s: %d (max %d)\n", fileName, numObservations, maxObservations);    
	
	if (numObservations > maxObservations)
        panic("Too many observations in file %s: %d (max %d)", fileName, numObservations, maxObservations);    

    for (int i = 0; i < numObservations; i++) {
    
		observation[i].zero();
		memset(linebuf, 0, sizeof(linebuf));
        fgets(linebuf, sizeof(linebuf)-1, f);
        
        char *s = strtok(linebuf, " ");
        int apIndex = 0;

		locationIndex[i] = atoi(s); 
        s = strtok(NULL, " ");
        while (s) {
            observation[i].sir[apIndex] = atoi(s);
            observation[i].snr[apIndex] = 97;
            apIndex ++;
            if (apIndex == MAX_AP)
                panic("Too many access points in %s", fileName);
                
            s = strtok(NULL, " ");
        }
		observation[i].dump();
    }
    fclose(f);
}

void trainingDataPackage::limitObservationsPerLocation(int limit)
{
    int firstObservationAt[MAX_LOCATIONS];
    int numObservationsAt[MAX_LOCATIONS];
    int *nextRecord;
    bool *marked;
    
    marked = (bool*)malloc(MAX_OBSERVATIONS * sizeof(bool));
    if (!marked)
        panic("Out of memory while allocating marker field");
    nextRecord = (int*)malloc(MAX_OBSERVATIONS * sizeof(int));
    if (!nextRecord)
        panic("Out of memory while allocating chain field");
    
    for (int i=0; i<MAX_LOCATIONS; i++) {
        firstObservationAt[i] = -1;
        numObservationsAt[i] = 0;
    }
    for (int i=0; i<numObservations; i++) {
        nextRecord[i] = firstObservationAt[locationIndex[i]];
        firstObservationAt[locationIndex[i]] = i;
        numObservationsAt[locationIndex[i]] ++;
        marked[i] = false;
    }

    for (int i=0; i<scenario->getNumLocations(); i++) {
        while (numObservationsAt[i] > limit) {
            int pos;
            int k = rand() % numObservationsAt[i];
            pos = firstObservationAt[i];
            while ((k>0) || (marked[pos])) {
                pos = nextRecord[pos];
                assert(pos>=0);
                if (!marked[pos])
                    k--;
            }
            assert(locationIndex[pos] == i);
            marked[pos] = true;
            numObservationsAt[i] --;
        }
    }
    
    int pos = 0;
    for (int i=0; i<numObservations; i++) {
        if (!marked[i]) {
            locationIndex[pos] = locationIndex[i];
            observation[pos] = observation[i];
            pos ++;
        }
    }
    
    assert(pos <= (limit * scenario->getNumLocations()));
    numObservations = pos;
    
    free(marked);
    free(nextRecord);
}

void trainingDataPackage::removeAP(int apIndex)
{
    for (int i=0; i<numObservations; i++)
        observation[i].removeAP(apIndex);
        
    scenario->removeAP(apIndex);
}
