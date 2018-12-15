#include "StdAfx.h"
#include "fileLoader.h"



fileLoader::fileLoader(void) {
	words = NULL;
	lines = NULL;
	bufferWords = NULL;
	bufferLines = NULL;
	iC=0;
	iD=0;

}

void fileLoader::clearFileLoader(void){
	if (words!=NULL){
		cout << "\n words is not null, deleting words..";
		delete[] words;
		words = NULL;

		cout << "\n deleting others..";
		delete[] lines;
		delete[] lineWordCount;
		delete[] linesToCopy;
		delete[] sectionFirstLine;
		delete[] sectionLinesCount;
	
	}

	if (bufferWords!=NULL) {
		cout << "\n buffer-words is not null, deleting..";

		delete[] bufferWords;
		bufferWords = NULL;
	}
	if (bufferLines!=NULL) {
		cout << "\n buffer-lines is not null, deleting..";
		delete[] bufferLines;
		bufferLines = NULL;
	}

	words = NULL;
	lines = NULL;
	bufferWords = NULL;
	bufferLines = NULL;


}

void fileLoader::initFileLoader(void)
{
	cout << "\nInit file loader..";
	clearFileLoader();
	cout << "\nFile loader cleared..";

	fileLinesCount=0;
	//fileWordsCount=0;
	

	// FUUUCK try to catch alloc errors
	try {

	words = new char*[appSettings::MAX_LINES*appSettings::MAX_WORDS_PER_LINE];
	lines = new char*[appSettings::MAX_LINES];
	lineWordCount = new long[appSettings::MAX_LINES];
	//firstWordIndexForLine = new long[MAX_LINES+1];
	//lineWord = new long[MAX_LINES*MAX_WORDS_PER_LINE];

	linesToCopy = new char*[appSettings::MAX_LINES+1];
	sectionFirstLine = new long[appSettings::MAX_LINES+1];
	sectionLinesCount = new long[appSettings::MAX_LINES+1];
	sectionsCount = 0;


	//emptyString è ""
	emptyString[0]=0;

	long i;
	//for (i = 0; i < MAX_LINES; i++)
	//	firstWordIndexForLine[i] = -1;

	for (i = 0; i < appSettings::MAX_LINES; i++)
		lineWordCount[i] = 0;

	// si poteva fare NULL ma dava errore cout<<
	for (i = 0; i < appSettings::MAX_LINES*appSettings::MAX_WORDS_PER_LINE; i++)
		words[i] = emptyString;

	for (i = 0; i < appSettings::MAX_LINES+1; i++){
		linesToCopy[i] = NULL;
		sectionLinesCount[i] = 0;
	}

	}
	catch (const std::bad_alloc& e) {
		std::cout << "\n\n*ERROR* system ran out of memory: " << e.what() << '\n';
		cout << "\nDEBUG\n\n";
		cout << "\nappSettings::MAX_LINES : " << appSettings::MAX_LINES;
		cout << "\nappSettings::MAX_WORDS_PER_LINE : " << appSettings::MAX_WORDS_PER_LINE;
		cout << "\nExit now..\n";
		exit(0);
	}

	cout << "\ninitFileLoader finished";
	cout.flush();
}


fileLoader::~fileLoader(void)
{
}


void fileLoader::pushComment(){
	if (linesToCopy[currentLineIndex - 1] != NULL){
		sectionLinesCount[sectionsCount]++;
		linesToCopy[currentLineIndex] = lines[currentLineIndex];
	}
	else{
		sectionsCount++;
		sectionFirstLine[sectionsCount] = currentLineIndex;
		linesToCopy[currentLineIndex] = lines[currentLineIndex];
		sectionLinesCount[sectionsCount]++;
	}
}

//void fileLoader::printComments(){
//	cout << endl <<endl << "Commented + unknown Lines:" << endl ;
//
//	long i, j;
//
//	for (i=1; i<= sectionsCount; i++) {
//		cout << endl << "Section " << i << endl;
//		for (j=sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++)
//			printf("\n %s", linesToCopy[j]);
//	}
//}

void fileLoader::writeCommentsToFiles(string basePathOut){
	ofstream ofs;
	string baseFileName = basePathOut + "\\x_section";
//	string lineOut;
	char fileName[1000];

	
	long i, j;

	for (i=1; i<= sectionsCount; i++) {
		cout << "\n export section " << i <<" ";
		sprintf_s(fileName, "%s%d.txt", baseFileName.c_str(), i); 
		ofs.open(fileName, ofstream::out);


		for (j=sectionFirstLine[i]; j< sectionFirstLine[i] + sectionLinesCount[i]; j++){
			ofs << linesToCopy[j] << "\n";
			//printf("\n %s", linesToCopy[j]);
		}

		ofs.close();

	}	
}

