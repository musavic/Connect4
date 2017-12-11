/*
4 in a row game
6 row seven column grid base
*/

#include <Arduino.h>

// The Adafruit_ILI9341 class from here inherits from the Adafruit_GFX class,
// which implements the graphics library functions.
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

// The display uses hardware SPI, plus #9 & #10
// Mega2560 Wiring: connect TFT_CLK to 52, TFT_MISO to 50, and TFT_MOSI to 51
#define TFT_DC 9
#define TFT_CS 10
#define SD_CD 6

// height is 240, width is 320 when oriented horizontally
#define TFT_HEIGHT 240
#define TFT_WIDTH 320

// touch screen pins, obtained from the documentaion
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM  5  // can be a digital pin
#define XP  4  // can be a digital pin

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// colors predefined
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

int movesPlayed = 0;
bool win = false;
void setup() {
	init();
	Serial.begin(9600);
	// Initialize the breakout board.
	// Required for both the display and the SD card.
	tft.begin();
	// because we hold it horizontally
	tft.setRotation(3);
	randomSeed(analogRead(0));
}

int columns = 0, rows = 5;
int fourinarow[7][6];        // our piecese

// if the piece is already played in that positoin
bool blockedrowr[7][6];
bool blockedrowl[7][6];
bool blockedcol[7][6];
bool blockeddiag[7][6];
bool ownPiece[7][6];

void drawPiece(); // preinitialize our drawPiece function

// funciton when cpu wins
void gameOver(){
		tft.setTextColor(RED,BLACK);
		tft.setTextSize(5);
		tft.setCursor(30,20);
		tft.print("GAME OVER");

		tft.setTextColor(GREEN,BLACK);
		tft.setTextSize(1);
		tft.setCursor(30,90);
		tft.print("Press the RESET button to get a REMATCH");
		while(true){}
}

// function when game is won by player
void youWon(){
		tft.setTextColor(BLUE,BLACK);
		tft.setTextSize(6.5);
		tft.setCursor(30,20);
		tft.print("YOU WON");

		tft.setTextColor(GREEN,BLACK);
		tft.setTextSize(1);
		tft.setCursor(30,90);
		tft.print("Press the RESET button to get a REMATCH");
	while(true){}
}

// when game is tied
void gameTied(){
	tft.setTextColor(BLUE,GREEN);
	tft.setTextSize(5);
	tft.setCursor(30,20);
	tft.print("TIE GAME");

	tft.setTextColor(GREEN,BLACK);
	tft.setTextSize(1);
	tft.setCursor(30,90);
	tft.print("Press the RESET button to get a REMATCH");
	while(true){}
}


/* this function will act to check if 4 in a row has been achieved
once a piece is dropped. It will check if a diagnol, vertical, or horizontal
row of pieces have been dropped. If so, game over and the said individual has won */
void wonGamePlayer(){
	//check for horizontal
	for(int col = 0; col < 4;col++){
		for(int row = 5; row >= 0;row--){
			if(fourinarow[col][row] == 1 && fourinarow[col+1][row] == 1 && fourinarow[col+2][row]== 1 && fourinarow[col+3][row] == 1){
				Serial.println("wrong: horizontal");
				win = true;
				youWon();
			}
		}
	}

	//check for vertical WRONG
	for(int row = 5;row > 2;row--){
		for(int col = 0;col < 7;col++){
			if(fourinarow[col][row] == 1 && fourinarow[col][row-1] == 1 && fourinarow[col][row-2]== 1 && fourinarow[col][row-3] == 1){
				Serial.println("wrong: vertical");
				win = true;
				youWon();
			}
		}
	}

	//check for right diagonol
	for(int row = 5;row > 2;row--){
		for(int col = 0;col < 4;col++){
			if(fourinarow[col][row] == 1 && fourinarow[col+1][row-1] == 1 && fourinarow[col+2][row-2] == 1 && fourinarow[col+3][row-3] == 1){
				Serial.println("got a right diagnol!!");
				win = true;

				youWon();
			}
		}
	}

	//check for left diagnol
	for(int row = 5;row > 2;row--){
		for(int col = 6;col > 2;col--){
			if(fourinarow[col][row] == 1 && fourinarow[col-1][row-1] == 1 && fourinarow[col-2][row-2] == 1 && fourinarow[col-3][row-3] == 1){
				Serial.println("got a left diagonol!!");
				win = true;

				youWon();
			}
		}
	}
	if(movesPlayed == 42)win=false;
}

