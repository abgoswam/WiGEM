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

extern int flag;

#define MIN_SIR 0
//#define USE_POSSIBILITIES
//#define USE_MINLIMIT					
#define LOG_LIKELIHOOD
//------------------------------------------------

GMMLocalizer::GMMLocalizer( trainingDataPackage *data) {
								
	/* initializing p */
	for (int j = 0; j < NUM_LOCATIONS ; j++) {
		p[j] = (double) 1.0 / NUM_LOCATIONS;
	}
	/* initializing q */
	for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
		q[k] = (double) 1.0 / NUM_POWER_LEVELS;
	}
	/* setting sigma to 1 (say) for all sniffers */
	for (int i = 0; i < NUM_SNIFFERS; i++) 
		for (int j = 0; j < NUM_LOCATIONS ; j++) 
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) 
				sigma[i][j][k] = (double) STD_DEVIATION;

	/* initializing mu */
	/* NOTE: 	I am initializing mu to different values based on power levels 
	*/
	for (int i = 0; i < NUM_SNIFFERS; i++) {
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) {

				if (k < MID_POINT_POWER_LEVEL)
					mu[i][j][k] = data->scenario->init_signal[i][j] - (RATE_OF_INCREASE * (MID_POINT_POWER_LEVEL - k)) ;
				else if (k == MID_POINT_POWER_LEVEL)
					mu[i][j][k] = data->scenario->init_signal[i][j];
				else
					mu[i][j][k] = data->scenario->init_signal[i][j] + (RATE_OF_INCREASE * (k - MID_POINT_POWER_LEVEL) ) ;
				
				/* make sure mu is not negative */
				if (mu[i][j][k] < 0)
					mu[i][j][k] = 0;

			}
		}
	}
}

long double GMMLocalizer::p_value(observationRecord *observation, int j, int k) {
	long double pfactor = 1.0;
	long double pdf;

	printf("\n");
	/* prob (S=observation | X=j , Z=k) */
	for ( int i = 0; i < NUM_SNIFFERS; i++) {
		int x = observation->sir[i];
		
		pdf = (1 / (sigma[i][j][k]*sqrt(2*3.1415926))) * (exp( -((x-mu[i][j][k]) * (x-mu[i][j][k])) / 
							      	       (2*sigma[i][j][k] * sigma[i][j][k]) ));
		printf("%LF-%Lf  ",mu[i][j][k], pdf);
		pfactor *= pdf;
	}
	
	pfactor *= (p[j] * q[k]);
	printf("pfactor:%Lf  ", pfactor);
	return pfactor;
} 


long double GMMLocalizer::log_p_value(observationRecord *observation, int j, int k) {
	long double log_term = 0.0, sum_logpdf = 0.0;
	long double m_jk = 0.0;
	dprintf("\n");
	/* log value of the numerator */
	for ( int i = 0; i < NUM_SNIFFERS; i++) {
		int x = observation->sir[i];
		dprintf ("sniffer#i=%d j=%d k=%d x=%4d mu=%14Le \n", i, j, k, x, mu[i][j][k]);
		
		log_term = ( ( (-1.0 /2.0 ) * (  ((x - mu[i][j][k]) * (x - mu[i][j][k])) / 
					            ( 	sigma[i][j][k] * sigma[i][j][k])	) ) - 
			      					log(sigma[i][j][k] * sqrt(2 * 3.1416))   );
		
		dprintf("value=%14Le:  ",log_term );
		long double prob = exp(log_term); 
		dprintf("prob=%14Le:   \n",prob); 
		
		sum_logpdf += log_term;
	}
	m_jk = 	log(p[j]) + log(q[k]) + sum_logpdf ;
	//cout<<p[j]<<":::"<<q[j]<<":::"<<log(p[j])<<":::"<<log(q[k])<<":::"<<sum_logpdf<<":::"<<m_jk<<":::"<<exp(m_jk); 
	return m_jk;
}

