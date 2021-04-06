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

namespace checkers::bot
{
    /**
     * @brief Zwraca ruch wykonywany przez bota za pomocą podanej taktyki.
     *
     * @param depth - glebokosc przeszukiwania
     * @return std::pair<int, int> - najlepszy mozliwy ruch
     */
    std::pair<int, int> bot_move(const GameState &, int depth);
} // namespace checkers::bot
