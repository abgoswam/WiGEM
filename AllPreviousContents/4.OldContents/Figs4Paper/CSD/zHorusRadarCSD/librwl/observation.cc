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

#include <stdio.h>
#include <assert.h>
#include "eval.h"

observationRecord::observationRecord()
{
	zero();
}

int observationRecord::numVisibleAPs()
{
	int result = 0;

	for (int i=0; i<MAX_AP; i++)
		if (sir[i]>0)
			result ++;
			
	return result;
}

void observationRecord::applyCorrective(float a, float b, float c)
{
	for (int i=0; i<MAX_AP; i++) {
		if (sir[i]>0) {
			sir[i] = (int)(a*sir[i]*sir[i] + b*sir[i] + c);
            if (sir[i] < 0)
                sir[i] = 0;
            if (sir[i] > 255)
                sir[i] = 255;
		}
	}
}

void observationRecord::setTo(int iSIR, int iSNR)
{
	for (int i=0; i<MAX_AP; i++) {
		sir[i] = iSIR;
		snr[i] = iSNR;
	}
}

void observationRecord::zero()
{
	setTo(0, 0);
}

bool observationRecord::isValid() {
	for ( int i = 0; i < MAX_AP; i++) {
		if (sir[i])	
			return 1;
	}
	return 0;
}

void observationRecord::dump()
{
	for (int i=0; i < NUM_SNIFFERS; i++)
		printf("%3d ", sir[i]);
	
	printf("\n");
}

int observationRecord::euclideanDistanceSqProj(observationRecord *other)
{
	int result = 0;

	for (int i=0; i<MAX_AP; i++)
		if (sir[i])
			result += (other->sir[i]-sir[i])*(other->sir[i]-sir[i]);

	return result;
}

int observationRecord::euclideanDistanceSq(observationRecord *other)
{
	int result = 0;

	for (int i=0; i<MAX_AP; i++)
		result += (other->sir[i]-sir[i])*(other->sir[i]-sir[i]);

	return result;
}

double observationRecord::euclideanDistanceSqLoc(locationRecord *LOC) {

	return	(	
			(LOC->get157() - sir[0]) * (LOC->get157() - sir[0]) +
			(LOC->get153() - sir[1]) * (LOC->get153() - sir[1]) +
			(LOC->get154() - sir[2]) * (LOC->get154() - sir[2]) +
			(LOC->get156() - sir[3]) * (LOC->get156() - sir[3])); 
}


void observationRecord::add(observationRecord *other)
{
	for (int i=0; i<MAX_AP; i++)
		sir[i] += other->sir[i];
}

void observationRecord::divideBy(int divisor)
{
	assert(divisor != 0);
	for (int i=0; i<MAX_AP; i++)
		sir[i] /= divisor;
}

void observationRecord::adjustMin(observationRecord *other)
{
	for (int i=0; i<MAX_AP; i++)
		if (other->sir[i] < this->sir[i])
			this->sir[i] = other->sir[i];
}

void observationRecord::adjustMax(observationRecord *other)
{
	for (int i=0; i<MAX_AP; i++)
		if (other->sir[i] > this->sir[i])
			this->sir[i] = other->sir[i];
}

bool observationRecord::distinctFrom(observationRecord *other)
{
	for (int i=0; i<MAX_AP; i++)
		if ((this->sir[i]>0) && (other->sir[i]>0))
			return false;
	
	return true;
}

void observationRecord::removeAP(int apIndex)
{
    for (int i=apIndex; i<(MAX_AP-1); i++) {
        sir[i] = sir[i+1];
        snr[i] = snr[i+1];
    }
    
    sir[MAX_AP-1] = 0;
    snr[MAX_AP-1] = 0;
}
