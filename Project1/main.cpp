#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include <vector>
#include <cmath>
#include <thread>
#include <algorithm> 
#include <random>						// std::default_random_engine
#include<dos.h>							// for delay()

using namespace std::this_thread;		 // sleep_for, sleep_until
using namespace std::chrono_literals;	 // ns, us, ms, s, h, etc.
using namespace std;

class Conway : public olc::PixelGameEngine
{
private:
	int G[2][300][300], neighboors = 0, evals = 0, c = 0, zoom_amount = 3;
	const int _s = 300, zoom_window_size = 120;

	const int ei[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	const int ej[8] = { 0, 1, 1, 1, 0, -1, -1, -1};
	const int ofs[4] = { -20, -15, -12, -10 };

public:

	void initialize()
	{
		for (int k = 0; k < 2; k++)
		{
			for (int i = 0; i < _s; i++)
			{
				for (int j = 0; j < _s; j++)
				{
					G[k][i][j] = 0;
				}
			}
		}
	}
	void update()
	{
		if (GetKey(olc::Key::U).bHeld)
		{ 
			sleep_for(15ms);
			for (int i = 0; i < _s; i++)
			{
				for (int j = 0; j < _s; j++)
				{
					find_number_of_neighboors(i, j, c);

					if (neighboors == 3 && G[c][i][j] == 0)
					{
						G[1 - c][i][j] = 1;
					}
					else if (neighboors < 2 && G[c][i][j] == 1)
					{
						G[1 - c][i][j] = 0;
					}
					else if (neighboors > 3 && G[c][i][j] == 1) {
						G[1 - c][i][j] = 0;
					}
					else if (G[c][i][j] == 1 && neighboors == 3) {
						G[1 - c][i][j] = 1;
					}
					else if (G[c][i][j] == 1 && neighboors == 2) {
						G[1 - c][i][j] = 1;
					}
					else {
						G[1 - c][i][j] = 0;
					}


				}

			}
			c = 1 - c;
			evals++;
		}
	}
	void find_number_of_neighboors(int i, int j, int current_grid)
	{
		neighboors = 0;
		for (int k = 0; k < 8; k++)
		{
			neighboors += G[current_grid][i + ei[k]][j + ej[k]];
		}
	}


	void print_evals(int e)
	{
		string sText = std::to_string(e);
		DrawString(_s + 20, 20, "E=", olc::WHITE, 1);
		DrawString(_s + 20 + 15, 20, sText, olc::WHITE, 1);

	}
	void print_neigboors(int i, int j, int current_grid) {

		find_number_of_neighboors(i, j, current_grid);

		string sText = std::to_string(neighboors);
		DrawString(_s + 20, 40, "N=", olc::WHITE, 1);
		DrawString(_s + 20 + 15, 40, sText, olc::WHITE, 1);
	}
	void print_coordinate(int i, int j, int ox, int oy) {
		string sText = std::to_string(i);
		string sText1 = std::to_string(j);
		string sText2 = std::to_string(zoom_amount);

		DrawString(ox, oy - 10, sText, olc::WHITE, 1);
		DrawString(ox + 30, oy - 10, sText1, olc::WHITE, 1);
		DrawString(ox + 60, oy - 10, sText2, olc::WHITE, 1);


	}

	void draw()
	{
		for (int i = 0; i < _s; i++)
		{
			for (int j = 0; j < _s; j++)
			{
				(G[c][i][j] == 1) ? Draw(11 + i,11 + j, olc::WHITE) : 0;
			}
		}
	}	
	void reverse_draw() {

		for (int i = 0; i < _s; i++)
		{
			for (int j = 0; j < _s; j++)
			{
				(G[1-c][i][j] == 1) ? Draw(11 + i, 11 + j, olc::WHITE) : 0;
			}
		}
		print_evals(evals-1);
		int x = int(GetMouseX());
		int y = int(GetMouseY());
		zoom_window(x, y, 1-c);
		print_neigboors(x - 11, y - 11, 1-c);
	}
	void zoom_window(int x, int y, int current_rid) {

		// Where we put the window
		int ox = _s + 20;
		int oy = _s - zoom_window_size + 10;
	
		if (GetMouseWheel() > 0 && zoom_amount != 6) { zoom_amount += 1; }
		if (GetMouseWheel() < 0 && zoom_amount != 3) { zoom_amount += -1; }

		//Drawing the outline of the small rectangle
		DrawRect(ox, oy, zoom_window_size, zoom_window_size, olc::YELLOW);
		DrawRect(x - int(zoom_window_size / zoom_amount)/2, y - int(zoom_window_size / zoom_amount) / 2, int(zoom_window_size / zoom_amount), int(zoom_window_size / zoom_amount), olc::YELLOW);

		//Drawing in the smaller rectange
		for (int i = 0; i < int(zoom_window_size /zoom_amount); i++)
		{
			for (int j = 0; j < int(zoom_window_size / zoom_amount); j++)
			{
				for (int k = 0; k < zoom_amount; k++)
				{
					for (int h = 0; h < zoom_amount; h++)
					{
						(G[current_rid][x + i - 11 + ofs[zoom_amount - 3]][y + j - 11 + ofs[zoom_amount - 3]] == 1) ? Draw(zoom_amount * i + ox + k, zoom_amount * j + oy + h, olc::WHITE) : 0;

					}
				}
			}
		}

		//Drawing where the cursor is currently at
		for (int k = 0; k < zoom_amount; k++)
		{
			for (int h = 0; h < zoom_amount; h++)
			{
				Draw(ox + zoom_window_size / 2 + k, oy + zoom_window_size / 2 + h, olc::GREY);
			}
		}

		print_coordinate(x - 11, y - 11, ox, oy);
	}


	void add_things(int x, int y) {
		//Adding a glider type 1
		if (GetKey(olc::Key::G).bHeld && GetKey(olc::Key::X).bHeld) {

			// Drawing a Glider
			Draw(x, y, olc::GREY);
			Draw(x + 1, y + 1, olc::GREY);
			Draw(x + 1, y + 2, olc::GREY);
			Draw(x, y + 2, olc::GREY);
			Draw(x - 1, y + 2, olc::GREY);



			if (GetMouse(1).bPressed) {

				x = x - 11;
				y = y - 11;
				// Adding a glider to the grid
				G[c][x][y] = 1;
				G[c][x + 1][y + 1] = 1;
				G[c][x + 1][y + 2] = 1;
				G[c][x][y + 2] = 1;
				G[c][x - 1][y + 2] = 1;

			}


		}

		//Adding a glider type 2
		if (GetKey(olc::Key::G).bHeld && GetKey(olc::Key::Y).bHeld) {


			// Drawing a Glider 
			Draw(x, y, olc::GREY);
			Draw(x, y + 2, olc::GREY);

			Draw(x - 1, y + 3, olc::GREY);
			Draw(x - 2, y + 3, olc::GREY);

			Draw(x - 3, y + 3, olc::GREY);
			Draw(x - 3, y, olc::GREY);

			Draw(x - 4, y + 3, olc::GREY);
			Draw(x - 4, y + 2, olc::GREY);
			Draw(x - 4, y + 1, olc::GREY);



			if (GetMouse(1).bPressed) {

				x = x - 11;
				y = y - 11;
				// Adding a glider to the grid
				G[c][x][y] = 1;
				G[c][x][y + 2] = 1;

				G[c][x - 1][y + 3] = 1;
				G[c][x - 2][y + 3] = 1;


				G[c][x - 3][y + 3] = 1;
				G[c][x - 3][y] = 1;

				G[c][x - 4][y + 3] = 1;
				G[c][x - 4][y + 2] = 1;
				G[c][x - 4][y + 1] = 1;

			}


		}





		// Pistole (glider gun)
		if (GetKey(olc::Key::P).bHeld) {

			int x = int(GetMouseX());
			int y = int(GetMouseY());


			// 1st) glider gun part
			Draw(x, y, olc::GREY);
			Draw(x, y - 1, olc::GREY);
			Draw(x + 1, y, olc::GREY);
			Draw(x + 1, y - 1, olc::GREY);

			// 2nd) glider gun part
			Draw(x + 10, y, olc::GREY);
			Draw(x + 10, y - 1, olc::GREY);
			Draw(x + 10, y + 1, olc::GREY);

			Draw(x + 11, y - 2, olc::GREY);
			Draw(x + 11, y + 2, olc::GREY);

			Draw(x + 12, y - 3, olc::GREY);
			Draw(x + 12, y + 3, olc::GREY);
			Draw(x + 13, y - 3, olc::GREY);
			Draw(x + 13, y + 3, olc::GREY);

			Draw(x + 14, y, olc::GREY);

			Draw(x + 15, y - 2, olc::GREY);
			Draw(x + 15, y + 2, olc::GREY);

			Draw(x + 16, y, olc::GREY);
			Draw(x + 16, y - 1, olc::GREY);
			Draw(x + 16, y + 1, olc::GREY);

			Draw(x + 17, y, olc::GREY);



			// 3rd) glider gun part

			Draw(x + 20, y - 1, olc::GREY);
			Draw(x + 20, y - 2, olc::GREY);
			Draw(x + 20, y - 3, olc::GREY);
			Draw(x + 21, y - 1, olc::GREY);
			Draw(x + 21, y - 2, olc::GREY);
			Draw(x + 21, y - 3, olc::GREY);

			Draw(x + 22, y - 4, olc::GREY);
			Draw(x + 22, y, olc::GREY);

			Draw(x + 24, y - 4, olc::GREY);
			Draw(x + 24, y, olc::GREY);
			Draw(x + 24, y - 5, olc::GREY);
			Draw(x + 24, y + 1, olc::GREY);


			Draw(x + 34, y - 2, olc::GREY);
			Draw(x + 34, y - 3, olc::GREY);
			Draw(x + 35, y - 2, olc::GREY);
			Draw(x + 35, y - 3, olc::GREY);


			if (GetMouse(1).bPressed) {

				x = x - 11;
				y = y - 11;

				// 1st) glider gun part
				G[c][x][y] = 1;
				G[c][x][y - 1] = 1;
				G[c][x + 1][y] = 1;
				G[c][x + 1][y - 1] = 1;

				// 2nd) glider gun part
				G[c][x + 10][y] = 1;
				G[c][x + 10][y - 1] = 1;
				G[c][x + 10][y + 1] = 1;

				G[c][x + 11][y - 2] = 1;
				G[c][x + 11][y + 2] = 1;

				G[c][x + 12][y - 3] = 1;
				G[c][x + 12][y + 3] = 1;
				G[c][x + 13][y - 3] = 1;
				G[c][x + 13][y + 3] = 1;

				G[c][x + 14][y] = 1;

				G[c][x + 15][y - 2] = 1;
				G[c][x + 15][y + 2] = 1;

				G[c][x + 16][y] = 1;
				G[c][x + 16][y - 1] = 1;
				G[c][x + 16][y + 1] = 1;

				G[c][x + 17][y] = 1;


				// 3rd) glider gun part

				G[c][x + 20][y - 1] = 1;
				G[c][x + 20][y - 2] = 1;
				G[c][x + 20][y - 3] = 1;

				G[c][x + 21][y - 1] = 1;
				G[c][x + 21][y - 2] = 1;
				G[c][x + 21][y - 3] = 1;

				G[c][x + 22][y - 4] = 1;
				G[c][x + 22][y] = 1;

				Draw(x + 24, y - 4, olc::GREY);
				Draw(x + 24, y, olc::GREY);
				Draw(x + 24, y - 5, olc::GREY);
				Draw(x + 24, y + 1, olc::GREY);

				G[c][x + 24][y - 4] = 1;
				G[c][x + 24][y] = 1;
				G[c][x + 24][y - 5] = 1;
				G[c][x + 24][y + 1] = 1;

				G[c][x + 34][y - 2] = 1;
				G[c][x + 34][y - 3] = 1;
				G[c][x + 35][y - 2] = 1;
				G[c][x + 35][y - 3] = 1;
				// Adding a glider to the grid


			}


		}
	}
	void manual_grid_change()
	{
		int x = int(GetMouseX());
		int y = int(GetMouseY());

		if (GetKey(olc::Key::I).bPressed) {

			initialize();
			evals = 0;
			print_evals(evals);
		}

		// Show neighboors
		if (GetMouse(0).bHeld)
		{
			Draw(x, y, olc::GREY);
			print_neigboors(x-11, y-11, c);
			zoom_window(x, y, c);

			if (GetMouse(1).bHeld) {
				G[c][x - 11][y - 11] = 1;
			}
		}

		// Reverse one step
		if (GetKey(olc::Key::R).bHeld) {

			Clear(olc::BLACK);
			DrawLine(10, 10, _s + 10, 10, olc::YELLOW);
			DrawLine(10, _s + 10, _s + 10, _s + 10, olc::YELLOW);
			DrawLine(_s + 10, 10, _s + 10, _s + 10, olc::YELLOW);
			DrawLine(10, 10, 10, _s + 10, olc::YELLOW);
			reverse_draw();

		}


		// Adding things like glider guns, guns, etc...
		add_things(x, y);

	}


	bool OnUserCreate() override
	{
		initialize();
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		
		Clear(olc::BLACK);
		DrawRect(10, 10, _s, _s, olc::YELLOW);

		draw();
		update();
		print_evals(evals);
		manual_grid_change();


		return true;
	}
};

int main()
{
	Conway demo;
	if (demo.Construct(470, 320, 2, 2))
		demo.Start();
	return 0;
}
