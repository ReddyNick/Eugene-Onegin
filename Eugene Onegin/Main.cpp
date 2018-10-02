#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <utility>

							
#define ALPHABETSZ 32+26	//! size of the russian + english alphabet
#define MAXSTRSZ 200		//! maximum size of a string
typedef std::pair<char*, char*> PointerC;

//! arguments for sorting function
struct Arguments {
	char** text = NULL; 
	int *trackw = NULL;
	int**trackl = NULL;
	int index = 0, lvl = 0;
	bool inv = false;
	PointerC *pointc = NULL;
};

/*
convert char to int
à - ÿ(russian letters) --> 0 - 31
a - z(english letters) --> 32 - 57
*/
int uc(char letter) {
	//! 224 - ASCII for the russian 'a' 
	//! 32 - the size of the russian alphabet
	unsigned char c = letter;
	
	// if russian letter
	if (int(c) > 191) {

		//! if the symbol is uppercase it 
		//! gives lowercase analog
		c = (int(c) < 224) ? unsigned char(c + 32) : c;
		return int(unsigned char(c - 'à'));
	}
	// if english
		c =  (int(c) < 97) ? unsigned char (c + 32) : c;
		return	int(unsigned char(c - 'a')) + 32;
}

//! to initialize some massives
void init(int *a, int n,int k) {
	for (int i = 0; i < n; i++)
		a[i] = k;
}

//! this function allows to get chars 
//! from the string regardless of 
//! punctuation marks.
//! [in] bool inv tell how to read: 
//! from begining to end(false) or vice versa(true)
int Getchar(int str, bool inv, PointerC *&pointc)
{
	// from begining to end
	if (!inv) {
		// if we have read all the letters
		if (pointc[str].first == NULL)
			return -1;

		char* tmp = (pointc[str].first);
		// find next letter passing by punctuation marks
		// until the end of the string
		
		int k = int(unsigned char(*tmp));
		while (tmp != pointc[str].second && !isalpha(k)) {
			tmp++;
			k = int(unsigned char(*tmp));
		}
		if (tmp != pointc[str].second) {
			// uc() translates char to int
			pointc[str].first = tmp + 1;
			return uc(*tmp);
		}
		//if read all letters
		pointc[str].first = NULL;
		if (isalpha(k)) {
			return uc(*tmp);
		}
		return -1;
	}
	// from end to begining
	else {
		if (pointc[str].second == NULL)
			return -1;

		char* tmp = (pointc[str].second);
		int k = int(unsigned char(*tmp));
		while (tmp != pointc[str].first && !isalpha(k)) {
			tmp--;
			k = int(unsigned char(*tmp));
		}
		if (tmp != pointc[str].first) {
			// uc() translates char to int
			pointc[str].second = tmp - 1;
			return uc(*tmp);
		}
		pointc[str].second = NULL;
		if (isalpha(k)) {
			return uc(*tmp);
		}
		return -1;
	}
}

//! intialization of pointc
int Init_pointc(std::pair<char*, char*> *pointc, char**text)
{
	char** tmp = text;
	int i = 0;
	for (; tmp[i + 1] != NULL; i++) {
		pointc[i].first = tmp[i];
		char* end = tmp[i + 1] - 1;
		while (!isalpha(int(unsigned char(*end)))) end--;
		pointc[i].second = end;

	}
	pointc[i].first = tmp[i];
	char*end = tmp[i];
	while (*(end + 1) != '\0') end++;
	pointc[i].second = end;
	return 0;

}

//! this function is called from Sort() and
//! it does radix sorting
void radixSort(Arguments &arg, char**&result) {
	int index = arg.index;
	int lvl = arg.lvl;

	//! exit from recursion 
	if (arg.trackw[index] == -1) {
		*result = arg.text[index];
		result++;
		return;
	}
	// flag = true if trackl[lvl] is not changed 
	bool flag = false;
	
	//! fill trackl[lvl]
	while (index != -1) {
	
		int c = Getchar(index, arg.inv,arg.pointc);
		if (c == -1) {
			*result = arg.text[index];
			result++;
			index = arg.trackw[index];
			continue;
		}
		
		// swap 3 variables 
		int tmp = arg.trackw[index];	
		arg.trackw[index] = arg.trackl[lvl][c];
		arg.trackl[lvl][c] = index;
		index = tmp;
		flag = true;
	}
	
	//! go deeper
	arg.lvl++;
	if(flag)
		for (int i = 0; i < ALPHABETSZ; i++) 
			if (arg.trackl[lvl][i] != -1) {
				arg.index = arg.trackl[lvl][i];				
				radixSort(arg, result);
				arg.trackl[lvl][i] = -1;
			}
	arg.lvl--;
}

