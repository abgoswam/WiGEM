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

#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "eval.h"

/* localize.cc -- Perform wireless localization
 * This tool estimates the location of a device based on signal strength measurements
 * from 802.11 access points. To run this tool, you need:
 *    - GIF images of all the floor plans
 *    - a list of all known locations
 *    - a list of all access points in the building
 *    - a signal map (after postprocessing)
 *    - a topology definition
 * If run with just four parameters, the tool continuously performs AP scans,
 * estimates the current position, and displays it using crosshairs on a floor plan.
 * If you already have a set of AP scans (e.g. one that has been collected during
 * a test run), you can specify them as additional parameters; in this case, the
 * tool will just localize these scans without running the graphical interface.
*/

#define dprintf(a...)

#define HISTORY 3
#define USE_HIDDEN_MARKOV
#define USE_MASKING

#define MAX_PLACES 300
#define MAX_MARKERS 600
#define MAX_PATHLEN 200

#define CARD 0

typedef struct {
  const char *name;
  const char *device;
  int offset;
  float factor;
} cardInfo;

cardInfo card[] = {
  { "D-Link DWL-650+", "wlan0",  0, 1.00 },
  { "LinkSys WPC11", "eth1",  50, 0.85 }
};

typedef struct {
  int active;
  int key;
  int xPos, yPos, zPos;
  int type;
  int color;
} markerRecord;

markerRecord marker[MAX_MARKERS];
observationRecord opath[MAX_PATHLEN];
int olocidx[MAX_PATHLEN];
int commandPipe = 0, pathlen = 0, pathidx = 0;

int scanSeq = 0;

void parseEvents(unsigned char *aptab, int len, scenarioDescriptor *scenario, observationRecord *observation)
{
  struct iw_event *currentEvent = (struct iw_event*)aptab;
  char buffer[200], essid[50];
  int apidx = -1;
  int i;

  /* Parse the data returned by the Linux wireless driver after SIOCGIWSCAN,
   * and return an observation vector, which contains the measured signal strength
   * for each known access point */

  observation->zero();
  
  while (len>0) {
    dprintf("%4X %d bytes (%d left)\n", currentEvent->cmd, currentEvent->len, len);
    switch (currentEvent->cmd) {
      case SIOCGIWAP:
        sprintf(buffer, "%2x:%2x:%2x:%2x:%2x:%2x\n", 
          (unsigned char)currentEvent->u.ap_addr.sa_data[0],
          (unsigned char)currentEvent->u.ap_addr.sa_data[1],
          (unsigned char)currentEvent->u.ap_addr.sa_data[2],
          (unsigned char)currentEvent->u.ap_addr.sa_data[3],
          (unsigned char)currentEvent->u.ap_addr.sa_data[4],
          (unsigned char)currentEvent->u.ap_addr.sa_data[5]
        ); 
        dprintf("  MAC %s", buffer);
        apidx = scenario->getApIndex(buffer);
        break;
      case SIOCGIWESSID:
        dprintf("  ESSID %s\n", ((char*)(&currentEvent->u)+8));
        for (i=0; i<32; i++)
          essid[i] = *((char*)(&currentEvent->u)+8+i);
        essid[32] = 0;
        break;
      case IWEVQUAL:
        if (apidx < 0) {
          printf("Ignoring unknown AP %s (ESSID %s)\n", buffer, essid);
        } else if (observation->sir[apidx]>0) {
          printf("Duplicate observation for AP %s (ESSID %s)", buffer, essid);
        } else {
          observation->sir[apidx] = currentEvent->u.qual.level;
          observation->snr[apidx] = currentEvent->u.qual.noise;
        }
        break;
    }
          
    len -= currentEvent->len;
    currentEvent = (struct iw_event*)(((char*)currentEvent) + currentEvent->len);
  }
}

