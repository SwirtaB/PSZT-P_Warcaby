/**
 * @file BotMove.cpp
 * @author Bartosz Świrta
 * @brief Zawiera definicje metody bot_move, estimate_move - heurystyka ewaluujaca ruch, minimax - implementuje algorytm minimax
 * @version 1.0
 * @date 2021-04-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "../include/BotMove.hpp"
#include <climits>

using namespace checkers;
using namespace checkers::bot;

std::pair<Coord, Coord> checkers::bot::bot_move(const GameState &gameState, PlayerEnum player, HeuristicEnum heuristicType, int depth)
{
    std::pair<Coord, Coord> bestMove = std::make_pair(Coord(0,0), Coord(0,0));
    int bestScore = 0, score = 0;
    GameState localState = gameState;
    if(player == WHITE){
        bestScore = INT_MIN;
        for(auto piece : gameState.pieces_with_moves()) {
            for (auto move : gameState.piece_moves(piece)) {
                if (localState.try_make_move(piece, move))
                    score = minimax(localState, depth - 1, INT_MIN, INT_MAX, BLACK, heuristicType);
                localState = gameState;

                if (bestScore < score)
                {
                    bestScore = score;
                    bestMove.first = piece;
                    bestMove.second = move;
                }
            }
        }
    }
    else{
        bestScore = INT_MAX;
        for(auto piece : gameState.pieces_with_moves()) {
            for (auto move : gameState.piece_moves(piece)) {
                if (localState.try_make_move(piece, move))
                    score = minimax(localState, depth - 1, INT_MIN, INT_MAX, WHITE, heuristicType);
                localState = gameState;

                if (bestScore > score)
                {
                    bestScore = score;
                    bestMove.first = piece;
                    bestMove.second = move;
                }
            }
        }
    }
    std::cerr << gameState.get_current_player() << ": " << "bestScore= " << bestScore << std::endl;
    return bestMove;
}

int checkers::bot::basic_heuristic(const GameState &gameState){
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
    score = basicHeuristicTable[1]*whiteQueens + basicHeuristicTable[0]*whitePawns - (basicHeuristicTable[3]*blackQueens + basicHeuristicTable[2]*blackPawns);

    return score;
}

int checkers::bot::aggressive_basic_heuristic(const GameState &gameState) {
    BoardState board = gameState.get_board_state();
    int score = 0;
    int whitePawnsValue = 0, whiteQueensValue = 0, blackPawnsValue = 0, blackQueensValue = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(board.fields[i][j].has_value()){
                switch (board.fields[i][j].value())
                {
                    case WHITE_PAWN:
                        whitePawnsValue += basicHeuristicTable[0] + j;
                        break;
                    case WHITE_QUEEN:
                        whiteQueensValue += basicHeuristicTable[1] + 8;
                        break;
                    case BLACK_PAWN:
                        blackPawnsValue += basicHeuristicTable[2] + 8 - j;
                        break;
                    case BLACK_QUEEN:
                        blackQueensValue += basicHeuristicTable[3] + 8;
                        break;
                }
            }
        }
    }
    score = whiteQueensValue + whitePawnsValue - (blackQueensValue + blackPawnsValue);
    return score;
}

int checkers::bot::board_aware_heuristic(const GameState &gameState) {
    BoardState board = gameState.get_board_state();
    int score = 0;
    int whitePawns = 0, whiteQueens = 0, blackPawns = 0, blackQueens = 0;
    int whitePieceSafe = 0, whitePieceNear = 0, blackPieceSafe = 0, blackPieceNear = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(board.fields[i][j].has_value()){
                switch (board.fields[i][j].value())
                {
                    case WHITE_PAWN:
                        ++whitePawns;
                        if(i == 0 || i == 7)
                            ++whitePieceSafe;
                        else if(j > 4 && i > 0 && i < 7)
                            ++whitePieceNear;
                        break;
                    case WHITE_QUEEN:
                        ++whiteQueens;
                        if(i == 0 || i == 7)
                            ++whitePieceSafe;
                        else if(j > 4 && i > 0 && i < 7)
                            ++whitePieceNear;
                        break;
                    case BLACK_PAWN:
                        ++blackPawns;
                        if(i == 0 || i == 7)
                            ++blackPieceSafe;
                        else if(j < 3 && i > 0 && i < 7)
                            ++blackPieceNear;
                        break;
                    case BLACK_QUEEN:
                        ++blackQueens;
                        if(i == 0 || i == 7)
                            ++blackPieceSafe;
                        else if(j < 3 && i > 0 && i < 7)
                            ++blackPieceNear;
                        break;
                }
            }
        }
    }
    score = boardAwareHeuristicTable[1]*whiteQueens + boardAwareHeuristicTable[0]*whitePawns + boardAwareHeuristicTable[4]*whitePieceSafe
            + boardAwareHeuristicTable[6]*whitePieceNear - (boardAwareHeuristicTable[3]*blackQueens + boardAwareHeuristicTable[2]*blackPawns
            + boardAwareHeuristicTable[5]*blackPieceSafe + boardAwareHeuristicTable[7]*blackPieceNear);

    return score;
}

int checkers::bot::estimate_move(const GameState &gameState, HeuristicEnum heuristicType)
{
    switch(gameState.get_game_progress())
    {
        case WHITE_WON:
            return 1000;
        case BLACK_WON:
            return -1000;
        case TIE:
            return 0;
        default:
            break;
    }
    int score = 0;
    switch (heuristicType) {
        case checkers::BASIC:
            score = basic_heuristic(gameState);
            break;
        case checkers::A_BASIC:
            score = aggressive_basic_heuristic(gameState);
            break;
        case checkers::BOARD_AWARE:
            score = board_aware_heuristic(gameState);
            break;
    }
    return score;
}

int checkers::bot::minimax(const GameState &gameState, int depth, int alpha, int beta, PlayerEnum player, HeuristicEnum heuristicType)
{
    if (!depth || gameState.get_game_progress() != PLAYING)
    {
        return estimate_move(gameState, heuristicType);
    }
    int score = 0;
    GameState localState = gameState;
    if(player == WHITE){
        for(auto piece : gameState.pieces_with_moves()){
            for(auto move : gameState.piece_moves(piece)){
                if(localState.try_make_move(piece, move))
                    score = minimax(localState, depth - 1, alpha, beta, BLACK, heuristicType);
                localState = gameState;

                //alpha-beta pruning (dwie linie)
                alpha = std::max(alpha, score);
                if(beta <= alpha)
                    return beta;
            }
        }
        return alpha;
    }
    else{
        for(auto piece : gameState.pieces_with_moves()){
            for(auto move : gameState.piece_moves(piece)){
                if(localState.try_make_move(piece, move))
                    score = minimax(localState, depth - 1, alpha, beta, WHITE, heuristicType);
                localState = gameState;

                //alpha-beta pruning (dwie linie)
                beta = std::min(beta, score);
                if(beta <= alpha)
                    return alpha;
            }
        }
        return beta;
    }
}