//! this funcion (along with radixSort) is intended to 
//! sort the set of strings
//! the algorithm is msd radix sort
//! [in] bool backw tell how to sort: 
//! from begining to end(false) or vice versa(true)
char** Sort_new(char** text,int size, int max_strsize, bool backw) 
{
	char** result = new char*[size + 1];
	for (int i = 0; i < size + 1; i++)
		result[i] = NULL;
	char** resultbeg = result; 

	//trakw("track word") is the array needed to sort the text
	int* trackw = new int[size];
	init(trackw, size, -1);

	//trackl("track letter") is the array needed to sort the text
	int** trackl = new int*[max_strsize];
	for (int i = 0; i < max_strsize; i++) {
		trackl[i] = new int[ALPHABETSZ];
		init(trackl[i], ALPHABETSZ, -1);
	}
	
	// pointc[i] contains the last letter of the
	// string we haven't read yet.
	// .first from begining to end
	// .second from end to begining
	typedef std::pair<char*, char*> PointerC;
	PointerC* pointc = new PointerC[size];
	Init_pointc(pointc, text);
	
	
	//! to prepare  the trackw for sorting
	//! Getchar is to get the letter from the string
	for (int i = 0; i < size; i++) {
		int c = Getchar(i, backw, pointc);
		if (c == -1) continue;
		trackw[i] = trackl[0][c];
		trackl[0][c] = i;
	}
	
	Arguments arg;
	arg.text = text;
	arg.trackw = trackw;
	arg.lvl = 1;
	arg.inv = backw;
	arg.pointc = pointc;
	arg.trackl = trackl;
	
	for (int i = 0; i < ALPHABETSZ; i++)
		if (trackl[0][i] != -1) {
			arg.index = trackl[0][i];
			radixSort(arg, resultbeg);
		}
	
	delete[] trackw;
	for (int i = 0; i < 10; i++)
		delete[]trackl[i];
	delete[]trackl;
	delete[] pointc;
	
	return result;
}
char** Read_Text_new(FILE* in,long int &nlines,long int& size)
{
	char* buf; //buffer for an input

	//to get the size of  the text
	fseek(in, 0, SEEK_END);
	size = ftell(in);
	rewind(in);
	assert(size != 0);
	buf = new char[size+1]; // +1 for '\0'
	
	// nessesary! initialization
	memset(buf, '\0', size+1);

	fread(buf, sizeof(buf[0]), size, in);
	
	//to get the amount of new lines
	nlines = 1;
	for (char* s = strchr(buf, '\n'); 
		 s != NULL;
		 s = strchr(s, '\n')) 
	{
		if (((s - 1) != NULL) && *(s - 1) == '\r') 
			*(s - 1) = ' ';// std::cout << "rrer";// NULL && *(s - 1) == '\r'); *(s - 1) = '\0';
		while (*(++s) == '\n'); // to pass '\n's
		if (*s != '\0') nlines++;
	} 
	
	//to create massive of pointers
	char**text = new char*[nlines+1];
	text[nlines] = NULL;
	
	int position = 0;
	text[position] = buf;
	position = 1;
	
	for (char* s=buf; (s=strchr(s,'\n'))!= NULL; ++position) {
		*s = '\0';
		while (*(++s) == '\n'); // to pass '\n's
		if(*s!='\0')
			text[position] = s;
	}
	
	return text;
}

//! to delete the dynamic memory
int DeleteText(char** text,int txtsize)
{
	memset(text[0], '\0', txtsize);
	delete[] text[0]; // to delete the massive of chars 
	delete[] text;  // to delete the massive of pointers

	return 0;
}
int Write(FILE* f, char** t)
{
	for (; (*t) != NULL; t++)
		fprintf(f, "%s\n", *t);
	
	return 0;
}

int main() 
{
	FILE* in = fopen("input.txt","r");
	FILE *output1 = fopen("output.txt", "w");
	FILE *output2 = fopen("output2.txt", "w");
	setlocale(LC_ALL, "Rus");

	// txtsize - the size of the text
	// nlines - the amount of new lines
	long int txtsize = 0, nlines = 0;
	char** text = Read_Text_new(in,nlines,txtsize);

	// alphabet sorted text
	char** version1 = NULL;
	// alphabet sorted text from the end of the line
	char** version2 = NULL;
	
	version1 = Sort_new(text, nlines, MAXSTRSZ, false);
	version2 = Sort_new(text, nlines, MAXSTRSZ, true); // sort backward
	
	Write(output1, version1);
	Write(output2, version2);

	fclose(in);
	fclose(output1);
	fclose(output2);
	
	DeleteText(text,txtsize);
	delete[]version1;
	delete[]version2;

	system("pause");
	
	return 0;
}