/**
 * @file MessageQueues.cpp
 * @author Maciej Wojno
 * @brief Zawiera definicję motod klasy MessageQueues.
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "../include/MessageQueues.hpp"

using namespace checkers;

/**
 * @brief Wysłanie wiadomości o akcji gracza do kolejki.
 * @details Wkłada do kolejki podaną akcję i sygnalizuje condvar
 *          czekający na to aż kolejka przestanie być pusta.
 * 
 * @param input - wiadomość z akcją gracza.
 */
void MessageQueues::send_player_input(const PlayerInputMessage input)
{
    {
        std::lock_guard<std::mutex> lg(playerInputQueueMutex);
        playerInputQueue.push(input);
    }
    playerInputWaitCondition.notify_one();
}

/**
 * @brief Odebranie wiadomości o akcji gracza z kolejki.
 * @details Jeśli kolejka jest pusta czeka na zmiennej aż kolejka nie będzie pusta.
 * 
 * @return const PlayerInputMessage - wiadomość z akcją gracza
 */
PlayerInputMessage MessageQueues::wait_for_player_input()
{
    std::unique_lock<std::mutex> lk(playerInputQueueMutex);
    std::queue<PlayerInputMessage> *queue = &this->playerInputQueue;
    playerInputWaitCondition.wait(lk, [queue] { return !queue->empty(); });

    const PlayerInputMessage pi = playerInputQueue.front();
    playerInputQueue.pop();
    return pi;
}

/**
 * @brief Wysłanie nowego stanu gry do kolejki.
 * 
 * @param state - wiadomość ze stanem gry wstawiana do kolejki.
 */
void MessageQueues::send_game_state(const GameStateMessage state)
{
    gameStateQueue.push(state);
}

/**
 * @brief Wyciągnięcie nowego stanu gry z kolejki stanów gry jeśli nie jest pusta.
 * 
 * @return const std::optional<const GameStateMessage> - odebrana wiadomość ze stanem gry.
 */
std::optional<const GameStateMessage> MessageQueues::check_for_game_state()
{
    if (gameStateQueue.empty())
    {
        return std::nullopt;
    }
    else
    {
        const GameStateMessage gs = gameStateQueue.front();
        gameStateQueue.pop();
        return gs;
    }
}
