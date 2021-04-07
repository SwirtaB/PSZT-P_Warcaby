/**
 * @file BotMove.cpp
 * @author Bartosz Świrta
 * @brief Zawiera definicje metody bot_move, estimate_move - heurystyka ewaluujaca ruch, minimax - implementuje algorytm minimax
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "../include/BotMove.hpp"
#include <climits>

using namespace checkers;
using namespace checkers::bot;

/**
 * @brief Implemetuje obsluge ruchu bot'a. Wywoluje odpowiednie funkcje w zaleznosci od wartosci wczytanych z pliku konfiguracyjnego.
 *
 * @param fieldBoard - rozpatrywana plansza
 * @param player - ktory gracz ma wykonac ruch
 * @param tactic - taktyka uzywana przez bot'a
 * @param depth - glebokosc przegladania
 * @return std::pair<int, int> - najlepszy mozliwy ruch
 */
std::pair<Coord, Coord> checkers::bot::bot_move(const GameState &gameState, PlayerEnum player, HeuristicEnum heuristicType, int depth)
{
    GameState localState = gameState;
    std::pair<Coord, Coord> bestMove = std::make_pair(Coord(0,0), Coord(0,0));
    int bestScore = 0, score = 0;

    switch (player)
    {
    case BLACK:
        bestScore = INT_MAX;
        for(auto piece : localState.pieces_with_moves()) {
            for (auto move : localState.piece_moves(piece)) {
                if (localState.try_make_move(piece, move))
                    score = minimax(move, depth - 1, INT_MIN, INT_MAX, localState, WHITE);

                if (bestScore > score)
                {
                    bestScore = score;
                    bestMove.first = piece;
                    bestMove.second = move;
                }
            }
        }
        break;
    case WHITE:
        bestScore = INT_MIN;
        for(auto piece : localState.pieces_with_moves()) {
            for (auto move : localState.piece_moves(piece)) {
                if (localState.try_make_move(piece, move))
                    score = minimax(move, depth - 1, INT_MIN, INT_MAX, localState, BLACK);

                if (bestScore < score)
                {
                    bestScore = score;
                    bestMove.first = piece;
                    bestMove.second = move;
                }
            }
        }
        break;
    default:
        break;
    }
    return bestMove;
}

/* Biały MAKSYMALIZUJE wynik, czarny MINIMALIZUJE wynik
 * int basicHeuristicTable = {  waga mojego piona,      - mm
                                waga mojej damy,        - mk 
                                waga piona przeciwnika, - hm
                                waga damy przeciwnika   - hk}
 * function - H = mk*<ilość moich damek> + mm*<ilość moich pionów> - (hk*<ilość wrogich damek> + hm*<ilość wrogich pionów>)
 * 
 * int betterHeuristicTable = { waga mojego piona,                                      - mm  
 *                              waga mojej damy,                                        - mk
 *                              waga pól brzegowych (zajętych przez moje bierki),       - msf
 *                              waga pól brzegowych (zajętych przez wrogie bierki),     - hsf
 *                              waga pól bliskich polom promocji (dla moich pionów),    - mpf
 *                              waga pól bliskich polom promocji (dla wrogich pionów)   - hpf}
 * 
 * function - H = mk*<ilość moich damek> + mm*<ilość moich pionów> + msf*<ilość moich bierek na bezpiecznych polach> + mpf*<ilość moich bierek na polach bliskich polom promocji>
 *              - (hk*<ilość wrogich damek> + hm*<ilość wrogich pionów> + hsf*<ilość wrogich bierek na bezpiecznych polach> + hpf*<ilość wrogich bierek na polach bliskich polom promocji>)
 * 
 * Możliwe jest dodanie heurystyki końcowej. W momencie gdy na planszy są już tylko damki stosujemy inną heurystykę.
 * Przykład suma odległości:
 *  Dla każdej damki liczymy sumę odległości od wrogich damek -> Preferujemy ucieczkę
 *  Dodatkwo możliwość zbicia wrogiej damki jest bardzo wysoko punktowana.
 * 
 * Podsumowując, uciekamy jak tylko się da, chyba że pojawia się okazja zbicia damy przeciwnika
*/ 