void fileLoader::loadFile(void)
{
	ifstream file( fileName , ios::binary);
	file.seekg(0, ios::end);
	bufferSize = (long) file.tellg();
	file.seekg(0, std::ios::beg);

	// FUU* catch the out of memory errors
try {
	bufferWords = new char[bufferSize];

	if (file.read(bufferWords , bufferSize)) {
		cout << "\nOpening file:\n<" << fileName << ">\nFile size: " << bufferSize << "\n";
		bufferLines = new char[bufferSize];
		memcpy(bufferLines, bufferWords, bufferSize);
	}
	else {
		cout << " ERROR reading file ?! " << endl << fileName << endl << endl << "exit now..";
		system("PAUSE");
		exit(0);
	}

	}
	catch (const std::bad_alloc& e) {
		std::cout << "\n\n*ERROR* system ran out of memory: " << e.what() << '\n';
		cout << "\nDEBUG\n\n";
		cout << "\nappSettings::MAX_LINES : " << appSettings::MAX_LINES;
		cout << "\nappSettings::MAX_WORDS_PER_LINE : " << appSettings::MAX_WORDS_PER_LINE;
		cout << "\n file size: " << bufferSize;
		cout << "\nExit now..\n";
		exit(0);
	}

	iC=0;
}


//OLD COMMENT
// getLine legge una line (la legge in-place) dal buffer
// a fine buffer, restisuisce 0
// out: s: la linea letta
int fileLoader::getLine(char** s){
	currentLineIndex++ ;
	currentWordIndex = 0;

	*s = lines[currentLineIndex];

	if (currentLineIndex <= fileLinesCount)
		return 1;
	else
		return 0;
}

// getLine legge una line (la legge in-place) dal buffer
// a fine buffer, restituisce 0
// out: s: la linea letta
int fileLoader::getLine_Init(char** s){
	if(iC>=bufferSize)
		return 0;

	iniLine = iC;

	long i2 = iC;
	while ( ( bufferWords[i2]!=10 ) && ( bufferWords[i2]!=13 ) && ( i2 < bufferSize ) )
		i2++;

	endLine = i2;

	bufferWords[i2]=0;
	// termina la line anche nel buffer copia!
	bufferLines[i2]=0;


	// finalmente, *s è la line
	*s = &(bufferLines[iC]) ;

	fileLinesCount++;

	// TOO MANY DATA; PROGRAM WOULD CRASH
	if (fileLinesCount>=appSettings::MAX_LINES){
		cout << "\n\n ERROR the file contains more than MAX_LINES=" << appSettings::MAX_LINES << " lines. Check the MAX_LINES parameter! Exit now.\n\n";
		system("PAUSE"); exit(1);
	}

	lines[fileLinesCount] = &(bufferLines[iC])  ;

	//inizializza a 0 il wordCount per questa linea
	lineWordCount[fileLinesCount] = 0;
	// non sappiamo ancora l'indice della prima WORD
	//firstWordIndexForLine[fileLinesCount] = -1;

	// NEXT indexes
	iC = i2 + 2;
	iD = iniLine;

	return 1;
}

// getWord legge una parola (in-place) dal buffer
// nota: getLine legge una linea intera (fino a ritorno a capo), getWord "consuma" parole da quella linea.
// quando ha esaurito la linea, getWord restituisce 0.
// out: s: la parola letta
//TODO: usare nomi di variabili piu significative..
int fileLoader::getWord(char** s){
	
	currentWordIndex++ ;

	if (currentWordIndex > lineWordCount[currentLineIndex]){
		*s = emptyString;
		return 0;
	}

	if (currentWordIndex > lineWordCount[currentLineIndex]){
		*s = emptyString;
		return 0;
	}

	// getWordMatrix : input parameters are: row, column
	*s = getWordMatrix(currentLineIndex, currentWordIndex);
	return 1;

}



