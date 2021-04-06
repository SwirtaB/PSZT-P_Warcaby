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
#include <iostream>
#include <climits>

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
    gameState.init();
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
        if (true) //need_player_input())  <- replace "true" with right side after fixing the other branch
        {
            PlayerInputMessage message = get_player_input();
            switch (message.messageType)
            {
                case EXIT:
                    return exit();

                case SELECT:
                    if (gameState.can_select_field(Coord(message.x, message.y))) {
                        selectedField = Coord(message.x, message.y);
                        // prevent second branch
                    } else if (selectedField.has_value()) {
                        bool moved = gameState.try_make_move(selectedField.value(), Coord(message.x, message.y));
                        if (moved && gameState.can_select_field(Coord(message.x, message.y))) {
                            selectedField = Coord(message.x, message.y);
                        } else if (moved) {
                            selectedField = std::nullopt;
                        }
                    }
                    break;
            }
            send_state();
        }
        else
        {
            // std::pair<Coord, Coord> move = bot::bot_move(gameState, config.depth);
            // std::this_thread::sleep_for(std::chrono::milliseconds(250));
            // if (!gameState.try_make_move(move.first, move.second)) {
            //     std::cerr << "Bot tried to make illegal move!" << std::endl;
            // }
            // send_state();
        }
    }
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
    return gameState.get_game_progress() != PLAYING ||
        (gameState.get_current_player() == WHITE && !config.white_is_bot
        || gameState.get_current_player() == BLACK && !config.black_is_bot);
}

/**
 * @brief Wyślij obacny stan gry do widoku.
 * 
 */
void Controller::send_state() const
{
    messageQueues->send_game_state(
        GameStateMessage(gameState.get_game_progress(), gameState.get_board_state(), selectedField)
    );
}

/**
 * @brief Zakończ pracę kontrolera, zapisz stan gry jeśli była w trakcie.
 *
 */
void Controller::exit()
{
}