int checkers::bot::basic_heuristic(const GameState &gameState, PlayerEnum player, Coord coord){
    BoardState board = gameState.get_board_state();
    int score = 0;
    int whitePawns = 0, whiteQueens = 0, blackPawns = 0, blackQueens = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(board.fields[i][j].has_value()){
                switch (board.fields[i][j].value())
                {
                    case WHITE_PAWN:
                        ++whitePawns;
                        break;
                    case WHITE_QUEEN:
                        ++whiteQueens;
                        break;
                    case BLACK_PAWN:
                        ++blackPawns;
                        break;
                    case BLACK_QUEEN:
                        ++blackQueens;
                        break;
                }
            } 
        }
    }
    switch (player)
    {
        case WHITE:
            score = basicHeuristicTable[1]*whiteQueens + basicHeuristicTable[0]*whitePawns - (basicHeuristicTable[3]*blackQueens + basicHeuristicTable[2]*blackPawns);
            break;
        case BLACK:
            score = basicHeuristicTable[3]*blackQueens + basicHeuristicTable[2]*blackPawns - (basicHeuristicTable[1]*whiteQueens + basicHeuristicTable[0]*whitePawns);
    }
    return score;    
}

int checkers::bot::better_heuristic(const GameState &gameState, PlayerEnum player, Coord coord) {
    BoardState board = gameState.get_board_state();
    int score = 0;
    int whitePawns = 0, whiteQueens = 0, blackPawns = 0, blackQueens = 0;
    int myPieceSafe = 0, myPieceNear = 0, hostilePieceSafe = 0, hostilePieceNear = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(board.fields[i][j].has_value()){
                switch (board.fields[i][j].value())
                {
                    case WHITE_PAWN:
                        ++whitePawns;
                        break;
                    case WHITE_QUEEN:
                        ++whiteQueens;
                        break;
                    case BLACK_PAWN:
                        ++blackPawns;
                        break;
                    case BLACK_QUEEN:
                        ++blackQueens;
                        break;
                }
                switch (player)
                {
                    case WHITE:
                        if(i == 0 || i == 7){
                            if (board.fields[i][j].value() == WHITE_PAWN || board.fields[i][j].value() == WHITE_QUEEN)
                                ++myPieceSafe;
                            else if (board.fields[i][j].has_value())
                                ++hostilePieceSafe;
                        }
                        else if(j > 4 && i > 0 && i < 7){
                            if (board.fields[i][j].value() == WHITE_PAWN || board.fields[i][j].value() == WHITE_QUEEN)
                                ++myPieceNear;
                            else if (board.fields[i][j].has_value())
                                ++hostilePieceNear;
                        }
                        break;
                    case BLACK:
                        if(i == 0 || i == 7){
                            if (board.fields[i][j].value() == BLACK_PAWN || board.fields[i][j].value() == BLACK_QUEEN)
                                ++myPieceSafe;
                            else if (board.fields[i][j].has_value())
                                ++hostilePieceSafe;
                        }
                        else if(j < 3 && i > 0 && i < 7){
                            if (board.fields[i][j].value() == BLACK_PAWN || board.fields[i][j].value() == BLACK_QUEEN)
                                ++myPieceNear;
                            else if (board.fields[i][j].has_value())
                                ++hostilePieceNear;
                        }
                        break;
                }
            }
        }
    }
    switch (player)
    {
        case WHITE:
            score = betterHeuristicTable[1]*whiteQueens + betterHeuristicTable[0]*whitePawns + betterHeuristicTable[4]*myPieceSafe
                    + betterHeuristicTable[6]*myPieceNear - (betterHeuristicTable[3]*blackQueens + betterHeuristicTable[2]*blackPawns
                    + betterHeuristicTable[5]*hostilePieceSafe + betterHeuristicTable[7]*hostilePieceNear);
            break;
        case BLACK:
            score = betterHeuristicTable[3]*blackQueens + betterHeuristicTable[2]*blackPawns + betterHeuristicTable[5]*myPieceSafe
                    + betterHeuristicTable[7]*myPieceNear - (betterHeuristicTable[1]*whiteQueens + betterHeuristicTable[0]*whitePawns
                    + betterHeuristicTable[4]*hostilePieceSafe + betterHeuristicTable[6]*hostilePieceNear);
    }
    return score;
}