// getWord legge una parola (in-place) dal buffer
// nota: getLine legge una linea intera (fino a ritorno a capo), getWord "consuma" parole da quella linea.
// quando ha esaurito la linea, getWord restituisce 0.
// out: s: la parola letta
//TODO: usare nomi di variabili piu significative..
int fileLoader::getWord_Init(char** s){
	bool delimitedWordFound;

	while ( ( bufferWords[iD]==' ' ) && ( bufferWords[iD]!=10 ) && ( bufferWords[iD]!=13 ) && ( iD < endLine ) )
		iD++;

	delimitedWordFound = (bufferWords[iD]=='"');

//	if (delimitedWordFound)
//		iD++;
//	delimited words keep the delimiter "

	iniWord=iD;
	long i3 = iD;

	if (i3>=endLine) {
		// returns an empty word.. don't push into iniWords
		iniWord = endLine;
		*s = &(bufferWords[endLine]) ;
		return 0;
	}

	// if word is delimited	
	if (delimitedWordFound){
		i3++;
		while ( ( bufferWords[i3]!='"' ) && ( i3 < endLine ) )
			i3++;
		if (bufferWords[i3]!='"'){
			cout <<" error, delimited word not terminated! ";
			bufferWords[i3]=0;
			return 0;
		}
		
		// keep the ending <">
		i3++;

		endWord = i3;
		bufferWords[i3] = 0;
	}
	// if word is NOT delimited	
	else {
		while ( ( bufferWords[i3]!=' ' ) && ( bufferWords[i3]!=10 ) && ( bufferWords[i3]!=13 ) && ( i3 < endLine ) )
			i3++;
		endWord = i3;
		bufferWords[i3] = 0;
	}

	// NEXT indexes
	iD= i3 + 1;

	// *s è la WORD
	*s = &(bufferWords[iniWord]) ;
	
	//fileWordsCount++;
	
	// push to words array
	//words[fileWordsCount] = *s ;

	// incrementa il wordCount per la LINE corrente
	lineWordCount[fileLinesCount] ++;

	//
	long numWord = lineWordCount[fileLinesCount] ;
	if (numWord>=appSettings::MAX_WORDS_PER_LINE){
		cout << "\n\n ERROR, line " << fileLinesCount << " contains more than MAX_WORDS_PER_LINE=" <<
			appSettings::MAX_WORDS_PER_LINE<< " words. Check the MAX_WORDS_PER_LINE parameter! Exit now.\n\n";
		system("PAUSE"); exit(1);
	}
	
	
	//wordsOfLine[fileLinesCount][numWord] = & (words[fileWordsCount]) ;
	words[(numWord-1) + (fileLinesCount-1) * appSettings::MAX_WORDS_PER_LINE ] = *s ;


	// è la prima word della LINE?
	//if (firstWordIndexForLine[fileLinesCount] == -1)
	//	firstWordIndexForLine[fileLinesCount] = fileWordsCount;

	

	// non-empty word.. push back to iniWords
	//lineWord[fileWordsCount] = fileLinesCount;

	return 1;
}

char* fileLoader::getWordMatrix(long i, long j){
	
	if (i > this->fileLinesCount)
		//return NULL;
		return emptyString;
	if (j > this->lineWordCount[i])
		//return NULL;
		return emptyString;


	return words[(j-1) + (i-1) * appSettings::MAX_WORDS_PER_LINE ] ;

}

/* long fileLoader::lineLength(){
	return this->endLine - this->iniLine;

} */

bool fileLoader::emptyLine(){
	return (lineWordCount[currentLineIndex]<=0);
}

bool fileLoader::nextLineIsEmpty(){
	return (lineWordCount[currentLineIndex + 1]<=0);
}


/* long fileLoader::wordLength(){
	return this->endWord - this->iniWord;
} */

void fileLoader::splitAllFile(){
	char *line1;
	char *word;
	
	// LOOP su tutte le righe del file
	while (getLine_Init(&line1)) {
		
		// LOOP su tutte le words della linea corrente
		while( getWord_Init(&word) ) {
			// debug: print word
//			cout <<" word: [" << word << "]" << endl ;
		}
	}

	currentLineIndex = 0;
	currentWordIndex = 0;
}



char* fileLoader::nextWord(){

	// getWordMatrix: row, column
//	string s;
	if (currentWordIndex >= appSettings::MAX_WORDS_PER_LINE)
		//s = "";
		return emptyString;
	else
		//s = getWordMatrix(currentLineIndex, currentWordIndex + 1);
		return getWordMatrix(currentLineIndex, currentWordIndex + 1);
//	return s;
}


bool fileLoader::nextWordIsEmpty(){
	return (*(nextWord())==0 ) ;
}


string fileLoader::firstWordOfNextLine(){

	// getWordMatrix: row, column
	string s;
	if (currentLineIndex >= fileLinesCount)
		s = "";
	else
		s = getWordMatrix(currentLineIndex + 1, 1);
	return s;
}


