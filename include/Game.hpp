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
        void init() {
            for (size_t x = 0; x < 8; ++x) {
                for (size_t y = 0; y < 8; ++y) {
                    if ((x + y) % 2 == 0 && y < 3) {
                        board.fields[x][y] = WHITE_PAWN;
                    } else if ((x + y) % 2 == 0 && y > 4) {
                        board.fields[x][y] = BLACK_PAWN;
                    } else {
                        board.fields[x][y] = std::nullopt;
                    }
                }
            }
            gameProgress = PLAYING;
            currentPlayer = WHITE;
            lastMove = std::nullopt;
        }

        BoardState get_board_state() const {
            return board;
        }

        PlayerEnum get_current_player() const {
            return currentPlayer;
        }

        GameProgressEnum get_game_progress() const {
            return gameProgress;
        };

        std::optional<PieceEnum> get_field(Coord field) const {
            return board.fields[field.x][field.y];
        };

        bool can_select_field(Coord field) const {
            if (lastMove.has_value() && lastMove.value() != field) return false;
            if (!is_current_player_piece(field)) return false;
            if (piece_moves(field).size() == 0) return false;
            if (count_pieces_with_attack() > 0 && !piece_has_attacks(field)) return false;
            return true;
        }

        bool try_make_move(Coord from, Coord to) {
            if (can_move_piece(from, to)) {
                move_piece(from, to);

                bool attacked = false;
                if (!is_empty_between(from, to)) {
                    attacked = true;
                    clear_between(from, to);
                }

                if (to.y == 7 && currentPlayer == WHITE && get_field(to).value() == WHITE_PAWN) {
                    set_field(to, WHITE_QUEEN);
                } else if (to.y == 0 && currentPlayer == BLACK && get_field(to).value() == BLACK_PAWN) {
                    set_field(to, BLACK_QUEEN);
                }

                if (!attacked || !piece_has_attacks(to)) {
                    flip_current_player();
                }
                update_game_progress();
                return true;
            } else {
                return false;
            }
        };

        std::optional<Coord> get_last_move() const {
            return lastMove;
        }

        bool can_move_piece(Coord from, Coord to) const {
            if (!can_move_piece_relaxed(from, to)) return false;
            int attacking = count_pieces_with_attack();
            if (attacking > 0 && !piece_has_attacks(from)) return false;
            if (piece_has_attacks(from) && is_empty_between(from, to)) return false;
            return true;
        };

        std::vector<Coord> pieces_with_moves() const {
            std::vector<Coord> pieces;
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    if (piece_moves(Coord(x, y)).size() > 0) {
                        pieces.push_back(Coord(x, y));
                    }
                }
            }
            return pieces;
        };

        std::vector<Coord> piece_moves(Coord field) const {
            std::vector<Coord> vec;
            if (!get_field(field).has_value()) return vec;

            switch (get_field(field).value()) {
                case WHITE_PAWN:
                    if (is_in_board(field.x - 1, field.y + 1) && can_move_piece(field, field + Coord(-1, 1))) {
                        vec.push_back(field + Coord(-1, 1));
                    }
                    if (is_in_board(field.x + 1, field.y + 1) && can_move_piece(field, field + Coord(1, 1))) {
                        vec.push_back(field + Coord(1, 1));
                    }
                    if (is_in_board(field.x - 2, field.y + 2) && can_move_piece(field, field + Coord(-2, 2))) {
                        vec.push_back(field + Coord(-2, 2));
                    }
                    if (is_in_board(field.x + 2, field.y + 2) && can_move_piece(field, field + Coord(2, 2))) {
                        vec.push_back(field + Coord(2, 2));
                    }
                    if (is_in_board(field.x - 2, field.y - 2) && can_move_piece(field, field + Coord(-2, -2))) {
                        vec.push_back(field + Coord(-2, -2));
                    }
                    if (is_in_board(field.x + 2, field.y - 2) && can_move_piece(field, field + Coord(2, -2))) {
                        vec.push_back(field + Coord(2, -2));
                    }
                    break;

                case BLACK_PAWN:
                    if (is_in_board(field.x - 1, field.y - 1) && can_move_piece(field, field + Coord(-1, -1))) {
                        vec.push_back(field + Coord(-1, -1));
                    }
                    if (is_in_board(field.x + 1, field.y - 1) && can_move_piece(field, field + Coord(1, -1))) {
                        vec.push_back(field + Coord(1, -1));
                    }
                    if (is_in_board(field.x - 2, field.y - 2) && can_move_piece(field, field + Coord(-2, -2))) {
                        vec.push_back(field + Coord(-2, -2));
                    }
                    if (is_in_board(field.x + 2, field.y - 2) && can_move_piece(field, field + Coord(2, -2))) {
                        vec.push_back(field + Coord(2, -2));
                    }
                    if (is_in_board(field.x + 2, field.y + 2) && can_move_piece(field, field + Coord(2, 2))) {
                        vec.push_back(field + Coord(2, 2));
                    }
                    if (is_in_board(field.x - 2, field.y + 2) && can_move_piece(field, field + Coord(-2, 2))) {
                        vec.push_back(field + Coord(-2, 2));
                    }
                    break;

                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    for (int dx = 1, dy = 1; dx < 8; ++dx, ++dy) {
                        if (is_in_board(field.x + dx, field.y + dy)
                        && can_move_piece(field, field + Coord(dx, dy))) {
                            vec.push_back(field + Coord(dx, dy));
                        }
                        if (is_in_board(field.x - dx, field.y + dy)
                        && can_move_piece(field, field + Coord(-dx, dy))) {
                            vec.push_back(field + Coord(-dx, dy));
                        }
                        if (is_in_board(field.x + dx, field.y - dy)
                        && can_move_piece(field, field + Coord(dx, -dy))) {
                            vec.push_back(field + Coord(dx, -dy));
                        }
                        if (is_in_board(field.x - dx, field.y - dy)
                        && can_move_piece(field, field - Coord(-dx, -dy))) {
                            vec.push_back(field - Coord(-dx, -dy));
                        }
                    }
            }
            return vec;
        };

    private:
        BoardState board;
        PlayerEnum currentPlayer;
        GameProgressEnum gameProgress;
        std::optional<Coord> lastMove;

        void set_field(Coord field, std::optional<PieceEnum> piece) {
            board.fields[field.x][field.y] = piece;
        }

        void move_piece(Coord src, Coord dst) {
            auto piece = get_field(src);
            set_field(src, std::nullopt);
            set_field(dst, piece);
        }

        void flip_current_player() {
            if (currentPlayer == WHITE) {
                currentPlayer = BLACK;
            } else {
                currentPlayer = WHITE;
            }
            lastMove = std::nullopt;
        }

        int count_pieces_with_attack() const {
            int count = 0;
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    if (is_current_player_piece(Coord(x, y))
                    && piece_has_attacks(Coord(x, y))) {
                        ++count;
                    }
                }
            }
            return count;
        }

        // relaxed bo nie sprawdza czy są inne możliwe ruchy bijące
        bool can_move_piece_relaxed(Coord from, Coord to) const {
            if (!get_field(from).has_value()) return false;
            if (get_field(to).has_value()) return false;
            if (is_enemy_player_piece(from)) return false;
            if (lastMove.has_value() && lastMove.value() != from) return false;

            switch (get_field(from).value()) {
                case WHITE_PAWN:
                    if (to != from + Coord(1, 1) && to != from + Coord(-1, 1)
                    && to != from + Coord(2, 2) && to != from + Coord(-2, 2)
                    && to != from + Coord(2, -2) && to != from + Coord(-2, -2)) return false;
                    if (to == from + Coord(1, 1) && is_in_board(to.x, to.y)
                    && get_field(to).has_value()) return false;
                    if (to == from + Coord(-1, 1) && is_in_board(to.x, to.y)
                    && get_field(to).has_value()) return false;
                    if (to == from + Coord(2, 2) && is_in_board(to.x, to.y)
                    && (!is_enemy_player_piece(from + Coord(1, 1))
                        || get_field(to).has_value() )) return false;
                    if (to == from + Coord(-2, 2) && is_in_board(to.x, to.y)
                    && (!is_enemy_player_piece(from + Coord(-1, 1))
                        || get_field(to).has_value() )) return false;
                    if (to == from + Coord(2, -2) && is_in_board(to.x, to.y)
                        && (!is_enemy_player_piece(from + Coord(1, -1))
                            || get_field(to).has_value() )) return false;
                    if (to == from + Coord(-2, -2) && is_in_board(to.x, to.y)
                        && (!is_enemy_player_piece(from + Coord(-1, -1))
                            || get_field(to).has_value() )) return false;
                    break;

                case BLACK_PAWN:
                    if (to != from + Coord(1, -1) && to != from + Coord(-1, -1)
                    && to != from + Coord(2, -2) && to != from + Coord(-2, -2)
                    && to != from + Coord(2, 2) && to != from + Coord(-2, 2)) return false;
                    if (to == from + Coord(1, -1) && is_in_board(to.x, to.y)
                    && get_field(to).has_value()) return false;
                    if (to == from + Coord(-1, -1) && is_in_board(to.x, to.y)
                    && get_field(to).has_value()) return false;
                    if (to == from + Coord(2, -2) && is_in_board(to.x, to.y)
                    && (!is_enemy_player_piece(from + Coord(1, -1))
                        || get_field(to).has_value() )) return false;
                    if (to == from + Coord(-2, -2) && is_in_board(to.x, to.y)
                    && (!is_enemy_player_piece(from + Coord(-1, -1))
                        || get_field(to).has_value() )) return false;
                    if (to == from + Coord(2, 2) && is_in_board(to.x, to.y)
                        && (!is_enemy_player_piece(from + Coord(1, 1))
                            || get_field(to).has_value() )) return false;
                    if (to == from + Coord(-2, 2) && is_in_board(to.x, to.y)
                        && (!is_enemy_player_piece(from + Coord(-1, 1))
                            || get_field(to).has_value() )) return false;
                    break;

                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    if (abs(from.x - to.x) != abs(from.y - to.y)) return false;
                    if (is_in_board(to.x, to.y)
                        && (get_field(to).has_value()
                            || !(is_only_one_enemy_between(from, to)
                                || is_empty_between(from, to)))) return false;
            }

            return true;
        }

        bool piece_has_attacks(Coord field) const {
            if (!get_field(field).has_value()) return false;

            switch (get_field(field).value()) {
                case WHITE_PAWN:
                case BLACK_PAWN:
                    if (is_in_board(field.x - 2, field.y + 2) && can_move_piece_relaxed(field, field + Coord(-2, 2))) {
                        return true;
                    }
                    if (is_in_board(field.x + 2, field.y + 2) && can_move_piece_relaxed(field, field + Coord(2, 2))) {
                        return true;
                    }
                    if (is_in_board(field.x - 2, field.y - 2) && can_move_piece_relaxed(field, field + Coord(-2, -2))) {
                        return true;
                    }
                    if (is_in_board(field.x + 2, field.y - 2) && can_move_piece_relaxed(field, field + Coord(2, -2))) {
                        return true;
                    }
                    break;

                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    for (int dx = 1, dy = 1; dx < 8; ++dx, ++dy) {
                        if (is_in_board(field.x + dx, field.y + dy)
                        && !get_field(field + Coord(dx, dy)).has_value()
                        && is_only_one_enemy_between(field, field + Coord(dx, dy))) return true;
                        if (is_in_board(field.x - dx, field.y + dy)
                            && !get_field(field + Coord(-dx, dy)).has_value()
                            && is_only_one_enemy_between(field, field + Coord(-dx, dy))) return true;
                        if (is_in_board(field.x + dx, field.y - dy)
                            && !get_field(field + Coord(dx, -dy)).has_value()
                            && is_only_one_enemy_between(field, field + Coord(dx, -dy))) return true;
                        if (is_in_board(field.x - dx, field.y - dy)
                            && !get_field(field + Coord(-dx, -dy)).has_value()
                            && is_only_one_enemy_between(field, field + Coord(-dx, -dy))) return true;
                    }
            }
            return false;
        }

        bool has_tie_happened() const {
            return false;
        }

        bool is_empty_between(Coord start, Coord end) const {
            int dx, dy;
            if (start.x < end.x) {
                dx = 1;
            } else {
                dx = -1;
            }
            if (start.y < end.y) {
                dy = 1;
            } else {
                dy = -1;
            }
            for (int i = 1; i < abs(end.x - start.x); ++i) {
                if (get_field(start + Coord(dx * i, dy * i)).has_value()) {
                    return false;
                }
            }
            return true;
        }

        bool is_only_one_enemy_between(Coord start, Coord end) const {
            int dx, dy;
            if (start.x < end.x) {
                dx = 1;
            } else {
                dx = -1;
            }
            if (start.y < end.y) {
                dy = 1;
            } else {
                dy = -1;
            }
            int count = 0;
            for (int i = 1; i < abs(end.x - start.x); ++i) {
                if (get_field(start + Coord(dx * i, dy * i)).has_value()) {

                }
                if (is_enemy_player_piece(start + Coord(dx * i, dy * i))) {
                    ++count;
                } else if (is_current_player_piece(start + Coord(dx * i, dy * i))) {
                    return false;
                }
            }
            return count == 1;
        }

        void update_game_progress() {
            if (has_tie_happened()) {
                gameProgress = TIE;
            } else if (currentPlayer == BLACK && pieces_with_moves().size() == 0) {
                gameProgress = WHITE_WON;
            } else if (currentPlayer == WHITE && pieces_with_moves().size() == 0) {
                gameProgress = BLACK_WON;
            }
        };

        bool is_current_player_piece(Coord field) const {
            if (!get_field(field).has_value()) return false;
            if (currentPlayer == WHITE
            && !(get_field(field).value() == WHITE_PAWN || get_field(field).value() == WHITE_QUEEN)) return false;
            if (currentPlayer == BLACK
            && !(get_field(field).value() == BLACK_PAWN || get_field(field).value() == BLACK_QUEEN)) return false;
            return true;
        }

        bool is_enemy_player_piece(Coord field) const {
            if (!get_field(field).has_value()) return false;
            if (currentPlayer == WHITE
                && (get_field(field).value() == WHITE_PAWN || get_field(field).value() == WHITE_QUEEN)) return false;
            if (currentPlayer == BLACK
                && (get_field(field).value() == BLACK_PAWN || get_field(field).value() == BLACK_QUEEN)) return false;
            return true;
        }

        void clear_between(Coord start, Coord end) {
            int dx, dy;
            if (start.x < end.x) {
                dx = 1;
            } else {
                dx = -1;
            }
            if (start.y < end.y) {
                dy = 1;
            } else {
                dy = -1;
            }
            for (int i = 1; i < abs(end.x - start.x); ++i) {
                set_field(start + Coord(dx * i, dy * i), std::nullopt);
            }
        }

        static bool is_in_board(int x, int y) {
            return x >= 0 && x < 8 && y >= 0 && y < 8;
        }
    };

} // namespace checkers

