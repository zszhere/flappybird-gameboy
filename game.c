#include <gb/gb.h>
#include <stdio.h>
#include <time.h>
#include <gb/drawing.h>
#include <rand.h>
#include <types.h>

#include "tiledata.h"

const UINT8 pipeGap = 8;

int abs(int num) {
  if(num < 0)
    return -num;
  else
    return num;
}


typedef struct GameSpriteObject_ {
  UBYTE width;
  UBYTE height;
  UBYTE *tileDataPointer;
  UBYTE firstTileNum;
  UBYTE *lastFreeTilePointer;
} GameSpriteObject;

void initGso(GameSpriteObject *gso) {
  UBYTE i;
  UBYTE tileSum;
  UBYTE lastFreeTileValue;

  lastFreeTileValue = *(gso->lastFreeTilePointer);
  tileSum = gso->width * gso->height;

  set_sprite_data(lastFreeTileValue, tileSum, gso->tileDataPointer);
  // Refresh lastFreeTile value
  *(gso->lastFreeTilePointer) += tileSum;
  // Link tiles and sprites
  for (i = lastFreeTileValue; i < lastFreeTileValue + tileSum; i++) {
    set_sprite_tile(i, i);
  }
}

void newGso(GameSpriteObject *gsoPointer,
            UINT8 width,
            UINT8 height,
            UINT8 *tileDataPointer,
            UINT8 *lastFreeTilePointer) {
  // Without this lines some params === 0 ¯\_(ツ)_/¯
  gsoPointer->height;
  gsoPointer->tileDataPointer;

  gsoPointer->width = width;
  gsoPointer->height = height;
  gsoPointer->tileDataPointer = tileDataPointer;
  gsoPointer->firstTileNum = *lastFreeTilePointer;
  gsoPointer->lastFreeTilePointer = lastFreeTilePointer;

  initGso(gsoPointer);
}

// Work correctly with 8*8 sprites
void moveGso(GameSpriteObject *gso, UINT8 x, UINT8 y) {
  UBYTE i, j, c;
  c = gso->firstTileNum;

  for (i = 0; i < gso->height; ++i) {
    for (j = 0; j < gso->width; ++j) {
      move_sprite(c++, x + 8 * j, y + 8 * i);
    }
  }
}

// TODO: increase render speed by using line-by-line drawing (vertical)
void drawPipe(UINT8 x, INT8 level) {
  const UINT8 pipeWidth = 5;
  const INT8 capHeight = 3;
  INT8 i, j;
  UINT8 pipeTiles[18 * 5];
  UINT8 tileNum = 0;

  // printf("%d\n", level);

  for (j = 0; j != 18; j++) {
    for (i = 0; i != pipeWidth; i++) {
      tileNum = 0;
      if (j + capHeight <= level || j > level + pipeGap + capHeight) {
        // Draw pipe body
        if (i == 0) {
          tileNum = 1;
        } else if (i == 1 || i == 2) {
          tileNum = 2;
        } else if (i == 3) {
          tileNum = 3;
        } else if (i == 4) {
          tileNum = 4;
        }
        // printf("b%d\n", (UWORD)j);
      } else if (j <= level && j > level - capHeight) {
        // Draw top pipe cap
        // printf("t%d\n", (UWORD)capHeight);
        if (j + 2 == level) {
          if (i == 0) {
            tileNum = 18;
          } else if (i == 1) {
            tileNum = 19;
          } else if (i == 2) {
            tileNum = 20;
          } else if (i == 3) {
            tileNum = 21;
          } else if (i == 4) {
            tileNum = 22;
          }
        } else if (j + 1 == level) {
          if (i == 0) {
            tileNum = 23;
          } else if (i == 1) {
            tileNum = 24;
          } else if (i == 2) {
            tileNum = 25;
          } else if (i == 3) {
            tileNum = 25;
          } else if (i == 4) {
            tileNum = 26;
          }

        } else if (j == level) {
          if (i == 0) {
            tileNum = 27;
          } else if (i == 1) {
            tileNum = 28;
          } else if (i == 2) {
            tileNum = 28;
          } else if (i == 3) {
            tileNum = 29;
          } else if (i == 4) {
            tileNum = 30;
          }
        }
      } else if (j > level + pipeGap && j <= level + pipeGap + capHeight) {
        // Draw bottom pipe cap
        // printf("l%d\n", (UWORD)j);
        if (j == level + pipeGap + 1) {
          if (i == 0) {
            tileNum = 5;
          } else if (i == 3) {
            tileNum = 7;
          } else if (i == 4) {
            tileNum = 8;
          } else if (i == 1 || i == 2) {
            tileNum = 6;
          }
        } else if (j == level + pipeGap + 2) {
          if (i == 0) {
            tileNum = 9;
          } else if (i == 3) {
            tileNum = 11;
          } else if (i == 4) {
            tileNum = 12;
          } else if (i == 1 || i == 2) {
            tileNum = 10;
          }
        } else if (j == level + pipeGap + 3) {
          if (i == 0) {
            tileNum = 13;
          } else if (i == 1) {
            tileNum = 14;
          } else if (i == 2) {
            tileNum = 15;
          } else if (i == 3) {
            tileNum = 16;
          } else if (i == 4) {
            tileNum = 17;
          }
        }
      }
      pipeTiles[pipeWidth * j + i] = tileNum;
    }
  }
  set_bkg_tiles(x, 0, pipeWidth, 18, pipeTiles);
}

