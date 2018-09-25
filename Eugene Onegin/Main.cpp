#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
using namespace std;

//КАК сортировать пропуская символы!!!!!






// max number of strings in the text
#define MAXTEXTSZ 1000
// max lentgth of the string + '\0'
#define MAXLINE 100 + 1										

// to delete dynamic memory
int Delete_text(char** text,int textsz) {
	for (int i = 0; i < textsz; i++)
		free(text[i]);
	delete[] text;
	return 0;
}
void Sort_oneletter(char** lhs, char** rhs, int letter){
// conditions for sorting algorithm
#define bool_1 ptr2[0][letter]==ptr2[1][letter] && strlen(ptr2[0]) > strlen(ptr2[1])
#define bool_2 ptr2[0][letter] > ptr2[1][letter]
	
	for (char** ptr1 = lhs + 1; ptr1 <= rhs; ptr1++) {
		 char** ptr2 = ptr1 - 1;
		
		while (ptr2 >= lhs && (bool_1 || bool_2 )){
				//swop(ptr2[0],ptr2[1])
				char* tmp = ptr2[1];
				ptr2[1] = ptr2[0];
				ptr2[0] = tmp;
				ptr2--;
				//update bools
				if (ptr2 >= lhs) {bool_1; bool_2;}
		}	
	}
}
void AlphabetSort(char** lhs, char** rhs, int letter) {
	
	if (lhs >= rhs) return;
	
	if (strlen(*lhs) < letter+1) { 
		AlphabetSort(lhs + 1, rhs, letter);
		return;
	}
	
	Sort_oneletter(lhs, rhs, letter);
	
	char **lhs2 = lhs, **rhs2 = lhs;
	char cur = (*lhs)[letter];
	
	for (; rhs2 < rhs; rhs2++) {
		if ((*rhs2)[letter] == cur) continue;
		
		AlphabetSort(lhs2, rhs2 - 1, letter + 1);
		
		cur = (*rhs2)[letter];
		lhs2 = rhs2;

	}
	if ((*rhs2)[letter] == cur)
		 AlphabetSort(lhs2, rhs2, letter + 1);
	else AlphabetSort(lhs2, rhs2-1, letter + 1);
}


int main() {
	
	FILE *input  = fopen("input.txt", "r");
	FILE *output = fopen("output.txt", "w");
	setlocale(LC_ALL, "Rus");
	
	// buffer for the input
	char buff[MAXLINE];									   
	
	// the text is here
	char **text = new char*[MAXTEXTSZ] {NULL};				
	int textsize = 0;

	//reading the file and making the text
	for (int i = 0; fgets(buff, MAXLINE, input) != NULL; i++) { 
		
		//to delete '\n' in the end
		char *ptr = strchr(buff, '\n');
		if (ptr == buff) { i--; continue; }
		if (ptr != NULL) *ptr = '\0';

		text[i] = _strdup(buff);
		textsize++ ;
	}

	AlphabetSort(text, text+textsize - 1, 0);
	
	
	for (int i = 0; i < textsize; i++)
		fprintf(output, "%s\n", text[i]);
	
	Delete_text(text,textsize);
	fclose(input);
	fclose(output);
	
	system("pause");
}