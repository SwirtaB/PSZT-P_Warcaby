/**
 * @file BotMove.hpp
 * @author Bartosz Świrta
 * @brief Tablice wag heurystyk, deklaracje funkcji implementujących bota.
 * @version 1.0
 * @date 2021-04-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include "Game.hpp"
#include "Config.hpp"

namespace checkers::bot
{
    const int basicHeuristicTable[] = {4, 8, 4, 8};
    const int boardAwareHeuristicTable[] = {4, 8, 4, 8, 5, 6, 7, 8};

    /**
     * @brief Zwraca ruch wykonywany przez bota za pomocą podanej taktyki.
     * @param player - gracz dla którego będzie budowane drzewo
     * @param heuristicType - enumerator używanej heurystyki
     * @param depth - głębokość budowania drzewa gry
     * @return std::pair<Coord, Coord> - współrzędne najlepszego ruchu (skąd dokąd)
     */
    std::pair<Coord, Coord> bot_move(const GameState &, PlayerEnum player, HeuristicEnum heuristicType, int depth);
    /**
     * @brief Heurystyka bierze pod uwagę ilość własnych bierek i bierek przeciwnika z wagami.
     * @param gameState - rozpatrywany stan gry
     * @return int - wynik ewaluacji
     */
    int basic_heuristic(const GameState &gameState);
    /**
     * @brief Modyfikacja basic_heuristic, która premiuje przesuwanie pionów na połowę przeciwnika.
     * @param gameState - rozpatrywany stan gry
     * @return - jakość danego satnu
     */
    int aggressive_basic_heuristic(const GameState &gameState);
    /**
     * @brief Heurystyka bierze pod uwagę ilość własnych bierek i bierek przeciwnika z wagami. Dodatkowo premiuje pola bezpieczne i te blisko przemiany.
     * @param gameState - rozpatrywany stan gry
     * @return - jakość danego stanu
     */
    int better_heuristic(const GameState &gameState);
    /**
     * @brief Weryfikuje czy doszło do zakończenia rozgrywki (kto wygrał, bądź remis). Jeżeli nie to wywołuje stosowną funkcję heurystyczną.
     * @param gameState - rozpatrywany stan gry
     * @param heuristicType - typ heurystyki używanej przez bota
     * @return - jakość danego stanu
     */
    int estimate_move(const GameState &gameState, HeuristicEnum heuristicType);
    /**
     * @brief Implementuje algorytm minimax z przycinaniem alpha-beta
     * @param gameState - rozpatrywany stan gry
     * @param depth - głębokość przeszukiwania
     * @param alpha - wartość zmiennej alfa (alpha-beta pruning)
     * @param beta - wartość zmiennej beta (alpha-beta pruning)
     * @param player - gracz wykonujący ruch
     * @param heuristicType - typ heurystyki jaką posługuje się bot
     * @return - jakość danego stanu
     */
    int minimax(const GameState &gameState, int depth, int alpha, int beta, PlayerEnum player, HeuristicEnum heuristicType);
} // namespace checkers::bot
