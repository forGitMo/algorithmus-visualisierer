#include "Renderer.h"

#include <iostream>
#include <memory>

// Diese Pointer löschen SDL-Flächen und Texturen automatisch
using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
using TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;


Renderer::Renderer(SDL_Window* window, SDL_Renderer* sdlRenderer)
    : m_window(window), m_renderer(sdlRenderer) {

    if (!TTF_WasInit()) {
        if (!TTF_Init()) {
            std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
        }
    }

    {
        const char* basePath = SDL_GetBasePath();
        std::string base = basePath ? std::string(basePath) : "";

        auto tryFont = [&](const std::string& rel) -> TTF_Font* {
            TTF_Font* f = TTF_OpenFont((base + rel).c_str(), 30);

            if (!f) {
                f = TTF_OpenFont(rel.c_str(), 30);
            }

            return f;
        };

        m_font = tryFont("assets/fonts/Arial.ttf");

        // Font Fallback
        if (!m_font) {
            m_font = tryFont("assets/fonts/Helvetica.ttf");
        }

        if (!m_font) {
            m_font = TTF_OpenFont(
                "/System/Library/Fonts/Supplemental/Arial.ttf",
                30.0
            );
        }

        if (!m_font) {
            m_font = TTF_OpenFont(
                "/System/Library/Fonts/Helvetica.ttc",
                30.0
            );
        }

        if (!m_font) {
            std::cerr
                << "TTF_OpenFont Error: Keine Schriftart gefunden. "
                << SDL_GetError()
                << std::endl;
        }
    }
}


Renderer::~Renderer() {
    // Gibt die geladene Schrift wieder frei.
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}


void Renderer::clear() {
    // Setzt die background color
    SDL_SetRenderDrawColor(m_renderer, 18, 18, 22, 255);
    // cleart vorhandene Render
    SDL_RenderClear(m_renderer);
}


void Renderer::present() {
    // Rendert das fertige img im Fenster
    SDL_RenderPresent(m_renderer);
}


void Renderer::render(
    const std::vector<int>& values,
    int highlightIndexA,
    int highlightIndexB,
    EventType highlightType,
    const std::unordered_set<std::size_t>& sortedIndices,
    const SDL_Rect& viewport) {

    // Legt Render Bereich fest
    SDL_SetRenderViewport(m_renderer, &viewport);

    SDL_SetRenderDrawColor(m_renderer, 26, 26, 30, 255);

    SDL_FRect panelRect = {
        0.0f,
        0.0f,
        static_cast<float>(viewport.w),
        static_cast<float>(viewport.h)
    };

    SDL_RenderFillRect(m_renderer, &panelRect);

    SDL_SetRenderDrawColor(m_renderer, 55, 55, 68, 255);
    SDL_RenderRect(m_renderer, &panelRect);

    if (values.empty()) {
        return;
    }

    const std::size_t n = values.size();

    // Berechnung des Balkenbereichs
    const float drawWidth = static_cast<float>(viewport.w) - 6.0f;
    const float drawHeight = static_cast<float>(viewport.h) - 15.0f;
    const float barWidth = drawWidth / static_cast<float>(n);

    int max_val = 1;

    // Max Wert für Balkenhöhen berechnung
    for (int val : values) {
        if (val > max_val) {
            max_val = val;
        }
    }

    // Balken rendern
    for (std::size_t i = 0; i < n; ++i) {
        float barHeight =
            (static_cast<float>(values[i]) / static_cast<float>(max_val))
            * drawHeight;

        if (barHeight < 2.0f) {
            barHeight = 2.0f;
        }

        SDL_FRect rect;

        rect.x = 3.0f + static_cast<float>(i) * barWidth;
        rect.y = drawHeight - barHeight + 8.0f;
        rect.w = barWidth > 2.0f ? barWidth - 1.0f : barWidth;
        rect.h = barHeight;

        Uint8 r = 0;
        Uint8 g = 0;
        Uint8 b = 0;
        Uint8 a = 255;


        // Fertig soriert = Grün
        if (sortedIndices.count(i) > 0) {
            r = 50;
            g = 205;
            b = 50;
        }
        // Aktuell betrachteter Balken Farbe
        else if (
            static_cast<int>(i) == highlightIndexA ||
            static_cast<int>(i) == highlightIndexB) {

            if (highlightType == EventType::Compare) {
                r = 255;
                g = 215;
                b = 0;
            }
            else if (highlightType == EventType::Swap) {
                r = 220;
                g = 20;
                b = 60;
            }
            else if (highlightType == EventType::SetPivot) {
                r = 138;
                g = 43;
                b = 226;
            }
            else {
                r = 255;
                g = 255;
                b = 255;
            }
        }
        // Balken Farbe nach größe
        else {
            float ratio =
                static_cast<float>(values[i]) /
                static_cast<float>(max_val);

            r = static_cast<Uint8>(30.0f + ratio * 50.0f);
            g = static_cast<Uint8>(100.0f + ratio * 120.0f);
            b = static_cast<Uint8>(180.0f + ratio * 75.0f);
        }

        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderFillRect(m_renderer, &rect);
    }
}


