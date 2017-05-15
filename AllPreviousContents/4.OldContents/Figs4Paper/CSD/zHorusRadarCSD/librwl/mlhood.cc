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

#define MIN_SIR 0
//#define USE_POSSIBILITIES
//#define USE_MINLIMIT					

GMMLocalizer::GMMLocalizer() {

	long double init_signal[NUM_SNIFFERS][NUM_LOCATIONS];

/*---------------------------------*/

	/*	Sniffer	0	-	conf	room	-157	*/
	init_signal[0][0]	=	23.1	;				
	init_signal[0][1]	=	27.11	;				
	init_signal[0][2]	=	27.74	;				
	init_signal[0][3]	=	29.6	;				
	init_signal[0][4]	=	34.2	;				
	init_signal[0][5]	=	36.34	;				
	init_signal[0][6]	=	42.07	;				
	init_signal[0][7]	=	43.83	;				
	init_signal[0][8]	=	43.83	;				
	init_signal[0][9]	=	40.26	;				
	init_signal[0][10]	=	44.46	;				
	init_signal[0][11]	=	54.11	;				
	init_signal[0][12]	=	34.61	;				
	init_signal[0][13]	=	36.4	;				
	init_signal[0][14]	=	52.63	;				
	init_signal[0][15]	=	49.98	;				
	init_signal[0][16]	=	29.61	;				
	init_signal[0][17]	=	32.8	;				
	init_signal[0][18]	=	43.21	;				
	init_signal[0][19]	=	44.87	;				
	init_signal[0][20]	=	39.91	;				
	init_signal[0][21]	=	33.94	;				
	init_signal[0][22]	=	27.97	;				
	init_signal[0][23]	=	32.98	;				
	init_signal[0][24]	=	37.11	;				
	init_signal[0][25]	=	38.57	;				
	init_signal[0][26]	=	39.22	;				
	init_signal[0][27]	=	27.04	;				
	init_signal[0][28]	=	28.37	;				
	init_signal[0][29]	=	35.89	;				
	init_signal[0][30]	=	36.86	;				
	init_signal[0][31]	=	37.19	;				
	init_signal[0][32]	=	30.3	;				
	init_signal[0][33]	=	26.32	;				
	init_signal[0][34]	=	26.24	;				
	init_signal[0][35]	=	27.54	;				
	/*	Sniffer	1	;	wings	lab	-153	*/
	init_signal[1][0]	=	32.82	;				
	init_signal[1][1]	=	40.58	;				
	init_signal[1][2]	=	39.71	;				
	init_signal[1][3]	=	44.6	;				
	init_signal[1][4]	=	42.85	;				
	init_signal[1][5]	=	41.8	;				
	init_signal[1][6]	=	36.41	;				
	init_signal[1][7]	=	28.43	;				
	init_signal[1][8]	=	26.89	;				
	init_signal[1][9]	=	50.28	;				
	init_signal[1][10]	=	46.33	;				
	init_signal[1][11]	=	34.71	;				
	init_signal[1][12]	=	46.75	;				
	init_signal[1][13]	=	52.56	;				
	init_signal[1][14]	=	40.19	;				
	init_signal[1][15]	=	33.47	;				
	init_signal[1][16]	=	39.67	;				
	init_signal[1][17]	=	46.49	;				
	init_signal[1][18]	=	43.25	;				
	init_signal[1][19]	=	34.52	;				
	init_signal[1][20]	=	28.39	;				
	init_signal[1][21]	=	23.76	;				
	init_signal[1][22]	=	35.58	;				
	init_signal[1][23]	=	40.98	;				
	init_signal[1][24]	=	39.57	;				
	init_signal[1][25]	=	38.2	;				
	init_signal[1][26]	=	33.3	;				
	init_signal[1][27]	=	33	;				
	init_signal[1][28]	=	36.97	;				
	init_signal[1][29]	=	37.5	;				
	init_signal[1][30]	=	33.54	;				
	init_signal[1][31]	=	32.28	;				
	init_signal[1][32]	=	27.86	;				
	init_signal[1][33]	=	26.82	;				
	init_signal[1][34]	=	31.49	;				
	init_signal[1][35]	=	29.01	;				
	/*	Sniffer	2	;	classroom	-154	*/	
	init_signal[2][0]	=	24.76	;				
	init_signal[2][1]	=	25.35	;				
	init_signal[2][2]	=	26.05	;				
	init_signal[2][3]	=	29.6	;				
	init_signal[2][4]	=	27.09	;				
	init_signal[2][5]	=	27.69	;				
	init_signal[2][6]	=	28.16	;				
	init_signal[2][7]	=	28.41	;				
	init_signal[2][8]	=	28.83	;				
	init_signal[2][9]	=	30.69	;				
	init_signal[2][10]	=	31.91	;				
	init_signal[2][11]	=	32.71	;				
	init_signal[2][12]	=	29.78	;				
	init_signal[2][13]	=	33.99	;				
	init_signal[2][14]	=	36.51	;				
	init_signal[2][15]	=	36.9	;				
	init_signal[2][16]	=	28.79	;				
	init_signal[2][17]	=	34.36	;				
	init_signal[2][18]	=	39.92	;				
	init_signal[2][19]	=	41.77	;				
	init_signal[2][20]	=	40.93	;				
	init_signal[2][21]	=	33.16	;				
	init_signal[2][22]	=	36.89	;				
	init_signal[2][23]	=	38.79	;				
	init_signal[2][24]	=	43.99	;				
	init_signal[2][25]	=	46.69	;				
	init_signal[2][26]	=	49.72	;				
	init_signal[2][27]	=	37.19	;				
	init_signal[2][28]	=	39.35	;				
	init_signal[2][29]	=	44.53	;				
	init_signal[2][30]	=	48.3	;				
	init_signal[2][31]	=	56.42	;				
	init_signal[2][32]	=	48.99	;				
	init_signal[2][33]	=	43.73	;				
	init_signal[2][34]	=	30.76	;				
	init_signal[2][35]	=	33.42	;				
				;				
	/*	Sniffer	3	;	sekars	lab	-155	*/
	init_signal[3][0]	=	25.14	;				
	init_signal[3][1]	=	35.2	;				
	init_signal[3][2]	=	35.96	;				
	init_signal[3][3]	=	32.7	;				
	init_signal[3][4]	=	29.66	;				
	init_signal[3][5]	=	29.37	;				
	init_signal[3][6]	=	25.68	;				
	init_signal[3][7]	=	25.04	;				
	init_signal[3][8]	=	24.48	;				
	init_signal[3][9]	=	34.22	;				
	init_signal[3][10]	=	30.42	;				
	init_signal[3][11]	=	28.73	;				
	init_signal[3][12]	=	37.72	;				
	init_signal[3][13]	=	38.79	;				
	init_signal[3][14]	=	33.48	;				
	init_signal[3][15]	=	31.61	;				
	init_signal[3][16]	=	36.58	;				
	init_signal[3][17]	=	43.94	;				
	init_signal[3][18]	=	39.59	;				
	init_signal[3][19]	=	36.22	;				
	init_signal[3][20]	=	33.8	;				
	init_signal[3][21]	=	29.37	;				
	init_signal[3][22]	=	47.62	;				
	init_signal[3][23]	=	52.52	;				
	init_signal[3][24]	=	47.8	;				
	init_signal[3][25]	=	43.02	;				
	init_signal[3][26]	=	40.1	;				
	init_signal[3][27]	=	47.75	;				
	init_signal[3][28]	=	53.13	;				
	init_signal[3][29]	=	47.64	;				
	init_signal[3][30]	=	42.81	;				
	init_signal[3][31]	=	39.97	;				
	init_signal[3][32]	=	34.94	;				
	init_signal[3][33]	=	33.55	;				
	init_signal[3][34]	=	42.28	;				
	init_signal[3][35]	=	44.17	;				
	
/*---------------------------------*/

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
					mu[i][j][k] = init_signal[i][j] - (RATE_OF_INCREASE * (MID_POINT_POWER_LEVEL - k)) ;
				else if (k == MID_POINT_POWER_LEVEL)
					mu[i][j][k] = init_signal[i][j];
				else
					mu[i][j][k] = init_signal[i][j] + (RATE_OF_INCREASE * (k - MID_POINT_POWER_LEVEL) ) ;
				
				/* make sure mu is not negative */
				if (mu[i][j][k] < 0)
					mu[i][j][k] = 0;

			}
		}
	}
}