void GMMLocalizer::ExpectationMaximization(trainingDataPackage *data) {

	typedef struct ProbValueNode {
		long double pi[NUM_LOCATIONS][NUM_POWER_LEVELS];
		struct ProbValueNode *next;
	} probNode ;
	
	probNode *head, *tmp;

	long double lambda[NUM_LOCATIONS];
	long double rho[NUM_POWER_LEVELS];
	long double N[NUM_LOCATIONS][NUM_POWER_LEVELS];

	long double Y = 0.0;
	long double sum = 0.0;
	long double m[NUM_LOCATIONS][NUM_POWER_LEVELS];
	int M = data->numObservations;

	printf("No of observations: %d\n", M);
	printf("No of Locations: %d\n", NUM_LOCATIONS);
	printf("No of PowerLevels: %d\n", NUM_POWER_LEVELS);
	printf("No of Sniffers: %d\n", NUM_SNIFFERS);
	fflush(stdout);

	if ( M == 0) {
		printf("No of observations 0 !! No EM possible !!\n"); fflush(stdout);
		return ;
	}

	for ( int t = 0; t < MAX_CONVERGE; t++) {
		
		/* Print Values for this round */
		dprintf("\n\n t = %d\n", t);
		dprintf( "\np-vector: ");
		for ( int j = 0; j < NUM_LOCATIONS; j++) {
			dprintf ("%Lf ", p[j]);
		}
		dprintf( "\nq-vector: ");
		for ( int k = 0; k < NUM_POWER_LEVELS; k++) {
			dprintf ("%Lf ", q[k]);
		}
		dprintf("\nmu-vector: \n");
		for (int i = 0; i < NUM_SNIFFERS; i ++) {
			dprintf("\n\n***Sniffer: %d***\n", i);
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				dprintf("\n\nlocid:%d ", j);
				for (int k = 0; k < NUM_POWER_LEVELS ; k++)
					dprintf(" [#%2d]%2.4Lf ",k, mu[i][j][k]);
			}
		}

		/* EXPECTATION STEP */
		head = NULL;

		/*for each element of the DataSet find the prob of (location, power-level) */
		for (int l = 0; l < M ; l++) {
	
			tmp = (probNode *) malloc(sizeof (probNode));	
			for ( int j = 0; j < NUM_LOCATIONS ; j++) {
				for( int k = 0; k < NUM_POWER_LEVELS; k++)
					tmp->pi[j][k] = 0;
			}	
			tmp->next = head;
			
			//printf("\nobservation %d (locationIndex:%d):", l, data->locationIndex[l]);
			//data->observation[l].dump();
			Y = 0.0;
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
					
					m[j][k] = log_p_value( &(data->observation[l]), j, k);
					Y += exp(m[j][k]);
				}
			}
			dprintf("\n Y=: %Lf  ", Y);	
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				dprintf("\n\n Location - %d \n", j);
				for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
					long double numerator = exp(m[j][k]) ;
					if ( Y != 0.0 ) {
						tmp->pi[j][k] = numerator / Y;
					} else {
						printf("\n\nERROR !! in function 'GMMLocalizer::ExpectationMaximization'.Invalid value of Y. Exiting ! \n"); fflush(stdout);
						exit(1);
					}
					
#ifdef USE_MINLIMIT					
					if( tmp->pi[j][k] < 0.000001)
						tmp->pi[j][k] = 0.0;
