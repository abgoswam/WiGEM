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

#ifndef __eval_h__
#define __eval_h__


#include <string.h>
#include <iostream>
#include <iomanip>
using namespace std;

#define MAX_AP 80
#define MAX_LOCATIONS 600
#define MAX_OBSERVATIONS 55000
#define MAX_SNIFFERS 9

#define NUM_SNIFFERS 6 
#define NUM_LOCATIONS 58

/* GMM parameters */
	#define NUM_POWER_LEVELS 19 
	#define MID_POINT_POWER_LEVEL 9
	#define RATE_OF_INCREASE 1.0
	#define STD_DEVIATION 2 
	#define MAX_CONVERGE 500 
	/* NUM_OBSERVATIONS is the only value which might need be changed based on the test set */
	#define NUM_OBSERVATIONS 50

/* HORUS parameters */
	#define TRAINING_SET_SIZE 100 

#define TIME_DIVISIONS 72

#define HORUS
#define RADAR

#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); exit(1); } while (0)

class BSSID {
private:
  unsigned char octet[6];
  
public:
  BSSID(const char *textual);
  bool equals(BSSID *other);
  bool equals(const char *textual);
  char *render(char *buf);
};

class accessPointRecord {
private:
  BSSID *bssid;
  char essid[32];
  
public: 
  accessPointRecord(const char *bssidTextual, const char *essid);
  bool equals(const char *bssidTextual, const char *essid);
  bool essid_equals(const char *essid);
  BSSID *getBSSID();
  const char *getSSID();
  void dump();
  accessPointRecord *clone();
};

class locationRecord {
private:
  char description[50];
  int x, y, z;
  double ssi_152, ssi_153, ssi_154, ssi_156, ssi_157, ssi_160;
  
public:
  locationRecord(const char *description, int x, int y, int z, double ssi_152, double ssi_153, double ssi_154, double ssi_156, double ssi_157, double ssi_160);
  locationRecord(const char *description, int x, int y, int z);
  bool location_name_equals(const char *description);
  bool equals(const char *description, int x, int y, int z);
  bool descriptionEquals(const char *description);
  bool equals(locationRecord *other);
  bool isAt(int x, int y, int z);
  void dump();
  char *render(char *buf);
  double distanceTo(locationRecord *other);
  double distanceTo(int x, int y, int z);
  int getX();
  int getY();
  int getZ();
  double get152();
  double get153();
  double get154();
  double get156();
  double get157();
  double get160();
  locationRecord *clone();
};

class scenarioDescriptor {
private:
  static const int maxAP = MAX_AP;
  static const int maxLocations = MAX_LOCATIONS;
  
  accessPointRecord *ap[maxAP];
  locationRecord *location[maxLocations];
  int numAP, numLocations;
  bool isLocked;
  
public: 
  scenarioDescriptor();
  scenarioDescriptor(const char *placeFile, const char *apFile = NULL);
  int getLocationIndex(const char *locationName, int xPos, int yPos, int zPos);
  int getLoctnIndex(const char *locationName);
  int justgetLocationIndex(int xPos, int yPos, int zPos);
  int getLocationIndex(locationRecord *loc);
  int getLocationIndex(const char *locationName);
  int getApIndex(const char *bssid, const char *essid = NULL);
  int getSnifferIndex(const char *essid);
  locationRecord *getLocation(int index);
  locationRecord *findLocation(int x, int y, int z);
  int getNumLocations();
  int getNumAP();
  accessPointRecord *getAP(int index);
  void dump();
  void lock();
  scenarioDescriptor *clone();
  void removeAP(int apIndex);
};

class probabilityVector {
private:
  static const int maxLocations = MAX_LOCATIONS;
  
public:
  long double pLocation[maxLocations];
  
  probabilityVector();
  void setAll(double p);
  void set(int index, double p);
  void setRange(int begin, int end, double p);
  int bestLocationIndex();
  int NNSS_bestLocationIndex();
};

class observationRecord {
public:
  //int locId;
  int sir[MAX_AP];
  int snr[MAX_AP];
  
  observationRecord();
  int euclideanDistanceSqProj(observationRecord *other);
  int euclideanDistanceSq(observationRecord *other);
  double euclideanDistanceSqLoc(locationRecord *LOC);
  void add(observationRecord *other);
  void divideBy(int divisor);
  void zero();
  bool isValid();
  void dump();
  void setTo(int sir, int snr);
  void adjustMax(observationRecord *other);
  void adjustMin(observationRecord *other);
  void applyCorrective(float a, float b, float c);
  bool distinctFrom(observationRecord *other);
  int numVisibleAPs();
  void removeAP(int apIndex);
};

class trainingDataPackage {
private:
  static const int maxObservations = MAX_OBSERVATIONS;

public:
  observationRecord observation[maxObservations];
  int locationIndex[maxObservations];
  scenarioDescriptor *scenario;
  int numObservations;

