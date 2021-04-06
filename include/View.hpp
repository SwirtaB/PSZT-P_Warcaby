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

#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <optional>

#include "MessageQueues.hpp"


namespace checkers
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
        View(std::shared_ptr<MessageQueues> messageQueues);
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
        static ImVec2 get_button_size() ;
        /**
         * @brief Wyświetla przycisk na planszy w podanym stanie.
         *
         */
        bool board_button(std::optional<PieceEnum> &pawn, bool is_selected, bool is_dark) const;
        /**
         * @brief Sprawdza czy przyszedł nowy stan od kontrolera.
         * 
         */
        float get_font_scale() const;
        void check_for_new_state();
        /**
         * @brief Wysyła akcję gracza do kontrolera.
         * 
         */
        void send_player_input(PlayerInputMessage message) const;
    };

} // namespace checkers
