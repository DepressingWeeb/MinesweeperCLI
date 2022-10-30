#include <iostream>

#include <vector>

#include <set>

#include <cstdlib>

#include <time.h>

#include <Windows.h>

#include <iomanip>

#include <fstream>

using namespace std;
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
	int bomb_number;
	bool lose;
	vector < vector < bool >> visible;
	vector < vector < bool >> flag;
	vector < vector < int >> grid_content;
	Grid() {}
	Grid(int n_rows, int n_cols, int bomb_number) {
		this->rows = n_rows;
		this->cols = n_cols;
		this->bomb_number = bomb_number;
		this->lose = false;
		this->visible = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
		this->flag = vector < vector < bool >>(n_rows, vector < bool >(n_cols, false));
		this->grid_content = init_grid_content(n_rows, n_cols, bomb_number);
		//print_grid_content();
		//cout << "\n";
	}

	static bool check_valid(int row, int col, int n_rows, int n_cols) {
		return row >= 0 && row < n_rows&& col >= 0 && col < n_cols;
	}

	static vector < vector < int >> init_grid_content(int rows, int cols, int bomb_number) {
		vector < vector < int >> grid_content(rows, vector < int >(cols, 0));
		set < pair < int, int >> bomb_coordinate;
		int row, col, intersect;
		srand(time(0));
		while (true) {
			row = rand() % rows;
			col = rand() % cols;
			bomb_coordinate.insert(make_pair(row, col));
			if (bomb_coordinate.size() >= bomb_number) break;
		}
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (bomb_coordinate.find(make_pair(i, j)) != bomb_coordinate.end()) {
					grid_content[i][j] = -1;
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
					if (bomb_coordinate.find(p) != bomb_coordinate.end()) intersect++;
				}
				grid_content[i][j] = intersect;
			}
		}
		return grid_content;
	}

	void print_grid_current() {
		cout << "      ";
		for (int i = 0; i < cols; i++) {
			if (i < 10) cout << "   " << i << "  ";
			else cout << "  " << i << "  ";
		}
		cout << endl;

		for (int i = 0; i < rows * 2 + 1; i++) {
			if (i % 2 == 0) {
				cout << "      ";
				for (int k = 0; k < cols * 6 + 1; k++) {
					if (k % 6 == 0) {
						cout << '+';
						continue;
					}
					cout << '-';
				}
				cout << '\n';
				continue;
			}
			if ((i - 1) / 2 < 10) cout << (i - 1) / 2 << "     ";
			else cout << (i - 1) / 2 << "    ";
			for (int j = 0; j < cols; j++) {
				cout << "|  ";
				if (visible[(i - 1) / 2][j]) {
					if (grid_content[(i - 1) / 2][j] == 0) {
						cout << "   ";
					}
					else {
						int content = grid_content[(i - 1) / 2][j];
						if (content == -1) {
							Color(75);
							cout << "b  ";
							Color(7);
						}
						else {
							Color(content + 8);
							cout << content << "  ";
							Color(7);
						}
					}
				}
				else if (flag[(i - 1) / 2][j]) {
					Color(4);
					cout << "F  ";
					Color(7);
				}
				else {
					cout << "X  ";
				}
			}
			cout << '|' << '\n';
		}
	}
	void recur(int row, int col) {
		//cout << row << " " << col << endl;
		if (check_valid(row, col, rows, cols) == false) {
			return;
		}
		else if (visible[row][col]) {
			return;
		}
		else if (grid_content[row][col] > 0) {
			visible[row][col] = true;
		}
		else if (grid_content[row][col] == 0) {
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
			cout << "find bomb" << endl;
			lose = true;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					visible[i][j] = true;
				}
			}
		}
	}

	bool check_win() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (not visible[i][j] && grid_content[i][j] > -1) return false;
			}
		}
		return true;
	}

	bool check_lose() {
		return lose;
	}
};

Grid Introduction() {
	int difficulty;
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
	cin >> difficulty;
	if (difficulty == 1) {
		ClearScreen();
		return Grid(8, 8, 2);
	}
	else if (difficulty == 2) {
		ClearScreen();
		return Grid(12, 12, 12);
	}
	else if (difficulty == 3) {
		ClearScreen();
		return Grid(16, 16, 40);
	}
	else {
		int rows, cols, bombNumber;
		while (true) {
			cout << "Input the number of rows (1->99) : ";
			cin >> rows;
			cout << endl;
			cout << "Input the number of cols(1->99) : ";
			cin >> cols;
			cout << endl;
			cout << "Input the number of bombs : ";
			cin >> bombNumber;
			if (rows < 1 || rows > 99 || cols < 1 || cols > 99 || bombNumber >= rows * cols) cout << "Invalid input. Please type again" << endl;
			else break;
		}
		ClearScreen();
		return Grid(rows, cols, bombNumber);
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
		if (not grid->check_valid(row, col, grid->rows, grid->cols)) cout << "Invalid row/col input .Please type again." << endl;
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
		if (not grid->check_valid(row, col, grid->rows, grid->cols)) cout << "Invalid row/col input .Please type again." << endl;
		else break;
	}
	grid->recur(row, col);
}

void saveGame(Grid* grid) {
	FILE* stream;
	freopen_s(&stream, "save.txt", "w", stdout);
	cout << grid->rows << " " << grid->cols << " " << grid->bomb_number << endl;
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->cols; j++) {
			cout << grid->grid_content[i][j] << " ";
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
			grid.grid_content[i][j] = tmp;
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
inline bool is_exist(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

void Game() {
	Grid grid;
	FILE* stream;
	string ans;
	if (is_exist("save.txt")) {
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
	ClearScreen();
	string command;
	clock_t prev, now;
	prev = clock();
	while (true) {
		now = clock();
		double gap = ((double)now - (double)prev) / double(CLOCKS_PER_SEC);
		cout << "----------------------------------------------------------Timer : " << fixed << gap << setprecision(5) << "----------------------------------------------------------" << endl;
		grid.print_grid_current();
		if (grid.check_lose()) {
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
		else if (grid.check_win()) {
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
		command = Command();
		if (command == "f") {
			setFlag(&grid);
		}
		else if (command == "r") {
			revealSquare(&grid);
		}
		else if (command == "n") {
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
		Sleep(1000);
		ClearScreen();
	}
}

int main() {
	SetConsoleWidthAndHeight(0, 0, 1200, 800);
	Game();
	return 0;
}