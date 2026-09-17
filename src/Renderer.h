#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <unordered_set>
#include <string>
#include "SortingEvent.h"

class Renderer {
public:
    Renderer(SDL_Window* window, SDL_Renderer* sdlRenderer);
    ~Renderer();

    void clear();

    void present();

    void render(const std::vector<int>& values,
                int highlightIndexA,
                int highlightIndexB,
                EventType highlightType,
                const std::unordered_set<std::size_t>& sortedIndices,
                const SDL_Rect& viewport);

    void renderStats(char letter,
                     std::size_t comparisons,
                     std::size_t swaps,
                     std::size_t maxComparisons,
                     std::size_t maxSwaps,
                     const SDL_Rect& viewport);

    void drawText(const std::string& text, float x, float y, float charWidth, float charHeight, const SDL_Color& color);
    void drawTextWrapped(const std::string& text, float x, float y, float charWidth, float charHeight, float maxWidth, const SDL_Color& color);


    void drawButton(const std::string& text, const SDL_Rect& rect, bool hovered, bool active, bool isDropdown = false);

private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font = nullptr;
};
