#ifndef IO_HPP
#define IO_HPP
/*************************************/
#include <stdio.h>
#include "objectives.hpp"

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
