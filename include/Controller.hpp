/**
 * @file Controller.hpp
 * @author Bartosz Świrta
 * @brief Zawiera definicję klasy Controller - kontroluje przebieg gry.
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include <optional>

#include "MessageQueues.hpp"
#include "Game.hpp"
#include "Config.hpp"

namespace checkers
{
    /**
     * @brief Kontroler gry.
     *        Przyjmuje ruchy graczy, wywołuje ruchy botów i rozgrywa grę.
     * 
     */
    class Controller
    {
    public:
        /**
         * @brief Konstruktor kontrolera z podaną konfiguracją.
         *        Komunikuje się z widokiem za pomocą podanego wspólnego pośrednika.
         * 
         */
        Controller(Config &config_, std::shared_ptr<MessageQueues> queuesHandler_);
        /**
         * @brief Uruchamia kontroler, przejmuje wątek na czas działania. Pętla rozgrywki.
         * 
         */
        void run();

    private:
        /// Konfiguracja rozgrywki.
        Config config;
        /// Stan razgrywki.
        GameState gameState;
        std::optional<Coord> selectedField;
        /// Pośrednik komunikacji z widokiem.
        std::shared_ptr<MessageQueues> messageQueues;

        /// Czy ruch ma wykonać gracz który nie jest botem.
        bool need_player_input() const;
        /// Odbierz akcję gracza od widoku.
        PlayerInputMessage get_player_input();
        /// Wyślij obacny stan gry do widoku.
        void send_state() const;
        /// Zakończ pracę kontrolera, zapisz stan gry jeśli była w trakcie.
        void exit();
    };

} // namespace checkers