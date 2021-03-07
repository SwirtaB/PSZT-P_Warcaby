/**
 * @file View.hpp
 * @author Maciej Wojno
 * @brief Zawiera definicję klasy View
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "../include/MessageQueues.hpp"
#include "../include/Config.hpp"

#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

#include <optional>

namespace ox
{
    /**
     * @brief Widok gry.
     *        Wyświetla interfejs gracza i wysyła wykonywane przez niego akcje do kontrolera.
     * 
     */
    class View : public mahi::gui::Application
    {
    public:
        /**
         * @brief Konstruktor widoku z podaną konfiguracją.
         * 
         */
        View(ox::Config &config, std::shared_ptr<MessageQueues> messageQueues);
        virtual ~View();
        /**
         * @brief Uruchamia widok, przejmuje wątek na czas działania.
         * 
         */
        void run() { mahi::gui::Application::run(); }

    private:
        /**
         * @brief Pośrednik komunikacji z kontrolerem.
         * 
         */
        std::shared_ptr<MessageQueues> messageQueues;
        /**
         * @brief Ostatni stan gry otrzymany od kontrolera.
         * 
         */
        std::optional<GameStateMessage> lastState;
        /**
         * @brief Funkcja aktualizacji interfejsu gracza.
         * 
         */
        void update() override;
        /**
         * @brief Aktualizacja interfejsu w stanie gry PREPARING.
         * 
         */
        void update_preparing();
        /**
         * @brief Aktualizacja interfejsu w stanie gry LOAD_SAVE_QUESTION.
         * 
         */
        void update_load_save_question();
        /**
         * @brief Aktualizacja interfejsu w stanie gry IN_PROGRESS.
         * 
         */
        void update_game_in_progress(FieldBoard &fields);
        /**
         * @brief Aktualizacja interfejsu w stanie gry FINISHED.
         * 
         */
        void update_game_finished(PlayerEnum winner);
        /**
         * @brief Oblicza współczynnik skalowania tekstu w zależności od rozmiaru okna.
         * 
         */
        float get_font_scale() const;
        /**
         * @brief Wyświetla przycisk na planszy w podanym stanie.
         * 
         */
        bool field_button(FieldState state) const;
        /**
         * @brief Sprawdza czy przyszedł nowy stan od kontrolera.
         * 
         */
        void check_for_new_state();
        /**
         * @brief Wysyła akcję gracza do kontrolera.
         * 
         */
        void send_player_input(PlayerInputMessage message) const;
    };

} // namespace ox
