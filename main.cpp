/*
* PTHaptics
* November 16, 2014
* Programmer: Bret Oplinger
*/

//Include all necessary headers
#include <QHHeadersGLUT.h>
#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <ctime>
#include <string>

using namespace std;

const unsigned int width = 15;
const unsigned int height = 7;


const int shapeSize = 105;

//Success checking
int successCount = 47;

string level = "3";

int selectedCubes = 0;

boolean hasWon = false;

//Timer ( in milliseconds)
long long currentTime = 0;

//Global state for directing callback function behavior
QHGLUT* displayObject;


DeviceSpace* omniSpace;

Cursor* cursor;

TriMesh* engravedBlock;

Box* grid[height*width];
bool gridCheck[height*width];

Text* success;
Text* score;
Text* elapsedTimer;

int scoreValue = 0;
int timerValue = 0;
bool hasNotStarted = true;

//Record keeping
string userNumber = "";
string blockText = "";
string fontType = "";
regex atMostFiveCharacters("^[a-z]{1,3}$");
regex print("^[Pp]rint$");
regex cursive("^[Cc]ursive$");

long long totalTime = 0;
bool hasRecorded = false;

void touchCallback(unsigned int ShapeID);
void setUpBlocksAndGrid();
void setUpBlockObject();
void setSuccessCount();
void showGrid();
void hideGrid();
void resetGrid();
void checkIfWon();
int isBoxInGrid(Box* box);
void updateScore(int addValue);
void updateElapsedTimer(int addValue);
void resetScore();
void resetElapsedTimer();
void graphicsCallback();
void engravedBlockTouched();
void promptForUserNumber();
void promptForBlockText();
void saveSessionDataToFile();
void userWon();
long long milliseconds_now();

int main(int argc, char *argv[])
{
	try{
		if (argc == 2){
			level = argv[1];
		}
	}
	catch (const exception& e){
		throw e;
	}

	promptForUserNumber();

	promptForBlockText();

	setSuccessCount();

	//Creates a display window
	displayObject = new QHGLUT(argc, argv);
	displayObject->setWindowTitle("PTHaptics");
	
	//Find Phantom device is called Omni
	omniSpace = new DeviceSpace();

	//Tells QuickHaptics that Omni exists
	displayObject->tell(omniSpace);

	//Creates a cursor object
	cursor = new Cursor("Models/pencil.3DS");
	cursor->scaleCursor(0.0009);
	TriMesh* ModelTriMeshPointer = cursor->getTriMeshPointer();
	ModelTriMeshPointer->setTexture("Models/pencil.jpg");
	cursor->setRelativeShapeOrientation(1.0, 0.0, 0.0, -30.0);
	displayObject->tell(cursor);

	setUpBlocksAndGrid();

	displayObject->preDrawCallback(graphicsCallback);

	// workspace looking from right
	HLdouble handx = 1, handy = 0, handz = 0;
	// at scene origin
	HLdouble centerx = 0, centery = 1, centerz = 0;
	// up vector
	HLdouble upx = 0, upy = 0, upz = 1;
	hluFeelFrom(handx, handy, handz, centerx, centery,
		centerz, upx, upy, upz);

	//Set everything in motion
	qhStart();
	
	return 0;
}

void touchCallback(unsigned int ShapeID){
	//
	//Find a pointer to touched Box
	Box* touchedBox = Box::searchBox(ShapeID);
	if (touchedBox == NULL){
		TriMesh* touchedBlock = TriMesh::searchTriMesh(ShapeID);
		if (touchedBlock == engravedBlock){
			engravedBlockTouched();
		}

		if (hasNotStarted){
			hasNotStarted = false;
		}
	} else{
		int index = isBoxInGrid(touchedBox);
		if (gridCheck[index]){
			gridCheck[index] = false;
			touchedBox->setShapeColor(0.0, 1.0, 0.0);
			selectedCubes++;
			checkIfWon();
		}

		if (hasNotStarted){
			hasNotStarted = false;
		}
	}
}