#endif
					dprintf("[%d: %.4Lf] ", k, pi[l][j][k]);
				}
			}
			head = tmp;
		}

		/* Calculate expectation for location(lambda)
					     powerlevel(rho) 
					     mean of signal strength given location, powerlevel (N)
		*/
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			sum = 0.0;
			tmp = head;
			while(tmp != NULL) { 			
				for (int k = 0; k < NUM_POWER_LEVELS; k++){
					sum += tmp->pi[j][k];
				}
				tmp = tmp->next;
			}
			lambda[j] = sum;
		}
		for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
			sum = 0.0;
			tmp = head;
			while(tmp !=NULL) {			
				for (int j = 0; j < NUM_LOCATIONS; j++) {
					sum += tmp->pi[j][k];
				}
				tmp = tmp->next;
			}
			rho[k] = sum;
		}

		dprintf("\nDEBUG-0\n");
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
				sum = 0.0;
				tmp = head;	
				while ( tmp != NULL ) {
					sum += tmp->pi[j][k];
					tmp = tmp->next;
				}
				N[j][k] = sum;
				dprintf("j=%d, k=%d, N=%Le\n", j,k, N[j][k]);
			}
		}
	
		/* MAXIMIZATION STEP */
		
		/* maximizing p and q */
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			p[j] = lambda[j] / M;
		}

		for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
			q[k] = rho[k] / M;
			dprintf("k:%d rho:%Lf  sum:%Lf q[%d]:%Lf\n",k, rho[k], sum,k, q[k]);
		}

		dprintf("\nDEBUG\n");
		/* maximizing mu*/
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
				for ( int i = 0; i < NUM_SNIFFERS; i++) {
			
					sum = 0.0;
					tmp = head;
					for (int l = (data->numObservations - 1); l >=0 && tmp !=NULL;  l--) {
						sum += ( tmp->pi[j][k] * data->observation[l].sir[i] );
						tmp = tmp->next;
					}
					
					if (N[j][k] != 0.0)
						mu[i][j][k] = sum / N[j][k];
					else 
						mu[i][j][k] = 0.0;

					dprintf("\n i=%d , j=%d, k=%d, mu=%Le, sum=%Le, N=%Le\n", i, j, k, mu[i][j][k], sum, N[j][k]);
				}
			}
		}
		/* End of Maximization Step */

		while(head) {
			tmp = head->next;
			free(head);
			head = tmp;
		}


#ifdef LOG_LIKELIHOOD	
		/* Finding the log likelihood */
		long double log_likelihood = 0.0;
		for (int l = 0; l < M ; l++) {
			printf("\nobservation %d:", l);
			data->observation[l].dump();

			Y = 0.0;
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
					m[j][k] = log_p_value( &(data->observation[l]), j, k);
					Y += exp(m[j][k]);
				}
			}
			printf("\n Y=: %Le  ", Y);	
			log_likelihood += log(Y);
			printf("\n cumulative log_likelihood = : %Le  ", log_likelihood);	
		}
		log_likelihood = (log_likelihood) / M ;
		
		FILE *llhood;
		llhood = fopen ("log_likelihood.txt", "a+");
		if (!llhood) {
			printf("Error! Failed in writing to log_likelihood");
		        exit(1);
		}
		fprintf(llhood, "%Le\n", log_likelihood);
		fclose(llhood);
		/* ---- */
#endif
	}
}

void GMMLocalizer::localize(observationRecord *observation, probabilityVector *v) {

	long double Y = 0.0;
	long double m[NUM_LOCATIONS][NUM_POWER_LEVELS];
	long double p[NUM_LOCATIONS][NUM_POWER_LEVELS];

	/* Bayesian localization */

	for (int j = 0; j < NUM_LOCATIONS ; j++) {
		for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
			
			m[j][k] = log_p_value( observation, j, k);
			Y += exp(m[j][k]);
		}
	}

	for (int j = 0; j < NUM_LOCATIONS ; j++) {
		for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
			if ( Y <= 0) {
				printf("\n\nERROR !! in function 'GMMLocalizer::localize'. Exiting ! \n"); fflush(stdout);
				exit(1);
			}
			p[j][k] = exp( m[j][k]) / (Y);
		}
	}
	
	for (int j = 0; j < NUM_LOCATIONS ; j++) {
		long double sum_k = 0.0;
		for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
			/* prob of ( X=j, Z=k | S=S[l] */
			sum_k += p[j][k];
		}			
        v->pLocation[j] = sum_k;
	}
}
		
//-------------------------------------------------

