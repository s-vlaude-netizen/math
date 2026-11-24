/* Direct definition, no covering model: gaps between consecutive integers coprime to prime(n)# (one full period),
   smallest even number that is not a gap.  usage: brute n   (n <= 10) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int c,char**v){ int n=atoi(v[1]); int pr[16]={2,3,5,7,11,13,17,19,23,29,31,37}; unsigned long long P=1; for(int i=0;i<n;i++)P*=pr[i];
  unsigned char *cop=malloc(P+2); memset(cop,1,P+2); for(int i=0;i<n;i++) for(unsigned long long m=0;m<=P+1;m+=pr[i]) cop[m]=0;
  static char seen[4096]; unsigned long long last=1; /* 1 is coprime */
  for(unsigned long long x=2;x<=P+1;x++) if(cop[x]){ seen[x-last]=1; last=x; }
  int g=2; while(seen[g]) g+=2; printf("n=%d P=%llu smallest missing even gap = %d\n",n,P,g); return 0; }
