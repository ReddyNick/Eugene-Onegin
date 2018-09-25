#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define MAXTEXTSZ 1000	//! max number of strings in the text
#define MAXLINE 100 + 1	//! max lentgth of the string + '\0'									

#define alphabetsz 32	//! size of the russian alphabet

//! convert char to int
#define uc(c) int(unsigned char((c)-'à'))

//! to delete dynamic memory
int Delete_text(char** text,int textsz) {
	for (int i = 0; i < textsz; i++)
		free(text[i]);
	delete[] text;
	return 0;
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
	int ch = -1;
	int k = 0;
	bool found = false;
	if (!inv)
		for (int i = 0; i < strlen(str); i++) {
			k = int(unsigned char(str[i]));

			//! is this an alpha?
			if (k <= 255 && k >= 192) ch++;

			if (ch == n) { found = true;	break; }
		}
	else
		for (int i = strlen(str)-1; i >= 0; i--) {
			k = int(unsigned char(str[i]));

			//! is this an alpha?
			if (k <= 255 && k >= 192) ch++;

			if (ch == n) { found = true;	break; }
		}

	
	if (!found) return '#';
	//! if the symbol is uppercase it 
	//! gives lowercase analog
	return (k < 224) ? (k + 32) : k;
}
//! this function is called from EO_Sort() and
//! it does radix sorting

void radixSort(char** text, int *track_w, int index, int lvl, bool inv,char** &result) {
	//! exit from recursion 
	if (track_w[index] == -1) {
		*result = text[index];
		result++;
		return;
	}

	int* track_l = new int[alphabetsz];
	init(track_l, alphabetsz, -1);
	
	//! fill track_l
	while (index != -1) {
		char c = Getchar(text[index], lvl,inv);
		
		//! if we can't get the char then
		//! the char will be just russian 'a'
		if (c=='#') c = 'à'; 

		//! swop 3 variables in circle
		int tmp = track_w[index];		
		track_w[index] = track_l[uc(c)];
		track_l[uc(c)] = index;
		index = tmp;
	}
	//! go deeper
	for (int i = 0; i < alphabetsz; i++)
		if (track_l[i] != -1)
			radixSort(text, track_w, track_l[i], lvl + 1,inv, result);

	delete[] track_l;
}

//! this funcion (along with radixSort) is intended to 
//! sort the set of strings
//! the algorithm is msd radix sort
//! [in] bool inv tell how to sort: 
//! from begining to end(false) or vice versa(true)
void EO_Sort(char** text, int * track_w, int txtsz,bool inv, char** result) {
	
	//! initialization of track_l
	int* track_l = new int[alphabetsz];
	init(track_l, alphabetsz, -1);
	
	//! initialization of track_w
	for (int i = 0; i < txtsz; i++) {
		for (int j = i - 1; j >= 0; j--)
			if (Getchar(text[j],0,inv) == Getchar(text[i],0,inv)) {
				track_w[i] = j;
				break;
			}
		track_l[uc(Getchar(text[i],0,inv))] = i;
	}
	
	for (int i = 0; i < alphabetsz; i++)
		if (track_l[i] != -1)
			radixSort(text, track_w, track_l[i], 1, inv, result);
	
	delete[] track_l;
}

int main() {
	
	FILE *input  = fopen("input.txt", "r");
	FILE *output1 = fopen("output.txt", "w");
	FILE *output2 = fopen("output2.txt", "w");
	setlocale(LC_ALL, "Rus");
	
	//! buffer for the input
	char buff[MAXLINE];									   
	
	//! the text is here
	char **text = new char*[MAXTEXTSZ]; 
	//! initialize
	for (int i = 0; i < MAXTEXTSZ; i++) text[i] = NULL;
	int textsize = 0;

	//! reading the file and making the text
	for (int i = 0; fgets(buff, MAXLINE, input) != NULL; i++) { 
		//! to delete '\n' in the end
		char *ptr = strchr(buff, '\n');
		if (ptr == buff) { i--; continue; }
		if (ptr != NULL) *ptr = '\0';

		//! write in the text[][]
		text[i] = _strdup(buff);
		textsize++ ;
	}
	//! trakw("track word") is the massive to sort the text
	int* trackw = new int[textsize];
	init(trackw, textsize, -1);
	
	//! result contains sorted text
	char**result = new char*[textsize]; 
	for (int i = 0; i < textsize; i++) result[i] = NULL;
	
	
	EO_Sort(text, trackw, textsize, false, result);

	//! to sort strings from the end
	char**invresult = new char*[textsize];
	for (int i = 0; i < textsize; i++) invresult[i] = NULL;

	init(trackw, textsize, -1);

	EO_Sort(text, trackw, textsize, true, invresult);
	
	
	for (int i = 0; i < textsize; i++) {
		fprintf(output1, "%s\n", result[i]);
		fprintf(output2, "%s\n", invresult[i]);
	}
	
	delete[]trackw;
	delete[]result;
	delete[]invresult;
	Delete_text(text,textsize);
	
	fclose(input);
	fclose(output1);
	fclose(output2);
	return 0;
}