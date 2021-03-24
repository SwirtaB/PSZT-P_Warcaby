/**
 * @file Controller.cpp
 * @author Bartosz Świrta
 * @brief Zawiera definicję metod klasy Controller
 * @version 1.0
 * @date 2021-01-10
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "../include/Controller.hpp"
//#include "../include/BotMove.hpp"

#include <chrono>
#include <thread>
#include <climits>
#include <iostream>

using namespace checkers;

/**
 * @brief Konstruktor kontrolera z podaną konfiguracją.
 *        Komunikuje się z widokiem za pomocą podanego wspólnego pośrednika.
 * 
 * @param config_ - struktura przechowująca dane konfiguracyjne.
 * @param queuesHandler_ - uchwyt na koljkę komunikatów.
 */
Controller::Controller(Config &config_, std::shared_ptr<MessageQueues> queuesHandler_)
    : gameState(), messageQueues(std::move(queuesHandler_)), config(config_)
{
    init();
    send_state();
}

/**
 * @brief Uruchamia kontroler, przejmuje wątek na czas działania.
 * 
 */
void Controller::run()
{
    while (true)
    {
        if (need_player_input())
        {
            PlayerInputMessage message = get_player_input();
            switch (message.messageType)
            {
                case EXIT:
                    return exit();

                case SELECT:
                    if (try_select_field(message.x, message.y)) {
                        // prevent second branch
                    } else if (try_move_piece(message.x, message.y)) {
                        flip_current_player();
                        gameState.board.selectedField = std::nullopt;
                    }
                    break;
            }
            gameState.gameProgress = check_game_progress();
            send_state();
        }
        else
        {
            // TODO: adapt to checkers
//            std::pair<int, int> move = bot::bot_move(fieldBoard, currentPlayer, config.bot_tactic, config.depth);
//            std::this_thread::sleep_for(std::chrono::milliseconds(250));
//            select_field(move.first, move.second, currentPlayer);

            flip_current_player();
            gameState.gameProgress = check_game_progress();
            send_state();
        }
    }
}

/**
 * @brief Zainicjuj rozgrywkę z pustą planszą.
 * 
 */
void Controller::init()
{
    for (size_t x = 0; x < 8; ++x) {
        for (size_t y = 0; y < 8; ++y) {
            if ((x + y) % 2 == 0 && y < 3) {
                gameState.board.fields[x][y] = WHITE_PAWN;
            } else if ((x + y) % 2 == 0 && y > 4) {
                gameState.board.fields[x][y] = BLACK_PAWN;
            } else {
                gameState.board.fields[x][y] = std::nullopt;
            }
        }
    }
    gameState.gameProgress = PLAYING;
    gameState.currentPlayer = WHITE;
}

/**
 * @brief Odbierz akcję gracza od widoku.
 * 
 * @return PlayerInputMessage - wiadomość z wartościami wprowadzonymi przez gracza.
 */
PlayerInputMessage Controller::get_player_input()
{
    return messageQueues->wait_for_player_input();
}

/**
 * @brief Czy ruch ma wykonać gracz który nie jest botem.
 * 
 * @return true 
 * @return false 
 */
bool Controller::need_player_input() const
{
    return gameState.gameProgress != PLAYING ||
        (gameState.currentPlayer == WHITE && !config.white_is_bot
        || gameState.currentPlayer == BLACK && !config.black_is_bot);
}

/**
 * @brief Wyślij obacny stan gry do widoku.
 * 
 */
void Controller::send_state() const
{
    messageQueues->send_game_state(GameStateMessage(gameState.gameProgress, gameState.board));
}

/**
 * @brief Spróbuj wykonać ruch podanego gracza na danym polu.
 * 
 * @param x - wartość x pola.
 * @param y - wartość y pola.
 * @return true - wybrano wykonany.
 * @return false - ruch był nieprawidłowy.
 */
bool Controller::try_select_field(int x, int y)
{
    if ((gameState.currentPlayer == BLACK
        && gameState.board.fields[x][y]
        && (gameState.board.fields[x][y].value() == BLACK_PAWN || gameState.board.fields[x][y].value() == BLACK_QUEEN))
        ||
        (gameState.currentPlayer == WHITE
         && gameState.board.fields[x][y]
         && (gameState.board.fields[x][y].value() == WHITE_PAWN || gameState.board.fields[x][y].value() == WHITE_QUEEN)))
    {
        gameState.board.selectedField = Coord(x, y);
        return true;
    } else {
        return false;
    }
}

bool Controller::try_move_piece(int x, int y) {
    if (!gameState.board.selectedField.has_value() || gameState.board.fields[x][y] || (x + y) % 2 == 1) {
        return false;
    }
    // TODO: check valid move
    Coord pc = gameState.board.selectedField.value();
    gameState.board.fields[x][y] = gameState.board.fields[pc.x][pc.y];
    gameState.board.fields[pc.x][pc.y] = std::nullopt;
    return true;
}

/**
 * @brief Zamień gracza który będzie teraz wykonywał ruch.
 * 
 */
void Controller::flip_current_player() {
    if (gameState.currentPlayer == WHITE)
    {
        gameState.currentPlayer = BLACK;
    }
    else
    {
        gameState.currentPlayer = WHITE;
    }
}

GameProgressEnum Controller::check_game_progress() const {
    // TODO: implement game progress checking
    return PLAYING;
}

/**
 * @brief Zakończ pracę kontrolera, zapisz stan gry jeśli była w trakcie.
 *
 */
void Controller::exit()
{
}
