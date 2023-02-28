/****************************************************************
 * Copyright (c) 2023 Jerome Brenig, Sigrun May
 * Ostfalia Hochschule für angewandte Wissenschaften
 *
 * This software is distributed under the terms of the MIT license
 * which is available at https://opensource.org/licenses/MIT
 *
 *   timeStamps.h
 *
 */

#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

// public functions
int tsSetTimer(void);

int tsStopTimer(void);

int tsGetTime(double *time);

#endif /* TIMESTAMPS_H */