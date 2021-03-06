#include <SDL.h>
#include <SDL_ttf.h>

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#include "AssetManager.h"
#include "FileSystem.h"
#include "Game.h"
#include "Renderer_SDL2.h"

#ifdef USE_HUMBLE_API
#include "humble_api.h"
#endif

bool done = false;

static SDL_Window *win = NULL;
static SDL_Renderer *renderer = NULL;

static Uint32 last_time = 0;

void loop_iteration(Game* game)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (e.key.keysym.scancode) {
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_RIGHT: {
                        const Uint8* keys = SDL_GetKeyboardState(0);
                        Sint16 xpos = 0;

                        if (keys[SDL_SCANCODE_LEFT] && keys[SDL_SCANCODE_RIGHT]) {
                            xpos = 0;
                        } else if (keys[SDL_SCANCODE_LEFT]) {
                            xpos = -1.0f;
                        } else if (keys[SDL_SCANCODE_RIGHT]) {
                            xpos = 1.0f;
                        }

                        game->apply_input(Game::InputForce_X_AXIS, xpos);
                    }
                        break;
                    case SDL_SCANCODE_UP:
                        game->apply_input(Game::InputForce_SHOOT, e.key.state == SDL_PRESSED ? 1 : 0);
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        game->apply_input(Game::InputForce_START, e.key.state == SDL_PRESSED ? 1 : 0);
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                done = true;
                return;
                break;

            default:
                break;
        }
    }

    Uint32 cur_time = SDL_GetTicks();
    float delta = float(cur_time - last_time) / 1000.0f;
    last_time = cur_time;

    game->update(delta);
    game->render();
}


int main(int argc, char* argv[])
{
#ifdef USE_HUMBLE_API
    humble_init();
#endif

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    int width = 640, height = 480;
#ifdef USE_HUMBLE_API
    if (humble_get_player_size(&width, &height) == 0) {
        width = 640;
        height = 480;
    }
#endif

    SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &win, &renderer);

    int w,h;
    SDL_GetWindowSize(win, &w, &h);

    char *base = SDL_GetBasePath();
    std::string base_path(base);
    SDL_free(base);

    std::string asset_path_marker = FileSystem::join_path(base_path, "asset_path.txt");
    if (FileSystem::exists(asset_path_marker)) {
        FILE* fp = FileSystem::open(asset_path_marker);
        if (fp) {
            char buff[1024];
            fgets(buff, sizeof(buff), fp);
            base_path = FileSystem::join_path(base_path, buff);
            fclose(fp);
        }
    }
#ifdef EMSCRIPTEN
    else {
        base_path = FileSystem::join_path(base_path, "assets");
    }
#endif

    AssetManager *asset_manager = new AssetManager();
    asset_manager->add_path(base_path);
    AssetManager::setDefaultManager(asset_manager);

    Renderer* game_renderer = new Renderer_SDL2(win, renderer);

    Game* game = new Game(*game_renderer);

    last_time = SDL_GetTicks();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop_arg((em_arg_callback_func)loop_iteration, game, 0, 1);
#else
    while (!done) {
        loop_iteration(game);
    }
#endif

    delete game;
    delete game_renderer;

    SDL_Quit();

    return 0;
}
