#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <malloc.h>


#include "basics.h"
#include "bitrankw32int.h"

typedef struct sFTRep {
	  uint listLength;
	  byte nLevels;
	  uint tamCode;
	  uint * levels;
	  uint * levelsIndex;
	  uint * iniLevel;
	  uint * rankLevels;
	  bitRankW32Int * bS;	
	  uint * base;
	  ushort * base_bits;
	  uint * tablebase;
	  uint tamtablebase;

  	
} FTRep;



// public:
	FTRep* createFT(uint *list,uint listLength);
	uint accessFT(FTRep * listRep,uint param);
	void saveFT(FTRep * listRep, char * filename);
	uint * decompressFT(FTRep * listRep, uint n);
	FTRep* loadFT(char * filename);
	void destroyFT(FTRep * listRep);
