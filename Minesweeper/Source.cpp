#include <iostream>

#include <stdio.h>

#include <vector>

#include <set>

#include <string>

#include <cstdlib>

#include <time.h>

#include <Windows.h>

#include <iomanip>

#include <fstream>

#include <thread>

#include <mutex>

#include <atomic>

#include <chrono>

using namespace std;
using namespace std::chrono_literals;

mutex mu;
atomic <bool> exitFlag = false;

void gotoxy(int x, int y) {
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

void ClearScreen() {
	COORD topLeft = {
	  0,
	  0
	};
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}
void ClearLines(COORD oldCoord,int nLines) {
	for (int i = 0; i < nLines; i++) {
		gotoxy(0, oldCoord.Y - i);
		cout << "                                                                                                             ";

		
	}
}
void Color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void SetConsoleWidthAndHeight(int left, int top, int width, int height) {
	HWND console = GetConsoleWindow();
	MoveWindow(console, left, top, width, height, TRUE);
}

class Grid {
public:
	int rows;
	int cols;
	int bombNumber;
	bool lose;
	vector < vector < bool >> visible;
	vector < vector < bool >> flag;
	vector < vector < int >> gridContent;
	Grid() {}
	Grid(int n_rows, int n_cols, int bomb_number) {
		this->rows = n_rows;
		this->cols = n_cols;
		this->bombNumber = bomb_number;
		this->lose = false;
		this->visible = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
		this->flag = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
		this->gridContent = initGridContent(n_rows, n_cols, bomb_number);
	}

	static bool checkValid(int row, int col, int rows, int cols) {
		return row >= 0 && row < rows&& col >= 0 && col < cols;
	}

	static vector < vector < int >> initGridContent(int rows, int cols, int bomb_number) {
		vector < vector < int >> gridContent(rows, vector < int >(cols, 0));
		set < pair < int, int >> bombCoordinate;
		int row, col, intersect;
		srand(time(0));
		while (true) {
			row = rand() % rows;
			col = rand() % cols;
			bombCoordinate.insert(make_pair(row, col));
			if (bombCoordinate.size() >= bomb_number) break;
		}
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (bombCoordinate.find(make_pair(i, j)) != bombCoordinate.end()) {
					gridContent[i][j] = -1;
					continue;
				}
				set < pair < int, int >> adjacent = {
				  make_pair(i - 1, j - 1),
				  make_pair(i - 1, j),
				  make_pair(i - 1, j + 1),
				  make_pair(i, j - 1),
				  make_pair(i, j + 1),
				  make_pair(i + 1, j - 1),
				  make_pair(i + 1, j),
				  make_pair(i + 1, j + 1)
				};
				intersect = 0;
				for (auto p : adjacent) {
					if (bombCoordinate.find(p) != bombCoordinate.end()) intersect++;
				}
				gridContent[i][j] = intersect;
			}
		}
		return gridContent;
	}

	void printGridCurrent() {
		printf("      ");
		for (int i = 0; i < cols; i++) {
			if (i < 10) cout << "   " << i << "  ";
			else printf("  %d  ",i);
		}
		printf("\n");

		for (int i = 0; i < rows * 2 + 1; i++) {
			if (i % 2 == 0) {
				printf("      ");
				for (int k = 0; k < cols * 6 + 1; k++) {
					if (k % 6 == 0) {
						printf("+");
						continue;
					}
					printf("-");
				}
				printf("\n");
				continue;
			}
			if ((i - 1) / 2 < 10) printf("%d     ", (i - 1) / 2);
			else printf("%d    ", (i - 1) / 2);
			for (int j = 0; j < cols; j++) {
				printf("|  ");
				if (visible[(i - 1) / 2][j]) {
					if (gridContent[(i - 1) / 2][j] == 0) {
						printf("   ");
					}
					else {
						int content = gridContent[(i - 1) / 2][j];
						if (content == -1) {
							Color(75);
							printf("b  ");
							Color(7);
						}
						else {
							Color(content + 8);
							printf("%d  ",content);
							Color(7);
						}
					}
				}
				else if (flag[(i - 1) / 2][j]) {
					Color(4);
					printf("F  ");
					Color(7);
				}
				else {
					printf("X  ");
				}
			}
			printf("|\n");
		}
	}
	void recur(int row, int col) {
		if (checkValid(row, col, rows, cols) == false) {
			return;
		}
		else if (visible[row][col]) {
			return;
		}
		else if (gridContent[row][col] > 0) {
			visible[row][col] = true;
		}
		else if (gridContent[row][col] == 0) {
			visible[row][col] = true;
			recur(row - 1, col - 1);
			recur(row - 1, col);
			recur(row - 1, col + 1);
			recur(row, col - 1);
			recur(row, col + 1);
			recur(row + 1, col - 1);
			recur(row + 1, col);
			recur(row + 1, col + 1);
		}
		else {
			lose = true;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					visible[i][j] = true;
				}
			}
		}
	}

	bool checkWin() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (not visible[i][j] && gridContent[i][j] > -1) return false;
			}
		}
		return true;
	}

	bool checkLose() {
		return lose;
	}
};

