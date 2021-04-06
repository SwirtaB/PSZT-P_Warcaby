/**
 * @file Game.hpp
 * @author Maciej Wojno
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include <optional>
#include <vector>
#include <iostream>

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
        TIE,
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

    enum PieceEnum
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

        Coord(int x_, int y_) : x(x_), y(y_)
        {};

        int operator== (Coord other) const {
            return x == other.x && y == other.y;
        }

        int operator!= (Coord other) const {
            return x != other.x || y != other.y;
        }

        Coord operator+ (Coord other) const {
            return Coord(x + other.x, y + other.y);
        }

        Coord operator- (Coord other) const {
            return Coord(x - other.x, y - other.y);
        }
    };

    struct BoardState
    {
        std::optional<PieceEnum> fields[8][8];
    };

    class GameState
    {
    public:
        void init();
        BoardState get_board_state() const;
        PlayerEnum get_current_player() const;
        GameProgressEnum get_game_progress() const;
        std::optional<PieceEnum> get_field(Coord field) const;
        bool can_select_field(Coord field) const;
        bool try_make_move(Coord from, Coord to);
        std::optional<Coord> get_last_move() const;
        bool can_move_piece(Coord from, Coord to) const;
        std::vector<Coord> pieces_with_moves() const;
        std::vector<Coord> piece_moves(Coord field) const;

    private:
        BoardState board;
        PlayerEnum currentPlayer;
        GameProgressEnum gameProgress;
        std::optional<Coord> lastMove;

        void set_field(Coord field, std::optional<PieceEnum> piece);
        void move_piece(Coord src, Coord dst);
        void flip_current_player();
        int count_pieces_with_attack() const;
        // relaxed bo nie sprawdza czy są inne możliwe ruchy bijące
        bool can_move_piece_relaxed(Coord from, Coord to) const;
        bool piece_has_attacks(Coord field) const;
        bool has_tie_happened() const;
        bool is_empty_between(Coord start, Coord end) const;
        bool is_only_one_enemy_between(Coord start, Coord end) const;
        void update_game_progress();
        bool is_current_player_piece(Coord field) const;
        bool is_enemy_player_piece(Coord field) const;
        void clear_between(Coord start, Coord end);

        static bool is_in_board(int x, int y) {
            return x >= 0 && x < 8 && y >= 0 && y < 8;
        }
    };

} // namespace checkers