int scanForStations(unsigned char *buffer, int maxlen)
{
  int sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd<0) {
    printf("Cannot open socket\n");
    exit(1);
  }
  
  struct iwreq wrq;
  int ret;

  /* Ask the Linux wireless driver to scan for APs */

  memset(buffer, 0, maxlen);
  strncpy(wrq.ifr_name, card[CARD].device, IFNAMSIZ);
  wrq.u.data.pointer = (caddr_t)buffer;
  wrq.u.data.length = 0;
  wrq.u.data.flags = 0;
  if (ioctl(sd, SIOCSIWSCAN, &wrq) < 0) {
    printf("IOCTL SIOCSIWSCAN failed: %d errno=%d\n", ret, errno);
    exit(1);
  }
  dprintf("Returned %d bytes\n", wrq.u.data.length);

  /* Wait until the result is available */

  do {
    wrq.u.data.pointer = (caddr_t)buffer;
    wrq.u.data.length = maxlen;
    wrq.u.data.flags = 0;
    ret = ioctl(sd, SIOCGIWSCAN, &wrq);
  } while ((ret<0) && (errno==EAGAIN));
  
  if (ret < 0) {
    printf("IOCTL SIOCGIWSCAN failed: %d errno=%d\n", ret, errno);
    exit(1);
  }

  dprintf("Returned %d bytes\n", wrq.u.data.length);
  close(sd);

  return wrq.u.data.length;
}

void scan(observationRecord *observation, scenarioDescriptor *scenario)
{
  unsigned char buffer[4096];
  int len;
  
  observation->zero();
  len = scanForStations(buffer, sizeof(buffer));
  parseEvents(buffer, len, scenario, observation);
}

void frontendInit()
{
  pid_t frontendPid;
  int cPipe[2], dPipe[2];

  pipe(cPipe);
  pipe(dPipe);
  
  /* Start the Java frontend */
  
  if ((frontendPid = fork())==0) {
    close(cPipe[1]);
    close(dPipe[0]);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    dup2(cPipe[0], STDIN_FILENO);
    dup2(dPipe[1], STDOUT_FILENO);
    const char *javahome = getenv("JAVA_HOME");
    if (!javahome)
      panic("Please set environment variable JAVA_HOME!\n");
    char binname[200];
    sprintf(binname, "%s/bin/java", javahome);
    execl(binname, binname,  "-classpath", "frontend", "SnooperControl", NULL);
    fprintf(stderr, "exec failed: %d\n", errno);
    exit(1);
  }
  
  close(cPipe[0]);
  close(dPipe[1]);
  
  commandPipe = cPipe[1];
  close(STDIN_FILENO);
  dup2(dPipe[0], STDIN_FILENO);
}

void frontendCmd(const char *cmd)
{
  if (!commandPipe)
    return;
    
  write(commandPipe, cmd, strlen(cmd));
  write(commandPipe, "\n", 1);
}

void modelInit()
{
  for (int i=0; i<MAX_MARKERS; i++)
    marker[i].active = 0;
}

void modelRedraw(int _mapNo)
{
  char *typeName[] = { "crosshairs", "box", "redbox", "yellowbox", "greenbox" };
  int numMarkers = 0;
  char sbuf[200];
  
  frontendCmd("deleteAllMarkers()");
  sprintf(sbuf, "showImage(floor%d.gif)", _mapNo);
  frontendCmd(sbuf);
  
  for (int i=0; i<MAX_MARKERS; i++) {
    if ((marker[i].active) && (marker[i].zPos == _mapNo)) {
      if (marker[i].type>4) {
        printf("Type>1\n");
        exit(1);
      }
      
      sprintf(sbuf, "createMarker(%d,%s%d,%d,%d)", 
        numMarkers++, typeName[marker[i].type],
        marker[i].color, 
        marker[i].xPos, marker[i].yPos
      );
      frontendCmd(sbuf);
    }
  }
  
  frontendCmd("redraw()");
}
      
void modelUpdate(int key, int type, int xPos, int yPos, int zPos, int color, int _mapNo)
{
  int startPos = key % MAX_MARKERS;
  int foundPos = -1, emptyPos = -1;

  for (int i=0; i<MAX_MARKERS; i++) {
    int pos = (startPos+i)%MAX_MARKERS;
    if (!marker[pos].active && (emptyPos == -1))
      emptyPos = pos;
    if (marker[pos].active && (marker[pos].key == key))
      foundPos = pos;
  }

  if ((foundPos == -1) && (emptyPos == -1)) {
    printf("Too many markers\n");
    exit(1);
  }

  if (foundPos == -1)
    foundPos = emptyPos;
    
  int needsRedraw = 0;
  if ((marker[foundPos].active && (marker[foundPos].zPos == _mapNo)) || (zPos == _mapNo))
    needsRedraw = 1;
    
  marker[foundPos].active = 1;
  marker[foundPos].key = key;
  marker[foundPos].xPos = xPos;
  marker[foundPos].yPos = yPos;
  marker[foundPos].zPos = zPos;
  marker[foundPos].type = type;
  marker[foundPos].color = color;
  
  if (needsRedraw)
    modelRedraw(_mapNo);
}