Grid Introduction() {
	string difficulty;
	Color(9);
	cout << "Welcome to the Minesweeper Command Line Interface. Here we have 4 difficulties for you to choose\n\n";
	Color(7);
	cout << "-> 1 : Difficulty Easy ( 8 rows, 8 cols, 2 bombs )\n\n";
	Color(14);
	cout << "-> 2 : Difficulty Medium ( 12 rows, 12 cols, 12 bombs )\n\n";
	Color(12);
	cout << "-> 3 : Difficulty Hard ( 16 rows, 16 cols, 40 bombs )\n\n";
	Color(15);
	cout << "-> 4 : Difficulty Custom\n\n\n\n\n";
	Color(7);
	cout << "-> Please input your difficulty (1/2/3/4) : ";
	while (true) {
		cin >> difficulty;
		if (difficulty == "1") {
			ClearScreen();
			return Grid(8, 8, 2);
		}
		else if (difficulty == "2") {
			ClearScreen();
			return Grid(12, 12, 12);
		}
		else if (difficulty == "3") {
			ClearScreen();
			return Grid(16, 16, 40);
		}
		else if (difficulty == "4") {
			int rows, cols, bombNumber;
			while (true) {
				cout << "Input the number of rows (1->99) : ";
				cin >> rows;
				cout << endl;
				cout << "Input the number of cols(1->99) : ";
				cin >> cols;
				cout << endl;
				cout << "Input the number of bombs (1->"<< rows * cols<<"): ";
				cin >> bombNumber;
				if ( rows < 1 || rows > 99 || cols < 1 || cols > 99 || bombNumber > rows * cols || bombNumber < 1) cout << "Invalid input. Please type again" << endl;
				else break;
			}
			ClearScreen();
			return Grid(rows, cols, bombNumber);
		}
		else cout<<"Invalid" << endl;
	}
		
	}

string Command() {
	string command;
	cout << "List of commands :\n-  Reveal square(r)\n-  Set flag(f)\n-  New game(n)\n-  Save and quit(s)\n";
	cout << "Please type your command : ";
	cin >> command;
	return command;
}
void setFlag(Grid* grid) {
	int row, col;
	while (true) {
		cout << "Type the x-th row that you want to set flag : ";
		cin >> row;
		cout << "Type the y-th column that you want to set flag : ";
		cin >> col;
		if (not grid->checkValid(row, col, grid->rows, grid->cols)) cout << "Invalid row/col input .Please type again." << endl;
		else break;
	}
	grid->flag[row][col] = true;
	return;
}
void revealSquare(Grid* grid) {
	int row, col;
	while (true) {
		cout << "Type the x-th row that you want to reveal : ";
		cin >> row;
		cout << "Type the y-th column that you want to reveal : ";
		cin >> col;
		if (not grid->checkValid(row, col, grid->rows, grid->cols)) cout << "Invalid row/col input .Please type again." << endl;
		else break;
	}
	grid->recur(row, col);
}

