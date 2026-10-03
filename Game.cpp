#include "stdafx.h"
#include "Game.h"
#include <cstring>

Game::Game()
{
	Reset();
}

void Game::Reset()
{
	Console::SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Console::CursorVisible(false);
	paddle.width = 12;
	paddle.height = 2;
	paddle.x_position = 32;
	paddle.y_position = 30;

	ball.visage = 'O';
	ball.color = ConsoleColor::Cyan;
	ResetBall();

	gameWon = false;
	gameLost = false;

	// TODO #2 - Add this brick and 4 more bricks to the vector
	bricks.clear();
	for (int i = 0; i < 5; ++i)
	{
		Box brick;
		brick.width = 10;
		brick.height = 2;
		brick.x_position = 5 + i * 15;	// 5, 20, 35, 50, 65 -> evenly spaced across the 80-wide window
		brick.y_position = 5;
		brick.doubleThick = true;
		brick.color = ConsoleColor::DarkCyan;	// 3 -> 2 -> 1 -> 0 (Black) = 3 hits
		bricks.push_back(brick);
	}
}

void Game::ResetBall()
{
	ball.x_position = paddle.x_position + paddle.width / 2;
	ball.y_position = paddle.y_position - 1;
	ball.x_velocity = rand() % 2 ? 1 : -1;
	ball.y_velocity = -1;
	ball.moving = false;
}

bool Game::Update()
{
	if (GetAsyncKeyState(VK_ESCAPE) & 0x1)
		return false;

	if (GetAsyncKeyState(VK_RIGHT) && paddle.x_position < WINDOW_WIDTH - paddle.width)
		paddle.x_position += 2;

	if (GetAsyncKeyState(VK_LEFT) && paddle.x_position > 0)
		paddle.x_position -= 2;

	if ((GetAsyncKeyState(VK_SPACE) & 0x1) && !gameWon && !gameLost)
		ball.moving = !ball.moving;

	if (GetAsyncKeyState('R') & 0x1)
		Reset();

	ball.Update();
	CheckCollision();
	return true;
}

//  All rendering, including text, should occur in the Render function
void Game::Render() const
{
	Console::Lock(true);
	Console::Clear();

	paddle.Draw();
	ball.Draw();

	// TODO #3 - Update render to render all bricks
	for (const Box& brick : bricks)
		brick.Draw();

	if (gameWon)
	{
		const char* msg = "You win! Press 'R' to play again.";
		Console::ForegroundColor(ConsoleColor::Green);
		Console::SetCursorPosition((WINDOW_WIDTH - (int)strlen(msg)) / 2, WINDOW_HEIGHT / 2);
		std::cout << msg;
	}
	else if (gameLost)
	{
		const char* msg = "You lose. Press 'R' to play again.";
		Console::ForegroundColor(ConsoleColor::Red);
		Console::SetCursorPosition((WINDOW_WIDTH - (int)strlen(msg)) / 2, WINDOW_HEIGHT / 2);
		std::cout << msg;
	}

	Console::Lock(false);
}

void Game::CheckCollision()
{
	if (gameWon || gameLost)
		return;

	// TODO #4 - Update collision to check all bricks
	for (auto it = bricks.begin(); it != bricks.end(); ++it)
	{
		if (it->Contains(ball.x_position + ball.x_velocity, ball.y_position + ball.y_velocity))
		{
			it->color = ConsoleColor(it->color - 1);
			ball.y_velocity *= -1;

			// TODO #5 - If the ball hits the same brick 3 times (color == black), remove it from the vector
			if (it->color == ConsoleColor::Black)
				bricks.erase(it);

			break;	// only one brick hit per frame; also keeps the iterator safe after erase
		}
	}

	// TODO #6 - If no bricks remain, pause ball and display (render) victory text with R to reset
	if (bricks.empty())
	{
		ball.moving = false;
		gameWon = true;
		return;
	}


	if (paddle.Contains(ball.x_position + ball.x_velocity, ball.y_velocity + ball.y_position))
	{
		ball.y_velocity *= -1;
	}

	// TODO #7 - If ball touches bottom of window, pause ball and display (render) defeat text with R to reset
	if (ball.y_position >= WINDOW_HEIGHT - 1)
	{
		ball.moving = false;
		gameLost = true;
	}
}
