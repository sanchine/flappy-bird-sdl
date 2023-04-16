#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

using namespace std;

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 500;

bool q = false;
float x = 0, y = 0;
float cellLen = 5;
float speed = 0.06;
float mirror = -1;
float cellSpace = 25;
float scale = 0.05;

// game
bool game_isPlaying = false;

int stopperWidth = 35;
int rectSize = 32;

SDL_Rect stopper[4] = { {0 - rectSize, 0, stopperWidth, 50},
    {0 - rectSize, SCREEN_HEIGHT - 150, stopperWidth, 150},
    {SCREEN_WIDTH / 2, 0, stopperWidth, 50},
    {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 150, stopperWidth, 150} };

SDL_Event e;


SDL_Rect rect = { 25, 100, rectSize, rectSize };
int rect_weight = 50;

int spaceThroughStopperByHeight = 128;

bool rect_isFlying = false;
bool rect_isJumping = false;
bool rect_isFiring = false;

int difficult = 1;
int scores = -1;

int lastHeight = 0;

/////////////////////////


SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* back_tex;
SDL_Rect back_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

SDL_Texture* bird_fly;
SDL_Texture* bird_chill;
SDL_Texture* bird_fall;

bool init() {
    bool ok = true;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Can't init SDL: " << SDL_GetError() << endl;
    }

    win = SDL_CreateWindow("Howis?", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        cout << "Can't create window: " << SDL_GetError() << endl;
        ok = false;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL) {
        cout << "Can't create renderer: " << SDL_GetError() << endl;
        ok = false;
    }

    SDL_Surface* tmp = NULL;
    tmp = SDL_LoadBMP("D:\\source\\repos\\ConsoleApplication1\\ConsoleApplication1\\Images\\back.bmp");
    SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(tmp->format, 255, 255, 255));
    back_tex = SDL_CreateTextureFromSurface(ren, tmp);
    if (back_tex == NULL) {
        cout << "Can't create texture: " << SDL_GetError() << endl;
        ok = false;
    }
    SDL_FreeSurface(tmp);

    SDL_Surface* tmp2;
    tmp2 = SDL_LoadBMP("D:\\source\\repos\\ConsoleApplication1\\ConsoleApplication1\\Images\\fly.bmp");
    SDL_SetColorKey(tmp2, SDL_TRUE, SDL_MapRGB(tmp2->format, 255, 255, 255));
    bird_fly = SDL_CreateTextureFromSurface(ren, tmp2);
    if (bird_fly == NULL) {
        cout << "Can't create texture 0: " << SDL_GetError() << endl;
        ok = false;
    }
    SDL_FreeSurface(tmp2);

    SDL_Surface* tmp3 = NULL;
    tmp3 = SDL_LoadBMP("D:\\source\\repos\\ConsoleApplication1\\ConsoleApplication1\\Images\\chill.bmp");
    SDL_SetColorKey(tmp3, SDL_TRUE, SDL_MapRGB(tmp3->format, 255, 255, 255));
    bird_chill = SDL_CreateTextureFromSurface(ren, tmp3);
    if (bird_chill == NULL) {
        cout << "Can't create texture 1: " << SDL_GetError() << endl;
        ok = false;
    }
    SDL_FreeSurface(tmp3);

    SDL_Surface* tmp4 = NULL;
    tmp4 = SDL_LoadBMP("D:\\source\\repos\\ConsoleApplication1\\ConsoleApplication1\\Images\\fall.bmp");
    SDL_SetColorKey(tmp4, SDL_TRUE, SDL_MapRGB(tmp4->format, 255, 255, 255));
    bird_fall = SDL_CreateTextureFromSurface(ren, tmp4);
    if (bird_fall == NULL) {
        cout << "Can't create texture 2: " << SDL_GetError() << endl;
        ok = false;
    }
    SDL_FreeSurface(tmp4);

    return ok;
}

void quit() {

    SDL_DestroyTexture(back_tex);
    SDL_DestroyTexture(bird_fly);
    SDL_DestroyTexture(bird_chill);
    SDL_DestroyTexture(bird_fall);

    SDL_DestroyWindow(win);
    win = NULL;

    SDL_DestroyRenderer(ren);
    ren = NULL;

    SDL_Quit;
}

struct Point {
    float x;
    float y;
    Point(float _x, float _y) {
        x = _x;
        y = _y;
    }
};

struct TableCoord {
    float _x[400];
    float _y[400];
};

