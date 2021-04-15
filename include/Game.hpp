/**
 * @file Game.hpp
 * @author Maciej Wojno
 * @brief 
 * @version 1.0
 * @date 2021-03-24
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
         * @brief Ustawia wartość pola.
         * 
         * @param field Koordynaty pola.
         * @param piece Enumerator bierki.
         */
        void set_field(Coord field, std::optional<PieceEnum> piece);
        /**
         * @brief Wykonuje ruch bierką. Przestawia ją z pola obecnego na pole docelowe.
         * 
         * @param src Obecna pozycja bierki.
         * @param dst Pole docelowe.
         */
        void move_piece(Coord src, Coord dst);
        /**
         * @brief Decyduje, który gracz wykonuje ruch jako kolejny.
         * 
         */
        void flip_current_player();
        /**
         * @brief Zlicza bierki obecnego gracza, które mają dostępne bicie.
         * 
         * @return int liczba bierek z biciem.
         */
        int count_pieces_with_attack() const;
        /**
         * @brief Sprawdza możliwość ruchu bez weryfikacji czy bierka ma dostępne bicia.
         * 
         * @param from Obecne pozycja bierki.
         * @param to Pole docelowe.
         * @return true Można wykonać ruch.
         * @return false Nie można wykonać ruchu.
         */
        bool can_move_piece_relaxed(Coord from, Coord to) const;
        /**
         * @brief Czy bierka ma dostępne bicie.
         * 
         * @param field Koordynaty bierki.
         * @return true 
         * @return false 
         */
        bool piece_has_attacks(Coord field) const;
        /**
         * @brief Zapisuje zakodowany stan planszy. Stany te pozwalają wykryć remis.
         * 
         */
        void push_past_board_state();
        /**
         * @brief Aktualizuje warunki remisu.
         * 
         * @param irreversible Czy został wykonany ruch resetujący warunki remisu, bo stan sprzed tego ruchu jest już niemożliwy do uzyskania.
         */
        void update_tie_conditions(bool irreversible);
        /**
         * @brief Sprawdza warunki remisu.
         * 
         * @return true Wystąpił remis.
         * @return false Remis nie wystąpił.
         */
        bool has_tie_happened() const;
        /**
         * @brief Sprawdza czy nie ma bierki między dwoma polami.
         * 
         * @param start Pole początkowe.
         * @param end Pole końcowe.
         * @return true Między polami nie ma bierki.
         * @return false Między polami jest bierka.
         */
        bool is_empty_between(Coord start, Coord end) const;
        /**
         * @brief Sprawdza czy między polami jest tylko jedna bierka.
         * 
         * @param start Pole początkowe.
         * @param end Pole końcowe.
         * @return true Między polami jest tylko jedna bierka.
         * @return false Między polami nie ma żadnej bierki.
         */
        bool is_only_one_enemy_between(Coord start, Coord end) const;
        /**
         * @brief Aktualizuje stan rozgrywki.
         * 
         */
        void update_game_progress();
        /**
         * @brief Czy bierka jest gracza obecnie wykonującego ruch.
         * 
         * @param field Koordynaty bierki.
         * @return true 
         * @return false 
         */
        bool is_current_player_piece(Coord field) const;
        /**
         * @brief Czy bierka należy do przeciwnika.
         * 
         * @param field Koordynaty bierki.
         * @return true 
         * @return false 
         */
        bool is_enemy_player_piece(Coord field) const;
        /**
         * @brief Ściąga zbity pion z planszy.
         * 
         * @param start Pole początkowe ruchu.
         * @param end Pole końcowe ruchu.
         */
        void clear_between(Coord start, Coord end);

        static bool is_in_board(int x, int y) {
            return x >= 0 && x < 8 && y >= 0 && y < 8;
        }
    };

} // namespace checkers

