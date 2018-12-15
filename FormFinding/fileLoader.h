#pragma once

#include <string>
#include <iostream>

using namespace std;


class fileLoader
{

public:
	// puntatori, sono usati per splittare i buffer in words/lines.	
	long iC, iD;

	// nome del file StrausTXT in input
	string fileName;
	// in bufferWords c'è l'intero file, viene spezzato in words
	// in bufferLines viene copiato l'intero file, viene spezzato in lines
	char *bufferWords, *bufferLines;
	

	// serve per restituire ""
	char emptyString[1];
	
	// dimensione dei buffer, puntatori a inizio e fine word e line
	long bufferSize, iniLine, endLine, iniWord, endWord;
	
	// conta il numero di lines del file
	long fileLinesCount;
	
	// puntatori, posizione CORRENTE (le word s sono storate in una matrice)
	long currentLineIndex, currentWordIndex;
	
	// linewordCount[i] dice quante WORDs contiene la line i-esima
	long *lineWordCount;


	// estrae la parola j-esima dalla riga i-esima, o restituisce emptyString (che contiene "" )
	char* getWordMatrix(long i, long j);

	// mettiamo qui tutte le linee del file
	// array di pointers
	char **lines;

	// mettiamo qui tutte le WORDs del file
	char **words;


	// mettiamo qui tutte le LINES da copiare "pari-pari"
	char **linesToCopy;
	// sectionFirstLine[i] è l'indice in linesToCopy delle prima linea da copiare uguale
	long *sectionFirstLine;
	// sectionLinesCount[i] è il numero di linee che compongono la sezione i-esima
	long *sectionLinesCount;
	long sectionsCount;

	void pushComment();
	//void printComments();
	void writeCommentsToFiles(string basePathOut);


	fileLoader(void);
	~fileLoader(void);
	void initFileLoader();
	void clearFileLoader();


	void loadFile();
	void splitAllFile();


	int getLine_Init(char** s);
	int getWord_Init(char** s);

	int getLine(char** s);
	int getWord(char** s);

	// dà la prossima word nella matrice words[,] , ma senza fare lo step avanti
	// se siamo a fine riga, dà ""
	char* nextWord();
	
	// dice se la prossima word è vuota, se siamo a fine riga dice true
	bool nextWordIsEmpty();

	// dà la prima word nella prossima LINE , ma senza fare lo step avanti
	string firstWordOfNextLine();

	// emptyLine dice se la linea corrente è vuota (non contiene words)
	bool emptyLine();
	
	bool nextLineIsEmpty();


	/// note:
	// questa classe è sia un container, legge il file, lo splitta, e dà una serie di iterators
	// sarebbe meglio estrarre gli iterators, fare refactoring etc.
};