void flushRow(UINT8 row) {
  UINT8 i = 0;
  UINT8 randomBkgTiles[31];
  for (i = 0; i < 32; i++) {
    randomBkgTiles[i] = 0;
  }
  set_bkg_tiles(0, row, 31, 1, randomBkgTiles);
}

void flushBkg() {
  UINT8 i = 0;
  for (i = 0; i < 32; i++) {
    flushRow(i);
  }
}

// int pipeRandomLevel( UINT8 max) {
//   int r = (int) max - min;
//   return (rand() % r) + (int) min;
// }

UBYTE pipeRandomLevel() {
  const UBYTE max = 10;
  UBYTE num;
  num = rand() % max;
  while (num > 200) {
    num = rand() % max;
  }
  return num;
}

UINT8 checkCollision(DWORD y) {
  if (y < 10 || y > GRAPHICS_HEIGHT) {
    return 0;
  } else {
    return 1;
  }
}

UINT8 checkPipeCollision(DWORD y, INT8 level) {
  if (y < level * 8 + flbird_tile_map_height * 8 || y > level * 8 + pipeGap * 8) {
    return 1;
  } else {
    return 0;
  }
}

enum GameState {
  TUTORIAL,
  MAIN
};

void main()
{
  const JUMP_DELAY = 15;
  const DOWNTEMPO_COEFFICIENT = 70;
  const DWORD v0 = 200;
  UBYTE x = 50, j;
  DWORD y = 0;
  DWORD coord = 0, vcoord = 0, dx0 = 150, dx1 = 150;
  UBYTE yd = 50;
  DWORD time_backup = 0;
  DWORD g = 10, gh = 144;
  DWORD delaying = 0;
  UBYTE delay = FALSE;
  UBYTE resume = TRUE;
  UBYTE yi = 0;
  UBYTE t = 0, u = 0;
  UINT8 i = 0;

  INT8 needToCheckPipeCollision = 0;

  UINT8 lastFreeTile = 0;
  UWORD scrollPositionX = 0;
  UWORD startFirstPipeCollisionX;
  UWORD endFirstPipeCollisionX;
  UWORD startSecondPipeCollisionX;
  UWORD endSecondPipeCollisionX;
  UINT8 firstPipeRerenderPoint = 0;
  UINT8 secondPipeRerenderPoint = 128;
  UINT8 screenScrollShift = 4;
  UBYTE randomLevel1;
  UBYTE randomLevel2;
  BOOLEAN resumeGame = TRUE;
  const UINT8 initialTutorialArrowPosition = 176;
  UINT8 tutorialArrowPosition = initialTutorialArrowPosition;
  const UINT8 tutorialArrowShownPosition = 140;
  const int arrowVerticalPosition = 68;

  enum GameState gameState = TUTORIAL;

  UINT8 playerWidth = flbird_tile_map_width;
  UINT8 playerHeight = flbird_tile_map_height;
  UINT8 *playerTileDataPointer = &flbird_tile_data;
  GameSpriteObject player;
  GameSpriteObject arrow;
  GameSpriteObject aButton;

  newGso(&player, playerWidth, playerHeight, playerTileDataPointer, &lastFreeTile);
  newGso(&arrow, arrow_tile_map_width, arrow_tile_map_height, &arrow_tile_data, &lastFreeTile);
  newGso(&aButton, ab_tile_map_width, ab_tile_map_height, &ab_tile_data, &lastFreeTile);

  set_bkg_data(1, flbody_tile_count, flbody_tile_data);
  set_bkg_data(5, flbottomtop_tile_count, flbottomtop_tile_data);
  set_bkg_data(18, fltopbottom_tile_count, fltopbottom_tile_data);

  flushBkg();

  SPRITES_8x8;
  SHOW_BKG;
  SHOW_SPRITES;
  DISPLAY_ON;

  startFirstPipeCollisionX = 10 * 8 - (x + (3 * 4));
  endFirstPipeCollisionX = startFirstPipeCollisionX + 40 + 2 * 8;
  startSecondPipeCollisionX = 255 - 40 - x - 3 * 4;
  endSecondPipeCollisionX = startSecondPipeCollisionX + 40 + 2 * 8;

  moveGso(&arrow, tutorialArrowPosition, arrowVerticalPosition);
  moveGso(&aButton, 168, 80);

  while(resume) {
    // Main loop
    j = joypad();

    if (gameState == TUTORIAL) {
      if (tutorialArrowPosition > tutorialArrowShownPosition) {
        tutorialArrowPosition -= 4;
        moveGso(&arrow, tutorialArrowPosition, arrowVerticalPosition);
        moveGso(&aButton, tutorialArrowPosition - 3, 80);
      }

      if (j & J_A && !delay) {
        time_backup = clock();
        delaying = clock() + JUMP_DELAY;
        delay = TRUE;
        gameState = MAIN;
      }
    }

    // Game loop
    if (gameState == MAIN && resumeGame) {

      if (tutorialArrowPosition < initialTutorialArrowPosition) {
        tutorialArrowPosition += 4;
        moveGso(&arrow, tutorialArrowPosition, arrowVerticalPosition);
        moveGso(&aButton, tutorialArrowPosition - 3, 80);
      }


      move_bkg(scrollPositionX, 0);
      if (scrollPositionX < 255 - screenScrollShift) {
        scrollPositionX += screenScrollShift;
      } else {
        scrollPositionX = 255 - scrollPositionX;
      }


      if (scrollPositionX > firstPipeRerenderPoint && scrollPositionX <= firstPipeRerenderPoint + screenScrollShift) {
        randomLevel1 = pipeRandomLevel();
        drawPipe(27, randomLevel1);
      }
      if (scrollPositionX > secondPipeRerenderPoint && scrollPositionX <= secondPipeRerenderPoint + screenScrollShift) {
        randomLevel2 = pipeRandomLevel();
        drawPipe(10, randomLevel2);
        needToCheckPipeCollision = 1;
      }

      if (j & J_A && !delay) {
        yd = gh - y;
        time_backup = clock();
        delaying = clock() + JUMP_DELAY;
        delay = TRUE;
      }

      if (clock() > delaying)
        delay = FALSE;
      t = clock() - time_backup;
      dx0 = v0 * t / DOWNTEMPO_COEFFICIENT;
      dx1 = g * t * t / DOWNTEMPO_COEFFICIENT / 2;
      coord =  dx0 - dx1;
      vcoord = coord;
      y = yd + vcoord;
      y = gh - y;

      moveGso(&player, x, y);

      if (needToCheckPipeCollision == 0) {
        resume = checkCollision(y);
      } else if (scrollPositionX >= startFirstPipeCollisionX && scrollPositionX <= endFirstPipeCollisionX) {
        if (checkPipeCollision(y, randomLevel2) == 1) {
          resume = 0;
        }
      } else if (scrollPositionX >= startSecondPipeCollisionX && scrollPositionX <= endSecondPipeCollisionX) {
        if (checkPipeCollision(y, randomLevel1) == 1) {
          resume = 0;
        }
      } else {
        resume = checkCollision(y);
      }
    }

    wait_vbl_done();
  }
}