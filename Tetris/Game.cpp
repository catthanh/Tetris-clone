#include "Game.h"

Game::Game():
	window_{nullptr},
	renderer_{nullptr},
	phase_{Game_phase::GAME_PHASE_START},
	running_{true},
	pausing_{ false },
	time_{0},
	next_drop_time_{300},
	lines_{0},
	scores_{0},
	level_{ 1 },
	input_{},
	matrix_{0}
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "Failed to init SDL" << std::endl;
		running_ = false;
		return;

	}

	window_ = SDL_CreateWindow("Thanh", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_PIXEL_WIDTH, WINDOW_PIXEL_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window_)
	{
		std::cout << "Failed to create window" << std::endl;
		running_ = false;
		return;
	}
	renderer_ = SDL_CreateRenderer(window_, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer_)
	{
		std::cout << "Failed to create renderer"<< std::endl;
		running_ = false;
		return;
	}
	if (TTF_Init() < 0)
	{
		std::cout << "Failed to init SDL_ttf" << std::endl;
		running_ = false;
		return;
	}
	
	next_ = static_cast<Type>(getRandomInt());
}

Game::~Game()
{
	
	renderer_ = nullptr;
	window_ = nullptr;
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}


void Game::change_game_phase(Game_phase phase)
{
	if (phase == Game_phase::GAME_PHASE_LINE)
	{
		time_ = SDL_GetTicks();
		line_clear_time_ = time_ + 150;
	}
	phase_ = phase;
}

void Game::handle_events()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
		{
			std::cout << "Quit event detected" << std::endl;
			running_ = false;
		}
	}
}

void Game::handle_inputs()
{
	s32 key_count;
	const u8* key_states = SDL_GetKeyboardState(&key_count);

	if (key_states[SDL_SCANCODE_ESCAPE])
	{
		running_ = false;
	}

	Input  prev_input = input_;

	input_.left = key_states[SDL_SCANCODE_LEFT];
	input_.right = key_states[SDL_SCANCODE_RIGHT];
	input_.up = key_states[SDL_SCANCODE_UP];
	input_.down = key_states[SDL_SCANCODE_DOWN];
	input_.space = key_states[SDL_SCANCODE_SPACE];
	input_.p = key_states[SDL_SCANCODE_P];
	

	input_.dleft = input_.left - prev_input.left;
	input_.dright = input_.right - prev_input.right;
	input_.dup = input_.up - prev_input.up;
	input_.ddown = input_.down - prev_input.down;
	input_.dspace = input_.space - prev_input.space;
	input_.dp = input_.p - prev_input.p;
	//delay processing when holding key
	u32 delaytime = 300;
	if (prev_input.left <= 0 && input_.left > 0)
	{
		input_.pressTime = SDL_GetTicks();
	}
	if (input_.left > 0 && prev_input.left > 0)
	{
		if (SDL_GetTicks() > input_.pressTime + delaytime)
			input_.dleft = input_.left;
	}
	if (prev_input.right <= 0 && input_.right > 0)
	{
		input_.pressTime = SDL_GetTicks();
	}
	if (input_.right > 0 && prev_input.right > 0)
	{
		if (SDL_GetTicks() > input_.pressTime + delaytime)
			input_.dright = input_.right;
	}
	if (prev_input.down <= 0 && input_.down > 0)
	{
		input_.pressTime = SDL_GetTicks();
	}
	if (input_.down > 0 && prev_input.down > 0)
	{
		if (SDL_GetTicks() > input_.pressTime + delaytime)
			input_.ddown = input_.down;
	}
	
	
	
}

