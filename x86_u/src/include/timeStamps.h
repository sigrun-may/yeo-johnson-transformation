/*
 *   timeStamps.h
 *
 *   created on 12.09.2022
 *   author: jbrenig
 *
 */

#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

// public functions
int tsSetTimer(void);
int tsStopTimer(void);
int tsGetTime(double *time); // todo maybe float possible?

#endif /* TIMESTAMPS_H */