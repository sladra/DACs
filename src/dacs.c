#include "dacs.h"


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

#define FACT_RANK 20




ushort * optimizationk(uint * list,int listLength, int * nkvalues){
	
	int t,i,m,k;
		
	uint maxInt=0;
	
	for(i=0;i<listLength;i++)
		if(maxInt<list[i])
			maxInt = list[i];

	uint nBits = bits(maxInt)-1;
	
	uint tamAux = nBits+2;
	

	uint * weight = (uint *) malloc(sizeof(uint)*(maxInt+1));


	for(i=0;i<maxInt+1;i++)
		weight[i]=0;

	for(i=0;i<listLength;i++)
		weight[list[i]]++;
	
	
	uint * acumFreq = (uint *) malloc(sizeof(uint)*tamAux);
	
	uint acumValue = 0;
	for(i=0;i<10;i++){
		acumValue += weight[i];
	}
	
	acumFreq[0]=0;	
	acumValue = 0;
	uint cntb = 1;
	for(i=0;i<maxInt+1;i++){
		if(i==(1<<cntb)){
			acumFreq[cntb]=acumValue;
			cntb++;
		}
			
		acumValue += weight[i];
		
	}
	free(weight);
		
	acumFreq[cntb]=listLength;
	

	long * s = (long *) malloc(sizeof(long)*(nBits+1));

	uint * l = (uint *) malloc(sizeof(uint)*(nBits+1));
	uint * b = (uint *) malloc(sizeof(uint*)*(nBits+1));


	ulong currentSize;
	
	m=nBits;

	s[m]=0;
	l[m]=0;
	b[m]=0;
		
	uint * fc = acumFreq;
	
	ulong minSize;
	uint minPos;
	for(t=m;t>=0;t--){
		minSize=-1;
		minPos=m;
		for(i=m;i>=t+1;i--){
			currentSize = s[i]+(fc[m+1]-fc[t])*(i-t+1)+(fc[m+1]-fc[t])/FACT_RANK;
			if(minSize>currentSize){
				minSize = currentSize;
				minPos=i;
			}
		}

		if(minSize < ((fc[m+1]-fc[t])*(m-t+1))){
			s[t]=minSize;
			l[t]=l[minPos]+1;
			b[t]=minPos-t;
			
		}
	
		else{
			s[t]=(fc[m+1]-fc[t])*(m-t+1);
			l[t]=1;
			b[t]=m-t+1;				
			
		}
		
	}
	
	
	int L = l[0];
	
	ushort *kvalues = (ushort*)malloc(sizeof(ushort)*L);
	
	t=0;
	for(k=0;k< L;k++){
		kvalues[k]=b[t];
		t = t+b[t];
	}
	
	free(acumFreq);
	free(l);
	free(b);
	free(s);
	
	(*nkvalues)=L;
	return kvalues;

	
}