void drawGrid(){ // draws the game grid
	//horizontal lines
	uint16_t customColour = tft.color565(255, 0, 0);

	for(int i = 0;i < 7;i++){
		for(int c = 0;c < 4;c++){
			tft.drawLine((45*i)+c, 0, (45*i)+c, TFT_HEIGHT, customColour);
		}
	}
	//vertical lines
	for(int i = 0;i < 6;i++){
		for(int c = 0;c < 4;c++){
			tft.drawLine(0, (40*i)+c, TFT_WIDTH,(40*i)+c , customColour);
		}
	}
	//border
	for(int i = 0;i < 4;i++){
		tft.drawRect(0, 0,TFT_WIDTH-i,TFT_HEIGHT-i,customColour);
	}
}

//function that checks if CPU can win with one move left and then acts to do it
void CPUwins(){
	//checks if one more move horizontally can get the win
	for(int col = 0; col < 4;col++){
		for(int row = 5; row >= 0;row--){
			//check to see if we can win in between pieces
			if(fourinarow[col][row] == 2 && fourinarow[col+1][row] == 2 && fourinarow[col+2][row]== 0 && fourinarow[col+3][row] == 2){
				Serial.println("almost there wastea");
				if(row == 5){
				//if on the same line (can drop a piece right next to it), then block it
				rows = row;
				columns = col+2;

				fourinarow[columns][rows] = 2;
				//draw last piece, then walk away with the dub
				drawPiece();
				win = true;
				gameOver();
				}
				else if(row != 5 && fourinarow[col+2][row+1] != 0){
					rows = row;
					columns = col+2;

					fourinarow[columns][rows] = 2;
					//draw last piece, then walk away with the dub
					drawPiece();
					win = true;
					gameOver();
				}
			}


			else if(fourinarow[col][row] == 2 && fourinarow[col+1][row] == 2 && fourinarow[col+2][row]== 2 && fourinarow[col+3][row] == 0){
				Serial.println("almost there wastea");
				//if on the same line (can drop a piece right next to it), then block it
				rows = row;
				columns = col+3;

				fourinarow[columns][rows] = 2;
				//draw last piece, then walk away with the dub
				drawPiece();
				win = true;
				gameOver();
			}
		}
	}

	//checks if one more move vertically can get the win
	for(int row = 5;row > 2;row--){
		for(int col = 0;col < 7;col++){
			if(fourinarow[col][row] == 2 && fourinarow[col][row-1] == 2 && fourinarow[col][row-2]== 2 && fourinarow[col][row-3] == 0){
				Serial.println("almost there wastea");
				rows = (row-3);
				columns = col;

				fourinarow[columns][rows] = 2;
				//draw last piece, then walk away with the dub
				drawPiece();
				win = true;
				gameOver();
			}
		}
	}

	//checks if one more move right diagonally can get the win
	for(int row = 5;row > 2;row--){
		for(int col = 0;col < 4;col++){
			//check if one more in between pieces diagonally can win
			if(fourinarow[col][row] == 2 && fourinarow[col+1][row-1] == 2 && fourinarow[col+2][row-2] == 0
				&& fourinarow[col+3][row-3] == 1 && fourinarow[col+2][row-1] != 0){
				Serial.println("almost there wastea");
				rows = (row-3);
				columns = col+3;

				fourinarow[columns][rows] = 2;
				//draw last piece, then walk away with the dub
				drawPiece();
				win = true;
				gameOver();
			}

			//check if one more diagnollay can win
			else if(fourinarow[col][row] == 2 && fourinarow[col+1][row-1] == 2 && fourinarow[col+2][row-2] == 2
				&& fourinarow[col+3][row-2] != 0&& fourinarow[col+3][row-3] == 0){
				Serial.println("almost there wastea");
				rows = (row-3);
				columns = col+3;

				fourinarow[columns][rows] = 2;
				//draw last piece, then walk away with the dub
				drawPiece();
				win = true;
				gameOver();
			}
		}
	}

	//checks if one more move left diagonally can get the win
	for(int row = 5;row > 2;row--){
		for(int col = 6;col > 2;col--){
			//ensuring that there are three in a diagnol, as well as the column being filled just before winning move is dropped
			if(fourinarow[col][row] == 2 && fourinarow[col-1][row-1] == 2 && fourinarow[col-2][row-2] == 2
				&& fourinarow[col-3][row-2] != 0 && fourinarow[col-3][row-3] == 0){
				Serial.println("almost there wastea");
				rows = (row-3);
				columns = col-3;

				fourinarow[columns][rows] = 2;
				//draw last piece, then walk away with the dub
				drawPiece();
				win = true;
				gameOver();

			}
		}
	}
}

