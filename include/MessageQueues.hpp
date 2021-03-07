/**
 * @file MessageQueues.hpp
 * @author Maciej Wojno, Bartosz Świrta.
 * @brief Zawiera definicję struktur wiadomości GameStateMessage, PlayerInputMessage, oraz klasy 
 *        agregujacej i zarządającej kolejkami wiadomości MessageQueues.
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "Field.hpp"
#include <queue>
#include <memory>
#include <condition_variable>

namespace ox
{
    /**
     * @brief Struktura reprezentująca wiadomość wysyłaną przez kontroler do widoku.
     * 
     */
    struct GameStateMessage
    {
        GameStateMessage(GameStateEnum gs, FieldBoard fb)
            : gameStateEnum(gs), fieldBoard(fb) {}

        GameStateEnum gameStateEnum;
        FieldBoard fieldBoard;
    };

    /**
     * @brief Typy wiadomości od widoku do kontrolera.
     * 
     */
    enum PlayerInputMessageType
    {
        EXIT,
        RESET_GAME,
        INPUT,
        LOAD_SAVE,
        NO_LOAD_SAVE
    };

    /**
     * @brief Klasa reprezentująca wiadomość wysłąną przez widok do kontrolera.
     * 
     */
    class PlayerInputMessage
    {
    public:
        /// Typ wysyłanej wiadomości
        PlayerInputMessageType messageType;
        /// Współrzędne pola gdy typem wiadomości jest INPUT.
        int x;
        /// Współrzędne pola gdy typem wiadomości jest INPUT.
        int y;

        PlayerInputMessage(PlayerInputMessageType messageType_, int x_, int y_)
            : messageType(messageType_), x(x_), y(y_) {}
    };

    /**
     * @brief Współdzielony pośrednik komunikacji pomiędzy widokiem a kontrolerem.
     * @details Widok wysyła tylko PlayerInputMessage i odbiera tylko GameStateMessage.
     *          Kontroler wysyła tylko GameStateMessage i odbiera tylko PlayerInputMessage.
     * 
     */
    class MessageQueues
    {
    public:
        /// Wysłanie wiadomości o akcji gracza do kolejki.
        void send_player_input(const PlayerInputMessage input);
        /// Odebranie wiadomości o akcji gracza z kolejki.
        const PlayerInputMessage wait_for_player_input();
        /// Wysłanie nowego stanu gry do kolejki.
        void send_game_state(const GameStateMessage state);
        /// Wyciągnięcie nowego stanu gry z kolejki stanów gry jeśli nie jest pusta.
        const std::optional<const GameStateMessage> check_for_game_state();

    private:
        /// Kolejka z wiadomościami o akcjach gracza od widoku do kontrolera.
        std::queue<PlayerInputMessage> playerInputQueue;
        /// Condvar do oczekiwania na niepustą kolejkę akcji gracza.
        std::condition_variable playerInputWaitCondition;
        /// Mutex do oczekiwania na niepustą kolejkę akcji gracza.
        std::mutex playerInputQueueMutex;

        /// Kolejka z wiadmościami o stanie gry od kontorlera do widoku.
        std::queue<GameStateMessage> gameStateQueue;
    };

} // namespace ox
