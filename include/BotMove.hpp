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
    const int basicHeuristicTable[] = {1, 2, 1, 2};
    const int betterHeuristicTable[] = {1, 2, 1, 2, 3, 4, 5, 6};

    enum HeuristicEnum
    {
        BASIC,
        BETTER
    };

    /**
     * @brief Zwraca ruch wykonywany przez bota za pomocą podanej taktyki.
     *
     * @param depth - glebokosc przeszukiwania
     * @return std::pair<int, int> - najlepszy mozliwy ruch
     */
    std::pair<Coord, Coord> bot_move(const GameState &, PlayerEnum player, HeuristicEnum heuristicType, int depth);
    int basic_heuristic(const GameState &gameState, PlayerEnum player, Coord coord);
    int better_heuristic(const GameState &gameState, PlayerEnum player, Coord coord);
    int minimax(Coord coord, int depth, int alpha, int beta, GameState gameState, PlayerEnum player);
} // namespace checkers::bot
