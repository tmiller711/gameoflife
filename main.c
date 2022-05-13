#include <stdio.h>
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

int checkneighbors(blocks *headNode)
{
    // create a linked list of all the neighbors
    // create a list of all the neighbors that will survivors

    for (blocks *tmp = headNode; tmp != NULL; tmp = tmp->next)
    {
        int numofneighbors = 0;
        int curblockx = tmp->block.x;
        int curblocky = tmp->block.y;
        int top = curblocky - grid_cell_size;
        int right = curblockx + grid_cell_size;
        int bottom = curblocky + grid_cell_size;
        int left = curblockx - grid_cell_size;

        for (blocks *tmp = headNode; tmp != NULL; tmp = tmp->next)
        {
            if (tmp->block.y == top && tmp->block.x == curblockx)
            {
                printf("found a top neighbor\n");
                numofneighbors++;
            }
            if (tmp->block.x == right && tmp->block.y == curblocky)
            {
                printf("found a right neighbor\n");
                numofneighbors++;
            }
            if (tmp->block.y == bottom && tmp->block.x == curblockx)
            {
                printf("found a bottom neighbor\n");
                numofneighbors++;
            }
            if (tmp->block.x== left && tmp->block.y == curblocky)
            {
                printf("found a left neighbor\n");
                numofneighbors++;
            }
            // find top left neighbor
            if (tmp->block.x == left && tmp->block.y == top)
            {
                printf("Found top left neighbor\n");
                numofneighbors++;
            }
            // top right neighbr
            if (tmp->block.x == right && tmp->block.y == top)
            {
                printf("Found a top right neighbor\n");
                numofneighbors++;
            }
            // bottom right neighbor
            if (tmp->block.x == right && tmp->block.y == bottom)
            {
                printf("Found a bottm right neighbor\n");
                numofneighbors++;
            }
            // bottom left neighbor
            if (tmp->block.x == left && tmp->block.y == bottom)
            {
                printf("found a bottom left neighbor\n");
                numofneighbors++;
            }
        }
        printf("Num of neighbors: %i\n", numofneighbors);
    }
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
    int drawblocks = 0;
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
                            list = headNode;
                            checkneighbors(headNode);
                            drawblocks = 1; 
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

        SDL_RenderPresent(rend);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}