MaximumLikelihoodLocalizer::MaximumLikelihoodLocalizer(trainingDataPackage *data, float wildcards)
{
	NumSniffers = data->scenario->getNumAP();
    	scenario = data->scenario;
    	this->wildcards = wildcards;
	
	for (int k = 0; k < data->scenario->getNumLocations(); k++) {
		
		double sigmasquare_diff[NumSniffers][NumSniffers];
		double average_diff[NumSniffers][NumSniffers];

		for (int i = 0; i < NumSniffers; i++) {
		    for (int j = 0 ; j < NumSniffers; j++) {
			average_diff[i][j] = 0;
			sigmasquare_diff[i][j] = 0;
		    }
		}
		
		/* Compute maximum likelihood estimate for the mean (parameter mu).
   		   Note that 'zero' observations are ignored. */
		
		for (int i = 0; i < data->numObservations; i++) {
			if (data->locationIndex[i] == k) {
				for (int alpha = 0; alpha < NumSniffers - 1; alpha++) {
					for (int beta = alpha + 1; beta < NumSniffers ; beta++) {
						/*
						 * this is the i th observation under consideration 
						 * find the difference between aplha and beta
						 */
						average_diff[alpha][beta] += (data->observation[i].sir[alpha] - data->observation[i].sir[beta]);
					/*
					if (data->observation[i].sir[j] > MIN_SIR) {
						average[j] += data->observation[i].sir[j];
						numSeen[j] ++;
					}
					phisto[j][data->observation[i].sir[j]] += 1.0;
					*/
					}
				}
			}
		}

		for (int i = 0; i < NumSniffers; i++)
		    for (int j = 0; j < NumSniffers; j++)
			average_diff[i][j] /= NUM_OBSERVATIONS;

		/* Compute maximum likelihood estimate for the standard deviation 
		   (parameter sigma) */
		
		for (int i = 0; i < data->numObservations; i++) {
			if (data->locationIndex[i] == k) {
				for (int alpha = 0; alpha < NumSniffers -1; alpha++) {
					for (int beta = alpha + 1; beta < NumSniffers; beta++) {

						int x = (data->observation[i].sir[alpha] - data->observation[i].sir[beta]);
						sigmasquare_diff[alpha][beta] += 
							(x - average_diff[alpha][beta]) * (x - average_diff[alpha][beta]);
					/*
					if (data->observation[i].sir[j] > MIN_SIR) {
						sigmasquare[j] += (data->observation[i].sir[j] - average[j]) * (data->observation[i].sir[j] - average[j]);
					}
					*/
					}
				}
			}
		}

		for (int i = 0; i < NumSniffers ; i++) {
		    for (int j = 0; j < NumSniffers ; j++) {
			/* if (sigmasquare_diff[i][j] == 0)  Special Case. Needs extra handling
			    continue;
			*/
			if (sigmasquare_diff[i][j] < 1)
			    sigmasquare_diff[i][j] = 1;
			else
			    sigmasquare_diff[i][j] /= NUM_OBSERVATIONS;
		    }
		}
		/*
		for (int i=0; i<maxAP; i++) {
			double sigma = sqrt(sigmasquare[i]);
			double deviation = 0;
			
			if (numSeen[i]) {
				for (int j=0; j<256; j++) {
					double normal = (1/(sigma*sqrt(2*3.1415926)))*(exp(-(j-average[i])*(j-average[i])/(2*sigmasquare[i])));
					phisto[i][j] /= numSeen[i];
					deviation += (phisto[i][j]-normal)*(phisto[i][j]-normal);
				}
			}
			
			goodAP[k][i] = (deviation < 0.2);
		}
		*/

		for (int i = 0; i < NumSniffers; i++) {
		    for (int j = 0; j < NumSniffers; j++) {
			sigma2[k][i][j] = sqrt(sigmasquare_diff[i][j]);
			mu2[k][i][j] = average_diff[i][j];
		    }
		}
	}
}

/* To handle rare modes, every observation can be given a certain number
   of 'wildcards'. The algorithm determines a corresponding number of SIRs 
   that are most inconsistent with the current location and ignores them. */

void MaximumLikelihoodLocalizer::localize(observationRecord *observation, probabilityVector *v)
{
	/* Bayesian localization */
	
	for (int i = 0; i < scenario->getNumLocations(); i++) {

	        double locationFactor = 0;
			
		char buf[256];
		dprintf("+++ Location update for (%d) %s\n", i, scenario->getLocation(i)->render(buf));
			
		/* Each SIR is considered a separate observation. We compute
		   how likely it is to observe this SIR for this AP at the
		   given location. */
				
		for (int alpha = 0; alpha < NumSniffers ; alpha++) {
			for (int beta = alpha + 1; beta < NumSniffers ; beta++) {

				/* if (sigma2[i][j][l] == 0)
					continue;  possible only when j and l BOTH heard nothing in training */
 
				double x = observation->sir[alpha] - observation->sir[beta];
				double factor = (1 /(sigma2[i][alpha][beta] * sqrt(2*3.14159265))) *
						exp( -((x - mu2[i][alpha][beta]) * (x - mu2[i][alpha][beta])) 
							/ (2 * sigma2[i][alpha][beta] * sigma2[i][alpha][beta]) );
				
				/* The probability can be very close to zero, so we
				   use a lower limit. */

#ifdef USE_MINLIMIT					
				if (factor<0.00001)
					factor = 0.00001;
#endif
				dprintf("AP#%2d: %s SIR=%3d factor=%.8f\n", j, scenario->getAP(j)->getBSSID()->render(buf), observation->sir[j], factor);

				locationFactor += factor;
			}
		}
        	v->pLocation[i] = locationFactor;
	}
}

