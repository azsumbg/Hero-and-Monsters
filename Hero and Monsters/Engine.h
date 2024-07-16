#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API _declspec(dllexport)
#else
#define ENGINE_API _declspec(dllimport)
#endif

#include <cstdlib>

constexpr float scr_width = 800.0f;
constexpr float scr_height = 700.0f;
struct client_area
{
	float up = 50.0f;
	float down = scr_height - 50.0f; 
};

constexpr int DLL_OK = 15001;
constexpr int DLL_FAIL = 15002;
constexpr int DLL_NaN = 15003;

enum class types {
	hero = 0, centaur1 = 1, centaur2 = 2, croc = 3, gargoyle1 = 4, gargoyle2 = 5,
	octopus1 = 6, octopus2 = 7, walking_tree = 8, no_type = 9, hero_shot = 10, evil_shot = 11
};
enum class actions { move = 0, shoot = 1, block = 2, stop = 3 };

struct AI_INPUT
{
	float enemy_ex;
	int enemy_lifes;
	bool now_shooting;
};

namespace dll
{
	class ENGINE_API ATOM
	{
		protected:
			float width = 0;
			float height = 0;
			
			ATOM* my_ptr = nullptr;

		public:
			float x = 0;
			float y = 0;
			float ex = 0;
			float ey = 0;

			ATOM(float sx, float sy, float swidth, float sheight)
			{
				x = sx;
				y = sy;
				width = swidth;
				height = sheight;
				ex = x + width;
				ey = y + height;
			}

			ATOM()
			{
				width = 1.0f;
				height = 1.0f;
				ex = 1.0f;
				ey = 1.0f;
			}

			virtual ~ATOM() {};

			ATOM* CreateHeapAtom(float sx, float sy, float swidth, float sheight) 
			{
				my_ptr = new ATOM(sx, sy, swidth, sheight);
				return my_ptr;
			}
			int ReleaseHeapAtom()
			{
				if (my_ptr)
				{
					delete my_ptr;
					return DLL_OK;
				}
				return DLL_FAIL;
			}

			float GetWidth() const
			{
				return width;
			}
			float GetHeight() const
			{
				return height;
			}

			bool InHeap() const
			{
				if (my_ptr)return true;
				return false;
			}

			void SetWidth(float new_width)
			{
				width = new_width;
				ex = x + width;
			}
			void SetHeight(float new_height)
			{
				height = new_height;
				ey = y + height;
			}
			void NewDims(float new_width, float new_height)
			{
				SetWidth(new_width);
				SetHeight(new_height);
			}
			void SetEdges()
			{
				ex = x + width;
				ey = y + height;
			}

	};

	class ENGINE_API CREATURE :public ATOM
	{
		protected:
			types my_type = types::no_type;
			float speed = 0;

			CREATURE(types atype, float first_x)
				:ATOM()
			{
				my_type = atype;

				x = first_x;
				
				switch (my_type)
				{
				case types::centaur1:
					y = scr_height - 250.0f;
					NewDims(150.0f, 150.0f);
					lifes = 200;
					strenght = 30;
					max_shoot_delay = 400;
					max_block_delay = 100;
					speed = 0.5f;
					break;

				case types::centaur2:
					y = scr_height - 240.0f;
					NewDims(85.0f, 140.0f);
					lifes = 180;
					strenght = 27;
					max_shoot_delay = 395;
					max_block_delay = 95;
					speed = 0.8f;
					break;

				case types::croc:
					y = scr_height - 220.0f;
					NewDims(110.0f, 120.0f);
					lifes = 160;
					strenght = 25;
					max_shoot_delay = 390;
					max_block_delay = 90;
					speed = 1.0f;
					break;

				case types::gargoyle1:
					y = scr_height - 230.0f;
					NewDims(95.0f, 130.0f);
					lifes = 190;
					strenght = 28;
					max_shoot_delay = 500;
					max_block_delay = 100;
					speed = 0.5f;
					break;

				case types::gargoyle2:
					y = scr_height - 230.0f;
					NewDims(125.0f, 130.0f);
					lifes = 190;
					strenght = 30;
					max_shoot_delay = 397;
					max_block_delay = 97;
					speed = 0.7f; 
					break;

				case types::octopus1:
					y = scr_height - 170.0f;
					NewDims(150.0f, 70.0f);
					lifes = 150;
					strenght = 24;
					max_shoot_delay = 380;
					max_block_delay = 80;
					speed = 1.0f;
					break;

				case types::octopus2:
					y = scr_height - 240.0f;
					NewDims(140.0f, 140.0f);
					lifes = 200;
					strenght = 30;
					max_shoot_delay = 500;
					max_block_delay = 100;
					speed = 0.6f;
					break;

				case types::walking_tree:
					y = scr_height - 230.0f;
					NewDims(115.0f, 130.0f);
					lifes = 190;
					strenght = 26;
					max_shoot_delay = 394;
					max_block_delay = 94;
					speed = 0.8f;
					break;
				}
			}

