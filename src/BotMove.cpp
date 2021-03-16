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

using namespace ox;

int estimate_move(const FieldBoard &, PlayerEnum, int x, int y);
int minimax(int x, int y, int depth_, int alpha, int beta, FieldBoard gameField, PlayerEnum player);

/**
 * @brief Implemetuje obsluge ruchu bot'a. Wywoluje odpowiednie funkcje w zaleznosci od wartosci wczytanych z pliku konfiguracyjnego.
 * 
 * @param fieldBoard - rozpatrywana plansza
 * @param player - ktory gracz ma wykonac ruch
 * @param tactic - taktyka uzywana przez bot'a
 * @param depth - glebokosc przegladania
 * @return std::pair<int, int> - najlepszy mozliwy ruch
 */
std::pair<int, int> ox::bot::bot_move(const FieldBoard &fieldBoard, PlayerEnum player, ConfigBotTactic tactic, int depth)
{
    FieldBoard tmpField = fieldBoard;
    std::pair<int, int> bestMove;
    int bestScore = 0, score = 0;

    switch (player)
    {
    case CIRCLE_PLAYER:
        bestScore = INT_MAX;
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                if (fieldBoard.get_field_state(x, y) == EMPTY)
                {
                    if (tactic == HEURISTIC)
                    {
                        tmpField.circle_field(x, y);
                        score = estimate_move(tmpField, player, x, y);
                        tmpField.empty_field(x, y);
                    }
                    else // config.bot_tactic == GAME_TREE
                    {
                        tmpField.circle_field(x, y);
                        /*alpha = INT_MIN, beta = INT_MAX do alpha-beta pruning'u*/
                        score = minimax(x, y, depth - 1, INT_MIN, INT_MAX, tmpField, CROSS_PLAYER);
                        tmpField.empty_field(x, y);
                    }
                    if (bestScore > score)
                    {
                        bestScore = score;
                        bestMove.first = x;
                        bestMove.second = y;
                    }
                }
            }
        }
        break;
    case CROSS_PLAYER:
        bestScore = INT_MIN;
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                if (fieldBoard.get_field_state(x, y) == EMPTY)
                {
                    if (tactic == HEURISTIC)
                    {
                        tmpField.cross_field(x, y);
                        score = estimate_move(tmpField, player, x, y);
                        tmpField.empty_field(x, y);
                    }
                    else // config.bot_tactic == GAME_TREE
                    {
                        tmpField.cross_field(x, y);
                        /*alpha = INT_MIN, beta = INT_MAX do alpha-beta pruning'u*/
                        score = minimax(x, y, depth - 1, INT_MIN, INT_MAX, tmpField, CIRCLE_PLAYER);
                        tmpField.empty_field(x, y);
                    }
                    if (bestScore < score)
                    {
                        bestScore = score;
                        bestMove.first = x;
                        bestMove.second = y;
                    }
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
 * 
 * int estimate_move(const FieldBoard &gameField, PlayerEnum player, int x, int y)
 * {
 *      switch(gameField.check_state())
 *      {
 *          case WHITE_WON:
 *              return 100;
 *          case BLACK_WON:
 *              return -100;
 *          case TIE:
 *              return 0;
 *          default:
 *              break;
 *      }
 *      int score = 0;
 *      switch(player)
 *      {
 *          case WHITE_PLAYER:
 *              for(gameField)
 *                  score = H;
 *              break;
 *          case BLACK_PLAYER:
 *              for(gameField)
 *                  score = H;
 *              break;
 *          default:
 *              score = 0;
 *              break;
 *      }
 *      return score;
 * }
 */

/**
 * @brief Funkcja ktora estymuje "jakosc" planszy dla danego gracza. Kolko minimalizuje jakosc, krzyzyk maksymalizuje jakosc
 * 
 * @param gameField - obecnie rozpatrywana plansza
 * @param player - ktory gracz wykonuje ruch
 * @param x - wpsolrzedna x rozpatrywanego ruchu
 * @param y - wspolrzedna y rozpatrywanego ruchu
 * @return int - jakosc danej planszy (w konsekwencji rowniez rozpatrywanego ruchu)
 */
int estimate_move(const FieldBoard &gameField, PlayerEnum player, int x, int y)
{
    switch (gameField.check_state())
    {
    case CIRCLE_WON:
        return -4;
    case CROSS_WON:
        return 4;
    case TIE:
        return 0;
    default:
        break;
    }

    int bestScore = 0, score = 0, xCounter = 0, oCounter = 0, eCounter = 0;
    switch (player)
    {
    case CIRCLE_PLAYER:
        score = INT_MAX;
        /*sprawdzam wiersz*/
        for (int i = 0; i < 3; ++i)
        {
            switch (gameField.get_field_state(i, y))
            {
            case EMPTY:
                ++eCounter;
                break;
            case CIRCLED:
                ++oCounter;
                break;
            case CROSSED:
                ++xCounter;
                break;
            default:
                break;
            }
        }
        if (oCounter == 1 && xCounter == 0)
            score = -1;
        else if (oCounter == 2 && xCounter == 0)
            score = -2;
        else if (xCounter == 2)
            score = -3;
        else
            score = 0;
        bestScore = std::min(bestScore, score);
        eCounter = 0;
        oCounter = 0;
        xCounter = 0;

        /*sprawdzam kolumne*/
        for (int i = 0; i < 3; ++i)
        {
            switch (gameField.get_field_state(x, i))
            {
            case EMPTY:
                ++eCounter;
                break;
            case CIRCLED:
                ++oCounter;
                break;
            case CROSSED:
                ++xCounter;
                break;
            default:
                break;
            }
        }
        if (oCounter == 1 && xCounter == 0)
            score = -1;
        else if (oCounter == 2 && xCounter == 0)
            score = -2;
        else if (xCounter == 2)
            score = -3;
        else
            score = 0;
        bestScore = std::min(bestScore, score);
        eCounter = 0;
        oCounter = 0;
        xCounter = 0;

        /*sprawdzam przekatne*/
        if (x == 0 && y == 0 || x == 1 && y == 1 || x == 2 && y == 2)
        {
            for (int i = 0, j = 0; i < 3; ++i, ++j)
            {
                switch (gameField.get_field_state(i, j))
                {
                case EMPTY:
                    ++eCounter;
                    break;
                case CIRCLED:
                    ++oCounter;
                    break;
                case CROSSED:
                    ++xCounter;
                    break;
                default:
                    break;
                }
            }
            if (oCounter == 1 && xCounter == 0)
                score = -1;
            else if (oCounter == 2 && xCounter == 0)
                score = -2;
            else if (xCounter == 2)
                score = -3;
            else
                score = 0;
            bestScore = std::min(bestScore, score);
            eCounter = 0;
            oCounter = 0;
            xCounter = 0;
        }

        if (x == 2 && y == 0 || x == 1 && y == 1 || x == 0 && y == 2)
        {
            for (int i = 2, j = 0; i >= 0; --i, ++j)
            {
                switch (gameField.get_field_state(i, j))
                {
                case EMPTY:
                    ++eCounter;
                    break;
                case CIRCLED:
                    ++oCounter;
                    break;
                case CROSSED:
                    ++xCounter;
                    break;
                default:
                    break;
                }
            }
            if (oCounter == 1 && xCounter == 0)
                score = -1;
            else if (oCounter == 2 && xCounter == 0)
                score = -2;
            else if (xCounter == 2)
                score = -3;
            else
                score = 0;
            bestScore = std::min(bestScore, score);
        }
        break;

    case CROSS_PLAYER:
        score = INT_MIN;
        /*sprawdzam wiersz*/
        for (int i = 0; i < 3; ++i)
        {
            switch (gameField.get_field_state(i, y))
            {
            case EMPTY:
                ++eCounter;
                break;
            case CIRCLED:
                ++oCounter;
                break;
            case CROSSED:
                ++xCounter;
                break;
            default:
                break;
            }
        }
        if (xCounter == 1 && oCounter == 0)
            score = 1;
        else if (xCounter == 2 && oCounter == 0)
            score = 2;
        else if (oCounter == 2)
            score = 3;
        else
            score = 0;
        bestScore = std::max(bestScore, score);
        eCounter = 0;
        oCounter = 0;
        xCounter = 0;

        /*sprawdzam kolumne*/
        for (int i = 0; i < 3; ++i)
        {
            switch (gameField.get_field_state(x, i))
            {
            case EMPTY:
                ++eCounter;
                break;
            case CIRCLED:
                ++oCounter;
                break;
            case CROSSED:
                ++xCounter;
                break;
            default:
                break;
            }
        }
        if (xCounter == 1 && oCounter == 0)
            score = 1;
        else if (xCounter == 2 && oCounter == 0)
            score = 2;
        else if (oCounter == 2)
            score = 3;
        else
            score = 0;
        bestScore = std::max(bestScore, score);
        eCounter = 0;
        oCounter = 0;
        xCounter = 0;

        /*sprawdzam przekatne*/
        if (x == 0 && y == 0 || x == 1 && y == 1 || x == 2 && y == 2)
        {
            for (int i = 0, j = 0; i < 3; ++i, ++j)
            {
                switch (gameField.get_field_state(i, j))
                {
                case EMPTY:
                    ++eCounter;
                    break;
                case CIRCLED:
                    ++oCounter;
                    break;
                case CROSSED:
                    ++xCounter;
                    break;
                default:
                    break;
                }
            }
            if (xCounter == 1 && oCounter == 0)
                score = 1;
            else if (xCounter == 2 && oCounter == 0)
                score = 2;
            else if (oCounter == 2)
                score = 3;
            else
                score = 0;
            bestScore = std::max(bestScore, score);
            eCounter = 0;
            oCounter = 0;
            xCounter = 0;
        }

        if (x == 2 && y == 0 || x == 1 && y == 1 || x == 0 && y == 2)
        {
            for (int i = 2, j = 0; i >= 0; --i, ++j)
            {
                switch (gameField.get_field_state(i, j))
                {
                case EMPTY:
                    ++eCounter;
                    break;
                case CIRCLED:
                    ++oCounter;
                    break;
                case CROSSED:
                    ++xCounter;
                    break;
                default:
                    break;
                }
            }
            if (xCounter == 1 && oCounter == 0)
                score = 1;
            else if (xCounter == 2 && oCounter == 0)
                score = 2;
            else if (oCounter == 2)
                score = 3;
            else
                score = 0;
            bestScore = std::max(bestScore, score);
        }
        break;
    default:
        break;
    }
    return bestScore;
}
/**
 * @brief Implementuje algorytm minimax wykorzystujac alpha-beta pruning
 * 
 * @param x - wspolrzedna x rozpatrywanego ruchu
 * @param y - wspolrzedna y rozpatrywanego ruchu
 * @param depth - gleboksc przegladania
 * @param alpha - wartosc zmiennej alpha (alpha-beta pruning)
 * @param beta - wartosc zmiennej beta (alpha-beta pruning)
 * @param gameField - obecnie rozpatrywana plansza
 * @param player - gracz wykonujacy ruch
 * @return int - jakosc danej planszy
 */
int minimax(int x, int y, int depth, int alpha, int beta, FieldBoard gameField, PlayerEnum player)
{
    if (!depth || gameField.check_state() != PLAYING)
    {
        return estimate_move(gameField, player, x, y);
    }
    int bestScore = 0, score = 0;
    switch (player)
    {
    case CIRCLE_PLAYER:
        bestScore = INT_MAX;
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                if (gameField.get_field_state(x, y) == EMPTY)
                {
                    gameField.circle_field(x, y);
                    score = minimax(x, y, depth - 1, alpha, beta, gameField, CROSS_PLAYER);
                    gameField.empty_field(x, y);
                    bestScore = std::min(score, bestScore);
                    //alpha-beta pruning (dwie linie)
                    beta = std::min(beta, score);
                    if (beta <= alpha)
                        return bestScore;
                }
            }
        }
        break;
    case CROSS_PLAYER:
        bestScore = INT_MIN;
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                if (gameField.get_field_state(x, y) == EMPTY)
                {
                    gameField.cross_field(x, y);
                    score = minimax(x, y, depth - 1, alpha, beta, gameField, CIRCLE_PLAYER);
                    gameField.empty_field(x, y);
                    bestScore = std::max(score, bestScore);
                    //alpha-beta pruning (dwie linie)
                    alpha = std::max(alpha, score);
                    if (beta <= alpha)
                        return bestScore;
                }
            }
        }
        break;
    default:
        break;
    }
    return bestScore;
}