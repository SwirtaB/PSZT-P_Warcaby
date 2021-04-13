/**
 * @file BotMove.hpp
 * @author Bartosz Świrta
 * @brief Zawiera deklaracje metody bot_move - ruch bota
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include "Game.hpp"
#include "Config.hpp"
#include <boost/array.hpp>

namespace checkers::bot
{
    const int basicHeuristicTable[] = {4, 8, 4, 8};
    const int betterHeuristicTable[] = {4, 8, 4, 8, 5, 8, 6, 10};

    /**
     * @brief Zwraca ruch wykonywany przez bota za pomocą podanej taktyki.
     * 
     * @param player - gracz dla którego będzie budowane drzewo
     * @param heuristicType - enumerator używanej heurystyki
     * @param depth - głębokość budowania drzewa gry
     * @return std::pair<Coord, Coord> - współrzędne najlepszego ruchu (skąd dokąd)
     */
    std::pair<Coord, Coord> bot_move(const GameState &, PlayerEnum player, HeuristicEnum heuristicType, int depth);
    /**
     * @brief 
     * 
     * @param gameState
     * @return int - wynik ewaluacji
     */
    int basic_heuristic(const GameState &gameState);
    /**
     *
     * @param gameState
     * @return
     */
    int aggressive_basic_heuristic(const GameState &gameState);
    /**
     *
     * @param gameState
     * @return
     */
    int better_heuristic(const GameState &gameState);
    /**
     *
     * @param gameState
     * @param heuristicType
     * @return
     */
    int estimate_move(const GameState &gameState, HeuristicEnum heuristicType);
    /**
     *
     * @param gameState
     * @param depth
     * @param alpha
     * @param beta
     * @param player
     * @param heuristicType
     * @return
     */
    int minimax(const GameState &gameState, int depth, int alpha, int beta, PlayerEnum player, HeuristicEnum heuristicType);
} // namespace checkers::bot
