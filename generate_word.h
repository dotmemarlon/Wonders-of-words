#include "raylib.h"
#include "raymath.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

Vector2 direction_arr[3] = {
	{1,0},
	{0,1},
	{0,-1}
};

typedef struct {
	Vector2 direction;
	Vector2 position;
	char* word;
} WordNode;

int GenerateWordNode(int n,int* char_num_p, char* base_chars, WordNode* words) {
	/* Really. This is some of the worst code I have ever done. Don't look into this.
	 * */
	FILE* f = fopen("word.txt", "rb");
	char** strarr = malloc(121341 * sizeof(*strarr));
	int strarr_size =0;
	int res = 0;
	int char_num = *char_num_p;
	while (res != EOF) {
		char str[50];
		res = fscanf(f,"%s", str);
		int str_len = strlen(str);
		strarr[strarr_size] = malloc(str_len + 1);
		strcpy(strarr[strarr_size], str);
		strarr_size++;
	}
	int base_word_id;
	int curr_char_num;
	int base_char_set['Z' - 'A'+1] = {};

init_base_chars:
	base_word_id = rand() % strarr_size;
	words[0].word = strdup(strarr[base_word_id]);

	free(strarr[base_word_id]);
	strarr[base_word_id] = strarr[strarr_size-1];
	strarr[strarr_size-1] = 0;
	strarr_size--;

	curr_char_num = 0;
	for (int i = 0,si = strlen(words[0].word);i < si; ++i) {
		if (curr_char_num >= char_num) {
			free(words[0].word);
			memset(base_char_set, 0, ('Z' - 'A' + 1)*sizeof(*base_char_set));
			goto init_base_chars;
		}
		base_char_set[words[0].word[i] - 'A']++;
		curr_char_num++;
	}

	words[0].direction = direction_arr[rand()%3];
	words[0].position = (Vector2){0,0};

	int** accupied_pos = malloc(sizeof(int*) * n);
	accupied_pos[0] = malloc(sizeof(int) * strlen(words[0].word));
	int* accupied_pos_sizes = malloc(sizeof(int) * n);
	for (int i = 0;i < n; ++i) {
		accupied_pos_sizes[i] = 0;
	}
	int real_size = 1;

	for (int i = 1;i < n; ++i) {
		int prev_id = i-1;
		WordNode prev = words[prev_id];
		int prev_len = strlen(prev.word);
		int didntmet = 0;
		int* avail_pos = 0;
		int avail_pos_size = 0; 
		while (prev_len <= 3) {
dude:
			prev_id--;
			if (prev_id < 0) {didntmet = 1; puts("DIDN'T MET THE REQUIRED N");break;}
			prev = words[prev_id];	
			prev_len = strlen(prev.word);
		}
		if (didntmet) break;
		avail_pos = malloc(sizeof(*avail_pos) * prev_len);
		for (int j = 0;j < prev_len; ++j) avail_pos[j] = 1;
		avail_pos_size = 0; 
		for (int j = 0;j < accupied_pos_sizes[prev_id]; ++j) {
			avail_pos[accupied_pos[prev_id][j]] = 0;
			if (accupied_pos[prev_id][j]-1 >= 0) avail_pos[accupied_pos[prev_id][j]-1] = 0;
			if (accupied_pos[prev_id][j]+1 < strlen(prev.word)) avail_pos[accupied_pos[prev_id][j]+1] = 0;
		}
		for (int j = 0;j < prev_len; ++j) {
			if (avail_pos[j] ==1) avail_pos_size++;
		}
		if (avail_pos_size ==0) {
			free(avail_pos);
			goto dude;
		}
		int* avail_arr = malloc(sizeof(*avail_arr) * avail_pos_size);
		avail_pos_size = 0;
		for (int j = 0;j < prev_len; ++j) {
			if (avail_pos[j] ==1) avail_arr[avail_pos_size++] = j;
		}
		free(avail_pos);

	 	int p1 = avail_arr[rand()% avail_pos_size];
		accupied_pos[prev_id][accupied_pos_sizes[prev_id]++] = p1;
		 
		char randchar = prev.word[p1];
		
		if (prev.direction.x == 1) {
			words[i].direction.x = 0;
			if (rand() % 2) words[i].direction.y = 1;
			else words[i].direction.y = -1;
		} else {
			words[i].direction.x = 1;
			words[i].direction.y = 0;
		}
		words[i].position.x = prev.position.x + (prev.direction.x * p1);
		words[i].position.y = prev.position.y + (prev.direction.y * p1);

		printf("%d\n", i);

		for (int j = 0;j < strarr_size; ++j) {
			int passing_count = char_num - curr_char_num;
			int base_char_set_tmp['Z'-'A'+1] = {};
			memcpy(base_char_set_tmp, base_char_set,('Z'-'A'+1) * sizeof(*base_char_set_tmp));

			for (int k = 0, si = strlen(strarr[j]);k < si;++k) {
				int isin = 0;

				if (base_char_set_tmp[strarr[j][k] - 'A'] >= 1) {
					isin=1;
					base_char_set_tmp[strarr[j][k] - 'A']--;
				}
				if (!isin) {
					passing_count--;
				}
			}
			if (passing_count <0) {
				free(strarr[j]);
				strarr[j] = strarr[strarr_size-1];
				strarr[strarr_size-1] = 0;
				strarr_size--;
				j--;
			}
		}

		printf("2.%d\n", i);
		char** word_candidates = malloc(sizeof(*word_candidates));
		int word_candidates_capacity = 1;
		int word_candidate_size = 0;

		for (int j = 0;j < strarr_size; ++j) {
			char* char_ptr = strchr(strarr[j],randchar);
			if (char_ptr) {
				if (word_candidate_size >= word_candidates_capacity) {
					word_candidates_capacity*=2;
					word_candidates = realloc(word_candidates, sizeof(*word_candidates) * word_candidates_capacity);
				}
				word_candidates[word_candidate_size++] = strarr[j];
			}
		}
		if (prev.direction.x == 1) {
			for (int j = 0;j < word_candidate_size; ++j) {
				char* char_ptr = strchr(word_candidates[j],randchar);
				int p2 = char_ptr - word_candidates[j];
				Vector2 temp = words[i].position;
				int k = 0;
				int sk = strlen(word_candidates[j]);
				int alright = 1;
				while (k < sk) {
					int passed = 1;
					for (int l = 0;l < i; ++l) {
						if (k+1 == sk && words[l].position.y == temp.y + words[i].direction.y * (k+1)){
							if (temp.x >= words[l].position.x && temp.x <= words[l].position.x + strlen(words[l].word) - 1) {
								passed = 0;
								break;
							}
						}
						if (words[l].position.y == temp.y + words[i].direction.y * k) {
							if (temp.x >= words[l].position.x && temp.x <= words[l].position.x + strlen(words[l].word) - 1) {
								if (word_candidates[j][k] != words[l].word[(int)(temp.x - prev.position.x)]) {
									passed = 0;
									break;
								}
							}
						}
					}
					if (!passed) {
						alright = 0;
						break;
					}
					k++;
				}
				if (alright == 0) {
					word_candidates[j] = word_candidates[word_candidate_size-1];
					word_candidates[word_candidate_size-1] =0;
					word_candidate_size--;
					j--;
				}
			}
		}
		if (word_candidate_size == 0) {
			free(word_candidates);
			break;
		}

		int word_i_choose_you = rand()% word_candidate_size;
		char* char_ptr = strchr(word_candidates[word_i_choose_you],randchar);
		int p2 = char_ptr - word_candidates[word_i_choose_you];

		words[i].word = strdup(word_candidates[word_i_choose_you]);
		int base_char_set_tmp['Z'-'A'+1] = {};
		for (int j = 0,sj = strlen(words[i].word);j < sj; ++j) {
			base_char_set_tmp[words[i].word[j] - 'A']++;
		}
		for (int j = 0;j < 'Z'-'A'+1; ++j) {
			if (base_char_set_tmp[j] > base_char_set[j]) {
				curr_char_num += base_char_set_tmp[j] - base_char_set[j];
				base_char_set[j] = base_char_set_tmp[j];
			}
		}
		

		words[i].position.x -= (words[i].direction.x * p2);
		words[i].position.y -= (words[i].direction.y * p2);

		puts("BRUH1");
		accupied_pos[i] = malloc(sizeof(int) * strlen(words[i].word));
		printf("%d\n", i);
		accupied_pos[i][accupied_pos_sizes[i]++] = p2;
		puts("BRUH3");
		free(word_candidates);
		puts("BRUH4");
		real_size++;
	}

	for (int i = 0;i < real_size; ++i) {
		free(accupied_pos[i]);
	}
	free(accupied_pos);
	free(accupied_pos_sizes);
	for (int i = 0;i < strarr_size; ++i) {
		free(strarr[i]);
	}
	free(strarr);
	fclose(f);

	{
		int i = 0;
		for (int j = 0;j < 'Z'-'A'+1;++j) {
			if (base_char_set[j] > 0) {
				base_chars[i++] = j + 'A';
				base_char_set[j]--;
				j--;
			}
		}
	}

	*char_num_p = curr_char_num;
	return real_size;
}
