#ifndef IO_H
#define IO_H
/*************************************/
#include <stdio.h>
#include "objectives.h"

int getNComms();

int getLine(FILE *);

int validArgs(int nArgs);

char *getToken(int i);

char *getCommandName();

void fpfInd(FILE *, int);

void saveTask(Task *, FILE *, int);

void saveAll();

Task *loadTask(FILE *);

void loadAll();
/*************************************/
#endif