void saveGame(Grid* grid) {
	FILE* stream;
	freopen_s(&stream, "save.txt", "w", stdout);
	cout << grid->rows << " " << grid->cols << " " << grid->bombNumber << endl;
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			cout << grid->gridContent[i][j] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			cout << grid->visible[i][j] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			cout << grid->flag[i][j] << " ";
		}
		cout << endl;
	}
}

Grid loadGameSave() {
	FILE* stream;
	freopen_s(&stream, "save.txt", "r", stdin);
	int rows, cols, bombNumber;
	cin >> rows >> cols >> bombNumber;
	Grid grid = Grid(rows, cols, bombNumber);
	int tmp;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			cin >> tmp;
			grid.gridContent[i][j] = tmp;
		}
	}
	int temp;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			cin >> temp;
			grid.visible[i][j] = temp;
		}
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			cin >> temp;
			grid.flag[i][j] = temp;
		}
	}
	freopen_s(&stream, "CON", "r", stdin);
	return grid;
}
inline bool isExist(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		// The function failed. Call GetLastError() for details.
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

bool KeyEventProc(Grid* grid)
{
	//xStart=9,yStart=3
	//0,0:9,3
	//1,0=9,5
	//=>y=3+2*row=>(y-3)/2=row
	//=>(x-9)/6=col
	//<=>col*6+9=x
	COORD pos = GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
	if (GetKeyState(VK_RIGHT) & 0x8000) { gotoxy(min(pos.X + 6, (grid->cols - 1) * 6 + 9), pos.Y); }
	if (GetKeyState(VK_LEFT) & 0x8000) { gotoxy(max(pos.X - 6, 9), pos.Y); }
	if (GetKeyState(VK_UP) & 0x8000) { gotoxy(pos.X, max(pos.Y - 2, 3)); }
	if (GetKeyState(VK_DOWN) & 0x8000) { gotoxy(pos.X, min(pos.Y + 2, (grid->rows - 1) * 2 + 3)); }
	if (GetKeyState('R') & 0x8000) {
		int row = (pos.Y - 3) / 2;
		int col = (pos.X - 9) / 6;
		grid->recur(row, col);
		return true;
	}
	if (GetKeyState('F') & 0x8000) {
		int row = (pos.Y - 3) / 2;
		int col = (pos.X - 9) / 6;
		grid->flag[row][col] = true;
		return true;
	}
	if (GetKeyState('S') & 0x8000) {
		saveGame(grid);
		exit(0);
		return false;
	}
	if (GetKeyState('N') & 0x8000) {
		return false;
	}
	return false;
}

void gameByCommand(Grid grid);
void gameByKey(Grid grid);
void setTimerSinglethreaded(double time) {
	COORD oldCoord;
	oldCoord = GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
	gotoxy(66, 0);
	cout << fixed << time<< setprecision(5);
	gotoxy(oldCoord.X, oldCoord.Y);
	return;
}

void setTimerMultithreaded(clock_t start) {
	//printf("----------------------------------------------------------Timer :       ----------------------------------------------------------");
	this_thread::sleep_for(500ms);
	COORD oldCoord = GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
	
	while (true) {
		if (exitFlag) { return; }
		mu.lock();
		gotoxy(0, 0);
		clock_t now = clock();
		printf( "---------------------------------------------------------- Timer : %5.3f ----------------------------------------------------------", ((double)now - (double)start) / (double)CLOCKS_PER_SEC);
		gotoxy(oldCoord.X, oldCoord.Y);
		mu.unlock();
		this_thread::sleep_for(1000ms);
		oldCoord = GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
	}

}
void Game() {
	Grid grid;
	FILE* stream;
	string ans;
	if (isExist("save.txt")) {
		cout << "Found a game save from last time. Do you want to continue from save?(y/n)";
		while (true) {
			cin >> ans;
			if (ans == "y") {
				grid = loadGameSave();
				break;
			}
			else if (ans == "n") {
				grid = Introduction();
				break;
			}
			else {
				cout << endl << "Invalid input. Please type again(y/n) : ";
			}
		}
	}
	else {
		grid = Introduction();
	}
	
	string choice;
	cout << "Do you want to navigate by command or by keyboard?(c/k) : ";
	while (true) {
		cin >> choice;
		if (choice == "c") {
			ClearScreen();
			exitFlag = false;
			thread t1(setTimerMultithreaded, clock());
			thread t2(gameByCommand, grid);
			t1.join();
			t2.join();
		}
		else if (choice == "k") gameByKey(grid);
		else cout << "Invalid. Please type again(c/k) :";
	}

}



void gameByCommand(Grid grid) {
	this_thread::sleep_for(200ms);
	string command,ans;
	clock_t prev, now;
	while (true) {
		gotoxy(0, 1);
		grid.printGridCurrent();
		if (grid.checkLose()) {
			cout << "Aha , seems like you have lose. Do you want to play a new game? (y/n) ";
			while (true) {
				cin >> ans;
				if (ans == "y") { 
					exitFlag = true;
					this_thread::sleep_for(100ms);
					ClearScreen(); 
					Game(); 
				}
				else if (ans == "n") { 
					exitFlag = true;
					this_thread::sleep_for(100ms);
					exit(0); 
				}
				else {
					cout << endl << "Invalid input. Please type again(y/n) : ";
				}
			}
		}
		else if (grid.checkWin()) {
			cout << "Congratulations,you have won .Do you want to play new game or quit?(y/n) : ";
			while (true) {
				cin >> ans;
				if (ans == "y") Game();
				else if (ans == "n") exit(0);
				else {
					cout << endl << "Invalid input. Please type again(y/n) : ";
				}
			}
		}
		cout << "List of commands :\n-  Reveal square(r)\n-  Set flag(f)\n-  New game(n)\n-  Save and quit(s)\n";
		cout << "Please type your command : ";
		cin >> command;
		if (command == "f") {
			setFlag(&grid);
		}
		else if (command == "r") {
			revealSquare(&grid);
		}
		else if (command == "n") {
			exitFlag = true;
			ClearScreen();
			Game();
		}
		else if (command == "s") {
			saveGame(&grid);
			exit(0);
		}
		else {
			cout << "Command invalid" << endl;
		}
		ClearLines(GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE)),8);
	}
}

