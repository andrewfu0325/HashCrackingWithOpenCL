#include "genKey.h"
#include "stdio.h"

char (*key)[MAXLENGTH];
bool finish = false;

int genKey(){
	int i;
	finish = false;
	for (i = 0; i < K && !finish; i++){
//		printf("%d %d %d %d\n", Index[0], Index[1], Index[2], Index[3]);
		for (int j = 0; j < len - GPU; j++){
			key[i][j] = set[Index[j]];
		}
//		printf("%c %c %c %c\n", key[i][0], key[i][1], key[i][2], key[i][3]);

		for (int j = len - GPU - 1; j >= 0; j--){
			Index[j]++;
			if (Index[j] == SETLENGTH){
				if (j == 0){
					finish = true;
					memset(Index, 0, sizeof(int)*MAXLENGTH);
					break;
				}
				Index[j] = 0;
			}
			else{
				break;
			}
		}
	}
	return  i;
}
