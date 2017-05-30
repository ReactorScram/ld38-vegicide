return [[
// SDL RW_ops
typedef struct SDL_RWops * SDL_RW_handle;
SDL_RW_handle SDL_RWFromConstMem (const void * mem, int size);

// SDL timer
void SDL_Delay(uint32_t ms);

// SDL video
typedef struct SDL_Surface SDL_Surface;
]]