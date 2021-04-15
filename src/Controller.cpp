/**
 * @file Controller.cpp
 * @author Maciej Wojno
 * @brief Zawiera definicję metod klasy Controller
 * @version 1.0
 * @date 2021-03-24
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "../include/Controller.hpp"
#include "../include/BotMove.hpp"

#include <chrono>
#include <thread>
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
    gameState.init();
    send_state();

    if (config.logPath.has_value()) {
        logFile = std::ofstream(config.logPath.value());
    }
    try_log_start_game();
}

/**
 * @brief Uruchamia kontroler, przejmuje wątek na czas działania.
 * 
 */
void Controller::run()
{
    while (has_human_player() || gameState.get_game_progress() == PLAYING)
    {
        try_log_start_move();

        if (need_player_input())
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
                            try_log_end_move();
                        } else if (moved) {
                            selectedField = std::nullopt;
                            try_log_end_move();
                        }
                    }
                    break;
            }
            send_state();
        }
        else
        {
            std::pair<Coord, Coord> move = std::make_pair(Coord(0,0), Coord(0,0));
            switch(gameState.get_current_player()) {
                case WHITE:
                    move = bot::bot_move(gameState, WHITE, config.whiteBotHeuristic, config.whiteBotDepth);
                    break;
                case BLACK:
                    move = bot::bot_move(gameState, BLACK, config.blackBotHeuristic, config.blackBotDepth);
                    break;
            }
            if (!gameState.try_make_move(move.first, move.second)) {
             std::cerr << "Bot tried to make illegal move!" << " "  << gameState.get_current_player()
                << "x: " << move.second.x << "y: " << move.second.y << std::endl;
            }
            try_log_end_move();

            if (config.showGUI) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            send_state();
        }

    }
    exit();
}


/**
 * @brief Czy w grze jest gracz który nie jest botem.
 *
 * @return Czy w grze jest gracz który nie jest botem.
 */
bool Controller::has_human_player() const {
    return !config.whiteIsBot || !config.blackIsBot;
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
        (gameState.get_current_player() == WHITE && !config.whiteIsBot
        || gameState.get_current_player() == BLACK && !config.blackIsBot);
}

/**
 * @brief Wyślij obacny stan gry do widoku.
 * 
 */
void Controller::send_state() const
{
    if (config.showGUI) {
        messageQueues->send_game_state(
                GameStateMessage(gameState.get_game_progress(), gameState.get_board_state(), selectedField)
        );
    }
}

/**
 * @brief Zakończ pracę kontrolera, zapisz stan gry jeśli była w trakcie.
 *
 */
void Controller::exit()
{
    try_log_end_game();
}

/**
 * @brief Spróbuj zapisać do logu informację o rozpoczętej grze.
 *
 */
void Controller::try_log_start_game() {
    if (logFile.has_value()) {
        // WHITE
        logFile.value() << "white_param ";
        // WHITE BOT
        if (config.whiteIsBot) {
            logFile.value() << "bot ";
            if (config.whiteBotHeuristic == BASIC) {
                logFile.value() << "basic ";
            } else if (config.whiteBotHeuristic == A_BASIC) {
                logFile.value() << "a_basic ";
            } else {
                logFile.value() << "board_aware ";
            }
            logFile.value() << config.whiteBotDepth;
        // WHITE PLAYER
        } else {
            logFile.value() << "player";
        }

        logFile.value() << std::endl;

        // BLACK
        logFile.value() << "black_param ";
        // BLACK BOT
        if (config.whiteIsBot) {
            logFile.value() << "bot ";
            if (config.blackBotHeuristic == BASIC) {
                logFile.value() << "basic ";
            } else if (config.blackBotHeuristic == A_BASIC) {
                logFile.value() << "a_basic ";
            } else {
                logFile.value() << "board_aware ";
            }
            logFile.value() << config.blackBotDepth;
        // BLACK PLAYER
        } else {
            logFile.value() << "player";
        }

        logFile.value() << std::endl;
    }
}

/**
 * @brief Spróbuj zapisać do logu informację o rozpoczęciu ruchu.
 *
 */
void Controller::try_log_start_move() {
    if (logFile.has_value()) {
        if (gameState.get_current_player() == WHITE) {
            logFile.value() << "white ";
        } else {
            logFile.value() << "black ";
        }
        lastMoveStart = std::chrono::steady_clock::now();
    }
}

/**
 * @brief Spróbuj zapisać do logu informację o zakończeniu ruchu..
 *
 */
void Controller::try_log_end_move() {
    if (logFile.has_value()) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastMoveStart.value()).count();
        logFile.value() << duration << std::endl;
    }
}


/**
 * @brief Spróbuj zapisać do logu wynik rozgrywki.
 *
 */
void Controller::try_log_end_game() {
    if (logFile.has_value()) {
        if (gameState.get_game_progress() == PLAYING) {
            logFile.value() << "game_interrupted" << std::endl;
        } else if (gameState.get_game_progress() == WHITE_WON) {
            logFile.value() << "white_won" << std::endl;
        } else if (gameState.get_game_progress() == BLACK_WON) {
            logFile.value() << "black_won" << std::endl;
        } else if (gameState.get_game_progress() == TIE) {
            logFile.value() << "tie" << std::endl;
        }
        logFile.value().close();
    }
}