  trainingDataPackage();
  trainingDataPackage(const char *placeFileName, const char *apFileName = NULL);
  void addFile(const char *fileName);
  void swapObservations(int i, int j);
  void dump();
  bool readNewFile(const char *fileName);//, observationRecord *observation, int *locationIndex);
  bool readFile(const char *fileName, observationRecord *o, int *location);
  void addTrainingDirectory(const char *directoryName);
  void addDirectory(const char *directoryName);
  int tshark_parseFile(const char *fileName, const char *out_file );
  void patchIncompleteObservations();
  void applyCorrective(float a, float b, float c);
  int numObservationsAtLocation(int index);
  locationRecord *findAP(int apIndex);
  observationRecord removeObservation(int index);
  int firstObservationAt(int index);
  trainingDataPackage *clone();
  void writeObservationsTo(const char *fileName);
  void readObservationsFrom(const char *fileName);
  void addObservation(observationRecord o, int locationIndex);
  void limitObservationsPerLocation(int limit);
  void removeAP(int apIndex);
};

class Localizer {
public:
  Localizer() {};
  virtual void localize(observationRecord *observation, probabilityVector *v) {};
};

class GMMLocalizer : public Localizer {
private:
  int m; 			/* m examples in the DataSet */
  long double p[NUM_LOCATIONS]; 	/* multinomial on the location(X) */
  long double q[NUM_POWER_LEVELS];	/* multinomial on powerlevels(Z) */

  long double mu[NUM_SNIFFERS][NUM_LOCATIONS][NUM_POWER_LEVELS];     	/* gaussian mean . conditioned on (X,Z) */
  long double sigma[NUM_SNIFFERS][NUM_LOCATIONS][NUM_POWER_LEVELS];	/* gaussian std dviation. conditioned on (X,Z) */

public:
  GMMLocalizer();
  void ExpectationMaximization(trainingDataPackage *data);
  long double log_p_value(observationRecord *observation, int j, int k);  
  long double p_value(observationRecord *observation, int j, int k);
  virtual void localize(observationRecord *observation, probabilityVector *v);
};

class MaximumLikelihoodLocalizer : public Localizer {
private:
  static const int maxLocations = MAX_LOCATIONS;
  static const int maxAP = MAX_AP;
  double sigma[maxLocations][maxAP], mu[maxLocations][maxAP];
  bool goodAP[maxLocations][maxAP];
  scenarioDescriptor *scenario;
  float wildcards;
  int NumSniffers;
  
public:
  MaximumLikelihoodLocalizer(trainingDataPackage *data, float wildcards);
  MaximumLikelihoodLocalizer(scenarioDescriptor *scenario, const char *mlFileName, float wildcards);

  virtual void localize_HORUS(observationRecord *observation, probabilityVector *v);
  virtual void localize_RADAR(observationRecord *observation, probabilityVector *v);
  
  double getSigma(int locationIndex, int apIndex);
  double getMu(int locationIndex, int apIndex);
  bool isGoodAP(int locationIndex, int apIndex);
    void updateRawProbabilities(observationRecord *observation, probabilityVector *v);
};

class BayesianLocalizer : public Localizer {
private:
  static const int maxLocations = MAX_LOCATIONS;
  static const int maxAP = MAX_AP;
  trainingDataPackage *trainingData;
  float p[maxLocations][maxAP][256];
    float wildcards;
  
public:
  BayesianLocalizer(trainingDataPackage *data, float wildcards);
  virtual void localize(observationRecord *observation, probabilityVector *v);
    void getRawProbabilities(observationRecord *observation, probabilityVector *v);
};

class Statistics {
private:
  static const int granularity = 10;
  bool sorted;
  float *data;
  int numValues;
  int maxValues;

  void sort();

public:
  Statistics();
  void addValue(float value);
  float fractionSmallerThan(float x);
  int numSmallerThan(float x);
  float fractionBetween(float x, float y);
  int numBetween(float x, float y);
  int numTotal();
  float percentile(float n);
  float average();
  float minimum();
  float maximum();
};

class TopologyModel {
private:
  static const int maxLocations = MAX_LOCATIONS;
  static const int maxNeighbours = 10;
   static const int maxInSameRoom = 10;

  int neighbour[maxLocations][maxNeighbours];
  int sameRoom[maxLocations][maxInSameRoom];
  int numNeighbours[maxLocations];
  int numInSameRoom[maxLocations];
  scenarioDescriptor *scenario;
  
public:
  TopologyModel(scenarioDescriptor *scenario, const char *neighbourFile, const char *sameRoomFile = NULL);
  int getNumNeighbours(int locationIndex);
  int getNeighbour(int locationIndex, int neighbourIndex);
  void addNeighbour(int locationIndex, int neighbourIndex);
  void writeTo(const char *neighbourFile);
  bool isNeighbourOf(int locationA, int locationB);
  bool inSameRoom(int locationA, int locationB);
};

class HiddenMarkovModel {
private:
  static const int maxLocations = MAX_LOCATIONS;
  TopologyModel *model;

public:
  HiddenMarkovModel(TopologyModel *model);
  void update(probabilityVector *v);
};

#endif /* defined(__eval_h__) */
