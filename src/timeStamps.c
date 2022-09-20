/*H***************************************************************************
 * FILENAME : timeStamps.c
 *
 * DESCRIPTION  :
 *          Functions for time measurement.
 *
 * PUBLIC FUNCTIONS :
 *
 * NOTES    :
 *
 * AUTHOR   :       jbrenig           START DATE    : 12 September 2022
 *
 * CHANGES  :
 *
 * DATE     WHO     DETAIL
 *
 *H*/

/*****************************************************************************
 *                               INCLUDES
 *****************************************************************************/

#include <time.h>

/*****************************************************************************
 *                               GLOBALS
 *****************************************************************************/

static clock_t start, end;

/*****************************************************************************
 *                           PRIVATE FUNCTIONS
 *****************************************************************************/

/*****************************************************************************
 *                           PUBLIC FUNCTIONS
 *****************************************************************************/

int tsSetTimer() {
  start = clock();
  return 0;
}

int tsStopTimer() {
  end = clock();
  return 0;
}

int tsGetTime(double *time) {
  *time = (double)(end - start) / CLOCKS_PER_SEC;
  return 0;
}