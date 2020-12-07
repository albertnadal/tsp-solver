/*****************************************************************
[ Travelling Salesman Problem Solver using Genetic Programming ]

gcc -O3 -std=c99 `pkg-config --libs --cflags raylib` tsp.c -o tsp

./tsp < mapa.txt

Author: Albert Nadal Garriga
Date: 13-10-2002
Update: 06-12-2020

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <raylib.h>

#define GENERATIONS 500000
#define MAX_SOLUTIONS 30
#define UNDEFINED (UINT32_MAX-1)
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 700

typedef struct City {
    int x;
    int y;
} City;

typedef struct Solutions {
    uint32_t *solution[MAX_SOLUTIONS];
    uint32_t score[MAX_SOLUTIONS];
} Solutions;

typedef struct WorldData {
    int total_cities;
    City *cities;
} WorldData;

void free_mem_solutions(Solutions *solutions) {
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        free(solutions->solution[i]);
    }
}

void free_mem(WorldData *data, Solutions *solutions_group_a, Solutions *inheritors_group_a, Solutions *solutions_group_b, Solutions *inheritors_group_b) {
    free(data->cities);
    free_mem_solutions(solutions_group_a);
    free_mem_solutions(inheritors_group_a);
    free_mem_solutions(solutions_group_b);
    free_mem_solutions(inheritors_group_b);
}

void load_data(WorldData *data) {
    printf("How many cities are in the map? ");
    scanf("%d", &data->total_cities);

    data->cities = (City *) malloc(data->total_cities * sizeof(City));
    for (int i = 0; i < data->total_cities; i++) {
        printf("\nCoordinate X of city %d:", i);
        scanf("%d", &data->cities[i].x);
        printf("\nCoordinate Y of city %d:", i);
        scanf("%d", &data->cities[i].y);
    }
}

void init_solutions(int total_cities, Solutions *solutions, Solutions *solutions_inheritors) {
    // Allocate memory for all solutions
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        solutions->solution[i] = (uint32_t *) malloc(total_cities * sizeof(uint32_t));
        solutions->score[i] = 0;
    }

    // Initialize solutions with cities indexes sequentially
    for (uint32_t i = 0; i < total_cities; i++) {
        for (int j = 0; j < MAX_SOLUTIONS; j++) {
            solutions->solution[j][i] = i;
        }
    }

    // Shuffle solutions
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        for (int j = 0; j < total_cities; j++) {
            int pos1 = rand() % (total_cities); // NOLINT(cert-msc30-c, cert-msc50-cpp)
            int pos2 = rand() % (total_cities); // NOLINT(cert-msc30-c, cert-msc50-cpp)
            uint32_t tmp = solutions->solution[i][pos1];
            solutions->solution[i][pos1] = solutions->solution[i][pos2];
            solutions->solution[i][pos2] = tmp;
        }
    }

    // Allocate memory for all solutions inheritors
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        solutions_inheritors->solution[i] = (uint32_t *) malloc(total_cities * sizeof(uint32_t));
        solutions_inheritors->score[i] = 0;
    }
}

uint32_t get_distance(int city_index_a, int city_index_b, WorldData *data) {
    return (uint32_t) floor(sqrt(pow((data->cities[city_index_a].x) - (data->cities[city_index_b].x), 2) +
                                 pow((data->cities[city_index_a].y) - (data->cities[city_index_b].y), 2)));
}

int get_city_index_in_solution(int city_index, const uint32_t *solution) {
    int i = 0;
    while (1) {
        if (solution[i] == city_index) {
            return i;
        } else i++;
    }
}

int pos(int x, int total_cities) {
    if (x < 0)
        return (total_cities + x);
    else
        return (x % total_cities);
}

bool are_equal(const uint32_t *solution_a, uint32_t *solution_b, int total_cities) {
    bool end = false;
    int i = 0, e = solution_a[0], direction; // direction: 1 = left, 0 = right
    int d = get_city_index_in_solution(e, solution_b);

    if (solution_b[pos(d + 1, total_cities)] == solution_a[pos(1, total_cities)]) direction = 0;
    else if (solution_b[pos(d - 1, total_cities)] == solution_a[pos(-1, total_cities)]) direction = 1;
    else return false;

    if (direction == 0) {
        while (!end) {
            d++;
            i++;
            if (solution_b[pos(d, total_cities)] != solution_a[pos(i, total_cities)]) return false;
            if (pos(i, total_cities) == 0) end = true;
        }
    } else {
        while (!end) {
            d--;
            i--;
            if (solution_b[pos(d, total_cities)] != solution_a[pos(i, total_cities)]) return false;
            if (pos(i, total_cities) == 0) end = true;
        }
    }

    return true;
}

void evaluate_solutions(Solutions *solutions, WorldData *data) {
    int d;
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        solutions->score[i] = 0;
        for (int j = 1; j < data->total_cities; j++) {
            d = get_distance(solutions->solution[i][j - 1], solutions->solution[i][j], data);
            solutions->score[i] = solutions->score[i] + d;
        }
        d = get_distance(solutions->solution[i][0], solutions->solution[i][data->total_cities - 1], data);
        solutions->score[i] = solutions->score[i] + d;
    }
}

void sort_solutions(Solutions *solutions) {
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        for (int j = 0; j < MAX_SOLUTIONS; j++) {
            if (solutions->score[j] > solutions->score[i]) {
                uint32_t *tmp_solution = solutions->solution[i];
                solutions->solution[i] = solutions->solution[j];
                solutions->solution[j] = tmp_solution;

                uint32_t tmp_score = solutions->score[i];
                solutions->score[i] = solutions->score[j];
                solutions->score[j] = tmp_score;
            }
        }
    }
}

uint32_t get_repeated_city(uint32_t *neighbours) {
    int i, e = 0;
    while (neighbours[e] != UNDEFINED) {
        i = e + 1;
        while (neighbours[i] != UNDEFINED) {
            if (neighbours[e] == neighbours[i])
                return neighbours[e];
            i++;
        }
        e++;
    }
    return UNDEFINED;
}

uint32_t get_best_neighbour(int city_index, uint32_t *neighbours, WorldData *data) {
    uint32_t distance, n, best_neighbour = UNDEFINED, best_distance = UINT32_MAX;
    int i = 0;

    if (neighbours[0] == UNDEFINED)
        return UNDEFINED;

    n = get_repeated_city(neighbours);

    if (n != UNDEFINED)
        return n;
    else {
        while (neighbours[i] != UNDEFINED) {
            distance = get_distance(city_index, neighbours[i], data);
            if (distance < best_distance) {
                best_neighbour = neighbours[i];
                best_distance = distance;
            }
            i++;
        }
        return best_neighbour;
    }
}

void fill_with_unused_cities(uint32_t *neighbours, const bool *cities_used, int total_cities) {
    int p = 0;
    for (uint32_t i = 0; i < total_cities; i++) {
        if (cities_used[i] == false) {
            neighbours[p] = i;
            p++;
        }
    }
    neighbours[p] = UNDEFINED;
}

void get_neighbours(int x, uint32_t *father, uint32_t *mother, bool *cities_used, int total_cities, uint32_t *veinat, int e) {
    int p, i = 0;

    if (!cities_used[father[pos(x - 1, total_cities)]]) {
        veinat[i] = father[pos(x - 1, total_cities)];
        i++;
    }

    if (!cities_used[father[pos(x + 1, total_cities)]]) {
        veinat[i] = father[pos(x + 1, total_cities)];
        i++;
    }

    p = get_city_index_in_solution(father[x], mother);

    if (!cities_used[mother[pos(p - 1, total_cities)]]) {
        veinat[i] = mother[pos(p - 1, total_cities)];
        i++;
    }

    if (!cities_used[mother[pos(p + 1, total_cities)]]) {
        veinat[i] = mother[pos(p + 1, total_cities)];
        i++;
    }

    veinat[i] = UNDEFINED;  //aquesta �s la marca de final

    if ((veinat[0] == UNDEFINED) && (e < total_cities))
        fill_with_unused_cities(veinat, cities_used, total_cities);
}

void mutate_solution(uint32_t *solution, int total_cities) {
    int x, y;
    uint32_t tmp;
    div_t e = div(total_cities, 10); // Mutate every 10 cities

    for (int i = e.quot; i > 0; i--) {
        y = rand() % (total_cities); // NOLINT(cert-msc30-c, cert-msc50-cpp)
        x = rand() % (total_cities); // NOLINT(cert-msc30-c, cert-msc50-cpp)
        tmp = solution[x];
        solution[x] = solution[y];
        solution[y] = tmp;
    }
}

void crossover_and_mutation(Solutions *solutions, Solutions *inheritors, WorldData *data) {
    for (int i = 0; i < MAX_SOLUTIONS ; i++) {
        int x = 0, pos = 0, total_cities = data->total_cities;
        bool cities_used[total_cities], end = false;
        memset(cities_used, 0, total_cities);  // Mark all cities as non used yet

        inheritors->solution[i][pos] = solutions->solution[i][x];
        pos++;
        cities_used[solutions->solution[i][x]] = true;
        uint32_t neighbours[total_cities + 1];

        while ((pos < total_cities) && !end) {
            get_neighbours(x, solutions->solution[i], solutions->solution[(i + 1)%MAX_SOLUTIONS], cities_used, total_cities, neighbours, pos);

            if (neighbours[0] != UNDEFINED) {
                inheritors->solution[i][pos] = get_best_neighbour(solutions->solution[i][x], neighbours, data);
                cities_used[inheritors->solution[i][pos]] = true;
                x = get_city_index_in_solution(inheritors->solution[i][pos], solutions->solution[i]);
                pos++;
            } else {
                // If there are no neighbours available then fill the rest of the inheritor solution with the rest of available cities
                int p = pos;
                for (int j = 0; (j < total_cities) && (p < total_cities); j++) {
                    if (cities_used[j] == false) {
                        inheritors->solution[i][p] = cities_used[j];
                        cities_used[j] = true;
                        p++;
                    }
                }
                end = true;
            }
        }

        if (are_equal(solutions->solution[i], inheritors->solution[i], total_cities))
            mutate_solution(inheritors->solution[i], total_cities);
    }
}

void print_solutions(Solutions *solutions, int total_cities) {
    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        for (int e = 0; e < total_cities; e++) {
            printf("%d ", solutions->solution[i][e]);
        }

        printf("-> %d%s\n", solutions->score[i], i == 0 ? " <- BEST SOLUTION" : "");
    }
}

void copy_solution(uint32_t *target, const uint32_t *source, int total_cities) {
    for (int e = 0; e < total_cities; e++) {
        target[e] = source[e];
    }
}

void shift_solutions(Solutions *pares, int i, int total_cities) {
    if (i == MAX_SOLUTIONS - 1) {}
    else {
        for (int e = MAX_SOLUTIONS - 2; e >= i; e--) {
            copy_solution(pares->solution[e + 1], pares->solution[e], total_cities);
            pares->score[e + 1] = pares->score[e];
        }
    }
}

void selection(Solutions *parent_solutions, Solutions *inheritors, int total_cities) {
    for (int i = 1; i < MAX_SOLUTIONS; i++) {
        uint32_t *solution = inheritors->solution[i];
        uint32_t solution_score = inheritors->score[i];
        bool inserted = false;
        int j = 0;

        while (!inserted && (j < MAX_SOLUTIONS)) {
            if (parent_solutions->score[j] > solution_score) {
                shift_solutions(parent_solutions, j, total_cities);
                copy_solution(parent_solutions->solution[j], solution, total_cities);
                parent_solutions->score[j] = solution_score;
                inserted = true;
            } else if (parent_solutions->score[j] == solution_score) {
                if (are_equal(parent_solutions->solution[j], solution, total_cities)) inserted = true;
                else j++;
            } else j++;
        }
    }
}

void shuffle_solutions(Solutions *solutions, int total_cities) {
    uint32_t aux;
    int pos1, pos2;

    for (int i = 0; i < MAX_SOLUTIONS; i++) {
        for (int j = 0; j < total_cities; j++) {
            pos1 = rand() % (total_cities); // NOLINT(cert-msc30-c, cert-msc50-cpp)
            pos2 = rand() % (total_cities); // NOLINT(cert-msc30-c, cert-msc50-cpp)
            aux = solutions->solution[i][pos1];
            solutions->solution[i][pos1] = solutions->solution[i][pos2];
            solutions->solution[i][pos2] = aux;
        }
    }
}

void draw(const uint32_t best_solution[], uint32_t score, RenderTexture2D *render_texture, float scale, WorldData *data) {
    ClearBackground(GREEN);
    BeginDrawing();
    BeginTextureMode(*render_texture);
    ClearBackground(RAYWHITE);
    for (int i = 0; i < data->total_cities; i++) {
        int current_city_index = best_solution[pos(i, data->total_cities)];
        int next_city_index = best_solution[pos(i + 1, data->total_cities)];
        DrawLine(data->cities[current_city_index].x * scale, data->cities[current_city_index].y * scale,
                 data->cities[next_city_index].x * scale, data->cities[next_city_index].y * scale, DARKBLUE);
    }

    for (int i = 0; i < data->total_cities; i++) {
        DrawCircle((int)(data->cities[i].x * scale), (int)(data->cities[i].y * scale), 4, DARKBLUE);
    }
    char str_score[40];
    sprintf(str_score, "score: %d", score);
    DrawText(str_score, 20, 20, 20, DARKGRAY);
    EndTextureMode();
    DrawTexturePro(render_texture->texture, (Rectangle) {0, 0, (float)render_texture->texture.width, (float)-render_texture->texture.height}, (Rectangle) {0, 0, 2 * SCREEN_WIDTH, 2 * SCREEN_HEIGHT}, (Vector2) {0.0f, 0.0f}, 0, WHITE);
    EndDrawing();
}

void init_window(const char title[], int width, int height, RenderTexture2D *render_texture, float *scale, WorldData *data) {
    InitWindow(width, height, title);
    //SetTargetFPS(60);
    *render_texture = LoadRenderTexture(width, height);
    int farthest_y = 0, farthest_x = 0;
    for (int i = 0; i < data->total_cities; i++) {
        if (data->cities[i].x > farthest_x)
            farthest_x = data->cities[i].x;
        if (data->cities[i].y > farthest_y)
            farthest_y = data->cities[i].y;
    }
    float scale_x = (float) width / (float) farthest_x;
    float scale_y = (float) height / (float) farthest_y;
    *scale = fminf(scale_x, scale_y);
}

int main(__unused int argc, __unused char *argv[]) {
    WorldData data;
    Solutions solutions_group_a, solutions_group_b;
    Solutions inheritors_group_a, inheritors_group_b;
    uint32_t best_score = UINT32_MAX; // max unsigned int (32bit) value
    const char title[] = "TRAVELLING SALESMAN PROBLEM Solver";

    printf("%s\n", title);
    srand((unsigned) time(NULL));
    load_data(&data);

    RenderTexture2D render_texture;
    float scale;
    init_window(title, SCREEN_WIDTH, SCREEN_HEIGHT, &render_texture, &scale, &data);

    uint32_t temps_inici = (unsigned) time(NULL);
    init_solutions(data.total_cities, &solutions_group_a, &inheritors_group_a);
    evaluate_solutions(&solutions_group_a, &data);
    sort_solutions(&solutions_group_a);

    init_solutions(data.total_cities, &solutions_group_b, &inheritors_group_b);
    evaluate_solutions(&solutions_group_b, &data);
    sort_solutions(&solutions_group_b);

    for (int g = 0; (g < GENERATIONS) && !WindowShouldClose(); g++) {
        crossover_and_mutation(&solutions_group_a, &inheritors_group_a, &data);
        evaluate_solutions(&inheritors_group_a, &data);
        sort_solutions(&inheritors_group_a);
        selection(&solutions_group_a, &inheritors_group_a, data.total_cities);

        crossover_and_mutation(&solutions_group_b, &inheritors_group_b, &data);
        evaluate_solutions(&inheritors_group_b, &data);
        sort_solutions(&inheritors_group_b);
        selection(&solutions_group_b, &inheritors_group_b, data.total_cities);

        if (g % 500 == 0) {
            selection(&solutions_group_a, &solutions_group_b, data.total_cities);
            shuffle_solutions(&solutions_group_b, data.total_cities);
            evaluate_solutions(&solutions_group_b, &data);
            sort_solutions(&solutions_group_b);
            draw(solutions_group_a.solution[0], best_score, &render_texture, scale, &data); // Draw frequently to avoid break the graphics main loop
        }

        if (solutions_group_a.score[0] < best_score) {
            best_score = solutions_group_a.score[0];
            printf("\n[ GENERATION: %d/%d ] [ BEST DISTANCE: %d ] [ TIME: %ds. ]\nSolutions found: \n", g, GENERATIONS, best_score, (unsigned) time(NULL) - temps_inici);
            print_solutions(&solutions_group_a, data.total_cities);
            draw(solutions_group_a.solution[0], best_score, &render_texture, scale, &data);
        }
    }

    free_mem(&data, &solutions_group_a, &inheritors_group_a, &solutions_group_b, &inheritors_group_b);
    UnloadRenderTexture(render_texture);
    CloseWindow();

    return 0;
}