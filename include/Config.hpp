/**
 * @file Config.hpp
 * @author Maciej Wojno
 * @brief Zawiera definicję struktury Config - przechowuje dane konfiguracyjne rozgrywki, oraz enumerator typu heurystyki HeuristicEnum.
 * @version 1.0
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <optional>
#include <string>

using std::optional;
using std::string;

namespace checkers
{
    /** \struct Config
     * @brief Struktura reprezentująca dane konfiguracyjne programu.
     */
     enum HeuristicEnum{
         BASIC,
         A_BASIC,
         BOARD_AWARE
     };

    struct Config
    {
        /**
         * @brief Czy białymi steruje bot.
         */
        bool whiteIsBot = false;
        /**
         * @brief Heurystyka, którą posługuje się biały bot.
         */
        HeuristicEnum whiteBotHeuristic = BASIC;
        /**
         * @brief Głębokość przeszukiwania drzewa gry przez białego bota.
         */
        int whiteBotDepth = 3;
        /**
         * @brief Czy czarnymi steruje bot.
         */
        bool blackIsBot = true;
        /**
         * @brief Heurystyka, którą posługuje się czarny bot.
         */
        HeuristicEnum blackBotHeuristic = BASIC;
        /**
         * @brief Głębokość przeszukiwania drzewa gry przez czernego bota.
         */
        int blackBotDepth = 3;
        /**
         * @brief Ścieżka do pliku z logami rozgrywki
         */
        std::optional<string> logPath = std::nullopt;
        /**
         * @brief Czy uruchomić GUI.
         */
         bool showGUI = true;

        static std::optional<Config> try_from_args(int argc, char *argv[]);
    };

} // namespace checkers