void Game::update()
{	
	switch (phase_)
	{
	case Game::Game_phase::GAME_PHASE_START:
	{
		matrixReset();
		lines_ = 0;
		scores_ = 0;
		level_ = 1;
		t_ = spawnTetromino(next_);
		next_ = static_cast<Type>(getRandomInt());
		if (input_.dspace > 0)
		{
			time_ = SDL_GetTicks();
			next_drop_time_ = time_ + getTimeToNextDrop(level_);
			change_game_phase(Game::Game_phase::GAME_PHASE_PLAY);
		}
		break;
	}
	case Game::Game_phase::GAME_PHASE_PLAY:
	{
		level_ = lines_ / 10+1;
		if (input_.dp > 0)
		{
			change_game_phase(Game::Game_phase::GAME_PHASE_PAUSE);
		}
		if (input_.dleft>0)
		{
			moveTetromino(t_, -1, 0);
		}
		if (input_.dright > 0)
		{
			moveTetromino(t_, 1, 0);
		}
		if (input_.dup > 0)
		{
			if ( t_.type() != Type::O)
			{
				rotateTetromino(t_,1);
			}			
		}
		if (input_.ddown > 0)
		{
			if (moveTetromino(t_,0,1))
			{
				next_drop_time_ = time_ + getTimeToNextDrop(level_);
			}
		}
		if (input_.dspace > 0)
		{
			while (moveTetromino(t_,0,1))
			{
			}
			lockTetromino(t_);
			t_ = spawnTetromino(next_);
			time_ = SDL_GetTicks();
			next_drop_time_ = time_ + getTimeToNextDrop(level_);
		} 
		if (!checkRowEmpty(-2))
		{
			change_game_phase(Game_phase::GAME_PHASE_OVER);
		}
		//ghost pieces
		g_ = t_;
		while (moveTetromino(g_,0,1))
		{
		}
		
		//auto-drop
		time_ = SDL_GetTicks();
		if (time_ > next_drop_time_)
		{
			if (moveTetromino(t_, 0, 1))
				next_drop_time_ = time_ + getTimeToNextDrop(level_);
			else
			{

				lockTetromino(t_);

		
				time_ = SDL_GetTicks();
				next_drop_time_ = time_ + getTimeToNextDrop(level_);
				t_ = spawnTetromino(next_);
			}
		}
		for (s32 row{}; row < 20; row++)
		{
			if (checkRowFilled(row))
			{

				change_game_phase(Game::Game_phase::GAME_PHASE_LINE);

			};
		}
		//clearFilledRow();
		//matrixDrop();
		break; 
	}
	case Game::Game_phase::GAME_PHASE_PAUSE:
	{
		if (input_.dp > 0)
			change_game_phase(Game::Game_phase::GAME_PHASE_PLAY);
		break;
	}
	case Game::Game_phase::GAME_PHASE_LINE:
	{
		
		time_ = SDL_GetTicks();
		if (time_ > line_clear_time_)
		{
			clearFilledRow();
			matrixDrop();
			change_game_phase(Game::Game_phase::GAME_PHASE_PLAY);
		}
		break;
	}
	case Game::Game_phase::GAME_PHASE_OVER:
		if (input_.dspace > 0)
			change_game_phase(Game::Game_phase::GAME_PHASE_START);

		break;
	default:
		break;
	}	
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer_,0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(renderer_);
	//render matrix
	drawMatrix();
	std::stringstream slines;
	slines << lines_;
	std::stringstream sscores;
	sscores << scores_;
	std::stringstream slevel;
	slevel << level_;

	switch (phase_)
	{
	case Game::Game_phase::GAME_PHASE_START:

		drawString("Press space ", 10, 4);
		drawString(" to start", 10, 5);
		break;
	case Game::Game_phase::GAME_PHASE_PLAY:
	{
		drawTetromino(t_, false);
		drawTetromino(g_, true);
		
		
		drawString("lines: ", 12, 0);
		drawString(slines.str(), 11, 1);
		drawString("scores: ", 12, 2);
		drawString(sscores.str(), 11, 3);
		drawString("level : ", 12, 4);
		drawString(slevel.str(), 11, 5);
		drawString("Next Pieces", 12, 6);
		//draw next tetromino
		Tetromino next{};
		next.set_type(next_);
		if (next.type() == Type::O)
			next.move(1, 1);
		next.move(9, 7 );
		drawTetromino(next, false);

		break;
	}
	case Game::Game_phase::GAME_PHASE_PAUSE:
	{
		drawString("Press p ", 10, 4);
		drawString(" to continue", 10, 5);
		drawTetromino(t_, false);
		drawTetromino(g_, true);
		break;
	}
	case Game::Game_phase::GAME_PHASE_LINE:
	{
		drawTetromino(t_, false);
		drawTetromino(g_, true);


		drawString("lines: ", 12, 0);
		drawString(slines.str(), 11, 1);
		drawString("scores: ", 12, 2);
		drawString(sscores.str(), 11, 3);
		drawString("level : ", 12, 4);
		drawString(slevel.str(), 11, 5);
		drawString("Next Pieces", 12, 6);
		//draw next tetromino
		Tetromino next{};
		next.set_type(next_);
		if (next.type() == Type::O)
			next.move(1, 1);
		next.move(9, 7);
		drawTetromino(next, false);

		//draw white line
		drawClearedLine();
		break;
	}
	case Game::Game_phase::GAME_PHASE_OVER:
	{
		drawString("lines: ", 12, 0);
		drawString(slines.str(), 11, 1);
		drawString("scores: ", 12, 2);
		drawString(sscores.str(), 11, 3);
		drawString("level: ", 12, 4);
		drawString(slevel.str(), 11, 5);
		//drawString("Next Pieces", 10, 6);
		drawString("Game over", 10, 7);
		break;
	}
	default:
		break;
	}

	//phase render

	SDL_RenderPresent(renderer_);
}

