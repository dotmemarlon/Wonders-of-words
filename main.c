#include "raylib.h"
#include "raymath.h"
#include "generate_word.h"
#include "math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 1280
#define HEIGHT 720


int main() {
  SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(WIDTH, HEIGHT, "WORLD OF WORD");
  SetTargetFPS(60);

	int wordnodes_size = 4;
	WordNode* wordnodes = malloc(sizeof(*wordnodes) * wordnodes_size);
	wordnodes_size = GenerateWordNode(wordnodes_size,6, wordnodes);
	

	Vector2 offset;
	int grid_size = 40;
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

		offset.x = ((float)WIDTH - fabsf(highest.x - lowest.x) * grid_size) / 2;
		offset.y = ((float)HEIGHT - fabsf(highest.y - lowest.y) * grid_size) / 2;

	for (int i = 0;i < wordnodes_size; ++i) {
		printf("%d. %s\n",i, wordnodes[i].word);
	}

	Font ComicSans_font = LoadFont("./DejaVuSans.ttf");

	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(GetColor(0x1f1f1fff));
			DrawRectangleLines(offset.x, offset.y, fabsf(highest.x - lowest.x) * grid_size, fabsf(highest.y - lowest.y) * grid_size, WHITE);
			for (int i = 0;i < wordnodes_size; ++i) {
				Vector2 world_position = wordnodes[i].position;
				for (int j = 0;wordnodes[i].word[j]!='\0';++j) {
					int rect_size = grid_size - 5;
					Vector2 draw_position = Vector2Add(Vector2Scale(world_position, grid_size), offset);
					Rectangle rect = {draw_position.x, draw_position.y, rect_size, rect_size};
					DrawRectangleRounded(rect, 0.3f, 4, GetColor(0x1f1fefff));
					world_position.x += wordnodes[i].direction.x;
					world_position.y += wordnodes[i].direction.y;
					char g_char[2] = "";
					g_char[0] = wordnodes[i].word[j];
					Vector2 text_position = draw_position;
					text_position.x = draw_position.x + (float)(rect_size - MeasureText(g_char, rect_size))/2;

					DrawTextEx(ComicSans_font, g_char, text_position, rect_size, 0, WHITE);
				}
			}
		EndDrawing();
	}

	for (int i = 0;i < wordnodes_size;++i) {
		MemFree(wordnodes[i].word);
	}
	MemFree(wordnodes);
	CloseWindow();

	return 0;
}