void Renderer::renderStats(
    char letter,
    std::size_t comparisons,
    std::size_t swaps,
    std::size_t maxComparisons,
    std::size_t maxSwaps,
    const SDL_Rect& viewport) {

    /*
    (void)maxComparisons;
    (void)maxSwaps;
    */

    SDL_SetRenderViewport(m_renderer, &viewport);

    SDL_SetRenderDrawColor(m_renderer, 22, 22, 26, 255);

    SDL_FRect panelRect = {
        0.0f,
        0.0f,
        static_cast<float>(viewport.w),
        static_cast<float>(viewport.h)
    };

    SDL_RenderFillRect(m_renderer, &panelRect);

    SDL_SetRenderDrawColor(m_renderer, 48, 48, 58, 255);
    SDL_RenderRect(m_renderer, &panelRect);

    std::string letterStr(1, letter);

    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color compareColor = {255, 255, 255, 255};
    SDL_Color swapColor = {255, 255, 255, 255};
    SDL_Color labelColor = {255, 255, 255, 255};

    // render Statisktik
    drawText(
        letterStr,
        12.0f,
        14.0f,
        16.0f,
        30.0f,
        titleColor
    );

    drawText(
        "VERGLEICHE:",
        50.0f,
        10.0f,
        5.0f,
        9.0f,
        labelColor
    );

    drawText(
        std::to_string(comparisons),
        150.0f,
        8.0f,
        8.0f,
        15.0f,
        compareColor
    );

    drawText(
        "TAUSCHE:",
        50.0f,
        33.0f,
        5.0f,
        9.0f,
        labelColor
    );

    drawText(
        std::to_string(swaps),
        150.0f,
        31.0f,
        8.0f,
        15.0f,
        swapColor
    );

    SDL_SetRenderViewport(m_renderer, nullptr);
}


void Renderer::drawText(
    const std::string& text,
    float x,
    float y,
    float charWidth,
    float charHeight,
    const SDL_Color& color) {

    (void)charWidth;

    if (text.empty() || !m_font) {
        return;
    }

    SurfacePtr surface(
        TTF_RenderText_Blended(
            m_font,
            text.c_str(),
            text.length(),
            color
        ),
        SDL_DestroySurface
    );

    if (!surface) {
        return;
    }

    TexturePtr texture(
        SDL_CreateTextureFromSurface(
            m_renderer,
            surface.get()
        ),
        SDL_DestroyTexture
    );

    if (!texture) {
        return;
    }

    SDL_SetTextureBlendMode(
        texture.get(),
        SDL_BLENDMODE_BLEND
    );

    float textW = 0.0f;
    float textH = 0.0f;

    SDL_GetTextureSize(
        texture.get(),
        &textW,
        &textH
    );

    float scale = charHeight / textH;

    SDL_FRect dstRect = {
        x,
        y,
        textW * scale,
        charHeight
    };

    SDL_RenderTexture(
        m_renderer,
        texture.get(),
        nullptr,
        &dstRect
    );
}