s32 Game::matrixGet(s32 col, s32 row)
{
	return matrix_[col + (row + VISIBLE_MATRIX_HEIGHT) * MATRIX_WIDTH];
}

void Game::matrixSet(s32 col, s32 row, s32 value)
{
	 matrix_[col + (row + VISIBLE_MATRIX_HEIGHT) * MATRIX_WIDTH] = value;
}

void Game::matrixReset()
{
	//fill matrix with 0
	for (s32 i{}; i < MATRIX_HEIGHT * MATRIX_WIDTH; i++)
		matrix_[i] = 0;
}

s32 Game::tetrominoGet(Tetromino t,s32 col, s32 row)
{
	//get current data
	const s32* data = tetrorino[static_cast<s32>(t.type())];
	s32 size{ t.size() };

	switch (t.orientation())
	{
	case 0: //0deg
	{
		return data[col + row * size];
		break;
	}
	case 1: //90deg
	{
		return data[(size - col - 1) * size + row];
		break;
	}
	case 2: //180deg
	{
		return data[(size - 1) - col + ((size - 1) - row) * size];
		break;
	}
	case 3: //270deg
	{
		return data[col * size + (size - row - 1)];
		break;
	}
	default:
		return 0;
		break;
	}
}

bool Game::checkTetrominoValid(const Tetromino &t)
{
	for (s32 y{}; y < t.size(); y++)
	{
		for (s32 x{}; x < t.size(); x++)
		{
			if (tetrominoGet(t, x, y) > 0)
			{
				s32 matrix_col = t.offset_col() + x;
				s32 matrix_row = t.offset_row() + y;
				if (matrix_col < 0)
					return false;
				if (matrix_col >= 10)
					return false;
				if (matrix_row < -20)
					return false;
				if (matrix_row >= 20)
					return false;
				if (matrixGet(matrix_col, matrix_row))
				{
					return false;
				}

			}

		}
	}


	return true;

}

bool Game::checkLockdown(Tetromino t)
{
	return false;
}

void Game::lockTetromino(Tetromino t)
{
	for (s32 y{}; y < t.size(); y++) {
		for (s32 x{}; x < t.size(); x++)
			if (t.tetromino_get(x, y))
				matrixSet(x + t.offset_col(), y + t.offset_row(),
					t.tetromino_get(x, y));
	}
}

