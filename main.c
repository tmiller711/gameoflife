#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct blocks
{
    SDL_Rect block;
    struct blocks *next;
}
blocks;

int grid_cell_size = 14;
int grid_height = 47;
int grid_width = 85;

blocks* checkneighbors(blocks *headNode)
{
    int counter = 0;
    bool nexists = false;
    blocks *newhead, *n, *list;
    for (int x = 0; x <= grid_cell_size * grid_width; x += grid_cell_size)
    {
        for (int y = 0; y <= grid_cell_size * grid_height; y += grid_cell_size)
        {
            int numofneighbors = 0;
            bool alreadyalive = false;
            // check if current block is in the list (alive)
            for (blocks *tmp = headNode; tmp != NULL; tmp = tmp->next)
            {
                if (x == tmp->block.x && y == tmp->block.y)
                    alreadyalive = true;
            }
            // check surrounding blocks
            for (int i = x - grid_cell_size; i <= x + grid_cell_size; i += grid_cell_size)
            {
                for (int j = y - grid_cell_size; j <= y + grid_cell_size; j += grid_cell_size)
                {
                    // check if surrounding blocks are in the list
                    for (blocks *tmp = headNode; tmp != NULL; tmp = tmp->next)
                    {
                        if (i == tmp->block.x && j == tmp->block.y)
                            numofneighbors += 1;
                    }
                }
            }
            // need to delete one neighbor because it counts itself
            if (alreadyalive) numofneighbors--;

            if (alreadyalive && numofneighbors == 2 || alreadyalive && numofneighbors == 3 
                    || !alreadyalive && numofneighbors == 3)
            {
                // add to new linked list
                n = malloc(sizeof(blocks));
                n->block.x = x;
                n->block.y = y;
                n->block.h = grid_cell_size;
                n->block.w = grid_cell_size;
                n->next = NULL;
                nexists = true;
            }

            // need to make sure there's a block to add to the list
            if (counter == 0 && nexists)
            {
                newhead = list = n;
                counter = 1;
                nexists = false;
            }
            else if (counter == 1 && nexists)
            {
                list->next = n;
                list = n;
                nexists = false;
            }
        }
    }    

    return newhead;
}

int main(void)
{
    int WIN_H = (grid_height * grid_cell_size) + 1;
    int WIN_W = (grid_width * grid_cell_size) + 1;

    SDL_Color grid_background = {22, 22, 22, 255};
    SDL_Color grid_line_color = {44, 44, 44, 255};
    SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
    SDL_Color grid_cursor_color = {255, 255, 255, 255};

    // initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win;
    SDL_Renderer* rend;
    if (SDL_CreateWindowAndRenderer(WIN_W, WIN_H, 0, &win, &rend) < 0)
    {
        printf("error initializing window and/or renderer: %s\n", SDL_GetError());
    }

    blocks *headNode, *n, *list;

    int close_requested = 0;
    int counter = 0;
    int startgame = 0;
    while (!close_requested)
    {
        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    close_requested = 1;
                case SDL_MOUSEBUTTONDOWN:
                    // add node to the linked list and then call it
                    n = malloc(sizeof(blocks));
                    n->block.x = (event.motion.x / grid_cell_size) * grid_cell_size;
                    n->block.y = (event.motion.y / grid_cell_size) * grid_cell_size;
                    n->block.h = grid_cell_size;
                    n->block.w = grid_cell_size;
                    n->next = NULL;

                    if (counter == 0)
                    {
                        headNode = list = n;
                        counter = 1;
                    }
                    else
                    {
                        list->next = n;
                        list = n;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_RETURN:
                            // start the game
                            startgame = 1;
                            break;
                    }
                    break;
            }
        }

        // draw grid background
        SDL_SetRenderDrawColor(rend, grid_background.r, grid_background.g, grid_background.b, grid_background.a);
        SDL_RenderClear(rend);

        // draw grid lines
        SDL_SetRenderDrawColor(rend, grid_line_color.r, grid_line_color.g, grid_line_color.b, grid_line_color.a);

        for (int x = 0; x < 1 + grid_width * grid_cell_size; x += grid_cell_size)
        {
            SDL_RenderDrawLine(rend, x, 0, x, WIN_H);
        }

        for (int y = 0; y < 1 + grid_height * grid_cell_size; y += grid_cell_size)
        {
            SDL_RenderDrawLine(rend, 0, y, WIN_W, y);
        }

        // draw the blocks
        if (counter)
        {
            for (blocks *tmp = headNode; tmp != NULL; tmp = tmp->next)
            {
                SDL_SetRenderDrawColor(rend, grid_cursor_color.r, grid_cursor_color.g, grid_cursor_color.b, grid_cursor_color.a);
                SDL_RenderFillRect(rend, &tmp->block);
            }
        }

        // game logic
        if (startgame)
        {
            headNode = checkneighbors(headNode);
        }

        SDL_RenderPresent(rend);
        SDL_Delay(1000/4);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}