		public:
			int lifes = 0;
			int strenght = 0;
			
			int max_shoot_delay = 0;
			int shoot_counter = 0;

			int max_block_delay = 0;
			int block_counter = 0;

			actions current_action = actions::move;

			static CREATURE* EvilFactory(types type, float start_x);

			virtual ~CREATURE() {};

			types GetType()const
			{
				return my_type;
			}
			
			void Release()
			{
				delete this;
			}

			bool Shoot()
			{
				current_action = actions::shoot;
				shoot_counter++;
				if (shoot_counter >= max_shoot_delay)
				{
					shoot_counter = 0;
					current_action = actions::stop;
					return true;
				}
				return false;
			}

			bool Block()
			{
				current_action = actions::block;
				block_counter++;
				if (block_counter >= max_block_delay)
				{
					block_counter = 0;
					current_action = actions::stop;
					return false;
				}
				return true;
			}

			void Move()
			{
				current_action = actions::move;
				x -= speed;
				SetEdges();
				current_action = actions::stop;
			}

			actions AINextMove(AI_INPUT situation)
			{
				actions proposal = actions::stop;

				int to_move = 0;
				int to_block = 0;
				int to_shoot = 0;
				
				if (current_action == actions::shoot && shoot_counter >= 0 && shoot_counter < max_shoot_delay)
				{
					proposal = actions::shoot;
					return proposal;
				}
				if (current_action == actions::block)
				{
					proposal = actions::block;
					return proposal;
				}

				if (x - situation.enemy_ex > 100) to_move++;
				else if (x - situation.enemy_ex <= 100) to_shoot++;
				
				if (situation.enemy_lifes > lifes + 30)
				{
					to_move++;
					to_block++;
				}
				else
				{
					switch (rand() % 2)
					{
					case 0:
						to_shoot++;
						break;

					case 1:
						to_move++;
						break;
					}
				}

				if (situation.now_shooting)to_block++;
				
				if (to_move > to_block && to_move > to_shoot)proposal = actions::move;
				else if (to_block > to_move && to_block > to_shoot)proposal = actions::block;
				else if (to_shoot > to_block && to_shoot > to_move)proposal = actions::shoot;
				else if (to_move == to_block && to_move == to_shoot)
				{
					switch (rand() % 3)
					{
					case 0:
						proposal = actions::shoot;
						break;

					case 1:
						proposal = actions::block;
						break;

					case 2:
						proposal = actions::move;
						break;
					}
				}
				else if (to_move == to_block)
				{
					if (to_shoot > to_move)proposal = actions::shoot;
					else switch (rand() % 2)
					{
					case 0:
						proposal = actions::block;
						break;

					case 1:
						proposal = actions::move;
						break;
					}

				}
				else if (to_move == to_shoot)
				{
					if (to_block > to_move)proposal = actions::block;
					else switch (rand() % 2)
					{
					case 0:
						proposal = actions::shoot;
						break;

					case 1:
						proposal = actions::move;
						break;
					}

				}
				else if (to_block == to_shoot)
				{
					if (to_move > to_block)proposal = actions::move;
					else switch (rand() % 2)
					{
					case 0:
						proposal = actions::shoot;
						break;

					case 1:
						proposal = actions::block;
						break;
					}

				}

				current_action = proposal;
				return proposal;
			}
	};

