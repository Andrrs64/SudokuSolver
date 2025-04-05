#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ubuf_values[9*9];
int values[9*9] = {
    0, 0, 6,  3, 0, 0,  0, 2, 1,
    0, 5, 2,  0, 0, 9,  6, 0, 0,
    1, 9, 0,  0, 7, 0,  5, 0, 0,

    0, 0, 0,  0, 9, 0,  0, 4, 2,
    0, 7, 8,  4, 2, 0,  0, 0, 0,
    3, 0, 0,  0, 0, 7,  0, 1, 0,

    0, 0, 1,  0, 4, 0,  0, 0, 6,
    0, 4, 0,  0, 0, 6,  0, 0, 0,
    0, 0, 9,  0, 0, 5,  0, 0, 3,
};

int ubuf_potentialValues[9*9][9];
int potentialValues[9*9][9];

int getIndex(int x, int y) {
    return x + (y * 9);
}

void setUndoBuffer() {
    memcpy(ubuf_values, values, sizeof(int)*9*9);

    for (int i = 0; i < 9*9; i++) {
        memcpy(ubuf_potentialValues[i], potentialValues[i], sizeof(int)*9);
    }
}

void undo() {
    memcpy(values, ubuf_values, sizeof(int)*9*9);

    for (int i = 0; i < 9*9; i++) {
        memcpy(potentialValues[i], ubuf_potentialValues[i], sizeof(int)*9);
    }
}

void initialize() {
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {

            int v = values[getIndex(x, y)];

            if (v > 0) {
                continue;
            }

            for (int p = 1; p <= 9; p++) {
                potentialValues[getIndex(x, y)][p-1] = p;
                for (int i = 0; i < 9; i++) {
                    if (
                        (i != x && values[getIndex(i, y)] == p) ||
                        (i != y && values[getIndex(x, i)] == p)
                    ) {
                        potentialValues[getIndex(x, y)][p-1] = 0;
                    }
                }

                int gOffsetX = floorf((float)x / 3.0f) * 3;
                int gOffsetY = floorf((float)y / 3.0f) * 3;
                for (int gx = gOffsetX; gx < gOffsetX + 3; gx++) {
                    for (int gy = gOffsetY; gy < gOffsetY + 3; gy++) {
                        if (
                            (gx != x || gy != y) &&
                            values[getIndex(gx, gy)] == p
                        ) {
                            potentialValues[getIndex(x, y)][p-1] = 0;
                        }
                    }
                }
            }
        }
    }
}

void removePotentialValue(int x, int y, int value) {
    potentialValues[getIndex(x, y)][value-1] = 0;
}

void setValue(int x, int y, int value) {
    values[getIndex(x, y)] = value;
    int gx = floorf((float)x / 3.0f);
    int gy = floorf((float)y / 3.0f);

    for (int i = 0; i < 9; i++) {
        removePotentialValue(i, y, value);
        removePotentialValue(x, i, value);
        removePotentialValue(
            (gx * 3) + (i % 3),
            (gy * 3) + floorf((float)i / 3.0f),
            value
        );
    }
}

bool isOnlyPotentialValue(int x, int y, int value) {
    bool res = true;
    for (int num = 1; num <= 9; num++) {
        if (num != value && potentialValues[getIndex(x, y)][num-1] == num) {
            res = false;
        }
    }

    return res;
}

void solveGroup(int gx, int gy) {
    for (int num = 1; num <= 9; num++) {
        int resX = -1;
        int resY = -1;
        bool sameX = true;
        bool sameY = true;

        for (int x = gx*3; x < gx*3 + 3; x++) {
            for (int y = gy*3; y < gy*3 + 3; y++) {
                if (values[getIndex(x, y)] > 0) {
                    continue;
                }

                if (potentialValues[getIndex(x, y)][num-1] != num) {
                    continue;
                }

                if (isOnlyPotentialValue(x, y, num)) {
                    resX = x;
                    resY = y;
                    setValue(x, y, num);
                    x = gx*3 + 3;
                    y = gy*3 + 3;
                    continue;
                }

                if ((resX == -1 || resX == x) && sameX) {
                    resX = x;
                } else {
                    sameX = false;
                    resX = -1;
                }

                if ((resY == -1 || resY == y) && sameY) {
                    resY = y;
                } else {
                    sameY = false;
                    resY = -1;
                }
            }
        }

        if (resX != -1 && resY != -1) {
            setValue(resX, resY, num);
            continue;
        }

        if (resY != -1) {
            for (int x = 0; x < 9; x++) {
                if (x < gx*3 && x >= gx*3 + 3) {
                    removePotentialValue(x, resY, num);
                }
            }

        }

        if (resX != -1) {
            for (int y = 0; y < 9; y++) {
                if (y < gy*3 || y >= gy*3 + 3) {
                    removePotentialValue(resX, y, num);
                }
            }
        }
    }
}

int main() {
    InitWindow(900, 900, "Sudoku solver");
    initialize();

    SetTargetFPS(60);

    char* stringBuffer = malloc(12);
    if (stringBuffer == NULL) {
        printf("ERROR: Could not allocate string buffer\n");
        return 1;
    }

    bool s_db = false;
    bool u_db = false;
    int gx = 0;
    int gy = 0;

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_SPACE)) {
            if (!s_db) {
                setUndoBuffer();
                solveGroup(gx, gy);
                gx++;
                if (gx == 3) {
                    gx = 0;
                    gy++;
                    if (gy == 3) {
                        gy = 0;
                    }
                }
                s_db = true;
            }
        } else { s_db = false; }

        if (IsKeyDown(KEY_U)) {
            if (!u_db) {
                undo();
                gx--;
                if (gx == -1) {
                    gx = 2;
                    gy--;
                    if (gy == -1) {
                        gy = 2;
                    }
                }
                u_db = true;
            }
        } else { u_db = false; }

        BeginDrawing();

            ClearBackground(WHITE);

            DrawRectangle(
                gx * 300, gy * 300,
                300, 300, YELLOW
            );

            for (int x = 0; x < 9; x++) {
                int xOffset = x * 100;
                int xHalfWidth = 2;

                if (x % 3 == 0) {
                    xHalfWidth *= 2;
                }

                DrawRectangle(
                    xOffset - xHalfWidth, 0,
                    xHalfWidth * 2, 900,
                    BLACK
                );

                for (int y = 0; y < 9; y++) {
                    int yOffset = y * 100;
                    int yHalfWidth = 2;

                    if (y % 3 == 0) {
                        yHalfWidth *= 2;
                    }

                    DrawRectangle(
                        0, yOffset - yHalfWidth,
                        900, yHalfWidth * 2,
                        BLACK
                    );

                    int v = values[getIndex(x, y)];

                    if (v == 0) {
                        for (int i = 0; i < 9; i++) {
                            int pv = potentialValues[getIndex(x, y)][i];
                            if (pv == 0) { continue; }
                            sprintf(stringBuffer, "%d", pv);
                            DrawText(stringBuffer,
                                    (i % 3) * 35 + 5 + (x * 100),
                                    floorf((float)i / 3.0f) * 35 + 5 + (y * 100),
                                    25, BLACK);
                        }

                        continue;
                    }

                    sprintf(stringBuffer, "%d", v);
                    DrawText(stringBuffer,
                            x * 100 + 20, y * 100,
                            90, BLACK);
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
