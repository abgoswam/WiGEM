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
#include <assert.h>

#include "eval.h"

TopologyModel::TopologyModel(scenarioDescriptor *scenario, const char *neighbourFile, const char *sameRoomFile)
{
	this->scenario = scenario;

	for (int i=0; i<maxLocations; i++) {
		numNeighbours[i] = 0;
        numInSameRoom[i] = 0;
    }
		
	FILE *f = fopen(neighbourFile, "r");
	if (!f) {
		fprintf(stderr, "Cannot open neighbour file for reading: %s\n", neighbourFile);
		exit(1);
	}

	char linebuf[200];
	int currentLocation = -1;
	
	while (!feof(f)) {
		do {
			fgets(linebuf, sizeof(linebuf), f);
			strtok(linebuf, "\r\n");
		} while (!linebuf[0] && !feof(f));

		if (feof(f))
			break;
			
		if (!strncmp(linebuf, "  - ", 4)) {
			if (currentLocation == -1)
				panic("Neighbour file format error: Neighbour listed, but no location defined!");
			
			int thisNeighbour = scenario->getLocationIndex(&linebuf[4]);
			if (thisNeighbour == -1)
				panic("Neighbour not found: %s", &linebuf[4]);
			if (numNeighbours[currentLocation] == maxNeighbours)
				panic("Too many neighbours for %s", scenario->getLocation(currentLocation)->render(linebuf));
			for (int i=0; i<numNeighbours[currentLocation]; i++)
				if (neighbour[currentLocation][i] == thisNeighbour)
					panic("Duplicate neighbour: %s", linebuf);
			if (thisNeighbour == currentLocation)
				panic("%s cannot be its own neighbour", linebuf);
			neighbour[currentLocation][numNeighbours[currentLocation]++] = thisNeighbour;
		} else {
			currentLocation = scenario->getLocationIndex(linebuf);
			if (currentLocation == -1)
				panic("Location not found: %s", linebuf);
		}
	}
	
	fclose(f);

    if (sameRoomFile != NULL) {
    	f = fopen(sameRoomFile, "r");
	    if (!f) {
		    fprintf(stderr, "Cannot open same room file for reading: %s\n", sameRoomFile);
    		exit(1);
	    }

        currentLocation = -1;
	    while (!feof(f)) {
		    do {
			    fgets(linebuf, sizeof(linebuf), f);
    			strtok(linebuf, "\r\n");
	    	} while (!linebuf[0] && !feof(f));

    		if (feof(f))
	    		break;
			
    		if (!strncmp(linebuf, "  - ", 4)) {
	    		if (currentLocation == -1)
		    		panic("Same room file format error: Neighbour listed, but no location defined!");
			
    			int thisNeighbour = scenario->getLocationIndex(&linebuf[4]);
	    		if (thisNeighbour == -1)
		    		panic("Neighbour not found: %s", &linebuf[4]);
			    if (numInSameRoom[currentLocation] == maxInSameRoom)
    				panic("Too many neighbours for %s", scenario->getLocation(currentLocation)->render(linebuf));
		    	for (int i=0; i<numInSameRoom[currentLocation]; i++)
	    			if (sameRoom[currentLocation][i] == thisNeighbour)
			    		panic("Duplicate neighbour: %s", linebuf);
    			if (thisNeighbour == currentLocation)
	    			panic("%s cannot be its own neighbour", linebuf);
		    	sameRoom[currentLocation][numInSameRoom[currentLocation]++] = thisNeighbour;
    		} else {
	    		currentLocation = scenario->getLocationIndex(linebuf);
		    	if (currentLocation == -1)
			    	panic("Location not found: %s", linebuf);
    		}
        }
	
    	fclose(f);
	}
}

int TopologyModel::getNumNeighbours(int locationIndex)
{
	return numNeighbours[locationIndex];
}

int TopologyModel::getNeighbour(int locationIndex, int neighbourIndex)
{
	assert((locationIndex>=0) && (locationIndex<maxLocations));
	assert((neighbourIndex>=0) && (neighbourIndex<numNeighbours[locationIndex]));
	
	return neighbour[locationIndex][neighbourIndex];
}

void TopologyModel::addNeighbour(int locationIndex, int neighbourIndex)
{
	assert((locationIndex>=0) && (locationIndex<maxLocations));
	assert((neighbourIndex>=0) && (neighbourIndex<maxLocations));
	
	if (numNeighbours[locationIndex] == maxNeighbours)
		panic("Too many neighbours for %d", locationIndex);
		
	neighbour[locationIndex][numNeighbours[locationIndex]++] = neighbourIndex;
}

void TopologyModel::writeTo(const char *neighbourFile)
{
	FILE *f = fopen(neighbourFile, "w+");
	char buf[256];
	
	if (!f) {
		fprintf(stderr, "Cannot open neighbour file for writing: %s\n", neighbourFile);
		exit(1);
	}

	for (int i=0; i<maxLocations; i++) {
		if (numNeighbours[i]>0) {
			fprintf(f, "%s\n", scenario->getLocation(i)->render(buf));
			for (int j=0; j<numNeighbours[i]; j++)
				fprintf(f, "  - %s\n", scenario->getLocation(neighbour[i][j])->render(buf));
		}
	}
	
	fclose(f);
}

bool TopologyModel::isNeighbourOf(int locationA, int locationB)
{
	assert((locationA>=0) && (locationA<maxLocations));
	
	for (int i=0; i<numNeighbours[locationA]; i++)
		if (neighbour[locationA][i] == locationB)
			return true;
			
	return false;
}

bool TopologyModel::inSameRoom(int locationA, int locationB)
{
    if (locationA == locationB)
        return true;

	assert((locationA>=0) && (locationA<maxLocations));
        
	for (int i=0; i<numInSameRoom[locationA]; i++)
		if (sameRoom[locationA][i] == locationB)
			return true;
			
	return false;
}
