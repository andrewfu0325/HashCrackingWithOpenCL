#pragma once
#ifndef GENKEY_H
#define GENKEY_H
#include <cstring>
//#define LENGTH 8//the bytes length
#define MAXLENGTH 16//the bytes length
#define HASHLENGTH 16
#define K 512*1024 //how many key gen in one time
#define GPU 2//how many bytes the GPU gen

#define SETLENGTH 26//the set length
static char set[52] = { 
	'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u',
	'v', 'w', 'x', 'y', 'z' ,
	'A', 'B', 'C', 'D', 'E', 'F', 'G',
'H', 'I', 'J', 'K', 'L', 'M', 'N',
'O', 'P', 'Q', 'R', 'S', 'T', 'U',
'V', 'W', 'X', 'Y', 'Z'
};


int genKey();
extern bool finish;
extern char (*key)[MAXLENGTH];
static int Index[MAXLENGTH] = {0};
extern int len;

#endif
