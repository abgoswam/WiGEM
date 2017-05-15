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
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "eval.h"

scenarioDescriptor::scenarioDescriptor()
{
	numLocations = 0;
	numAP = 0;
	isLocked = false;
}

scenarioDescriptor::scenarioDescriptor(const char *locationFileName, const char *apFileName)
{
    char linebuf[256];
    int i, j;

	numLocations = 0;
	numAP = 0;
	isLocked = false;

	if (locationFileName) {
	
		FILE *locationFile = fopen(locationFileName, "r");
		if (!locationFile) {
			printf("Cannot open for reading: %s\n", locationFileName);
			exit(1);
		}
    	while (fgets(linebuf, sizeof(linebuf), locationFile)) {
   		 	char *begin = &linebuf[0];
	        while ((*begin == ' ') || (*begin == '\r') || (*begin == '\n'))
   	        begin++;
	        if (*begin) {
				int xPos = atoi(strtok(begin, "\t"));
				int yPos = atoi(strtok(NULL, "\t"));
	  	        int zPos = atoi(strtok(NULL, "\t"));
	  	        
				double ssi_152 = atof(strtok(NULL, "\t"));
	  	        double ssi_153 = atof(strtok(NULL, "\t"));
	  	        double ssi_154 = atof(strtok(NULL, "\t"));
	  	        double ssi_156 = atof(strtok(NULL, "\t"));
	  	        double ssi_157 = atof(strtok(NULL, "\t"));
	  	        double ssi_160 = atof(strtok(NULL, "\t"));
   	     		const char *description = strtok(NULL, "\r\n");
	
				init_signal[0][numLocations] = ssi_152;
				init_signal[1][numLocations] = ssi_153;
				init_signal[2][numLocations] = ssi_154;
				init_signal[3][numLocations] = ssi_156;
				init_signal[4][numLocations] = ssi_157;
				init_signal[5][numLocations] = ssi_160;

				location[numLocations++] = new locationRecord(description, 
															xPos, yPos, zPos, 
															ssi_152, ssi_153, ssi_154, ssi_156, ssi_157, ssi_160);
				location[numLocations - 1]->dump();

   		        if (numLocations >= maxLocations) {
	            	printf("More than %d locations\n", maxLocations);
	                exit(1);
    	        }
			}
        }
	    fclose(locationFile);
    }
	
	if (apFileName) {
		FILE *apFile = fopen(apFileName, "r");
		if (!apFile) {
			printf("Cannot open for reading: %s\n", apFileName);
			exit(1);
		}

		while (fgets(linebuf, sizeof(linebuf), apFile)) {
			strtok(linebuf, "\r\n");
			if (linebuf[0] && (linebuf[0]!='#') && linebuf[1] && (linebuf[2] == ':')) {
				const char *bssidTextual = strtok(linebuf, " ");
				const char *essid = strtok(NULL, "#");	

				ap[numAP++] = new accessPointRecord(bssidTextual, essid);
				ap[numAP - 1]->dump();
				
				if (numAP >= maxAP) {
					printf("More than %d access points\n", maxAP);
					exit(1);
				}
			}
		}
	
		fclose(apFile);
	}
}

void scenarioDescriptor::dump()
{
	printf("Locations: ============================================\n");
	for (int i=0; i<numLocations; i++) {
		printf("%3d: ", i);
		location[i]->dump();
	}
	printf("Access points: ========================================\n");
	for (int i=0; i<numAP; i++) {
		printf("%3d: ", i);
		ap[i]->dump();
	}
}

int scenarioDescriptor::justgetLocationIndex(int xPos, int yPos, int zPos) {
	
	for( int i = 0; i < numLocations; i++)
		if (location[i]->isAt(xPos, yPos, zPos))
			return i;
}
int scenarioDescriptor::getLoctnIndex(const char *locationName)
{
	for (int i=0; i<numLocations; i++)
		if (location[i]->location_name_equals(locationName))
			return i;
}

int scenarioDescriptor::getLocationIndex(const char *locationName, int xPos, int yPos, int zPos)
{
	for (int i=0; i<numLocations; i++)
		if (location[i]->equals(locationName, xPos, yPos, zPos))
			return i;

	if (isLocked) {
		fprintf(stderr, "Scenario locked: getLocationIndex(%s, %d, %d, %d)\n", locationName, xPos, yPos, zPos);
		exit(1);
	}
			
	location[numLocations] = new locationRecord(locationName, xPos, yPos, zPos);
	if (numLocations > maxLocations) {
		fprintf(stderr, "Too many locations: %d\n", maxLocations);
		exit(1);
	}
	
	return numLocations++;
}

int scenarioDescriptor::getLocationIndex(const char *locationName)
{
	for (int i=0; i<numLocations; i++)
		if (location[i]->descriptionEquals(locationName))
			return i;
	
	return -1;
}

int scenarioDescriptor::getLocationIndex(locationRecord *loc)
{
	for (int i=0; i<numLocations; i++)
		if (location[i]->equals(loc))
			return i;
			
	return -1;
}

int scenarioDescriptor::getSnifferIndex(const char *essid)
{
	for (int i=0; i<numAP; i++)
		if (ap[i]->essid_equals(essid))
			return i;
			
	if (!essid) {
		fprintf(stderr, "getApIndex called with wildcards, but AP not found\n");
		return -1;
	}

	return -1;			
}

int scenarioDescriptor::getApIndex(const char *bssid, const char *essid)
{
	for (int i=0; i<numAP; i++)
		if (ap[i]->equals(bssid, essid))
			return i;
			
	if (!essid) {
		fprintf(stderr, "getApIndex called with wildcards, but AP (%s) not found\n", bssid);
		return -1;
	}

	return -1;			
}

locationRecord *scenarioDescriptor::getLocation(int index)
{
	assert((0<=index) && (index<numLocations));
	return location[index];
}

int scenarioDescriptor::getNumLocations()
{
	return numLocations;
}

int scenarioDescriptor::getNumAP()
{
	return numAP;
}

locationRecord *scenarioDescriptor::findLocation(int x, int y, int z)
{
	for (int i=0; i<numLocations; i++)
		if (location[i]->isAt(x, y, z))
			return location[i];
printf("Not found: %d %d %d\n", x, y, z);
exit(99);
	return NULL;
}

accessPointRecord *scenarioDescriptor::getAP(int index)
{
	assert((0<=index) && (index<numAP));
	return ap[index];
}

void scenarioDescriptor::lock()
{
	isLocked = true;
}

scenarioDescriptor *scenarioDescriptor::clone()
{
	scenarioDescriptor *peer = new scenarioDescriptor();
	
	for (int i=0; i<numAP; i++)
		peer->ap[i] = ap[i]->clone();
	for (int i=0; i<numLocations; i++)
		peer->location[i] = location[i]->clone();
	
	peer->numAP = numAP;
	peer->numLocations = numLocations;
	peer->isLocked = isLocked;	
	
	return peer;
}

void scenarioDescriptor::removeAP(int apIndex)
{
    assert((0<=apIndex) && (apIndex<numAP));

    delete ap[apIndex];
    for (int i=apIndex; i<(maxAP-1); i++)
        ap[i] = ap[i+1];
        
    numAP --;
}