//function that checks if the player is one move from winning and blocks it
void checkifoneAway(){
	//check for right horizontal
	for(int col = 0; col < 5;col++){
		for(int row = 5; row >= 0;row--){
			//checks to see if there is three in a row in the first place and one empty on the right
			if(fourinarow[col][row] == 1 && fourinarow[col+1][row] == 1 && fourinarow[col+2][row] == 1 && !blockedrowr[col][row] && fourinarow[col+3][row] == 0){
				if(row == 5){
					rows = row;
					columns = col+3;
					blockedrowr[col][row] = true;
					return;
				}
				else if(row != 5 && fourinarow[col+3][row+1] != 0){
					rows = row;
					columns = col+3;
					blockedrowr[col][row] = true;
					return;
				}
			}
			//checks to see if theres 2 in a row then space, and then another piece
			else if(fourinarow[col][row] == 1 && fourinarow[col+1][row] == 1 && fourinarow[col+3][row] == 1 && !blockedrowr[col][row] && fourinarow[col+2][row] == 0){
				if(row == 5){
					rows = row;
					columns = col+2;
					blockedrowr[col][row] = true;
					return;
				}
				else if(row != 5 && fourinarow[col+3][row+1] != 0){
					rows = row;
					columns = col+2;
					blockedrowr[col][row] = true;
					return;
				}
			}

			//if 2 consecutive elements are white, if the one to the right of it is empty, block it
			else if(fourinarow[col][row] == 1 && fourinarow[col+1][row] == 1 && !blockedrowr[col][row] && fourinarow[col+2][row] == 0){
				Serial.println("right horizontal");
				//if on the same line (can drop a piece right next to it), then block it
				if(row == 5){
					rows = row;
					columns = col+2;
					blockedrowr[col][row] = true;
					return;

				}
				else if(fourinarow[col+2][row+1] != 0 && row != 5){
					Serial.println("yupp");

					rows = row;
					columns = col+2;
					blockedrowr[col][row] = true;
					return;
				}
			}
		}
	}

	//check for left horizontal
	for(int col = 6; col > 1;col--){
		for(int row = 5; row >= 0;row--){
			//checking 3 in a row, empty on right, block it
			if(fourinarow[col][row] == 1 && fourinarow[col-1][row] == 1 && fourinarow[col-2][row] == 1 && !blockedrowl[col][row] && fourinarow[col-3][row] == 0){
				if(rows == 5){
					rows = row;
					columns = col-3;
					blockedrowl[col][row] = true;
					return;
				}
				else if(row != 5 && fourinarow[col+3][row+1] != 0){
					rows = row;
					columns = col-3;
					blockedrowl[col][row] = true;
					return;
				}
			}
			//checking 2 then space then 1 more, block in between
			else if(fourinarow[col][row] == 1 && fourinarow[col-1][row] == 1 && fourinarow[col-2][row] == 0 && !blockedrowl[col][row] && fourinarow[col-3][row] == 1){
				if(rows == 5){
					rows = row;
					columns = col-2;
					blockedrowl[col][row] = true;
					return;
				}
				else if(row != 5 && fourinarow[col+3][row+1] != 0){
					rows = row;
					columns = col-2;
					blockedrowl[col][row] = true;
					return;
				}
			}
			//2 in a row, then empty to the left, block it
			else if(fourinarow[col][row] == 1 && fourinarow[col-1][row] == 1 && !blockedrowl[col][row] && fourinarow[col-2][row] == 0){
				Serial.println("left horizontal");
				//if on the same line (can drop a piece right next to it), then block it
				if(row == 5){
					rows = row;
					columns = col-2;
					blockedrowl[col][row] = true;
				}
				else if(fourinarow[col-2][row+1] != 0 && row != 5){
					Serial.println("yupp");

					rows = row;
					columns = col-2;
					blockedrowl[col][row] = true;

				}

			}
		}
	}

	//check for vertical WRONG
	for(int row = 5;row > 1;row--){
		//Serial.println("shoulbe be coming in");
		for(int col = 0;col < 7;col++){
			//blocking 3
			if(fourinarow[col][row] == 1 && fourinarow[col][row-1] == 1 && fourinarow[col][row-2] == 0 && !blockedcol[col][row] && fourinarow[col][row-2] == 1){
				Serial.println("vertical");

				rows = row-2;
				columns = col;
				blockedcol[col][row] = true;

				return;

			}
			//blocking 2
			else if(fourinarow[col][row] == 1 && fourinarow[col][row-1] == 1 && !blockedcol[col][row] && fourinarow[col][row-2] == 0){
				Serial.println("vertical");

				rows = row-2;
				columns = col;
				blockedcol[col][row] = true;


			}
		}
	}

	//check for right diagonol
	for(int row = 5;row > 2;row--){
		for(int col = 0;col < 4;col++){
			//2 then space 1 in a row
			if(!blockeddiag[col][row] && fourinarow[col][row] == 1 && fourinarow[col+1][row-1] == 1 && fourinarow[col+2][row-2] == 0 && fourinarow[col+3][row-3] == 1 && fourinarow[col+2][row-1] != 0){
				rows = (row-2);
				columns = col+2;

				blockeddiag[col][row] = true;
				return;

			}
			//3 in a row
			else if(!blockeddiag[col][row] && fourinarow[col][row] == 1 && fourinarow[col+1][row-1] == 1 && fourinarow[col+2][row-2] == 1 && fourinarow[col+3][row-2] != 0){
				Serial.println("right diagonol");

				rows = (row-3);
				columns = col+3;

				blockeddiag[col][row] = true;
				return;
			}
		}
	}

	//check for left diagnol
	for(int row = 5;row > 2;row--){
		for(int col = 6;col > 2;col--){
			//2 space then 1 in diagonol
			if(!blockeddiag[col][row] && fourinarow[col][row] == 1 && fourinarow[col-1][row-1] == 1 && fourinarow[col-2][row-2] == 0 && fourinarow[col-3][row-3] == 1 && fourinarow[col-2][row-1] != 0){
				rows = (row-2);
				columns = col-2;

				blockeddiag[col][row] = true;

			}

			//ensuring that there are three in a diagnol, as well as the column being filled just before winning move is dropped
			else if(!blockeddiag[col][row] && fourinarow[col][row] == 1 && fourinarow[col-1][row-1] == 1 && fourinarow[col-2][row-2] == 1 && fourinarow[col-3][row-2] != 0){
				Serial.println("left diagnol");
				rows = (row-3);
				columns = col-3;

				blockeddiag[col][row] = true;

			}
		}
	}
}