void graphicsCallback(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-850.0, 400.0, -400.0, 300.0, -1000.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	
	HLdouble minpoint[3] = { 500.0, -500.0, 700.0 };
	HLdouble maxpoint[3] = { 1000.0, 1000.0, 800.0 };
	displayObject->setHapticWorkspace(minpoint, maxpoint);
	
	if (!hasWon && !hasNotStarted){
		long long tempTime = milliseconds_now();
		long long elapsedTime = tempTime - currentTime;

		if (elapsedTime >= 1000 && hasRecorded != true){
			totalTime += elapsedTime;
			currentTime = tempTime;
			updateElapsedTimer(1);
		}
	}
}

void checkIfWon(){

		if (selectedCubes >= (successCount)){
			userWon();
		}
}

//checks if box is in the grid
int isBoxInGrid(Box* box){
	for (unsigned int i = 0; i < width; ++i){
		for (unsigned int j = 0; j < height; ++j){
			if (grid[i * height + j] == box){
				return i * height + j;
			}
		}
	}
	return -1;
}

//Sets up and then hides engraved blocks and grid of boxes
void setUpBlocksAndGrid(){
	string scoreString = "Score: " + to_string(scoreValue);
	score = new Text(50, &scoreString[0], 0.08, 0.925);
	score->setShapeColor(1, 1, 0);
	displayObject->tell(score);

	string timerString = "Time: " + to_string(timerValue);
	elapsedTimer = new Text(50, &timerString[0], 0.7, 0.925);
	elapsedTimer->setShapeColor(1, 1, 0);
	displayObject->tell(elapsedTimer);

	success = new Text(150, "You did it!", 0.16, 0.5);
	success->setShapeColor(1, 1, 0);
	displayObject->tell(success);

	//Creates the block with writing engraved through it
	engravedBlock = new TriMesh("Models/engravedBlock.stl");
	engravedBlock->setTranslation(-shapeSize + 525, -shapeSize + 175, 0);
	engravedBlock->setShapeColor(1.0, 0.0, 1.0);
	engravedBlock->setUnDraggable();
	displayObject->tell(engravedBlock);

	//Initializes the Box grid
	for (unsigned int i = 0; i < width; ++i){
		for (unsigned int j = 0; j < height; ++j){
			try{
				grid[i * height + j] = new Box(shapeSize, shapeSize, 10);
				grid[i * height + j]->setTranslation((HLdouble)i * shapeSize - 525, (HLdouble)j * shapeSize - 325 , 50);
				grid[i * height + j]->setUnDraggable();
				displayObject->tell(grid[i*height + j]);
			}
			catch (int e){
				cout << e << "\n";
			}
		}
	}

	try{
		//Setting all the touchCallbacks
		for (unsigned int i = 0; i < width; ++i){
			for (unsigned int j = 0; j < height; ++j){
				omniSpace->touchCallback(touchCallback, grid[i * height + j]);
				gridCheck[i * height + j] = true;
			}
		}
	}
	catch (int e){
		cout << e << "\n";
	}

	omniSpace->touchCallback(touchCallback, engravedBlock);

	score->setHapticVisibility(false);
	elapsedTimer->setHapticVisibility(false);
	success->setHapticVisibility(false);
	success->setGraphicVisibility(false);
}

void setUpBlockObject(){
	string font = "";
	string previousBlockText = blockText;
	string temp = "";

	if (regex_match(fontType, print)){
		font = "Anna";

		for (int i = 0; i < blockText.length(); i++){
			temp = temp + blockText[i];
			if (i < blockText.length() - 1){
				temp = temp + " ";
			}
		}
		blockText = temp;
	}
	else{
		font = "Nexa Rust Script L";
	}

	string blockCreationCommand = "openscad -o Models\\engravedBlock.stl -D \"phrase=\\\"" 
		+ blockText + "\\\"\" -D \"font=\\\"" + font + "\\\"\" C:\\Work\\minkowski_block_" 
		+ level + "x.scad";

	blockText = previousBlockText;

	cout << "Loading... \n";

	system(blockCreationCommand.c_str());

	cout << "Finished loading!\n";
}