long double GMMLocalizer::log_p_value(observationRecord *observation, int j, int k) {
	long double log_term = 0.0, sum_logpdf = 0.0;
	long double m_jk = 0.0;
	dprintf("\n");
	/* log value of the numerator */
	for ( int i = 0; i < NUM_SNIFFERS; i++) {
		int x = observation->sir[i];
		dprintf ("sniffer#i=%d j=%d k=%d x=%4d mu=%14Le sigma=%14Le ", i, j, k, x, mu[i][j][k], sigma[i][j][k]);
		log_term = ( ( (-1.0 /2.0 ) * (  ((x - mu[i][j][k]) * (x - mu[i][j][k])) / 
					            ( sigma[i][j][k] * sigma[i][j][k])	) ) - 
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

	printf(" before Allocation \n"); fflush(stdout);
	printf(" NUM_OBSERVATIONS = %d \n", NUM_OBSERVATIONS); fflush(stdout);
	
	long double pi[NUM_OBSERVATIONS][NUM_LOCATIONS][NUM_POWER_LEVELS]; /* prob measure for ( X=j Z=k | S = S[l] ) */ 
	long double lambda[NUM_LOCATIONS];
	long double rho[NUM_POWER_LEVELS];
	long double N[NUM_LOCATIONS][NUM_POWER_LEVELS];

	long double Y = 0.0;
	long double sum = 0.0;
	long double m[NUM_LOCATIONS][NUM_POWER_LEVELS];


	printf(" after Allocation \n"); fflush(stdout);

	printf("No of observations: %d\n", data->numObservations);
	printf("No of Locations: %d\n", NUM_LOCATIONS);
	printf("No of PowerLevels: %d\n", NUM_POWER_LEVELS);
	printf("No of Sniffers: %d\n", NUM_SNIFFERS);

	fflush(stdout);

	if (data->numObservations == 0) {
		printf("No of observations 0 !! No EM possible !!\n"); fflush(stdout);
		return ;
	}

	for ( int t = 0; t < MAX_CONVERGE; t++) {
		
		/* Print Values for this round */
		printf("\n\n t = %d\n", t);
		printf( "\np-vector: ");
		for ( int j = 0; j < NUM_LOCATIONS; j++) {
			printf ("%Lf ", p[j]);
		}
		printf( "\nq-vector: ");
		for ( int k = 0; k < NUM_POWER_LEVELS; k++) {
			printf ("%Lf ", q[k]);
		}
		printf("\nmu-vector: \n");
		for (int i = 0; i < NUM_SNIFFERS; i ++) {
			printf("\n\n***Sniffer: %d***\n", i);
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				printf("\n\nlocid:%d ", j);
				for (int k = 0; k < NUM_POWER_LEVELS ; k++)
					printf(" [#%2d]%2.4Lf ",k, mu[i][j][k]);
			}
		}

		/* EXPECTATION STEP */

		/*for each element of the DataSet find the prob of (location, power-level) */
		for (int l = 0; l < data->numObservations; l++) {
			printf("\nobservation %d (locationIndex:%d):", l, data->locationIndex[l]);
			data->observation[l].dump();
			Y = 0.0;
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
					
					m[j][k] = log_p_value( &(data->observation[l]), j, k);
					Y += exp(m[j][k]);
					//cout << "--> Y =" << Y;	
				}
			}
			dprintf("\n Y=: %Lf  ", Y);	
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				dprintf("\n\n Location - %d \n", j);
				for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
					long double numerator = exp(m[j][k]) ;
					if ( Y != 0.0 ) {
						pi[l][j][k] = numerator / Y;
						//cout <<"["<< numerator <<"] "<<pi[l][j][k]<<"  ";
					} else {
						printf("\n\nERROR !! in function 'GMMLocalizer::ExpectationMaximization'.Invalid value of Y. Exiting ! \n"); fflush(stdout);
						exit(1);
					}
					
					/* Like the original RWL, we need to have a lower limit on this probability value */
					if (pi[l][j][k] < 0.000001)
						pi[l][j][k] = 0.0;
				
					dprintf("[%d: %.4Lf] ", k, pi[l][j][k]);
				}
			}
		}

		/* Calculate expectation for location(lambda)
					     powerlevel(rho) 
					     mean of signal strength given location, powerlevel (N)
		*/
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			sum = 0.0;
			for (int l = 0; l < data->numObservations; l++) { 			
				for (int k = 0; k < NUM_POWER_LEVELS; k++)
					sum += pi[l][j][k]; 
			}
			lambda[j] = sum;
		}
		for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
			sum = 0.0;
			for (int l = 0; l < data->numObservations; l++) {			
				for (int j = 0; j < NUM_LOCATIONS; j++)
					sum += pi[l][j][k]; 
			}
			rho[k] = sum;
		}

		dprintf("\nDEBUG-0\n");
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
				sum = 0.0;

				for (int l = 0; l < data->numObservations; l++) 
					sum += pi[l][j][k];
		
				N[j][k] = sum;
				dprintf("j=%d, k=%d, N=%Le\n", j,k, N[j][k]);
			}
		}
		printf("\nlambda-vector : ");	
		for (int j = 0; j < NUM_LOCATIONS ; j++) 
			cout << lambda[j] << "  ";
		
		printf("\nrho-vector : ");	
		for (int k = 0; k < NUM_POWER_LEVELS ; k++)
			cout << rho[k] << "   ";
	
		/* MAXIMIZATION STEP */
		
		/* maximizing p and q */
		sum = 0.0;
		for (int j = 0; j < NUM_LOCATIONS ; j++) 
			sum += lambda[j];
		if ( sum != 0.0 ) { 
			for (int j = 0; j < NUM_LOCATIONS ; j++) {
				p[j] = lambda[j] / sum;
			}
		} else {
			printf("\n\nERROR !! in function 'GMMLocalizer::ExpectationMaximization'.Invalid value of sum. Exiting ! \n"); fflush(stdout);
			exit(1);
		}

		sum = 0.0;
		for (int k = 0; k < NUM_POWER_LEVELS ; k++)
			sum += rho[k];
		if ( sum != 0.0 ) {
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
				q[k] = rho[k] / sum;
				dprintf("k:%d rho:%Lf  sum:%Lf q[%d]:%Lf\n",k, rho[k], sum,k, q[k]);
			}
		} else {
			printf("\n\nERROR !! in function 'GMMLocalizer::ExpectationMaximization'.Invalid value of sum. Exiting ! \n"); fflush(stdout);
			exit(1);
		}

		dprintf("\nDEBUG\n");
		/* maximizing mu*/
		for (int j = 0; j < NUM_LOCATIONS ; j++) {
			for (int k = 0; k < NUM_POWER_LEVELS ; k++) {
				for ( int i = 0; i < NUM_SNIFFERS; i++) {
			
					sum = 0.0;
					for (int l = 0; l < data->numObservations; l++) {
						sum += ( pi[l][j][k] * data->observation[l].sir[i] );
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

#ifdef LOG_LIKELIHOOD	
		/* Finding the log likelihood */
		long double log_likelihood = 0.0;
		for (int l = 0; l < data->numObservations; l++) {
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
		log_likelihood = (log_likelihood) / data->numObservations;
		
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

MaximumLikelihoodLocalizer::MaximumLikelihoodLocalizer(trainingDataPackage *data, float wildcards)
{
	scenario = data->scenario;
    this->wildcards = wildcards;
	
	for (int k=0; k<data->scenario->getNumLocations(); k++) {

		double sigmasquare[maxAP];
		double average[maxAP];
		double phisto[maxAP][256];
		int numSeen[maxAP];
		
		for (int i=0; i<maxAP; i++) {
			average[i] = 0;
			sigmasquare[i] = 0;
			numSeen[i] = 0;
			for (int j=0; j<256; j++)
				phisto[i][j] = 0;
		}
		
		/* Compute maximum likelihood estimate for the mean (parameter mu).
   		   Note that 'zero' observations are ignored. */
	
		for (int i=0; i<data->numObservations; i++) {
			if (data->locationIndex[i] == k) {
				for (int j=0; j<data->scenario->getNumAP(); j++) {
					dprintf(" i=%5d: j=%5d: value=%5d\n", i, j, data->observation[i].sir[j]);
					if (data->observation[i].sir[j] > MIN_SIR) {
						average[j] += data->observation[i].sir[j];
						numSeen[j] ++;
					}
					phisto[j][data->observation[i].sir[j]] += 1.0;
				}
			}
		}

		for (int i=0; i<maxAP; i++)
			if (numSeen[i])
				average[i] /= TRAINING_SET_SIZE;

		/* Compute maximum likelihood estimate for the standard deviation 
		   (parameter sigma) */
		
		for (int i=0; i<data->numObservations; i++) {
			if (data->locationIndex[i] == k) {
				for (int j=0; j<data->scenario->getNumAP(); j++) {
					if (data->observation[i].sir[j] > MIN_SIR) {
						sigmasquare[j] += (data->observation[i].sir[j] - average[j]) * (data->observation[i].sir[j] - average[j]);
					}
				}
			}
		}

		for (int i=0; i<maxAP; i++) {
			if (numSeen[i])
				sigmasquare[i] /= TRAINING_SET_SIZE;

			if (sigmasquare[i] < 1)
				sigmasquare[i] = 1;
		}

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
		

		for (int i=0; i<maxAP; i++) {
			sigma[k][i] = sqrt(sigmasquare[i]);
			mu[k][i] = average[i];
		}

		char buf[500];
		memset(buf, 0, sizeof(buf));
		printf("\n%20s  :", scenario->getLocation(k)->render(buf));
		for (int i=0; i < NUM_SNIFFERS; i++) 
			printf("mu:%12.6f  ", mu[k][i]);
	}
}

/* To handle rare modes, every observation can be given a certain number
   of 'wildcards'. The algorithm determines a corresponding number of SIRs 
   that are most inconsistent with the current location and ignores them. */

void MaximumLikelihoodLocalizer::localize_HORUS(observationRecord *observation, probabilityVector *v)
{
    int numVisibleAP = 0;
    for (int i=0; i<scenario->getNumAP(); i++)
        if (observation->sir[i]>0)
            numVisibleAP ++;
    if (!numVisibleAP)
        return;
    int numWildcards = (int)(numVisibleAP * wildcards + 0.5);
    assert((numWildcards < MAX_AP) && (numWildcards<numVisibleAP));
            
	/* Bayesian localization */
	
	double Ptotal = 0;

	for (int i=0; i<scenario->getNumLocations(); i++) {
		double Pmin[MAX_AP];
        double locationFactor = 1.0;
		for (int q=0; q<numWildcards; q++)
			Pmin[q] = 1.0;
			
		char buf[256];
		dprintf("+++ Location update for (%d) %s\n", i, scenario->getLocation(i)->render(buf));
			
		/* Each SIR is considered a separate observation. We compute
		   how likely it is to observe this SIR for this AP at the
		   given location. */
				
		for (int j=0; j<scenario->getNumAP(); j++) {
	
			/*
			 * In the probabilistic technique we are also considering which sniffers are not seen from a specific location 
			 * hence the '>=' logic
			 */
			if (observation->sir[j] >= MIN_SIR) { /* 'if' statement*/

#ifdef USE_POSSIBILITIES
				double factor = exp(-(observation->sir[j]-mu[i][j])*(observation->sir[j]-mu[i][j])/(2*sigma[i][j]*sigma[i][j]));				
#else
				double factor = (1/(sigma[i][j]*sqrt(2*3.14159265)))*exp(-(observation->sir[j]-mu[i][j])*(observation->sir[j]-mu[i][j])/(2*sigma[i][j]*sigma[i][j]));
#endif
				/* The probability can be very close to zero, so we
				   use a lower limit. */

#ifdef USE_MINLIMIT					
				if (factor<0.00001)
					factor = 0.00001;
#endif

				dprintf("AP#%2d: %s SIR=%3d mu=%12.6f factor=%.8f\n", j, scenario->getAP(j)->getBSSID()->render(buf), observation->sir[j], 
										     mu[i][j], factor);

				locationFactor *= factor;
					
				/* Keep track of the most inconsistent observations */
					
				for (int r=0; r<numWildcards; r++) {
					if (factor < Pmin[r]) {
						for (int s=numWildcards-2; s>=r; s--)
							Pmin[s+1] = Pmin[s];
						Pmin[r] = factor;
						break;
					}
				}
			 } /* end of 'if' statement */
		}
			
		/* Use the wildcards to remove some 'bad' observations */
		
		for (int q=0; q<numWildcards; q++) {
    		assert(Pmin[q]>0);
			locationFactor /= Pmin[q];
			dprintf("Wildcard (low) : %.8f\n", Pmin[q]);
		}
		
        v->pLocation[i] = v->pLocation[i]*locationFactor;
        
		Ptotal += v->pLocation[i];
	}

	/* Normalize */

	for (int i=0; i<scenario->getNumLocations(); i++) {
		v->pLocation[i] /= Ptotal;


// (!((v->pLocation[i]>cutoff) && (v->pLocation[i]<1.1)))
//>pLocation[i] = cutoff;
	}
}

/* RADAR Localization */

void MaximumLikelihoodLocalizer::localize_RADAR(observationRecord *observation, probabilityVector *v) {
 
	for (int i = 0; i < scenario->getNumLocations(); i++) {
		double sum = 0.0;
		for (int j = 0; j < scenario->getNumAP(); j++) {
			sum +=  ( (observation->sir[j] - mu[i][j]) * (observation->sir[j] - mu[i][j]));
		}
		v->pLocation[i] = sum;
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
