/****************************************************************
 * Copyright (c) 2023 Jerome Brenig, Sigrun May
 * Ostfalia Hochschule für angewandte Wissenschaften
 *
 * This software is distributed under the terms of the MIT license
 * which is available at https://opensource.org/licenses/MIT
 *
 * FILENAME : timeStamps.c
 *
 * DESCRIPTION  :
 *          Functions for time measurement.
 *
 * PUBLIC FUNCTIONS :
 * int tsSetTimer()
 * int tsStopTimer()
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

/**
 * @brief sets the timer
 *
 * @return int error return code
 */
int tsSetTimer() {
  start = clock();
  return 0;
}

/**
 * @brief stops the timer
 *
 * @return int error return code
 */
int tsStopTimer() {
  end = clock();
  return 0;
}

/**
 * @brief calculates the time between start and stop of the timer
 *
 * @param time result in seconds
 * @return int error return code
 */
int tsGetTime(double *time) {
  *time = (double)(end - start) / CLOCKS_PER_SEC;
  return 0;
}