Point beginCoord(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
TableCoord graphicTable;


int main(int arhc, char** argv) {
    if (!init()) {
        quit();
        system("pause");
        return 1;
    }

    // чёрный бэк и включение белой краски

    

    //// построение декартовой системы координат

    SDL_RenderDrawLine(ren, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);
    SDL_RenderPresent(ren);

    //// размещение шкальной разметки

    for (float i = 0; i < SCREEN_WIDTH; i += cellSpace) {
        SDL_RenderDrawLine(ren, beginCoord.x - cellLen, i, beginCoord.x + cellLen, i);
        SDL_RenderDrawLine(ren, i, beginCoord.y - cellLen, i, beginCoord.y + cellLen);
        SDL_RenderPresent(ren);
    }

    SDL_Event e;
    SDL_SetRenderDrawColor(ren, 0xEE, 0x00, 0xBB, 0xCC);

    //построение графика

     // уравнение

    int k = 0;
    for (x -= beginCoord.x; x <= SCREEN_WIDTH + beginCoord.x; x += speed) {
        y = x* x * x * mirror * scale / 100 + beginCoord.y;
       graphicTable._x[k] = x;
        graphicTable._y[k] = y;
        if (y >= 0 && y <= SCREEN_HEIGHT) {
            std::cout << "x: " << graphicTable._x[k] << " --- y:" << graphicTable._y[k] << std::endl;
            k += speed;

            SDL_RenderDrawPoint(ren, (x + beginCoord.x), y);
            SDL_RenderPresent(ren);
       }
    }

    //// взаимодействие с полем

    int* mouse_x = 0; int *mouse_y = 0;
    int cursorSearcher = 0;

    // pause

    SDL_Rect playButton = {130, 180, 140, 60};
    SDL_Rect exitButton = {130, 280, 140, 60};

    int animNumber = 0;
    int animCounter = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while (!q) {
        if (game_isPlaying) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        game_isPlaying = false;
                    }

                    if (e.key.keysym.sym == SDLK_SPACE) {
                        rect_isJumping = true;
                    }
                }

                if (e.type == SDL_KEYUP) {
                    if (e.key.keysym.sym == SDLK_SPACE) {
                        rect_isJumping = false;
                    }
                }
            }

            if (animCounter % 30 == 0 && animCounter != 0) {
                animNumber++;
                if (animNumber >= 3) animNumber = 0;
            }
            
            if (rect.y + rect.h < SCREEN_HEIGHT) {
                rect_isFlying = true;
            }
            else {
                rect_isFlying = false;
            }

            if (rect_isFlying) {
                rect.y += 1;
            }

            if (rect_isJumping) {
                rect.y -= 2;
            }

            if (!rect_isJumping) {
                animNumber = 1;
            }

            if (stopper[0].x + stopperWidth <= 0) {
                stopper[0].x = SCREEN_WIDTH;
                int heightFirstStopper = 50 + rand() % 250;
                stopper[0].h = heightFirstStopper;
                stopper[1].x = SCREEN_WIDTH;
                stopper[1].y = heightFirstStopper + spaceThroughStopperByHeight;
                stopper[1].h = SCREEN_HEIGHT - stopper[1].y;
                system("cls");
                std::cout << scores << std::endl;

                scores++;
            }

            if (stopper[2].x + stopperWidth <= 0) {
                stopper[2].x = (SCREEN_WIDTH + SCREEN_WIDTH) / 2;
                int heightFirstStopper = 50 + rand() % 250;
                stopper[2].h = heightFirstStopper;
                stopper[3].x = (SCREEN_WIDTH + SCREEN_WIDTH) / 2;
                stopper[3].y = heightFirstStopper + spaceThroughStopperByHeight;
                stopper[3].h = SCREEN_HEIGHT - stopper[2].y;
                system("cls");
                std::cout << scores << std::endl;

                scores++;
            }

            stopper[0].x -= 1;
            stopper[1].x -= 1;
            stopper[2].x -= 1;
            stopper[3].x -= 1;

            SDL_Point rectPoints[4] = { {rect.x, rect.y}, {rect.x + rect.w, rect.y}, {rect.x + rect.w, rect.y + rect.h}, {rect.x, rect.y + rect.h} };
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (SDL_PointInRect(&rectPoints[i], &stopper[j])) {
                        SDL_RenderClear(ren);
                        game_isPlaying = false;
                    }
                }
            }
           
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, back_tex, NULL, &back_rect);

            if (animNumber == 0) {
                SDL_RenderCopy(ren, bird_fall, NULL, &rect);
            }
            else if (animNumber == 1) {
                SDL_RenderCopy(ren, bird_chill, NULL, &rect);
            }
            else if (animNumber == 2) {
                SDL_RenderCopy(ren, bird_fly, NULL, &rect);
            }

            SDL_SetRenderDrawColor(ren, 0x00, 100, 0x00, 255);

            SDL_RenderFillRect(ren, &stopper[0]);
            SDL_RenderFillRect(ren, &stopper[1]);
            SDL_RenderFillRect(ren, &stopper[2]);
            SDL_RenderFillRect(ren, &stopper[3]);

            animCounter++;
        }
        else {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        int coursor_x = e.button.x, coursor_y = e.button.y;
                        if (coursor_x > playButton.x && coursor_x < playButton.x + playButton.w
                            && coursor_y > playButton.y && coursor_y < playButton.y + playButton.h) {
                            game_isPlaying = true;
                        }
                        if (coursor_x > exitButton.x && coursor_x < exitButton.x + exitButton.w
                            && coursor_y > exitButton.y && coursor_y < exitButton.y + exitButton.h) {
                            quit();
                            return 0;
                        }
                    }
                }
            }

            stopper[0] = {0 - rectSize, 0, stopperWidth, 50};
            stopper[1] = { 0 - rectSize, SCREEN_HEIGHT - 150, stopperWidth, 150 };
            stopper[2] = { SCREEN_WIDTH / 2, 0, stopperWidth, 50 };
            stopper[3] = { SCREEN_WIDTH / 2, SCREEN_HEIGHT - 150, stopperWidth, 150 };

            scores = 0;
            difficult = 4;
            rect_isJumping = false;

            SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(ren);

            SDL_SetRenderDrawColor(ren, 0, 255, 6, 255);
            SDL_RenderDrawRect(ren, &playButton);

            SDL_SetRenderDrawColor(ren, 255, 0, 3, 0xFF);
            SDL_RenderDrawRect(ren, &exitButton);
        }

        SDL_RenderPresent(ren);

        SDL_Delay(4);
    }

    SDL_Delay(500);

    quit();
    return 0;
}
