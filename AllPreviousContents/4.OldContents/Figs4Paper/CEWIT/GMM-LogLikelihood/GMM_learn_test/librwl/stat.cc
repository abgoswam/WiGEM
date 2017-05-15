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

#include "eval.h"

Statistics::Statistics()
{
  data = (float*)malloc(sizeof(float)*granularity);
  if (!data) {
    fprintf(stderr, "Cannot allocate statistics buffer\n");
    exit(1);
  }
  numValues = 0;
  maxValues = granularity;
  sorted = true;
}

void Statistics::addValue(float value)
{
  if (numValues == maxValues) {
    float *data2 = (float*)malloc(sizeof(float)*(maxValues+granularity));
    if (!data) {
      fprintf(stderr, "Cannot allocate statistics buffer\n");
      exit(1);
    }
    memcpy(data2, data, sizeof(float)*numValues);
    maxValues += granularity;
    free(data);
    data = data2;
  }
  
  data[numValues++] = value;
  sorted = false;
}

void Statistics::sort()
{
  for (int i=0; i<(numValues-1); i++) {
    float bestValue = data[i];
    int bestIndex = i;
    for (int j=i+1; j<numValues; j++) {
      if (data[j]<bestValue) {
        bestValue = data[j];
        bestIndex = j;
      }
    }
    if (bestIndex != i) {
      float h = data[bestIndex];
      data[bestIndex] = data[i];
      data[i] = h;
    }
  }
  
  sorted = true;
}

int Statistics::numBetween(float x, float y)
{
  int i = 0, j = 0;
  
  if (!sorted)
    sort();
  
  while ((i<numValues) && (data[i]<=x))
    i++;
    
  j = i;
  
  while ((j<numValues) && (data[j]<=y))
    j++;
  
  return (j-i);
}

int Statistics::numSmallerThan(float x)
{
  if (!sorted)
    sort();
    
  return numBetween(data[0]-1, x);
}

float Statistics::fractionBetween(float x, float y)
{
  if (numValues < 1)
    return 0;
    
  return ((float)numBetween(x, y))/numValues;
}

float Statistics::fractionSmallerThan(float x)
{
  if (numValues < 1)
    return 1;
    
  return ((float)numSmallerThan(x))/numValues;
}

int Statistics::numTotal()
{
  return numValues;
}

float Statistics::percentile(float n)
{
  if (!sorted)
    sort();

  assert((0.0<n) && (n<1.0));
  int index = (int)(n * numValues);

  return data[index];
}

float Statistics::minimum()
{
  if (!sorted)
    sort();

    return data[0];
}

float Statistics::maximum()
{
  if (!sorted)
    sort();

    return data[numValues-1];
}

float Statistics::average()
{
  float result = 0;
  
  if (!numValues)
    return 0;
  
  for (int i=0; i<numValues; i++)
    result += data[i];
  
  return result / numValues;
}