void checkFill(){
	//checks the closest piece thats filled, then makes sure to fill the
	//one above it
	Serial.println("CHECKING BUDDY");
	for(int i = 0;i <= 5;i++){
		// Serial.print("actual value");Serial.println(fourinarow[columns][i]);
		//checks if the column is filled. if it is
		if(fourinarow[columns][i] == 1 || fourinarow[columns][i] == 2 || fourinarow[columns][i] == 3 ){
			Serial.print("row: ");Serial.println(i);
			Serial.print("col: ");Serial.println(columns);
			rows = i-1;
			break;
		}
		//OPTIMIZE
		else rows = 5;
	}
}
bool isCPU = false;

void drawPiece(){ // function that draws the piece to its respective coloumn
	// Serial.println("in here");
	if(!isCPU){
		for(int col = 0;col <= 6;col++){
			for(int row = 5; row >= 0;row--){

				if(fourinarow[col][row] == 1 || fourinarow[col][row] == 3){
					tft.drawCircle(col*50+22-col*3,TFT_HEIGHT-(5-row)*39-21,15,WHITE);
					tft.fillCircle(col*50+22-col*3,TFT_HEIGHT-(5-row)*39-21,15,WHITE);
				}
			}
		}
	}
	else{
		for(int col = 0;col <= 6;col++){
			for(int row = 5; row >= 0;row--){

				if(fourinarow[col][row] == 2){
					tft.drawCircle(col*50+22-col*3,TFT_HEIGHT-(5-row)*39-21,15,YELLOW);
					tft.fillCircle(col*50+22-col*3,TFT_HEIGHT-(5-row)*39-21,15,YELLOW);
				}
			}
		}
	}
	// Serial.println("GOT OUT");
}

// a welcome screen
void welcomeScreen(){
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextSize(6.5);
	tft.setCursor(30,20);
	tft.print("WELCOME");

	tft.setTextSize(3);
	tft.setCursor(30,70);
	tft.setTextColor(RED);
	tft.print("INSTRUCTIONS:");

	tft.setTextSize(1.5);
	tft.setCursor(30,100);
	tft.setTextColor(BLUE);
	tft.print("Get 4 in a row pieces to line up...");
	tft.setCursor(30,120);
	tft.print("either vertically");
	tft.setCursor(30,140);
	tft.print("horizontally");
	tft.setCursor(30,160);
	tft.print("or even diagonally :)");

	delay(5000);
	tft.fillScreen(ILI9341_BLACK);
}