Tetromino Game::spawnTetromino(Type type)
{
	Tetromino a{};
	a.set_type(type);
	next_ = static_cast<Type>(getRandomInt());
	if (a.type() == Type::O)
		a.move(1 , 1);
	if (a.type() == Type::I)
		a.move(-1, 0);
	while (!checkTetrominoValid(a))
		a.move(0, -1);
	return a;
}

u32 Game::getRandomInt()
{
	//srand(SDL_GetTicks());
	return rand() % 7 + 1;
}

bool Game::moveTetromino(Tetromino &t, s32 col, s32 row)
{
	Tetromino tt = t;
	tt.move(col, row);
	if (checkTetrominoValid(tt))
	{
		t = tt;
		return true;
	}
	return false;
}

bool Game::rotateTetromino(Tetromino &t, s32 i)
{
	for (s32 test{}; test < 5; test++)
	{
		Tetromino tt = t;
		s32 new_col{};
		s32 new_row{};
		switch (t.type())
		{
		case Type::I:
		{
			new_col = offset_data_i[tt.orientation() * 10 + test * 2] - offset_data_i[((tt.orientation() + 1) % 4) * 10 + test * 2];
			new_row = offset_data_i[tt.orientation() * 10 + test * 2 + 1] - offset_data_i[((tt.orientation() + 1) % 4) * 10 + test * 2 + 1];
			std::cout << new_col << ","
				<< new_row << std::endl;
			break;
		}
		case Type::O:
		{
			break;
		}
		default:
		{
			new_col = -offset_data[tt.orientation() * 10 + test*2] + offset_data[((tt.orientation() + 1) % 4) * 10 + test*2];
			new_row = -offset_data[tt.orientation() * 10 + test * 2+1] + offset_data[((tt.orientation() + 1) % 4) * 10 + test * 2+1];
			std::cout << new_col << ","
			<< new_row << std::endl;
			break;
		}
		
		};
		tt.rotate(i);
		std::cout << tt.offset_col() << " , " << tt.offset_row() << std::endl;
		tt.move(new_col, new_row);
		std::cout << tt.offset_col() << " , " << tt.offset_row() << std::endl;
		if (checkTetrominoValid(tt))
		{
			t = tt;
			return true;
		}

		
	}
	return false;
}

u32 Game::getTimeToNextDrop(s32 level)
{
	std::cout <<  1000  / level << std::endl;
	return 1000  / level;
}

void Game::softDrop()
{

}

bool Game::checkRowFilled(s32 row)
{
	for (s32 i{ 9 }; i >= 0; i--)
	{
	
		if (matrixGet(i, row) == 0)
			return false;
	}

	return true;
}

bool Game::clearFilledRow()
{
	s32 rowFilledInOneTime = 0;
	for (s32 row{}; row < 20; row++)
	{
		if (checkRowFilled(row))
		{
			for (s32 i{}; i < MATRIX_WIDTH; i++)
				matrixSet(i, row, 0);
			lines_++;
			rowFilledInOneTime++;
		}
	}
	switch (rowFilledInOneTime)
	{
	case 1:
		{
			scores_ += 100;
			break;
		}
	case 2:
		{
			scores_ += 300;
			break;
		}
	case 3:
		{
			scores_ += 500;
			break;
		}
	case 4:
		{
			scores_ += 800;
			break;
		}
	default:
		break;
	}
	if (rowFilledInOneTime)
		return true;
	return false;
}

bool Game::checkRowEmpty(s32 row)
{
	for (s32 i{ 9 }; i >= 0; i--)
	{
		if (matrixGet(i, row) != 0)
			return false;
	}
	return true;
}

void Game::swapRow(s32 row1, s32 row2)
{
	{
		for (s32 i{}; i < MATRIX_WIDTH; i++)
		{
			s32 a{ matrixGet(i, row1) };

			matrixSet(i, row1, matrixGet(i, row2));
			matrixSet(i, row2, a);
		}
	}
}

void Game::matrixDrop()
{
	for (s32 row{ 19 }; row >= 0; row--)
	{
		if (checkRowEmpty(row))
			for (s32 a{}; a < 6; a++)
			{
				if (!checkRowEmpty(row - a)) {
					swapRow(row, row - a);
					break;
				}
				
			}
	};
}