void showGrid(){
	for (unsigned int i = 0; i < width; ++i){
		for (unsigned int j = 0; j < height; ++j){
			try{
				grid[i * height + j]->setHapticVisibility(true);
				grid[i * height + j]->setGraphicVisibility(true);
			}
			catch (int e){
				cout << e << "\n";
			}
		}
	}
}

void hideGrid(){
	for (unsigned int i = 0; i < width; ++i){
		for (unsigned int j = 0; j < height; ++j){
			try{
				grid[i * height + j]->setHapticVisibility(false);
				grid[i * height + j]->setGraphicVisibility(false);
			}
			catch (int e){
				cout << e << "\n";
			}
		}
	}
}

void resetGrid(){
	for (unsigned int i = 0; i < width; ++i){
		for (unsigned int j = 0; j < height; ++j){
			try{
				gridCheck[i * height + j] = true;
				grid[i * height + j]->setShapeColor(1.0, 1.0, 1.0);
			}
			catch (int e){
				cout << e << "\n";
			}
		}
	}
}

void updateScore(int addValue){
	scoreValue = scoreValue + addValue;
	if (scoreValue < 0){
		scoreValue = 0;
	}
	string scoreString = "Score: " + to_string(scoreValue);
	score->setText(&scoreString[0]);
}

void updateElapsedTimer(int addValue){
	timerValue = timerValue + addValue;
	string timerString = "Time: " + to_string(timerValue);
	elapsedTimer->setText(&timerString[0]);
}


void resetScore(){
	scoreValue = 0;
	updateScore(0);
}

void resetElapsedTimer(){
	timerValue = 0;
	updateElapsedTimer(0);
}

void engravedBlockTouched(){
	if (hasRecorded != true){
		PlaySound(TEXT("C:\\Work\\PTHaptics\\PTHaptics\\Sounds\\Door_Buzzer.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
		updateScore(1);
	}	
}

void promptForUserNumber(){
	bool notAcceptedUserNumber = true;
	regex threeIntegers("^[0-9][0-9][0-9]$");
	while (notAcceptedUserNumber){
		cout << "Enter your 3 digit user number: ";
		cin >> userNumber;

		if (regex_match(userNumber, threeIntegers)){
			notAcceptedUserNumber = false;
		}
	}
}

void promptForBlockText(){
	bool notAcceptedBlockText = true;
	while (notAcceptedBlockText){
		cout << "Enter the text you would like practice on: ";
		cin >> blockText;

		if (regex_match(blockText, atMostFiveCharacters)){
			notAcceptedBlockText = false;
		}
	}

	bool notAcceptedFontType = true;

	while (notAcceptedFontType){
		cout << "Print or cursive font: ";
		cin >> fontType;

		if (regex_match(fontType, print) || regex_match(fontType, cursive)){
			notAcceptedFontType = false;
		}
	}


	setUpBlockObject();
}

void setSuccessCount(){
	if (regex_match(fontType, print)){
		successCount = blockText.length() * 10;
	}
	else if (regex_match(fontType, cursive)){
		successCount = blockText.length() * 9;
	}
}

void saveSessionDataToFile(){
	string filename = "Reports/" + userNumber + ".txt";

	int currentLevel = 0;
	if (level == "1"){
		currentLevel = 3;
	}
	else if (level == "2"){
		currentLevel = 2;
	}
	else if (level == "3"){
		currentLevel = 1;
	}

	//Get current date and time
	time_t rawTime;
	tm * timeInfo;
	char dateTime[80];
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	strftime(dateTime, 80, "%x %X", timeInfo);
	
	fstream file;
	file.open(filename, fstream::out | fstream::app);
	
	file << dateTime << "\t";
	file << blockText << "\t";
	file << fontType << "\t";
	file << currentLevel << "\t";
	file << timerValue << "\t";
	file << scoreValue << endl;

	file.close();

	cout << "Latest session saved!\n";
}

void userWon(){
	hasWon = true;
	success->setGraphicVisibility(true);

	if (!hasRecorded){
		saveSessionDataToFile();

		hasRecorded = true;
	}
}

//From stack overflow
long long milliseconds_now(){
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc){
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return(1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else{
		return GetTickCount();
	}
}