void Renderer::drawTextWrapped(
    const std::string& text,
    float x,
    float y,
    float charWidth,
    float charHeight,
    float maxWidth,
    const SDL_Color& color) {

    (void)charWidth;

    if (text.empty() || !m_font) {
        return;
    }

    float scale = charHeight / 24.0f;

    Uint32 wrapLength =
        static_cast<Uint32>(maxWidth / scale);

    SurfacePtr surface(
        TTF_RenderText_Blended_Wrapped(
            m_font,
            text.c_str(),
            text.length(),
            color,
            wrapLength
        ),
        SDL_DestroySurface
    );

    if (!surface) {
        return;
    }

    TexturePtr texture(
        SDL_CreateTextureFromSurface(
            m_renderer,
            surface.get()
        ),
        SDL_DestroyTexture
    );

    if (!texture) {
        return;
    }

    SDL_SetTextureBlendMode(
        texture.get(),
        SDL_BLENDMODE_BLEND
    );

    float textW = 0.0f;
    float textH = 0.0f;

    SDL_GetTextureSize(
        texture.get(),
        &textW,
        &textH
    );

    SDL_FRect dstRect = {
        x,
        y,
        textW * scale,
        textH * scale
    };

    SDL_RenderTexture(
        m_renderer,
        texture.get(),
        nullptr,
        &dstRect
    );
}


void Renderer::drawButton(
    const std::string& text,
    const SDL_Rect& rect,
    bool hovered,
    bool active,
    bool isDropdown) {

    SDL_SetRenderViewport(m_renderer, &rect);
    // Farbe je Zustand
    if (active) {
        SDL_SetRenderDrawColor(
            m_renderer,
            48,
            48,
            62,
            255
        );
    }
    else if (hovered) {
        SDL_SetRenderDrawColor(
            m_renderer,
            38,
            38,
            48,
            255
        );
    }
    else {
        SDL_SetRenderDrawColor(
            m_renderer,
            26,
            26,
            32,
            255
        );
    }

    SDL_FRect r = {
        0.0f,
        0.0f,
        static_cast<float>(rect.w),
        static_cast<float>(rect.h)
    };

    SDL_RenderFillRect(m_renderer, &r);

    if (active) {
        SDL_SetRenderDrawColor(
            m_renderer,
            100,
            149,
            237,
            255
        );
    }
    else if (hovered) {
        SDL_SetRenderDrawColor(
            m_renderer,
            85,
            85,
            105,
            255
        );
    }
    else {
        SDL_SetRenderDrawColor(
            m_renderer,
            50,
            50,
            60,
            255
        );
    }

    SDL_RenderRect(m_renderer, &r);

    SDL_Color textColor = {
        220,
        220,
        230,
        255
    };

    float charH = 11.0f;

    SurfacePtr surface(
        TTF_RenderText_Blended(
            m_font,
            text.c_str(),
            text.length(),
            textColor
        ),
        SDL_DestroySurface
    );

    if (surface) {
        TexturePtr texture(
            SDL_CreateTextureFromSurface(
                m_renderer,
                surface.get()
            ),
            SDL_DestroyTexture
        );

        if (texture) {
            SDL_SetTextureBlendMode(
                texture.get(),
                SDL_BLENDMODE_BLEND
            );

            float textW = 0.0f;
            float textH = 0.0f;

            SDL_GetTextureSize(
                texture.get(),
                &textW,
                &textH
            );

            float scale = charH / textH;
            float drawW = textW * scale;

            // Text center
            float startX =
                (static_cast<float>(rect.w) - drawW) / 2.0f;

            if (isDropdown) {
                startX = 12.0f;
            }

            float startY =
                (static_cast<float>(rect.h) - charH) / 2.0f;

            SDL_FRect dstRect = {
                startX,
                startY,
                drawW,
                charH
            };

            SDL_RenderTexture(
                m_renderer,
                texture.get(),
                nullptr,
                &dstRect
            );
        }
    }

    // Pfeil bei dropdown
    if (isDropdown) {
        SDL_SetRenderDrawColor(
            m_renderer,
            160,
            160,
            175,
            255
        );

        float arrowX =
            static_cast<float>(rect.w) - 18.0f;

        float arrowY =
            (static_cast<float>(rect.h) - 5.0f) / 2.0f;

        if (active) {
            SDL_RenderLine(
                m_renderer,
                arrowX - 4.0f,
                arrowY + 5.0f,
                arrowX,
                arrowY
            );

            SDL_RenderLine(
                m_renderer,
                arrowX,
                arrowY,
                arrowX + 4.0f,
                arrowY + 5.0f
            );
        }
        else {
            SDL_RenderLine(
                m_renderer,
                arrowX - 4.0f,
                arrowY,
                arrowX,
                arrowY + 5.0f
            );

            SDL_RenderLine(
                m_renderer,
                arrowX,
                arrowY + 5.0f,
                arrowX + 4.0f,
                arrowY
            );
        }
    }
}