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
#include "../include/BotMove.hpp"

#include <chrono>
#include <thread>
#include <climits>
#include <fstream>

using namespace ox;

/**
 * @brief Konstruktor kontrolera z podaną konfiguracją.
 *        Komunikuje się z widokiem za pomocą podanego wspólnego pośrednika.
 * 
 * @param config_ - struktura przechowująca dane konfiguracyjne.
 * @param queuesHandler_ - uchwyt na koljkę komunikatów.
 */
Controller::Controller(Config &config_, std::shared_ptr<MessageQueues> queuesHandler_)
    : gameState(PREPARING), messageQueues(queuesHandler_), config(config_)
{
    send_state();
}

/**
 * @brief Uruchamia kontroler, przejmuje wątek na czas działania.
 * 
 */
void Controller::run()
{
    loadedSave = try_load_saved_state(config);
    init();
    if (loadedSave.has_value())
    {
        gameState = LOAD_SAVE_QUESTION;
    }
    else
    {
        gameState = PLAYING;
    }
    send_state();
    play_game();
}

/**
 * @brief Zainicjuj rozgrywkę z pustą planszą.
 * 
 */
void Controller::init()
{
    fieldBoard = FieldBoard();
    gameState = PLAYING;
    if (config.start == CROSS)
    {
        currentPlayer = CROSS_PLAYER;
    }
    else
    {
        currentPlayer = CIRCLE_PLAYER;
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
    return gameState != PLAYING || currentPlayer == CROSS_PLAYER && !config.cross_is_bot || currentPlayer == CIRCLE_PLAYER && !config.circle_is_bot;
}

/**
 * @brief Wyślij obacny stan gry do widoku.
 * 
 */
void Controller::send_state() const
{
    messageQueues->send_game_state(GameStateMessage(gameState, fieldBoard));
}

/**
 * @brief Spróbuj wykonać ruch podanego gracza na danym polu.
 * 
 * @param x - wartość x pola.
 * @param y - wartość y pola.
 * @param player - gracz który wykonuje ruch.
 * @return true - ruch wykonany.
 * @return false - ruch był nieprawidłowy.
 */
bool Controller::select_field(int x, int y, PlayerEnum player)
{
    switch (player)
    {
    case CIRCLE_PLAYER:
        return fieldBoard.circle_field(x, y);
    case CROSS_PLAYER:
        return fieldBoard.cross_field(x, y);
    default:
        return false;
    }
}

/**
 * @brief Zamień gracza który będzie teraz wykonywał ruch.
 * 
 */
void Controller::flip_player()
{
    if (currentPlayer == CROSS_PLAYER)
    {
        currentPlayer = CIRCLE_PLAYER;
    }
    else
    {
        currentPlayer = CROSS_PLAYER;
    }
}

/**
 * @brief Zakończ pracę kontrolera, zapisz stan gry jeśli była w trakcie.
 * 
 */
void Controller::exit()
{
    if (gameState == PLAYING)
    {
        try_save_state();
    }
}

/**
 * @brief Pętla rozgryki.
 * 
 */
void Controller::play_game()
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
                break;

            case RESET_GAME:
                init();
                break;

            case LOAD_SAVE:
                try_load_cached_save();
                break;

            case INPUT:
                /*Weryfikacja czy wykonany przez gracza ruch byl poprawny*/
                if (select_field(message.x, message.y, currentPlayer))
                    flip_player();
                break;
            }
            gameState = fieldBoard.check_state();
            send_state();
        }
        else
        {
            std::pair<int, int> move = bot::bot_move(fieldBoard, currentPlayer, config.bot_tactic, config.depth);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            select_field(move.first, move.second, currentPlayer);
            flip_player();
            gameState = fieldBoard.check_state();
            send_state();
        }
    }
}

/**
 * @brief Ustaw stan gry na ten z wczytanej gry.
 * 
 */
void Controller::try_load_cached_save()
{
    if (loadedSave)
    {
        fieldBoard = loadedSave.value().first;
        currentPlayer = loadedSave.value().second;
        gameState = PLAYING;
    }
}

/**
 * @brief Spróbuj wczytać stan gry z pliku o ścieżce podanej w pliku konfiguracyjnym.
 * 
 * @return true - poprawnie wczytano plik.
 * @return false - w każdym innym przypadku.
 */
std::optional<std::pair<FieldBoard, PlayerEnum>> Controller::try_load_saved_state(const Config &config)
{
    try
    {
        std::ifstream save(config.save_path);
        std::optional<FieldBoard> loaded_gf = FieldBoard::try_read(save);
        if (!loaded_gf.has_value())
        {
            return std::optional<std::pair<FieldBoard, PlayerEnum>>();
        }
        std::string player;
        PlayerEnum current_player;
        save >> player;
        if (player == "cross")
        {
            current_player = CROSS_PLAYER;
        }
        else if (player == "circle")
        {
            current_player = CIRCLE_PLAYER;
        }
        else
        {
            return std::optional<std::pair<FieldBoard, PlayerEnum>>();
        }

        return std::make_pair(loaded_gf.value(), current_player);
    }
    catch (std::exception &e)
    {
        return std::optional<std::pair<FieldBoard, PlayerEnum>>();
    }
}

/**
 * @brief Spróbuj zapisać obecny stan gry do pliku o ścieżce podanej w pliku konfiguracyjnym.
 * 
 * @return true - poprawnie zapisano plik.
 * @return false - w każdym innym przypadku.
 */
bool Controller::try_save_state() const
{
    try
    {
        std::ofstream save(config.save_path);
        fieldBoard.write(save);
        if (currentPlayer == CROSS_PLAYER)
        {
            save << "cross\n";
        }
        else
        {
            save << "circle\n";
        }
        return true;
    }
    catch (std::exception &e)
    {
        return false;
    }
}