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
#include <stdlib.h>
#include <math.h>

#include "eval.h"

bool useIt[MAX_AP];

/* postprocess.cc -- Fits Gaussians to the data set
 * This tool is run once after the data set has been gathered. It fits a Gaussian distribution
 * to each observed signal strength histogram and produces two output files that contain
 * the parameters of these distributions. These output files can then be used with the
 * localizer. */

int main(int argc, char *argv[])
{
  char buf1[256], buf2[256];

  //int locId[NUM_POSITIONS] = {0, 1, 2, 3, 4};

  if (argc != 6) {
    printf("Usage: postprocess <placeFile> <apFile> <DataSetDirectory> <mlOutFile> <compactOutfile>\n");
    exit(1);
  }

  printf("\n***1***\n");
  fflush(stdout);

  trainingDataPackage *data = new trainingDataPackage(argv[1], argv[2]);

  data->numObservations = NUM_OBSERVATIONS ;//* NUM_POSITIONS;

#if 0
  for ( int k = 0; k < NUM_POSITIONS; k++)
  	for ( int i = 0; i < NUM_OBSERVATIONS; i++)
		data->observation[ k * NUM_OBSERVATIONS + i].locId =  locId[k]; 
#endif

  for ( int i = 0; i < data->numObservations; i++ )
	data->observation[i].zero();

  data->scenario->lock();

  data->addTrainingDirectory(argv[3]);

#if 0
  //printf("\n**** 3.5**** %d\n",data->scenario->isLocked);
  data->addDirectory(argv[3]);

  printf("\n***4***\n");
  fflush(stdout);
#endif
  data->writeObservationsTo(argv[5]);

#if 0

  printf("\n***5***\n");
  fflush(stdout);

  FILE *f = fopen(argv[4], "w+");
  if (!f)
    panic("Cannot open for writing: %s", argv[4]);
  
  for (int locIndex = 0; locIndex < data->scenario->getNumLocations(); locIndex ++) {
    int max = data->numObservationsAtLocation(locIndex);
    if (max == 0)
      continue;

    fprintf(f, "%s\n", data->scenario->getLocation(locIndex)->render(buf1));

    for (int apIndex = 0; apIndex < data->scenario->getNumAP(); apIndex ++) {
      int histo[256];
      for (int i=0; i<256; i++)
        histo[i] = 0;
    
      for (int i=0; i<data->numObservations; i++)
        if (data->locationIndex[i] == locIndex)
          histo[data->observation[i].sir[apIndex]] ++;
      if (max == histo[0])
        continue;

      /* Maximum likelihood estimator */
  
      double average = 0;
      for (int i=1; i<256; i++)
        average += histo[i] * i;
      average /= (max - histo[0]);

      useIt[apIndex] = (max != histo[0]);
    
      double sigmasquare = 0;
      for (int i=1; i<256; i++)
        sigmasquare += histo[i] * (i - average) * (i - average);
      sigmasquare /= (max - histo[0]);
      double sigma = sqrt(sigmasquare);
      if (sigma < 0.1)
        continue;

      float phisto[256];
      phisto[0] = 0;
      for (int i=1; i<256; i++)
        phisto[i] = ((float)histo[i])/(max-histo[0]);

      float normal[256];
      for (int i=0; i<256; i++)
        normal[i] = (1/(sigma*sqrt(2*3.1415926)))*(exp(-(i-average)*(i-average)/(2*sigmasquare)));

      double deviation = 0;
      for (int i=1; i<256; i++)
        deviation += (phisto[i] - normal[i])*(phisto[i] - normal[i]);

      fprintf(f, "  - %s m=%6.2f s=%5.2f d=%.3f\n", 
        data->scenario->getAP(apIndex)->getBSSID()->render(buf1),
        average, sigma, deviation
      );
    }
  }
  
  fclose(f);
#endif      
  return 0;
}