/**
 * @brief 
 * 
 * @param game - obecnie rozpatrywana plansza
 * @param player - który gracz wykonuj ruch
 * @param coord - struktura z współrzędnymi rozpatrywanego ruchu
 * @return int - jakosc danej planszy (w konsekwencji rowniez rozpatrywanego ruchu)
 */
int estimate_move(const GameState &gameState, PlayerEnum player, Coord coord)
{
    switch(gameState.get_game_progress())
    {
        case WHITE_WON:
            return 100;
        case BLACK_WON:
            return -100;
        case TIE:
            return 0;
        default:
            break;
    }
    //int score = basic_heuristic(gameState, player, coord);
    int score = better_heuristic(gameState, player, coord);
    // int score = 0;
    // switch(player)
    // {
    //     case WHITE:
    //         //funkcja heurystyczna
    //         score = 0;
    //         break;
    //     case BLACK:
    //         //funkcja heurystyczna
    //         score = 0;
    //         break;
    //     default:
    //         score = 0;
    //         break;
    // }
    return score;
}



/**
* @brief Implementuje algorytm minimax z przycinaniem alpha-beta
*
* @param coord - koordynaty rozpatrywanej figury
* @param depth - gleboksc przegladania
* @param alpha - wartosc zmiennej alpha (alpha-beta pruning)
* @param beta - wartosc zmiennej beta (alpha-beta pruning)
* @param gameField - obecnie rozpatrywana plansza
* @param player - gracz wykonujacy ruch
* @return int - jakosc danej planszy
*/
int checkers::bot::minimax(Coord coord, int depth, int alpha, int beta, GameState game, PlayerEnum player)
{
    GameState localState = game;
    if (!depth || localState.get_game_progress() != PLAYING)
    {
        return estimate_move(localState, player, coord);
    }
    int bestScore = 0, score = 0;
    switch (player)
    {
    case BLACK:
        bestScore = INT_MAX;
        for(auto piece : localState.pieces_with_moves()){
            for(auto move : localState.piece_moves(piece)){
                if(localState.try_make_move(piece, move))
                    score = minimax(move, depth - 1, alpha, beta, localState, WHITE);
                
                //alpha-beta pruning (dwie linie)
                bestScore = std::min(beta, score);
                if(beta <= alpha)
                    return bestScore;
            }
        }
        break;
    case WHITE:
        bestScore = INT_MIN;
        for(auto piece : localState.pieces_with_moves()){
            for(auto move : localState.piece_moves(piece)){
                if(localState.try_make_move(piece, move))
                    score = minimax(move, depth - 1, alpha, beta, localState, BLACK);
                
                //alpha-beta pruning (dwie linie)
                bestScore = std::max(alpha, score);
                if(beta <= alpha)
                    return bestScore;
            }
        }
        // for (int y = 0; y < 3; ++y)
        // {
        //     for (int x = 0; x < 3; ++x)
        //     {
        //         if (gameField.get_field_state(x, y) == EMPTY)
        //         {
        //             gameField.cross_field(x, y);
        //             score = minimax(x, y, depth - 1, alpha, beta, gameField, CIRCLE_PLAYER);
        //             gameField.empty_field(x, y);
        //             bestScore = std::max(score, bestScore);
        //             //alpha-beta pruning (dwie linie)
        //             alpha = std::max(alpha, score);
        //             if (beta <= alpha)
        //                 return bestScore;
        //         }
        //     }
        // }
        break;
    default:
        break;
    }
    return bestScore;
}