void localize_main(trainingDataPackage *data, HiddenMarkovModel *hmm)
{
  char buf[200], buf2[200];
  int currentZ = -1;
  int numScans = 0;

  /* If we haven't been invoked with a predefined set of data points,
     start the graphical user interface */

  if (pathlen == 0) {  
    frontendInit();
    frontendCmd("setStatus(Location unknown)");
  }
  
  pathidx = 0;

  /* Instantiate the localizer */

  MaximumLikelihoodLocalizer *localizer = new MaximumLikelihoodLocalizer(data, 0);
  observationRecord o[HISTORY];
  for (int i=0; i<HISTORY; i++)
    o[i].zero();
  
  probabilityVector *v = new probabilityVector();
  v->setRange(0, data->scenario->getNumLocations()-1, 1.0);

  int currentGuessedLocation = -1;  
  while (!pathlen || (pathidx<pathlen)) {
    for (int i=HISTORY-1; i>0; i--)
      o[i] = o[i-1];

    /* If we've been invoked with a predefined set of data points, read the
       next data point; otherwise do a base station scan */

    numScans ++;
    if (pathlen == 0) {  
      scan(&o[0], data->scenario);
      if (currentGuessedLocation != -1) {
#ifdef USE_MASKING
        for (int i=0; i<data->scenario->getNumAP(); i++)
          if (!localizer->isGoodAP(currentGuessedLocation, i)) {
            o[0].sir[i] = 0;
            printf("Dropping AP %s\n", data->scenario->getAP(i)->getBSSID()->render(buf));
          }
#endif
      }
    } else {
      o[0] = opath[pathidx++];
      o[0].dump();
    }

    /* Perform localization */

#if !defined(USE_HIDDEN_MARKOV)
    v->setRange(0, data->scenario->getNumLocations()-1, 1.0);
    for (int i=0; i<HISTORY; i++)
      localizer->localize(&o[i], v);
#else
    if (numScans > 5)
      hmm->update(v);
    localizer->localize(&o[0], v);
#endif

    /* Output the most likely locations, together with our likelihood estimate */

    for (int i=0; i<data->scenario->getNumLocations()-1; i++) {
      if (v->pLocation[i] > 0.05) {
        printf("  - %.2f %s\n", v->pLocation[i], data->scenario->getLocation(i)->render(buf));
      }
    }

    currentGuessedLocation = v->bestLocationIndex();
    locationRecord *loc = data->scenario->getLocation(v->bestLocationIndex());
    printf("=== LOCATION %d: %s (%.2f)\n", pathidx, loc->render(buf), v->pLocation[v->bestLocationIndex()]);
    printf("Location: %s\n", loc->render(buf));

    /* Display the current location on the map */

    sprintf(buf, "setStatus(%s)", loc->render(buf2));
    frontendCmd(buf);
    if (loc->getZ() != currentZ) {
      sprintf(buf, "showImage(frontend/floor%d.gif)", loc->getZ());
      frontendCmd(buf);
      currentZ = loc->getZ();
    }
    frontendCmd("deleteAllMarkers()");
    sprintf(buf, "createMarker(0,crosshairs0,%d,%d)", loc->getX(), loc->getY());
    frontendCmd(buf);
    frontendCmd("redraw()");
  }
}

int main(int argc, char *argv[]) 
{
  if (argc < 5) {
    printf("Usage: localize <placeFile> <neighbourFile> <apFile> <scanFileDir> [scans...]\n\n");
    exit(1);
  }

  trainingDataPackage *data = new trainingDataPackage(argv[1], argv[3]);
  data->scenario->lock();
  data->readObservationsFrom(argv[4]);
  printf("%d scans loaded\n", data->numObservations);
  data->scenario->lock();
  TopologyModel *topo = new TopologyModel(data->scenario, argv[2]);
  HiddenMarkovModel *hmm = new HiddenMarkovModel(topo);

  pathlen = 0;  
  if (argc > 5) {
    int x;
    for (int i=5; i<argc; i++) {
      data->readFile(argv[i], &opath[pathlen], &olocidx[pathlen]);
      pathlen++;
    }
  }

  printf("### begin localization\n");
  localize_main(data, hmm);
  
  return 0;
}
