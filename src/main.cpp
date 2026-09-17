#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <utility>
#include <string>
#include <functional>

#include <SDL3_ttf/SDL_ttf.h>
#include "Renderer.h"
#include "AlgorithmInfo.h"
#include "algorithmen/BubbleSort.h"
#include "algorithmen/SelectionSort.h"
#include "algorithmen/HeapSort.h"
#include "algorithmen/InsertionSort.h"
#include "algorithmen/RadixSort.h"
#include "algorithmen/BucketSort.h"
#include "SoundEngine.h"

constexpr int SCREEN_WIDTH = 950; // Genug Platz für zwei Spalten
constexpr int SCREEN_HEIGHT = 700;

struct AlgorithmRegistration {
    std::string name;
    std::string complexity;
    std::string description;
    std::function<SortResult(const std::vector<int>&)> sortFunc;
};

const std::vector<AlgorithmRegistration> algorithms = {
    {
        "BUBBLE SORT",
        "BEST: O(N) | AVG: O(N^2) | WORST: O(N^2)",
        "VERGLEICHT BENACHBARTE ELEMENTE UND VERTAUSCHT SIE, WENN SIE IN DER FALSCHEN REIHENFOLGE SIND. WIEDERHOLT DIES, BIS DAS ARRAY SORTIERT IST. SEHR LANGSAM BEI GROSSEN DATENMENGEN.",
        bubbleSort
    },
    {
        "SELECTION SORT",
        "BEST: O(N^2) | AVG: O(N^2) | WORST: O(N^2)",
        "SUCHT WIEDERHOLT DAS KLEINSTE ELEMENT AUS DEM UNSORTIERTEN TEIL UND SETZT ES AN DEN ANFANG DES SORTIERTEN TEILS. DIE ANZAHL DER VERGLEICHE IST IMMER QUADRATISCH, DIE TAUSCHE SIND MINIMAL.",
        selectionSort
    },
    {
        "HEAP SORT",
        "BEST: O(N LOG N) | AVG: O(N LOG N) | WORST: O(N LOG N)",
        "ERSTELLT EINEN BINAEREN MAX-HEAP UND ENTNIMMT WIEDERHOLT DAS GROESSTE ELEMENT (WURZEL), UM ES AN DAS ENDE DES ARRAYS ZU SETZEN. SEHR STABIL UND SCHNELL.",
        heapSort
    },
    {
        "INSERTION SORT",
        "BEST: O(N) | AVG: O(N^2) | WORST: O(N^2)",
        "DURCHLAEUFT DAS ARRAY UND FUEGT JEDES ELEMENT AN DER KORREKTEN STELLE IN DEM BEREITS SORTIERTEN TEIL EIN. EFFIZIENT BEI KLEINEN ODER FAST SORTIERTEN ARRAYS.",
        insertionSort
    },
    {
        "RADIX SORT",
        "BEST: O(N) | AVG: O(N) | WORST: O(N)",
        "EIN NICHT-VERGLEICHSBASIERTER ALGORITHMUS. SORTIERT DIE ELEMENTE ZIFFERNWEISE VON DER WENIGER SIGNIFIKANTEN ZUR MEHR SIGNIFIKANTEN STELLE (LSB/LSD).",
        radixSort
    },
    {
        "BUCKET SORT",
        "BEST: O(N) | AVG: O(N) | WORST: O(N^2)",
        "VERTEILT ELEMENTE AUF MEHRERE EIMER (BUCKETS), SORTIERT DIESE EINZELN UND FUEHRT SIE DANN WIEDER ZUSAMMEN. SEHR SCHNELL BEI GLEICHMAESSIG VERTEILTEN WERTEN.",
        bucketSort
    }
};

static float valueToFreq(int value) {
    constexpr float MIN_VAL  = 10.0f;
    constexpr float MAX_VAL  = 520.0f;
    constexpr float MIN_FREQ = 150.0f;
    constexpr float MAX_FREQ = 1000.0f;
    float t = (static_cast<float>(value) - MIN_VAL) / (MAX_VAL - MIN_VAL);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return MIN_FREQ + t * (MAX_FREQ - MIN_FREQ);
}

