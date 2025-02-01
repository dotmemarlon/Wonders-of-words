#include "raylib.h"
#include "raymath.h"
#include "generate_word.h"
#include "math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 1280
#define HEIGHT 720


typedef struct  {
	int x;
	int y;
} Index2D;

int main() {
	srand(time(0));
  SetConfigFlags(FLAG_VSYNC_HINT);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(WIDTH, HEIGHT, "WONDERS OF WORDS");
  SetTargetFPS(60);

	int wordnodes_size = 7;
	int char_num = 6;
	WordNode* wordnodes = malloc(sizeof(*wordnodes) * wordnodes_size);
	char *base_chars = malloc(char_num);
	wordnodes_size = GenerateWordNode(wordnodes_size,&char_num, base_chars, wordnodes);

	int total_char_num = 0;
	for (int i = 0;i < wordnodes_size; ++i) {
		total_char_num += strlen(wordnodes[i].word);
	}

	int half_total_char_num = total_char_num;
	if (half_total_char_num == 0) half_total_char_num = 1;
	int unhided_char_num = rand() % (half_total_char_num);

	float wordnodes_height = (float)HEIGHT / 2;
	

	Vector2 offset;
	float grid_size = 40;
	Vector2 lowest = wordnodes[0].position;
	Vector2 highest = wordnodes[0].position;

	for (int i = 0;i < wordnodes_size; ++i) {
		int j = strlen(wordnodes[i].word);
		Vector2 endPos = Vector2Add(wordnodes[i].position, Vector2Scale(wordnodes[i].direction, (float)j));
		lowest.x = fminf(lowest.x, fminf( endPos.x, wordnodes[i].position.x));
		lowest.y = fminf(lowest.y, fminf( endPos.y, wordnodes[i].position.y));

		highest.x = fmaxf(highest.x, fmaxf( endPos.x, wordnodes[i].position.x));
		highest.y = fmaxf(highest.y, fmaxf( endPos.y, wordnodes[i].position.y));
	}

	for (int i = 0;i < wordnodes_size; ++i) {
		wordnodes[i].position.x -= lowest.x;
		wordnodes[i].position.y -= lowest.y;
	}
	highest.x -= lowest.x;
	highest.y -= lowest.y;
	lowest.x -= lowest.x;
	lowest.y -= lowest.y;



	int** hided_characters = MemAlloc(sizeof(*hided_characters) * ((int)highest.y+1));
	for (int i = 0;i <= (int)highest.y; ++i) {
		hided_characters[i] = MemAlloc(sizeof(int) * ((int)highest.x+1));
		for (int j = 0; j <= (int)highest.x; ++j) {
			hided_characters[i][j] = 0;
		}
	}
	for (int i = 0;i < wordnodes_size; ++i) {
		Vector2 pos = wordnodes[i].position;
		for (int j = 0,sj = strlen(wordnodes[i].word); j < sj; ++j) {
			hided_characters[(int)pos.y][(int)pos.x] = 1;
			pos = Vector2Add(pos, wordnodes[i].direction);
		}
	}
	Index2D* char_indexes = MemAlloc(sizeof(*char_indexes) * total_char_num);
	int char_indexes_size = 0;

	for (int i = 0;i <= (int)highest.y; ++i) {
		for (int j = 0; j <= (int)highest.x; ++j) {
			if (hided_characters[i][j]) {
				char_indexes[char_indexes_size].x = j;
				char_indexes[char_indexes_size++].y = i;
			}
		}
	}

	for (int i = 0;i < unhided_char_num; ++i) {
		int rnum = rand() % char_indexes_size;
		hided_characters[char_indexes[rnum].y][char_indexes[rnum].x] = 0;
		char_indexes[rnum] = char_indexes[char_indexes_size-1];
		char_indexes_size--;
	}

	MemFree(char_indexes);

	grid_size = wordnodes_height / (highest.y - lowest.y + 1);

	
	offset.x = ((float)WIDTH - (highest.x - lowest.x) * grid_size) / 2;
	offset.y = ((float)wordnodes_height - (highest.y - lowest.y) * grid_size) / 2;

	Vector2 char_circle_pos = {(float)WIDTH/2, ((float)HEIGHT + wordnodes_height)/2};
	float char_circle_radius = (float)HEIGHT / 2  / 3;

	float char_circle_omega = PI * 2 / char_num;
	float char_size = char_circle_radius * 2/char_num;
	Vector2* base_char_position = MemAlloc(sizeof(*base_char_position) * char_num);
	Vector2* base_char_real_size = MemAlloc(sizeof(*base_char_real_size) * char_num);
	Font ComicSans_font = LoadFontEx("./DejaVuSans.ttf",grid_size, 0,0);
	Font Circle_font = LoadFontEx("./DejaVuSans.ttf", char_size, 0, 0);
	for (int i = 0; i < char_num; i++) {
		char g_text[2] = "";
		g_text[0] = base_chars[i];
		base_char_real_size[i] = MeasureTextEx(ComicSans_font,g_text, char_size,0);
		base_char_position[i].x = char_circle_pos.x - sinf(char_circle_omega * i) * char_circle_radius * 3 / 4 - base_char_real_size[i].x/2;
		base_char_position[i].y = char_circle_pos.y - cosf(char_circle_omega * i) * char_circle_radius * 3 / 4 - base_char_real_size[i].y/2;
	}

	for (int i = 0;i < wordnodes_size; ++i) {
		printf("%d. %s\n",i, wordnodes[i].word);
	}
	for (int i = 0;i < char_num; ++i) {
		printf("%d ", base_chars[i]);
	}
	puts("");

	Texture background_txt = LoadTexture("./background/background.jpg");

	Rectangle background_src = {0,0,background_txt.width, background_txt.height};

	Rectangle background_dest = {};
	background_dest.height = HEIGHT;
	background_dest.width = (float)background_txt.width * background_txt.height / HEIGHT;
	background_dest.x = ((float)WIDTH - background_dest.width) / 2;
	background_dest.y = 0;

	Vector2* line_drawed = MemAlloc(sizeof(*line_drawed) * char_num);
	int line_drawed_size = 0;

	int* line_marked = MemAlloc(sizeof(*line_marked) * char_num);
	for (int i = 0;i < char_num; ++i) {
		line_marked[i] = 0;
	}

	char str_marked[1024] = {};
	int str_marked_size = 0;

	
	int is_mouse_holding = 0;
	puts("ADASDAD");

	while (!WindowShouldClose()) {
		if ( (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !is_mouse_holding) || is_mouse_holding) {
			Vector2 mouse_pos = GetMousePosition();
			for (int i = 0;i < char_num;++i) {
				if (mouse_pos.x >= base_char_position[i].x && mouse_pos.x <= base_char_position[i].x + char_size &&
						mouse_pos.y >= base_char_position[i].y && mouse_pos.y <= base_char_position[i].y + char_size) {
					if (line_marked[i]) continue;
					line_drawed[line_drawed_size].x = base_char_position[i].x + base_char_real_size[i].x /2;
					line_drawed[line_drawed_size].y = base_char_position[i].y + base_char_real_size[i].y /2;
					line_drawed_size++;
					line_marked[i] = 1;
					is_mouse_holding = 1;
					str_marked[str_marked_size++] = base_chars[i];
					break;
				}
			}
		}  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			is_mouse_holding = 0;
			line_drawed_size = 0;
			for (int i = 0;i < char_num; ++i) {
				line_marked[i] = 0;
			}
			str_marked[str_marked_size++] = 0;
			str_marked_size = 0;
			printf("%s\n", str_marked);
			for (int i = 0;i < wordnodes_size; ++i) {
				if (strcmp(str_marked,wordnodes[i].word) == 0) {
					Vector2 pos = wordnodes[i].position;
					for (int j = 0,sj = strlen(wordnodes[i].word); j <sj; ++j) {
						hided_characters[(int)pos.y][(int)pos.x] = 0;
						pos = Vector2Add(pos, wordnodes[i].direction);
					}
					break;
				}
			}
		}
		BeginDrawing();
			ClearBackground(GetColor(0x1f1f1fff));
			DrawTexturePro(background_txt, background_src, background_dest, Vector2Zero(), 0, WHITE);
			Vector2 draw_low_high = Vector2Add(offset, Vector2Scale(lowest, grid_size));
			DrawRectangleLines(draw_low_high.x, draw_low_high.y, (highest.x - lowest.x + 1) * grid_size, (highest.y - lowest.y + 1) * grid_size, WHITE);
			DrawCircleV(char_circle_pos, char_circle_radius, (Color){0xff,0xff,0xff,225});
			for (int i = 0;i < char_num; i++) {
				char g_text[2] = "";
				g_text[0] = base_chars[i];
				DrawTextEx(Circle_font,  g_text, base_char_position[i], char_size, 0.0, BLACK);
			}
			for (int i = 0;i < wordnodes_size; ++i) {
				Vector2 world_position = wordnodes[i].position;
				for (int j = 0;wordnodes[i].word[j]!='\0';++j) {
					int rect_size = grid_size - 5;
					Vector2 draw_position = Vector2Add(Vector2Scale(world_position, grid_size), offset);
					Rectangle rect = {draw_position.x, draw_position.y, rect_size, rect_size};
					DrawRectangleRounded(rect, 0.3f, 4, GetColor(0x1f1fefff));
					if (hided_characters[(int)world_position.y][(int)world_position.x]) {
						world_position.x += wordnodes[i].direction.x;
						world_position.y += wordnodes[i].direction.y;
						continue;
					}
					world_position.x += wordnodes[i].direction.x;
					world_position.y += wordnodes[i].direction.y;
					char g_char[2] = "";
					g_char[0] = wordnodes[i].word[j];
					Vector2 text_position = draw_position;
					text_position.x = draw_position.x + (float)(rect_size - MeasureText(g_char, rect_size))/2;

					DrawTextEx(ComicSans_font, g_char, text_position, rect_size, 0, WHITE);
				}
			}
			{
				int i = 0;
				for (i = 0;i < line_drawed_size-1; ++i) {
					DrawLineEx(line_drawed[i], line_drawed[i+1],10.f, RED);
				}
				if (i < line_drawed_size) {
					DrawLineEx(line_drawed[i], GetMousePosition(),10.f, RED);
				}
			}
		EndDrawing();
	}

	for (int i = 0;i < wordnodes_size;++i) {
		MemFree(wordnodes[i].word);
	}
	for (int i = 0;i < (int)highest.y; ++i) {
		MemFree(hided_characters[i]);
	}
	MemFree(hided_characters);
	MemFree(base_char_real_size);
	MemFree(base_char_position);
	free(base_chars);
	UnloadFont(ComicSans_font);
	MemFree(wordnodes);
	CloseWindow();

	return 0;
}