	class ENGINE_API HERO :public ATOM
	{
		private:
			int max_block_delay = 100;
			int block_count = 0;

		public:
			actions current_action = actions::stop;
			int lifes = 220;

			HERO(float start_x) :ATOM(start_x, scr_height - 180.0f, 65.0f, 80.0f){}

			void Release()
			{
				delete this;
			}
			void Move(bool forward)
			{
				if(forward)x += 10.0f;
				else
				{
					if (x - 10.0f >= 0)x -= 10.0f;
				}
				SetEdges();
			}
			bool Block()
			{
				current_action = actions::block;
				block_count++;
				if (block_count >= max_block_delay)
				{
					block_count = 0;
					current_action = actions::stop;
					return false;
				}
				return true;
			}
			
	};
	
	class ENGINE_API SHOT :public ATOM
	{
		private:
			types type = types::no_type;
			float speed = 0;
			float slope = 0;
			float intercept = 0;
			float dest_x = 0;
			float dest_y = 0;
			float x_jump_modifier = 0;
			bool go_up = true;
			
		public:

			SHOT(types what_type, float start_x, float start_y, float end_x = 0, float end_y = 0) :ATOM()
			{
				type = what_type;
				x = start_x;
				y = start_y;

				switch (type)
				{
				case types::hero_shot:
					NewDims(30.0f, 30.0f);
					speed = 3.0f;
					dest_x = end_x;
					dest_y = end_y;
					if (end_x - x != 0)
					{
						slope = (end_y - start_y) / (end_x - start_x);
						intercept = start_y - slope * start_x;
						x_jump_modifier = end_x - start_x;
					}
					break;

				case types::evil_shot:
					NewDims(60.0f, 22.0f);
					speed = 2.0f;
					break;
				}
			}

			void Release()
			{
				delete this;
			}

			bool Move()
			{
				if (type == types::evil_shot)
				{
					x -= speed;
					SetEdges();
					if (ex <= 0)return false;
				}
				else
				{
					if (x == dest_x)
					{
						if (go_up && y <= dest_y)go_up = false;

						if (go_up)y -= speed;
						else y += speed;
						SetEdges();
						if (y > scr_height - 50.0f)return false;
					}
					else if (x < dest_x)
					{
						x += speed;
						y = x * slope + intercept;
						SetEdges();
						if (y > scr_height - 50.0f)return false;
						if (y <= dest_y)
						{
							go_up = false;
							dest_x = x + x_jump_modifier;
							if (dest_x - x != 0)
							{
								slope = (scr_height - 50.0f - y) / (dest_x - x);
								intercept = y - slope * x;
							}
						}
					}
					else if (x > dest_x)
					{
						x -= speed;
						y = x * slope + intercept;
						SetEdges();
						if (y > scr_height - 50.0f)return false;
						if (y <= dest_y)
						{
							go_up = false;
							dest_x = x + x_jump_modifier;
							if (dest_x - x != 0)
							{
								slope = (scr_height - 50.0f - y) / (dest_x - x);
								intercept = y - slope * x;
							}
						}
					}
				}
				return true;
			}
	};

	typedef ATOM* Atom;
	typedef CREATURE* Creature;
	typedef HERO* Hero;
	typedef SHOT* Shot;

	extern ENGINE_API void* CreatureFactory(types what, float first_x);
	extern ENGINE_API Shot ShotFactory(types what, float start_x, float start_y, float end_x = 0, float end_y = 0);
}