std::vector<int> generateArray(std::size_t size, const std::string& type) {
    std::vector<int> arr(size);
    if (type == "ZUFÄLLIG") {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 520);
        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = dis(gen);
        }
    } else if (type == "SORTIERT") {
        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = static_cast<int>(20 + (i * 500) / size);
        }
    } else if (type == "UMGEKEHRT") {
        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = static_cast<int>(520 - (i * 500) / size);
        }
    }
    return arr;
}

bool isPointInRect(float px, float py, const SDL_Rect& rect) {
    return px >= static_cast<float>(rect.x) && px < static_cast<float>(rect.x + rect.w) &&
           py >= static_cast<float>(rect.y) && py < static_cast<float>(rect.y + rect.h);
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return 1;
        }
        std::cerr << "Warnung: Audio konnte nicht initialisiert werden." << std::endl;
    }

    SDL_Window *window = SDL_CreateWindow("Sortier-Vergleichs-Visualisierer", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(window, NULL);
    if (!sdlRenderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Renderer visualRenderer(window, sdlRenderer);
    SoundEngine soundEngine;

    bool inMenu = true;
    int selectedLeft = 0;
    int selectedRight = 2;
    std::size_t arraySize = 100;
    std::string arrayType = "ZUFÄLLIG";

    bool leftOpen = false;
    bool rightOpen = false;
    bool sizeOpen = false;
    bool typeOpen = false;

    std::vector<int> sizes = { 50, 100, 250, 500 };
    std::vector<std::string> types = { "ZUFÄLLIG", "SORTIERT", "UMGEKEHRT" };

    std::vector<int> originalArray;
    std::vector<int> leftArray, rightArray;
    SortResult leftRes, rightRes;

    std::size_t leftIndex = 0, rightIndex = 0;
    std::size_t leftComps = 0, leftSwaps = 0;
    std::size_t rightComps = 0, rightSwaps = 0;

    std::unordered_set<std::size_t> leftSorted, rightSorted;

    int leftHighlightA = -1, leftHighlightB = -1;
    int rightHighlightA = -1, rightHighlightB = -1;

    EventType leftHighlightType = EventType::ClearHighlight;
    EventType rightHighlightType = EventType::ClearHighlight;

    std::size_t maxCompsGlobal = 1;
    std::size_t maxSwapsGlobal = 1;

    bool paused = true;
    Uint64 delayMs = 15;
    Uint64 lastStepTime = SDL_GetTicks();
    bool printFinalReport = false;

    bool muteLeft  = false;
    bool muteRight = false;

    bool running = true;
    SDL_Event event;

    while (running) {
        float mx = 0.0f;
        float my = 0.0f;
        SDL_GetMouseState(&mx, &my);

        SDL_Rect leftAlgoBtn = { 150, 180, 280, 35 };
        SDL_Rect rightAlgoBtn = { 520, 180, 280, 35 };
        SDL_Rect sizeBtn = { 150, 310, 280, 35 };
        SDL_Rect typeBtn = { 520, 310, 280, 35 };
        SDL_Rect startBtn = { 325, 460, 300, 45 };

        std::vector<SDL_Rect> leftOptions(algorithms.size());
        for (std::size_t i = 0; i < algorithms.size(); ++i) {
            leftOptions[i] = { leftAlgoBtn.x, leftAlgoBtn.y + leftAlgoBtn.h + static_cast<int>(i) * 30, leftAlgoBtn.w, 30 };
        }

        std::vector<SDL_Rect> rightOptions(algorithms.size());
        for (std::size_t i = 0; i < algorithms.size(); ++i) {
            rightOptions[i] = { rightAlgoBtn.x, rightAlgoBtn.y + rightAlgoBtn.h + static_cast<int>(i) * 30, rightAlgoBtn.w, 30 };
        }

        std::vector<SDL_Rect> sizeOptions(sizes.size());
        for (std::size_t i = 0; i < sizes.size(); ++i) {
            sizeOptions[i] = { sizeBtn.x, sizeBtn.y + sizeBtn.h + static_cast<int>(i) * 30, sizeBtn.w, 30 };
        }

        std::vector<SDL_Rect> typeOptions(types.size());
        for (std::size_t i = 0; i < types.size(); ++i) {
            typeOptions[i] = { typeBtn.x, typeBtn.y + typeBtn.h + static_cast<int>(i) * 30, typeBtn.w, 30 };
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                float px = event.button.x;
                float py = event.button.y;

                if (inMenu) {
                    if (leftOpen) {
                        for (std::size_t i = 0; i < algorithms.size(); ++i) {
                            if (isPointInRect(px, py, leftOptions[i])) {
                                selectedLeft = static_cast<int>(i);
                                break;
                            }
                        }
                        leftOpen = false;
                    } else if (rightOpen) {
                        for (std::size_t i = 0; i < algorithms.size(); ++i) {
                            if (isPointInRect(px, py, rightOptions[i])) {
                                selectedRight = static_cast<int>(i);
                                break;
                            }
                        }
                        rightOpen = false;
                    } else if (sizeOpen) {
                        for (std::size_t i = 0; i < sizes.size(); ++i) {
                            if (isPointInRect(px, py, sizeOptions[i])) {
                                arraySize = sizes[i];
                                break;
                            }
                        }
                        sizeOpen = false;
                    } else if (typeOpen) {
                        for (std::size_t i = 0; i < types.size(); ++i) {
                            if (isPointInRect(px, py, typeOptions[i])) {
                                arrayType = types[i];
                                break;
                            }
                        }
                        typeOpen = false;
                    } else {
                        if (isPointInRect(px, py, leftAlgoBtn)) {
                            leftOpen = true;
                        } else if (isPointInRect(px, py, rightAlgoBtn)) {
                            rightOpen = true;
                        } else if (isPointInRect(px, py, sizeBtn)) {
                            sizeOpen = true;
                        } else if (isPointInRect(px, py, typeBtn)) {
                            typeOpen = true;
                        } else if (isPointInRect(px, py, startBtn)) {
                            originalArray = generateArray(arraySize, arrayType);
                            leftArray = originalArray;
                            rightArray = originalArray;

                            leftRes = algorithms[selectedLeft].sortFunc(originalArray);
                            rightRes = algorithms[selectedRight].sortFunc(originalArray);

                            leftIndex = 0; rightIndex = 0;
                            leftComps = 0; leftSwaps = 0;
                            rightComps = 0; rightSwaps = 0;

                            leftSorted.clear(); rightSorted.clear();
                            leftHighlightA = -1; leftHighlightB = -1;
                            rightHighlightA = -1; rightHighlightB = -1;

                            maxCompsGlobal = std::max(leftRes.stats.comparisons, rightRes.stats.comparisons);
                            maxSwapsGlobal = std::max(leftRes.stats.swaps, rightRes.stats.swaps);
                            if (maxCompsGlobal == 0) maxCompsGlobal = 1;
                            if (maxSwapsGlobal == 0) maxSwapsGlobal = 1;

                            paused = false;
                            inMenu = false;
                            printFinalReport = true;
                            lastStepTime = SDL_GetTicks();
                            muteLeft  = false; 
                            muteRight = false;
                        }
                    }
                }
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_SPACE:
                        if (!inMenu) {
                            paused = !paused;
                        }
                        break;
                    case SDLK_R:
                        if (!inMenu) {
                            inMenu = true;
                            paused = true;
                        }
                        break;
                    case SDLK_UP:
                        if (!inMenu) {
                            if (delayMs > 1) delayMs = (delayMs > 10) ? delayMs - 5 : delayMs - 1;
                        }
                        break;
                    case SDLK_DOWN:
                        if (!inMenu) {
                            delayMs = (delayMs >= 10) ? delayMs + 5 : delayMs + 1;
                            if (delayMs > 500) delayMs = 500;
                        }
                        break;
                    case SDLK_M:
                        soundEngine.toggleMute();
                        break;
                    case SDLK_1:
                        // Linken Algorithmus stummschalten/aktivieren
                        if (!inMenu) muteLeft = !muteLeft;
                        break;
                    case SDLK_2:
                        // Rechten Algorithmus stummschalten/aktivieren
                        if (!inMenu) muteRight = !muteRight;
                        break;
                }
            }
        }

        // --- Replay Schritt verarbeiten (Visualisierungsmodus) ---
        if (!inMenu && !paused) {
            Uint64 now = SDL_GetTicks();
            if (now - lastStepTime >= delayMs) {
                lastStepTime = now;

                // 1. Linker Algorithmus Schritt
                if (leftIndex < leftRes.events.size()) {
                    SortingEvent ev = leftRes.events[leftIndex];
                    leftHighlightA = static_cast<int>(ev.indexA);
                    leftHighlightB = static_cast<int>(ev.indexB);
                    leftHighlightType = ev.type;

                    if (ev.type == EventType::Compare) {
                        if (!muteLeft) soundEngine.playTone(valueToFreq(leftArray[ev.indexA]),
                                                           12.0f, -0.7f, 0.38f);
                        leftComps++;
                    } else if (ev.type == EventType::Swap) {
                        if (!muteLeft) soundEngine.playTone(valueToFreq(leftArray[ev.indexA]),
                                                           20.0f, -0.7f, 0.55f);
                        leftSwaps++;
                        std::swap(leftArray[ev.indexA], leftArray[ev.indexB]);
                    } else if (ev.type == EventType::Overwrite) {
                        if (!muteLeft) soundEngine.playTone(valueToFreq(ev.value),
                                                           20.0f, -0.7f, 0.55f);
                        leftSwaps++;
                        leftArray[ev.indexA] = ev.value;
                    } else if (ev.type == EventType::MarkSorted) {
                        if (!muteLeft) soundEngine.playTone(valueToFreq(leftArray[ev.indexA]),
                                                           28.0f, -0.7f, 0.30f);
                        leftSorted.insert(ev.indexA);
                    }
                    leftIndex++;
                } else if (leftSorted.size() < arraySize) {
                    for (std::size_t i = 0; i < arraySize; ++i) leftSorted.insert(i);
                    leftHighlightA = -1; leftHighlightB = -1;
                }

                // 2. Rechter Algorithmus Schritt
                if (rightIndex < rightRes.events.size()) {
                    SortingEvent ev = rightRes.events[rightIndex];
                    rightHighlightA = static_cast<int>(ev.indexA);
                    rightHighlightB = static_cast<int>(ev.indexB);
                    rightHighlightType = ev.type;

                    if (ev.type == EventType::Compare) {
                        if (!muteRight) soundEngine.playTone(valueToFreq(rightArray[ev.indexA]),
                                                            12.0f, +0.7f, 0.38f);
                        rightComps++;
                    } else if (ev.type == EventType::Swap) {
                        if (!muteRight) soundEngine.playTone(valueToFreq(rightArray[ev.indexA]),
                                                            20.0f, +0.7f, 0.55f);
                        rightSwaps++;
                        std::swap(rightArray[ev.indexA], rightArray[ev.indexB]);
                    } else if (ev.type == EventType::Overwrite) {
                        if (!muteRight) soundEngine.playTone(valueToFreq(ev.value),
                                                            20.0f, +0.7f, 0.55f);
                        rightSwaps++;
                        rightArray[ev.indexA] = ev.value;
                    } else if (ev.type == EventType::MarkSorted) {
                        if (!muteRight) soundEngine.playTone(valueToFreq(rightArray[ev.indexA]),
                                                            28.0f, +0.7f, 0.30f);
                        rightSorted.insert(ev.indexA);
                    }
                    rightIndex++;
                } else if (rightSorted.size() < arraySize) {
                    for (std::size_t i = 0; i < arraySize; ++i) rightSorted.insert(i);
                    rightHighlightA = -1; rightHighlightB = -1;
                }

                // Abschlussbericht ausgeben, wenn beide fertig sind
                bool allDone = (leftIndex >= leftRes.events.size()) && (rightIndex >= rightRes.events.size());
                if (allDone && printFinalReport) {
                    printFinalReport = false;
                    std::cout << "\n====================================="
                              << "\n=== SORTIER-VERGLEICH BEENDET ==="
                              << "\nElemente:        " << arraySize
                              << "\nArray-Typ:       " << arrayType
                              << "\n-------------------------------------"
                              << "\nAlgorithmus     | Vergleiche | Tausche"
                              << "\n-------------------------------------"
                              << "\n" << algorithms[selectedLeft].name << " | " << leftRes.stats.comparisons << " | " << leftRes.stats.swaps
                              << "\n" << algorithms[selectedRight].name << " | " << rightRes.stats.comparisons << " | " << rightRes.stats.swaps
                              << "\n=====================================\n"
                              << "Drücke [R] für das Menü." << std::endl;
                }
            }
        }

        // --- Rendern ---
        visualRenderer.clear();

        int winWidth = 0, winHeight = 0;
        SDL_GetWindowSize(window, &winWidth, &winHeight);

        if (inMenu) {
            // --- Menü zeichnen ---
            SDL_Color titleColor = { 100, 149, 237, 255 }; // Cornflower Blue
            SDL_Color subtitleColor = { 150, 150, 160, 255 };

            visualRenderer.drawText("SORTIER-VISUALISIERER VERGLEICH", 140.0f, 60.0f, 13.0f, 22.0f, titleColor);
            visualRenderer.drawText("WAEHLE ZWEI ALGORITHMEN UND STARTE DEN VERGLEICH", 185.0f, 105.0f, 6.0f, 11.0f, subtitleColor);

            // Labels zeichnen
            SDL_Color labelColor = { 130, 130, 140, 255 };
            visualRenderer.drawText("LINKER ALGORITHMUS", 150.0f, 162.0f, 5.0f, 9.0f, labelColor);
            visualRenderer.drawText("RECHTER ALGORITHMUS", 520.0f, 162.0f, 5.0f, 9.0f, labelColor);
            visualRenderer.drawText("ARRAY-GROESSE", 150.0f, 292.0f, 5.0f, 9.0f, labelColor);
            visualRenderer.drawText("INITIALISIERUNG", 520.0f, 292.0f, 5.0f, 9.0f, labelColor);

            // Hauptbuttons zeichnen (Hover ermitteln)
            visualRenderer.drawButton(algorithms[selectedLeft].name, leftAlgoBtn, isPointInRect(mx, my, leftAlgoBtn), leftOpen, true);
            visualRenderer.drawButton(algorithms[selectedRight].name, rightAlgoBtn, isPointInRect(mx, my, rightAlgoBtn), rightOpen, true);
            visualRenderer.drawButton(std::to_string(arraySize) + " ELEMENTE", sizeBtn, isPointInRect(mx, my, sizeBtn), sizeOpen, true);
            visualRenderer.drawButton(arrayType, typeBtn, isPointInRect(mx, my, typeBtn), typeOpen, true);
            visualRenderer.drawButton("VERGLEICH STARTEN", startBtn, isPointInRect(mx, my, startBtn), false, false);

            // Dropdowns (zuletzt drüberzeichnen)
            if (leftOpen) {
                for (std::size_t i = 0; i < algorithms.size(); ++i) {
                    visualRenderer.drawButton(algorithms[i].name, leftOptions[i], isPointInRect(mx, my, leftOptions[i]), i == static_cast<std::size_t>(selectedLeft), false);
                }
            } else if (rightOpen) {
                for (std::size_t i = 0; i < algorithms.size(); ++i) {
                    visualRenderer.drawButton(algorithms[i].name, rightOptions[i], isPointInRect(mx, my, rightOptions[i]), i == static_cast<std::size_t>(selectedRight), false);
                }
            } else if (sizeOpen) {
                for (std::size_t i = 0; i < sizes.size(); ++i) {
                    visualRenderer.drawButton(std::to_string(sizes[i]) + " ELEMENTE", sizeOptions[i], isPointInRect(mx, my, sizeOptions[i]), sizes[i] == static_cast<int>(arraySize), false);
                }
            } else if (typeOpen) {
                for (std::size_t i = 0; i < types.size(); ++i) {
                    visualRenderer.drawButton(types[i], typeOptions[i], isPointInRect(mx, my, typeOptions[i]), types[i] == arrayType, false);
                }
            }
        } else {
            // --- Visualisierungs-Modus ---
            int colWidth = winWidth / 2;
            int viewHeight = winHeight - 240; // 240px Platz für Stats & Infos

            SDL_Rect leftView = { 0, 0, colWidth, viewHeight };
            SDL_Rect rightView = { colWidth, 0, winWidth - colWidth, viewHeight };

            SDL_Rect leftStatsView = { 0, viewHeight, colWidth, 60 };
            SDL_Rect rightStatsView = { colWidth, viewHeight, winWidth - colWidth, 60 };

            SDL_Rect leftInfoView = { 0, viewHeight + 60, colWidth, 180 };
            SDL_Rect rightInfoView = { colWidth, viewHeight + 60, winWidth - colWidth, 180 };

            // 1. Arrays zeichnen
            visualRenderer.render(leftArray, leftHighlightA, leftHighlightB, leftHighlightType, leftSorted, leftView);
            visualRenderer.render(rightArray, rightHighlightA, rightHighlightB, rightHighlightType, rightSorted, rightView);

            // 2. Statistik-Balken zeichnen (B/S/H dynamisch ermittelt)
            char leftLetter = algorithms[selectedLeft].name[0];
            char rightLetter = algorithms[selectedRight].name[0];
            visualRenderer.renderStats(leftLetter, leftComps, leftSwaps, maxCompsGlobal, maxSwapsGlobal, leftStatsView);
            visualRenderer.renderStats(rightLetter, rightComps, rightSwaps, maxCompsGlobal, maxSwapsGlobal, rightStatsView);

            // 3. Informationen & Funktionsweise zeichnen
            // Linkes Info-Panel
            SDL_SetRenderViewport(sdlRenderer, &leftInfoView);
            SDL_SetRenderDrawColor(sdlRenderer, 22, 22, 26, 255);
            SDL_FRect infoRectL = { 0.0f, 0.0f, static_cast<float>(leftInfoView.w), static_cast<float>(leftInfoView.h) };
            SDL_RenderFillRect(sdlRenderer, &infoRectL);
            SDL_SetRenderDrawColor(sdlRenderer, 50, 50, 60, 255);
            SDL_RenderRect(sdlRenderer, &infoRectL);

            SDL_Color nameCol = { 0, 191, 255, 255 }; // Deep Sky Blue
            SDL_Color compCol = { 160, 160, 175, 255 };
            SDL_Color textCol = { 200, 200, 210, 255 };
            visualRenderer.drawText(algorithms[selectedLeft].name, 15.0f, 15.0f, 7.0f, 12.0f, nameCol);
            visualRenderer.drawText(algorithms[selectedLeft].complexity, 15.0f, 35.0f, 5.0f, 9.0f, compCol);
            visualRenderer.drawTextWrapped(algorithms[selectedLeft].description, 15.0f, 55.0f, 5.0f, 9.0f, static_cast<float>(colWidth) - 30.0f, textCol);

            // Farblegende links
            SDL_FRect legCompareL = { 15.0f, 150.0f, 10.0f, 10.0f };
            SDL_FRect legSwapL = { 140.0f, 150.0f, 10.0f, 10.0f };
            SDL_FRect legSortedL = { 250.0f, 150.0f, 10.0f, 10.0f };
            SDL_SetRenderDrawColor(sdlRenderer, 255, 215, 0, 255); SDL_RenderFillRect(sdlRenderer, &legCompareL);
            SDL_SetRenderDrawColor(sdlRenderer, 220, 20, 60, 255); SDL_RenderFillRect(sdlRenderer, &legSwapL);
            SDL_SetRenderDrawColor(sdlRenderer, 50, 205, 50, 255); SDL_RenderFillRect(sdlRenderer, &legSortedL);
            
            SDL_Color legLabelCol = { 140, 140, 150, 255 };
            visualRenderer.drawText("VERGLEICH", 30.0f, 150.0f, 5.0f, 9.0f, legLabelCol);
            visualRenderer.drawText("TAUSCH", 155.0f, 150.0f, 5.0f, 9.0f, legLabelCol);
            visualRenderer.drawText("SORTIERT", 265.0f, 150.0f, 5.0f, 9.0f, legLabelCol);

            // Mute-Indikator linke Seite ([1]-Taste)
            if (soundEngine.isAvailable()) {
                SDL_Color muteColL = muteLeft
                    ? SDL_Color{ 210,  60,  60, 255 }
                    : SDL_Color{  50, 210, 100, 255 };
                visualRenderer.drawText(
                    muteLeft ? "[1] TON: AUS" : "[1] TON: AN ",
                    360.0f, 150.0f, 5.0f, 9.0f, muteColL);
            }
            // Rechtes Info-Panel
            SDL_SetRenderViewport(sdlRenderer, &rightInfoView);
            SDL_SetRenderDrawColor(sdlRenderer, 22, 22, 26, 255);
            SDL_FRect infoRectR = { 0.0f, 0.0f, static_cast<float>(rightInfoView.w), static_cast<float>(rightInfoView.h) };
            SDL_RenderFillRect(sdlRenderer, &infoRectR);
            SDL_SetRenderDrawColor(sdlRenderer, 50, 50, 60, 255);
            SDL_RenderRect(sdlRenderer, &infoRectR);

            visualRenderer.drawText(algorithms[selectedRight].name, 15.0f, 15.0f, 7.0f, 12.0f, nameCol);
            visualRenderer.drawText(algorithms[selectedRight].complexity, 15.0f, 35.0f, 5.0f, 9.0f, compCol);
            visualRenderer.drawTextWrapped(algorithms[selectedRight].description, 15.0f, 55.0f, 5.0f, 9.0f, static_cast<float>(winWidth - colWidth) - 30.0f, textCol);

            // Farblegende rechts
            SDL_FRect legCompareR = { 15.0f, 150.0f, 10.0f, 10.0f };
            SDL_FRect legSwapR = { 140.0f, 150.0f, 10.0f, 10.0f };
            SDL_FRect legSortedR = { 250.0f, 150.0f, 10.0f, 10.0f };
            SDL_SetRenderDrawColor(sdlRenderer, 255, 215, 0, 255); SDL_RenderFillRect(sdlRenderer, &legCompareR);
            SDL_SetRenderDrawColor(sdlRenderer, 220, 20, 60, 255); SDL_RenderFillRect(sdlRenderer, &legSwapR);
            SDL_SetRenderDrawColor(sdlRenderer, 50, 205, 50, 255); SDL_RenderFillRect(sdlRenderer, &legSortedR);

            visualRenderer.drawText("VERGLEICH", 30.0f, 150.0f, 5.0f, 9.0f, legLabelCol);
            visualRenderer.drawText("TAUSCH", 155.0f, 150.0f, 5.0f, 9.0f, legLabelCol);
            visualRenderer.drawText("SORTIERT", 265.0f, 150.0f, 5.0f, 9.0f, legLabelCol);

            // Mute-Indikator rechte Seite ([2]-Taste)
            if (soundEngine.isAvailable()) {
                SDL_Color muteColR = muteRight
                    ? SDL_Color{ 210,  60,  60, 255 }
                    : SDL_Color{  50, 210, 100, 255 };
                visualRenderer.drawText(
                    muteRight ? "[2] TON: AUS" : "[2] TON: AN ",
                    360.0f, 150.0f, 5.0f, 9.0f, muteColR);
            }
        }

        visualRenderer.present();
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
