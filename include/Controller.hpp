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
        /// Pośrednik komunikacji z widokiem.
        std::shared_ptr<MessageQueues> messageQueues;

        /// Zainicjuj rozgrywkę z pustą planszą.
        void init();
        /// Czy ruch ma wykonać gracz który nie jest botem.
        bool need_player_input() const;
        /// Odbierz akcję gracza od widoku.
        PlayerInputMessage get_player_input();
        /// Wyślij obacny stan gry do widoku.
        void send_state() const;
        /// Spróbuj wybrać pionek o współrzędnych (x, y).
        bool try_select_field(int x, int y);
        /// Spróbuj wykonać ruch wybranym pionkiem na pozycję (x, y).
        bool try_move_piece(int x, int y);
        /// Zamień gracza który będzie teraz wykonywał ruch.
        void flip_current_player();
        /// Zwróć nowo sprawdzony stan gry.
        GameProgressEnum check_game_progress() const;
        /// Zakończ pracę kontrolera, zapisz stan gry jeśli była w trakcie.
        void exit();
    };

} // namespace checkers