FTRep* createFT(uint *list,uint listLength){
	FTRep * rep = (FTRep *) malloc(sizeof(struct sFTRep));
	uint *levelSizeAux;
	uint *cont;	
	uint *contB;
	
	ushort* kvalues;
	uint nkvalues;
	
	
	
	rep->listLength = listLength;
	register uint i;
	int j, k;
	uint value, newvalue;
	uint bits_BS_len = 0;
		
	kvalues = optimizationk(list,listLength,&nkvalues);
	
	
	ushort kval;
	uint oldval =0;
	uint newval =0;
	
	i=0;
	uint multval=1;
	do{
		oldval=newval;
		if(i>=nkvalues){
			kval = 1<<(kvalues[nkvalues-1]);
		}
		else
			kval=1<<(kvalues[i]);	
		multval*=kval;
		newval = oldval+multval;

		i++;
	}
	while(oldval<newval);
	
	rep->tamtablebase = i;
	rep->tablebase = (uint *) malloc(sizeof(uint)*rep->tamtablebase);
	levelSizeAux = (uint *) malloc(sizeof(uint)*rep->tamtablebase);
	cont = (uint *) malloc(sizeof(uint)*rep->tamtablebase);
	contB = (uint *) malloc(sizeof(uint)*rep->tamtablebase);

	oldval =0;
	newval =0;
	multval=1;	
	for(i=0;i<rep->tamtablebase;i++){
		oldval=newval;
		if(i>=nkvalues){
			kval = 1<<(kvalues[nkvalues-1]);
		}
		else
			kval=1<<(kvalues[i]);	
		multval*=kval;
		newval = oldval+multval;
		rep->tablebase[i]=oldval;
	}	
	
	
	for(i=0;i<rep->tamtablebase;i++){
		levelSizeAux[i]=0;

	}


	for (i=0;i<listLength;i++){
		value = list[i];
		for(j=0;j<rep->tamtablebase;j++)
			if(value>=rep->tablebase[j])
				levelSizeAux[j]++;
	}

	j=0;

	while((j<rep->tamtablebase)&&(levelSizeAux[j]!=0)){
		 j++;
		}
	rep->nLevels = j;

	rep->levelsIndex = (uint *) malloc(sizeof(uint)*(rep->nLevels+1));
	bits_BS_len =0;
	
	rep->base = (uint *)malloc(sizeof(uint)*rep->nLevels);
	rep->base_bits = (ushort *)malloc(sizeof(ushort)*rep->nLevels);
	
	for(i=0;i<rep->nLevels;i++){
			if(i>=nkvalues){
				rep->base[i]=1<<(kvalues[nkvalues-1]);
				rep->base_bits[i]=kvalues[nkvalues-1];
			}
		else{
			rep->base[i]=1<<(kvalues[i]);
			rep->base_bits[i]=kvalues[i];
		}
	}

	uint tamLevels =0;
		

	tamLevels=0;
	for(i=0;i<rep->nLevels;i++)
		tamLevels+=rep->base_bits[i]*levelSizeAux[i];

	rep->iniLevel = (uint *)malloc(sizeof(uint)*rep->nLevels);		
	rep->tamCode=tamLevels;
	
	uint indexLevel=0;
	rep->levelsIndex[0]=0;
	for(j=0;j<rep->nLevels;j++){
		rep->levelsIndex[j+1]=rep->levelsIndex[j] + levelSizeAux[j];
			rep->iniLevel[j] = indexLevel;
			cont[j]=rep->iniLevel[j];
			indexLevel+=levelSizeAux[j]*rep->base_bits[j];
		contB[j]=rep->levelsIndex[j];

	}


	rep->levels = (uint *) malloc(sizeof(uint)*(tamLevels/W+1));

	bits_BS_len = rep->levelsIndex[rep->nLevels-1]+1; 

	uint * bits_BS = (uint *) malloc(sizeof(uint)*(bits_BS_len/W+1));
	for(i=0; i<((bits_BS_len)/W+1);i++)
		bits_BS[i]=0;
	for(i=0;i<listLength;i++){
		value = list[i];
		j=rep->nLevels-1;

		while(j>=0){
			if(value >= rep->tablebase[j]){

				newvalue = value- rep->tablebase[j];

				for(k=0;k<j;k++){
					

					bitwrite(rep->levels,cont[k],rep->base_bits[k],newvalue%rep->base[k]);
					cont[k]+=rep->base_bits[k];
					contB[k]++;

					newvalue = newvalue/rep->base[k];
				}
				k=j;

					bitwrite(rep->levels,cont[j],rep->base_bits[j],newvalue%rep->base[j]);
					cont[j]+=rep->base_bits[j];
					contB[j]++;
				if(j<rep->nLevels-1){
					bitset(bits_BS,contB[j]-1);

				}
									
				break;
							}
			j--;
		}

		
	}


	bitset(bits_BS,bits_BS_len-1);

	rep->bS = createBitRankW32Int(bits_BS, bits_BS_len , 1, 20); 	




	
	rep->rankLevels = (uint *) malloc(sizeof(uint)*rep->nLevels);
	for(j=0;j<rep->nLevels;j++)
 			rep->rankLevels[j]= rank(rep->bS, rep->levelsIndex[j]-1);

		
	free(cont);
	free(contB);
	free(levelSizeAux);
	free(kvalues);
	return rep;
}


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

uint accessFT(FTRep * listRep,uint param){
 uint mult=0;
 register uint j;
 uint partialSum=0;
 uint ini = param-1;
 uint nLevels=listRep->nLevels;
 uint * level;
 uint readByte;
 uint cont,pos, rankini;



		partialSum=0;
		j=0;
		level=listRep->levels ;

		pos=listRep->levelsIndex[j]+ini;
			
			mult=0;

		cont = listRep->iniLevel[j]+ini*listRep->base_bits[j];


		readByte = bitread(level,cont,listRep->base_bits[j]);
		 if(nLevels == 1){
           return readByte;
       }
		while((!bitget(listRep->bS->data,pos))){

			rankini = rank(listRep->bS, listRep->levelsIndex[j]+ini-1) - listRep->rankLevels[j];
			ini = ini-rankini;


			partialSum = partialSum+ (readByte<<mult);
			
			mult+=listRep->base_bits[j];
			j++;

			cont = listRep->iniLevel[j]+ini*listRep->base_bits[j];
			pos=listRep->levelsIndex[j]+ini;
		

		readByte = bitread(level,cont,listRep->base_bits[j]);

			if(j==nLevels-1){
				break;
			}
	
					
		}

		partialSum = partialSum + (readByte<<mult) + listRep->tablebase[j];

	return partialSum;

}


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

