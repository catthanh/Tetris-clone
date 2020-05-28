#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "Tetromino.h"



class Game
{
public: 
	Game();
	~Game();
	bool isRunning() const { return running_; }
	enum class Game_phase
	{
		GAME_PHASE_START,
		GAME_PHASE_PLAY,
		GAME_PHASE_PAUSE,
		GAME_PHASE_OVER,

	};
	void change_game_phase(Game_phase phase);
	void handle_events();
	void handle_inputs();
	void update();

	void render();

	s32 matrixGet(s32 col, s32 row);

	void matrixSet(s32 col, s32 row, s32 value);

	void matrixReset();

	s32 tetrominoGet(Tetromino t, s32 col, s32 row);

	bool checkTetrominoValid(const Tetromino& t);

	bool checkLockdown(Tetromino t);

	void lockTetromino(Tetromino t);

	Tetromino spawnTetromino(Type type);

	u32 getRandomInt();
	bool moveTetromino(Tetromino &t, s32 col, s32 row);
	bool rotateTetromino(Tetromino &t, s32 i);
	u32 getTimeToNextDrop(s32 level);
	bool checkRowFilled(s32 row);
	void clearFilledRow();
	bool checkRowEmpty(s32 row);
	void swapRow(s32 row1, s32 row2);
	void matrixDrop();
	void drawCell(s32 col, s32 row, s32 value, bool outline);
	void drawMatrix();
	void drawTetromino(Tetromino t, bool outline);
	void drawString(std::string s, s32 col, s32 row);
private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	Game_phase phase_;
	bool running_;
	bool pausing_;
	u32 time_;
	u32 next_drop_time_{ 300 };
	s32 lines_{ 0 };
	s32 scores_{ 0 };
	s32 level_{ 1 };
	s32 matrix_[MATRIX_WIDTH * MATRIX_HEIGHT];
	Input input_{};
	Type next_;

	Tetromino t_;
	Tetromino g_;
		
};