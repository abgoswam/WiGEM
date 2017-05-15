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
#include <math.h>
#include <assert.h>
#include "eval.h"

//#define dprintf(a...) printf(a)
#define dprintf(a...)

/* Attenuation factors */

static const int attnLength = 5;
static const int attnCenter = 2;
static float attn[9] = { 1.0/16, 4.0/16, 6.0/16, 4.0/16, 1.0/16 };

void dumpSorted(double *pCond, trainingDataPackage *data)
{
    double lower = 1.0, upper = 1.1;
    int count = 0;
    char buf1[256];
    
    /* Print the location estimates, sorted by probability */
	
    while (upper > 0.0005) {
        double mx = 0;
        for (int i=0; i<data->scenario->getNumLocations(); i++) {
            if ((pCond[i] >= lower) && (pCond[i] < upper)) {
                printf("%.4f: %s\n", pCond[i], data->scenario->getLocation(i)->render(buf1));
                if ((++count) == 10)
                    return;
            }
            if ((pCond[i] < lower) && (pCond[i] > mx))
                mx = pCond[i];
        }
        upper = lower;
        lower = mx;
    }
}

BayesianLocalizer::BayesianLocalizer(trainingDataPackage *data, float wildcards)
{
    this->trainingData = data;
    this->wildcards = wildcards;
	
    /* Compute histograms for every location and every access point */
    
    for (int i=0; i<MAX_LOCATIONS; i++)
        for (int j=0; j<MAX_AP; j++)
            for (int k=0; k<256; k++)
                p[i][j][k] = 0;
                
    for (int i=0; i<data->numObservations; i++)
        for (int j=0; j<data->scenario->getNumAP(); j++)
            p[data->locationIndex[i]][j][data->observation[i].sir[j]] += 1.0;
            
    for (int i=0; i<data->scenario->getNumLocations(); i++) {
        for (int j=0; j<MAX_AP; j++) {
		
            /* Normalize the histograms */
		
            float total = 0;
            for (int k=1; k<256; k++)
                total += p[i][j][k];
            if (total>0.5) {
                for (int k=1; k<256; k++) {
                    p[i][j][k] /= total;
                }
            }
            
            /* Attenuation */
			
            float newP[256];
            for (int k=0; k<256; k++)
                newP[k] = 0;
                
            for (int k=1; k<256; k++) {
                for (int l=0; l<attnLength; l++) {
                    int idx = k+l-attnCenter;
                    if (idx < 0)
                        idx = 0;
                    if (idx > 255)
                        idx = 255;
                    newP[idx] += p[i][j][k]*attn[l];
                }
            }
            
            /* Write the new histogram back to the table */
			
            if (total > 0.5) {
                for (int k=0; k<256; k++) {
                    p[i][j][k] = newP[k];
                }
            }
        }
    }
}

/* To handle rare modes, every observation can be given a certain number
   of 'wildcards'. The algorithm determines a corresponding number of SIRs 
   that are most inconsistent with the current location and ignores them. */

void BayesianLocalizer::localize(observationRecord *observation, probabilityVector *v)
{
    int numVisibleAP = 0;
    for (int i=0; i<trainingData->scenario->getNumAP(); i++)
        if (observation->sir[i]>0)
            numVisibleAP ++;
            
    if (!numVisibleAP)
        panic("No APs visible");
        
    int numWildcards = (int)(numVisibleAP * wildcards + 0.5);
    assert((numWildcards < MAX_AP) && (numWildcards<numVisibleAP));

    /* Bayesian localization */

    double Ptotal = 0;
		
    for (int i=0; i<trainingData->scenario->getNumLocations(); i++) {
        double Pmin[MAX_AP];
        
        for (int q=0; q<numWildcards; q++)
            Pmin[q] = 1.0;

        char buf[256];
        dprintf("+++ Location update for (%d) %s\n", i, trainingData->scenario->getLocation(i)->render(buf));

        /* Each SIR is considered a separate observation. We compute
           how likely it is to observe this SIR for this AP at the
           given location. */
				
        for (int j=0; j<trainingData->scenario->getNumAP(); j++) {
            if (observation->sir[j]) {
                double factor = p[i][j][observation->sir[j]];
					
                /* The probability can be very close to zero, so we
                   use a lower limit. */

                if (factor<0.00001)
                    factor = 0.00001;

                dprintf("AP#%2d: %s SIR=%3d factor=%.8f\n", j, trainingData->scenario->getAP(j)->getBSSID()->render(buf), observation->sir[j], factor);

                v->pLocation[i] *= factor;

                /* Keep track of the most inconsistent observations */
					
                for (int r=0; r<numWildcards; r++) {
                    if (factor < Pmin[r]) {
                        for (int s=numWildcards-2; s>=r; s--)
                            Pmin[s+1] = Pmin[s];
                        
                        Pmin[r] = factor;
                        break;
                    }
                }
            }
        }

        /* Use the wildcards to remove some 'bad' observations */

        assert(Pmin>0);
        for (int q=0; q<numWildcards; q++) {
            v->pLocation[i] /= Pmin[q];
            dprintf("Wildcard: %.8f\n", Pmin[q]);
        }

        Ptotal += v->pLocation[i];
    }

    /* Normalize */

    for (int i=0; i<trainingData->scenario->getNumLocations(); i++) {
        v->pLocation[i] /= Ptotal;
    }
}

void BayesianLocalizer::getRawProbabilities(observationRecord *observation, probabilityVector *v)
{
    for (int i=0; i<trainingData->scenario->getNumLocations(); i++) {
        double locationFactor = 1.0;

        for (int j=0; j<trainingData->scenario->getNumAP(); j++) {
            if (observation->sir[j] > 0) {
                double factor = p[i][j][observation->sir[j]];

                if (factor<0.00001)
                    factor = 0.00001;

                locationFactor *= factor;
            }
        }
			
        v->pLocation[i] = locationFactor;
    }
    
    for (int i=trainingData->scenario->getNumLocations(); i<maxLocations; i++)
        v->pLocation[i] = 0;
}