uint * decompressFT(FTRep * listRep, uint n){
 uint mult=0;
 register uint i;
 register uint j;
 uint partialSum=0;
 uint nLevels=listRep->nLevels;
 uint * level=listRep->levels;
 uint readByte;
 uint * list = (uint *) malloc(sizeof(uint)*n);
 uint * cont = (uint *) malloc(sizeof(byte*)*listRep->nLevels);
 uint * pos = (uint *) malloc(sizeof(uint)*listRep->nLevels);

 	for(j=0;j<nLevels;j++){
		cont[j]=listRep->iniLevel[j];
		pos[j]=listRep->levelsIndex[j];

	}

	for(i=0;i<n;i++){
		partialSum=0;
		j=0;
			
			mult=0;
			readByte = bitread(level,cont[j],listRep->base_bits[j]);
			cont[j]+=listRep->base_bits[j];

		while((!bitget(listRep->bS->data,pos[j]))){

			pos[j]++;

			partialSum = partialSum+ (readByte<<mult);
			mult+=listRep->base_bits[j];
			j++;

					readByte = bitread(level,cont[j],listRep->base_bits[j]);
			cont[j]+=listRep->base_bits[j];

			if(j==nLevels-1){
				break;
			}
					
		}

		if(j<nLevels-1){
			pos[j]++;
		}
		
		
		partialSum = partialSum + (readByte<<mult) + listRep->tablebase[j];

		list[i]=partialSum;
	}
	free(cont);
	free(pos);
	return list;
}


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */
void destroyFT(FTRep * rep){
	free(rep->levelsIndex);
	free(rep->iniLevel);
	free(rep->rankLevels);
	free(rep->tablebase);
	free(rep->levels);
	destroyBitRankW32Int(rep->bS);
	free(rep->base);
	free(rep->base_bits);
	free(rep);
}

/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */
void saveFT(FTRep * rep, char * filename){
	FILE * flist;
	flist = fopen(filename,"w");
	fwrite(&(rep->listLength),sizeof(uint),1,flist);
	fwrite(&(rep->nLevels),sizeof(byte),1,flist);
	fwrite(&(rep->tamCode),sizeof(uint),1,flist);
	fwrite(&(rep->tamtablebase),sizeof(uint),1,flist);
	fwrite(rep->tablebase,sizeof(uint),rep->tamtablebase,flist);	
	fwrite(rep->base_bits,sizeof(ushort),rep->nLevels,flist);
	fwrite(rep->base,sizeof(uint),rep->nLevels,flist);
	fwrite(rep->levelsIndex,sizeof(uint),rep->nLevels+1,flist);
	fwrite(rep->iniLevel,sizeof(uint),rep->nLevels,flist);
	fwrite(rep->rankLevels,sizeof(uint),rep->nLevels,flist);

	fwrite(rep->levels,sizeof(uint),(rep->tamCode/W+1),flist);

	save(rep->bS,flist);
	
	fclose(flist);	
	
}


FTRep* loadFT(char * filename){
	FILE * flist;
	FTRep * rep = (FTRep *) malloc(sizeof(struct sFTRep));
	flist = fopen(filename,"r");
	fread(&(rep->listLength),sizeof(uint),1,flist);
	fread(&(rep->nLevels),sizeof(byte),1,flist);
	fread(&(rep->tamCode),sizeof(uint),1,flist);
	
	fread(&(rep->tamtablebase),sizeof(uint),1,flist);
	rep->tablebase = (uint *) malloc(sizeof(uint)*rep->tamtablebase);
	fread(rep->tablebase,sizeof(uint),rep->tamtablebase,flist);	
	
	rep->base_bits = (ushort *) malloc(sizeof(ushort)*rep->nLevels);
	fread(rep->base_bits,sizeof(ushort),rep->nLevels,flist);
	
	
	rep->base = (uint *) malloc(sizeof(uint)*rep->nLevels);
	fread(rep->base,sizeof(uint),rep->nLevels,flist);
	
	
	rep->levelsIndex = (uint *) malloc(sizeof(uint)*(rep->nLevels+1));
	fread(rep->levelsIndex,sizeof(uint),rep->nLevels+1,flist);
	
	rep->iniLevel = (uint *) malloc(sizeof(uint)*rep->nLevels);
	fread(rep->iniLevel,sizeof(uint),rep->nLevels,flist);

	rep->rankLevels = (uint *) malloc(sizeof(uint)*(rep->nLevels));
	fread(rep->rankLevels,sizeof(uint),rep->nLevels,flist);
	
	rep->levels = (uint *) malloc(sizeof(uint)*(rep->tamCode/W+1));	
	fread(rep->levels,sizeof(uint),(rep->tamCode/W+1),flist);
		
	
	rep->bS = (bitRankW32Int *) malloc(sizeof(struct sbitRankW32Int));
	load(rep->bS,flist);	
	
	fclose(flist);
	
	return rep;
}


uint memoryUsage(FTRep* rep) {
	return sizeof(uint)*rep->tamtablebase 
	+ sizeof(ushort)*rep->nLevels
	+ sizeof(ushort)*rep->nLevels
	+ sizeof(uint)*(rep->nLevels+1)
	+ sizeof(uint)*(rep->nLevels)
	+ sizeof(uint)*(rep->nLevels)
	+ sizeof(uint)*(rep->tamCode/W+1)
	+ spaceRequirementInBits(rep->bS)/8
	+ sizeof(struct sFTRep);
}
