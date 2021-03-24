/**
 * @file Utils.hpp
 * @author Bartosz Świrta
 * @brief Zawiera definicje GameProgressEnum i PlayerEnum
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include <optional>

namespace checkers
{
    /** \enum GameProgressEnum
     * @brief Możliwe stany rozgrywki.
     * 
     */
    enum GameProgressEnum
    {
        PLAYING,
        WHITE_WON,
        BLACK_WON,
    };

    /** \enum PlayerEnum
     * @brief Konkretny gracz.
     * 
     */
    enum PlayerEnum
    {
        WHITE,
        BLACK
    };

    enum PawnEnum
    {
        BLACK_PAWN,
        WHITE_PAWN,
        BLACK_QUEEN,
        WHITE_QUEEN
    };

    struct Coord
    {
        int x;
        int y;

        Coord(int x_, int y_) : x(std::max(0, std::min(7, x_))), y(std::max(0, std::min(7, y_)))
        {};
    };

    struct BoardState
    {
        std::optional<PawnEnum> fields[8][8];
        std::optional<Coord> selectedField;
    };

    struct GameState
    {
        BoardState board;
        PlayerEnum currentPlayer;
        GameProgressEnum gameProgress;
    };

} // namespace checkers