bool checkTouch() { // checks to see if screen is touched
	TSPoint touch = ts.getPoint();

	if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
	// no touch, just quit
		return false;
	}

	// get the y coordinate of where the display was touched
	// remember the x-coordinate of touch is really our y-coordinate
	// on the display
	int touchY = map(touch.x, TS_MINX, TS_MAXX, 0, TFT_HEIGHT - 1);

	// need to invert the x-axis, so reverse the
	// range of the display coordinates
	int touchX = map(touch.y, TS_MINY, TS_MAXY, TFT_WIDTH - 1, 0);

	/*
	if the screen is touched at a specific position, drop it in its specific column
	*/
	if (touchX <= 32 && touchX >= 0){
		columns = 0;
		delay(1300);
		return true;
	}
	else if (touchX <= 75 && touchX >= 50){
		columns = 1;
		delay(1300);
		return true;
	}
	else if (touchX <= 130 && touchX >= 98){
		columns = 2;
		delay(1300);
		return true;
	}
	else if (touchX <= 175 && touchX >= 150){
		columns = 3;
		delay(1300);
		return true;
	}
	else if (touchX <= 230 && touchX >= 205){
		columns = 4;
		delay(1300);
		return true;
	}
	else if (touchX <= 275 && touchX >= 250){
		columns = 5;
		delay(1300);
		return true;
	}
	else if (touchX >= 290){
		columns = 6;
		delay(1300);
		return true;
	}

}

void processMovement(){ // our decision maker function
	enum condition {welcome, player, CPU, tieGame};
	condition state = welcome;
	if(state == welcome){
		Serial.print("WWWWEEEEELLLLCCCOOOOMMMMEEE");
		state = player;
		welcomeScreen();
		drawGrid();
	}
	char readKey;
	while(true){
		if(state == player){

			isCPU = false;

				while(!checkTouch()){}
				checkFill();
				Serial.println("-------touched-------");

				//whatever column should remain the same, row should be filled with 1 (for reference)
				fourinarow[columns][rows] = 1;
				Serial.print("Col is: ");Serial.print(columns);Serial.print(" Row is:");Serial.println(rows);
				Serial.println(readKey,HEX);

				//draw circle at position [columns][rows]
				drawPiece();

				//row-- (therefore next time we'll draw at [columns][rows-1])
				if(rows > 0){
					rows--;
				}
				movesPlayed++;
				if(movesPlayed > 42){
					state = tieGame;
				}
				wonGamePlayer();
				delay(250);
				state = CPU;
				readKey = 'b';

		}

		else if(state == CPU){
			isCPU = true;

			CPUwins();
			columns = random(7);
			// Serial.print("column is -------------------------': ");Serial.println(columns);
			// Serial.print("row is ------------------------': ");Serial.println(rows);


			checkFill();

			while(rows <= 0){

				//keep generating random columns until we find one that isnt filled
				columns = random(7);
				checkFill();
				Serial.println("IN HERE MATE");
			}
			checkifoneAway();
			fourinarow[columns][rows] = 2;
			//draw circle at position [columns][rows]
			drawPiece();
			Serial.print("column is -------------------------': ");Serial.println(columns);
			Serial.print("row is ------------------------': ");Serial.println(rows);

			//row-- (therefore next time we'll draw at [columns][rows-1])
			if(rows > 0){
				rows--;
			}

			movesPlayed++;
			if(movesPlayed >= 42){
				state = tieGame;
			}
			state = player;
		}
	}

	// if the game is tied
	if(state == tieGame){
		gameTied();
	}
}
int main() {
	setup();
	// fill the screen with black
	tft.fillScreen(ILI9341_BLACK);

	//draw background grid
	drawGrid();

	//prefill array with 0
	for(int i = 0;i < 6;i++){
		for(int c = 0; c < 7;c++){
			//both currently unfilled
			fourinarow[i][c] = 0;
			blockedrowr[i][c] = 0;
			blockedrowl[i][c] = 0;

			blockedcol[i][c] = 0;
			blockeddiag[i][c] = 0;

		}
	}
	Serial.println("got out");

	processMovement();


	Serial.end();

	return 0;
}
