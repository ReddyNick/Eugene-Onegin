#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

#define MAXTEXTSZ 1000	//! max number of strings in the text
#define MAXLINE 100 + 1	//! max lentgth of the string + '\0'									

#define alphabetsz 32+26	//! size of the russian alphabet

//! convert char to int
int uc(char c) {
	if ((unsigned char((c)) > 223))
		return int(unsigned char((c)-'à'));
		return	int(unsigned char((c)-'a')) + 32;
}

//! to initialize some massives
void init(int*a, int n,int k) {
	for (int i = 0; i < n; i++)
		a[i] = k;
}

//! this function allows to get chars 
//! from the string regardless of 
//! punctuation marks.
//! [in] n - the number of the symbol we 
//! want to get
//! [in] bool inv tell how to read: 
//! from begining to end(false) or vice versa(true)
char Getchar(char* str, int n, bool inv) {
	//! letter - the number of the current letter
	//! found  - the flag wich tells if the needed letter is found
	//! c - to read symbols (unsigned char!)
	int letter = -1;
	bool found = false;
	unsigned char c = ' ';

	if (!inv)
		for (int i = 0; i < strlen(str); i++){
			c = str[i];

			//! is this an alpha?
			if (isalpha(c)) letter++; //k <= 255 && k >= 192

			if (letter == n) { found = true;	break; }
		}
	else
		for (int i = strlen(str)-1; i >= 0; i--) {
			c = str[i];

			//! is this an alpha?
			if (isalpha(c)) letter++;

			if (letter == n) { found = true;	break; }
		}

	if (!found) return '#';
	//! if the symbol is uppercase it 
	//! gives lowercase analog
	if (int(c)>191)
		return (int(c) < 224) ? (char)(c + 32) : c;
	else
		return (int(c) < 97) ? (char)(c + 26) : c;
}

//! this function is called from EO_Sort() and
//! it does radix sorting
void radixSort(char** text, int *track_w, int index, int lvl, bool inv,char**&result) {
	//! exit from recursion 
	if (track_w[index] == -1) {
		*result = text[index];
		result++;
		return;
	}

	int* track_l = new int[alphabetsz];
	init(track_l, alphabetsz, -1);
	bool flag = false;
	//! fill track_l
	while (index != -1) {
	
		char c;
		if (strlen(text[index]) < lvl + 1) {
			*result = text[index];
			result++;
			index = track_w[index];
			continue;
		}
		else
			c = Getchar(text[index], lvl,inv);
		
		int tmp = track_w[index];		
		track_w[index] = track_l[uc(c)];
		track_l[uc(c)] = index;
		index = tmp;
		flag = true;
	}
	
	//! go deeper
	if(flag)
		for (int i = 0; i < alphabetsz; i++) {
				radixSort(text, track_w, track_l[i], lvl + 1,inv, result);
		}
	delete[] track_l;
}

//! this funcion (along with radixSort) is intended to 
//! sort the set of strings
//! the algorithm is msd radix sort
//! [in] bool backw tell how to sort: 
//! from begining to end(false) or vice versa(true)
char** Sort(char** text,int size, bool backw) {
	
	char** result = new char*[size+1];
	for (int i = 0; i < size + 1; i++)
		result[i] = NULL;
	//result[size] = NULL;
	char** begining = result; // to save the begining of the result
	
	//trakw("track word") is the massive to sort the text
	int* trackw = new int[size];
	init(trackw, size, -1);
	
	//! initialization of track_l
	int* track_l = new int[alphabetsz];
	init(track_l, alphabetsz, -1);
	
	//! to prepare  the track_w for sorting
	//! Getchar is to get the letter from the string
	for (int i = 0; i < size; i++) {
		for (int j = i - 1; j >= 0; j--)
			if (Getchar(text[j],0,backw) == Getchar(text[i],0,backw)) {
				trackw[i] = j;
				break;
			}
		track_l[uc(Getchar(text[i],0,backw))] = i;
	}
	
	for (int i = 0; i < alphabetsz; i++)
		if (track_l[i] != -1)
			radixSort(text, trackw, track_l[i], 1, backw, result);
	
	delete[] trackw;
	delete[] track_l;
	
	return begining;
}
char** Read_Text_new(FILE* in,int &nlines,int& size)
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

// to delete the dynamic memory
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
	int txtsize = 0, nlines = 0;
	char** text = Read_Text_new(in,nlines,txtsize);
	
	// alphabet sorted text
	char** version1 = NULL;
	// alphabet sorted text from the end of the line
	char** version2 = NULL;
	
	version1 = Sort(text, nlines, false);
	version2 = Sort(text, nlines, true); // sort backward
	
	
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