double MaximumLikelihoodLocalizer::getSigma(int locationIndex, int apIndex)
{
	assert((locationIndex>=0) && (locationIndex<=scenario->getNumLocations()));
	assert((apIndex>=0) && (apIndex<=scenario->getNumAP()));

	return sigma[locationIndex][apIndex];
}

double MaximumLikelihoodLocalizer::getMu(int locationIndex, int apIndex)
{
	assert((locationIndex>=0) && (locationIndex<=scenario->getNumLocations()));
	assert((apIndex>=0) && (apIndex<=scenario->getNumAP()));

	return mu[locationIndex][apIndex];
}

bool MaximumLikelihoodLocalizer::isGoodAP(int locationIndex, int apIndex)
{
	assert((locationIndex>=0) && (locationIndex<=scenario->getNumLocations()));
	assert((apIndex>=0) && (apIndex<=scenario->getNumAP()));

	return goodAP[locationIndex][apIndex];
}

MaximumLikelihoodLocalizer::MaximumLikelihoodLocalizer(scenarioDescriptor *scenario, const char *mlFileName, float wildcards)
{
	this->scenario = scenario;
    this->wildcards = wildcards;

	for (int i=0; i<maxLocations; i++) {
		for (int j=0; j<maxAP; j++) {
			sigma[i][j] = 0;
			mu[i][j] = 0;
			goodAP[i][j] = false;
		}
	}
	
	FILE *f = fopen(mlFileName, "r");
	if (!f) 
		panic("Cannot open ML file for reading: %s", mlFileName);

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
				panic("ML file format error");
			
//			printf("[%s]\n", linebuf);
			char *bssid = strtok(&linebuf[4], " ");
			linebuf[23] = 0;
			char *m = &linebuf[24];
			linebuf[30] = 0;
			char *s = &linebuf[33];
			linebuf[38] = 0;
			char *d = &linebuf[41];
			linebuf[46] = 0;
			while (*m==' ') m++;
			while (*s==' ') s++;
			while (*d==' ') d++;
			int apIndex = scenario->getApIndex(bssid);
			if (apIndex < 0)
				panic("AP not found: %s", bssid);
//			printf("m [%s] s [%s] d [%s]\n", m, s, d);
			mu[currentLocation][apIndex] = atof(m);
			sigma[currentLocation][apIndex] = atof(s);
			goodAP[currentLocation][apIndex] = (atof(d)<0.02);
		} else {
			currentLocation = scenario->getLocationIndex(linebuf);
			if (currentLocation == -1)
				panic("Location not found: %s", linebuf);
		}
	}
	
	fclose(f);
}

void MaximumLikelihoodLocalizer::updateRawProbabilities(observationRecord *observation, probabilityVector *v)
{
	for (int i=0; i<scenario->getNumLocations(); i++) {
        double locationFactor = 1.0;

		for (int j=0; j<scenario->getNumAP(); j++) {
			if (observation->sir[j] > MIN_SIR) {
				double factor = (1/(sigma[i][j]*sqrt(2*3.14159265)))*exp(-(observation->sir[j]-mu[i][j])*(observation->sir[j]-mu[i][j])/(2*sigma[i][j]*sigma[i][j]));

				if (factor<0.00001)
					factor = 0.00001;

				locationFactor *= factor;
			}
		}
			
        v->pLocation[i] *= locationFactor;
	}
    
    for (int i=scenario->getNumLocations(); i<maxLocations; i++)
        v->pLocation[i] = 0;
}
