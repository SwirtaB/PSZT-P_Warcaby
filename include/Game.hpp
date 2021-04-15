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
     * @brief Możliwe stany fazy rozgrywki.
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

    /** \enum PlayerEnum
     * @brief Rodzaje bierek.
     *
     */
    enum PieceEnum
    {
        BLACK_PAWN,
        WHITE_PAWN,
        BLACK_QUEEN,
        WHITE_QUEEN
    };

    /** \struct Coord
     * @brief Współrzędne na planszy.
     */
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

    /** \struct BoardState
     * @brief Stan planszy gry.
     */
    struct BoardState
    {
        std::optional<PieceEnum> fields[8][8];
    };

    /**
     * @brief Stan gry. Pozwala na modyfikację tylko poprzez prowadzenie poprawnej rozgrywki.
     */
    class GameState
    {
    public:
        /**
         * @brief Inicjalizacja początkowego stanu gry.
         */
        void init();
        /**
         * @return Kopia stanu planszy.
         */
        BoardState get_board_state() const;
        /**
         * @return Gracz który obecnie ma wykonać ruch.
         */
        PlayerEnum get_current_player() const;
        /**
         * @return Obecny stan fazy rozgrywki.
         */
        GameProgressEnum get_game_progress() const;
        /**
         * @return Kopia pola o podanych współrzędnych.
         */
        std::optional<PieceEnum> get_field(Coord field) const;
        /**
         * @return Czy można wybrać to pole = czy na tym polu jest bierka, która ma możliwy ruch.
         */
        bool can_select_field(Coord field) const;
        /**
         * @brief Próba wykonania ruchu.
         *
         * @param from Pole na którym stoi bierka, którą próbujemy wykonać ruch.
         * @param to Pole na które chcemy postawić bierke.
         * @return Czy udało się wykonać ten ruch.
         */
        bool try_make_move(Coord from, Coord to);
        /**
         * @return Ostatni ruch obecnego gracza (istnieje jeśli jesteśmy w trakcie łańcucha bić).
         */
        std::optional<Coord> get_last_move() const;
        /**
         * @brief Sprawdzenie czy można wykonać ruch.
         *
         * @param from Pole na którym stoi bierka, którą próbujemy wykonać ruch.
         * @param to Pole na które chcemy postawić bierke.
         * @return Czy można wykonać taki ruch.
         */
        bool can_move_piece(Coord from, Coord to) const;
        /**
         * @return Wektor pozycji na których stoją bierki, które mają mozliwy ruch.
         */
        std::vector<Coord> pieces_with_moves() const;
        /**
         * @return Lista możliwych ruchów bierki stojącej na polu o podanych współrzędnych.
         */
        std::vector<Coord> piece_moves(Coord field) const;

    private:
        /// Obecny stan planszy.
        BoardState board;
        /// Gracz który ma wykonać następny ruch.
        PlayerEnum currentPlayer;
        /// Obecna faza rozgrywki.
        GameProgressEnum gameProgress;
        /// Poprzedni ruch w łańcuchu bić.
        /// Staje się std::nullopt po skończeniu tury gracza.
        std::optional<Coord> lastMove;
        /// Ilość ruchów pod rząd wykonanych królowymi bez bicia.
        int queenMovesNoTake = 0;
        /// Poprzednie stany planszy.
        /// Czyszczone po wykonaniu nieodwracalnego ruchu (ruch pionkiem lub bicie).
        std::vector<std::string> pastBoardStates;

        /**
         * @param field
         * @param piece
         */
        void set_field(Coord field, std::optional<PieceEnum> piece);
        void move_piece(Coord src, Coord dst);
        void flip_current_player();
        int count_pieces_with_attack() const;
        // relaxed bo nie sprawdza czy są inne możliwe ruchy bijące
        bool can_move_piece_relaxed(Coord from, Coord to) const;
        bool piece_has_attacks(Coord field) const;
        void push_past_board_state();
        void update_tie_conditions(bool irreversible);
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

