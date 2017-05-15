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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "eval.h"

locationRecord::locationRecord(const char *description, int x, int y, int z)
{
	strncpy(this->description, description, sizeof(this->description));
	this->description[sizeof(this->description)-1] = 0;

	this->x = x;
	this->y = y;
	this->z = z;
	
	if (strlen(description)>sizeof(this->description)) {
		printf("Description too long: %s\n", description);
		exit(1);
	}
}

locationRecord::locationRecord(const char *description, int x, int y, int z, double ssi_157, double ssi_153, double ssi_154, double ssi_156)
{
	strncpy(this->description, description, sizeof(this->description));
	this->description[sizeof(this->description)-1] = 0;

	this->x = x;
	this->y = y;
	this->z = z;

	this->ssi_157 = ssi_157;
	this->ssi_153 = ssi_153;
	this->ssi_154 = ssi_154;
	this->ssi_156 = ssi_156;
	
	if (strlen(description) > sizeof(this->description)) {
		printf("Description too long: %s\n", description);
		exit(1);
	}
}

void locationRecord::dump()
{
	printf("[%4d %3d %d][%4f %4f %4f %4f] %s\n", x, y, z, ssi_157, ssi_153, ssi_154, ssi_156, description);
}

bool locationRecord::isAt(int x, int y, int z)
{
	return ((this->x == x) && (this->y == y) && (this->z == z));
}

bool locationRecord::descriptionEquals(const char *description)
{
	return !strcmp(this->description, description);
}

bool locationRecord::equals(const char *description, int x, int y, int z)
{
	return (isAt(x, y, z) && descriptionEquals(description));
} 

bool locationRecord::location_name_equals(const char *description)
{
	return (descriptionEquals(description));
} 

bool locationRecord::equals(locationRecord *other)
{
	return equals(other->description, other->x, other->y, other->z);
}

char *locationRecord::render(char *buffer)
{
	sprintf(buffer, "%s", description);
	return buffer;
}

int locationRecord::getX()
{
	return x;
}

int locationRecord::getY()
{
	return y;
}

int locationRecord::getZ()
{
	return z;
}

double locationRecord::get153()
{
	return ssi_153;
}
double locationRecord::get154()
{
	return ssi_154;
}
double locationRecord::get156()
{
	return ssi_156;
}
double locationRecord::get157()
{
	return ssi_157;
}

double locationRecord::distanceTo(locationRecord *other)
{
	return distanceTo(other->x, other->y, other->z);
}

double locationRecord::distanceTo(int x, int y, int z)
{
	int dx = this->x - x;
	int dy = this->y - y;
	int dz = (this->z - z)*42;
		
	return (double) sqrt(dx*dx + dy*dy + dz*dz);
}

locationRecord *locationRecord::clone()
{
	return new locationRecord(description, x, y, z);
}