void gameByKey(Grid grid) {
	ClearScreen();
	string command,ans;
	clock_t prev, now;
	prev = clock();
	bool isChange = false;
	int delay = 0;
	cout << "----------------------------------------------------------Timer : " << fixed << ((double)clock() - (double)prev) / double(CLOCKS_PER_SEC) << setprecision(5) << "----------------------------------------------------------" << endl;
	grid.printGridCurrent();
	gotoxy(9, 3);
	while (true) {
		setTimerSinglethreaded(((double)clock() - (double)prev) / double(CLOCKS_PER_SEC));
		if (isChange) {
			COORD oldCoord = GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
			gotoxy(0, 1);
			grid.printGridCurrent();
			isChange = false;
			gotoxy(oldCoord.X, oldCoord.Y);
		}
		if (grid.checkLose()) {
			gotoxy(0, 4 + 2 * grid.rows);
			cout << "Aha , seems like you have lose. Do you want to play a new game? (y/n) ";
			while (true) {
				cin >> ans;
				if (ans == "y") Game();
				else if (ans == "n") exit(0);
				else {
					cout << endl << "Invalid input. Please type again(y/n) : ";
				}
			}
		}
		else if (grid.checkWin()) {
			gotoxy(0, 4 + 2 * grid.rows);
			cout << "Congratulations,you have won .Do you want to play new game or quit?(y/n) : ";
			while (true) {
				cin >> ans;
				if (ans == "y") Game();
				else if (ans == "n") exit(0);
				else {
					cout << endl << "Invalid input. Please type again(y/n) : ";
				}
			}
		}
		if (delay == 0) {
			isChange = KeyEventProc(&grid);
			delay = -2;
		}
		else delay++;

		Sleep(30);
	}
	return;
}
int main() {
	SetConsoleWidthAndHeight(0, 0, 1200, 800);
	Game();
	return 0;
}