void Game::drawCell(s32 col, s32 row, s32 value, bool outline)
{
	switch (value)
	{
	case 0:
	{
		SDL_SetRenderDrawColor(renderer_, 0x77, 0x77, 0x77, 0xff);
		break;
	}
	case 1:
	{
		SDL_SetRenderDrawColor(renderer_, 0x00, 0xff, 0xff, 0xff);
		break;
	}
	case 2:
	{
		SDL_SetRenderDrawColor(renderer_, 0x80, 0x00, 0x80, 0xff);
		break;
	}
	case 3:
	{
		SDL_SetRenderDrawColor(renderer_, 0xff, 0xff, 0x00, 0xff);
		break;
	}
	case 4:
	{
		SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0xff, 0xff);
		break;
	}
	case 5:
	{
		SDL_SetRenderDrawColor(renderer_, 0xff, 0xa5, 0x00, 0xff);
		break;
	}
	case 6:
	{
		SDL_SetRenderDrawColor(renderer_, 0x00, 0xff, 0x00, 0xff);
		break;
	}
	case 7:
	{
		SDL_SetRenderDrawColor(renderer_, 0xff, 0x00, 0x00, 0xff);
		break;
	}
	default:
		break;
	}
	int o = 1;
	if (outline) o = 0;
	SDL_Rect rect{
		col * CELL_PIXCEL_SIZE + 1 * o,
		row * CELL_PIXCEL_SIZE + 1 * o,
		CELL_PIXCEL_SIZE - 2 * o,
		CELL_PIXCEL_SIZE - 2 * o,
	};
	if (!outline)
	{
		SDL_RenderFillRect(renderer_, &rect);
	}
	else
	{
		SDL_RenderDrawRect(renderer_, &rect);
	}

}

void Game::drawMatrix()
{
	for (s32 y{}; y < VISIBLE_MATRIX_HEIGHT; y++)
		for (s32 x{}; x < VISIBLE_MATRIX_WIDTH; x++)
			if (matrixGet(x, y) == 0)
			{
				drawCell(x, y, matrixGet(x, y), false);
				drawCell(x, y, matrixGet(x, y), true);
			}
			else
			{
				drawCell(x, y, matrixGet(x, y), false);
				drawCell(x, y, 0, true);
			}
}

void Game::drawClearedLine()
{
	SDL_SetRenderDrawColor(renderer_,255, 255, 255, 255);
	for (s32 row{}; row < 20; row++)
	{
		if (checkRowFilled(row))
		{
			SDL_Rect line
			{
			0 ,
			row * CELL_PIXCEL_SIZE,
			10 * CELL_PIXCEL_SIZE,
			CELL_PIXCEL_SIZE,
			};
			SDL_RenderFillRect(renderer_, &line);
		}
	}
}

void Game::drawTetromino(Tetromino t, bool outline)
{
	for (s32 y{}; y < t.size(); y++) {
		for (s32 x{}; x < t.size(); x++)
			if (tetrominoGet(t, x, y))
				drawCell(x + t.offset_col(), y + t.offset_row(),
					tetrominoGet(t, x, y), outline);
	}
}

void Game::drawString(std::string s, s32 col, s32 row)
{
	static TTF_Font* Sans = TTF_OpenFont("BitDarling10.ttf", 20);

	SDL_Color White = { 255, 255, 255 };  
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, s.c_str(), White); 

	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer_, surfaceMessage); 

	SDL_Rect Message_rect; 
	Message_rect.x = col*CELL_PIXCEL_SIZE;  
	Message_rect.y = row*CELL_PIXCEL_SIZE; 
	Message_rect.w = surfaceMessage->w; 
	Message_rect.h = surfaceMessage->h; 

	SDL_RenderCopy(renderer_, Message, NULL, &Message_rect); 
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	surfaceMessage = nullptr;
	Message = nullptr;
	
}
