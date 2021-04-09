/**
 * @file Config.hpp
 * @author Maciej Wojno
 * @brief Zawiera definicję struktury Config - przechowuje dane konfiguracyjne wczytane z pliku, oraz enumeratorów ConfigStart, ConfigBotTactic, ConfigUiSize.
 * @version 1.0
 * @date 2021-01-10
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
     * @brief Struktura reprezentująca wczytane dane konfiguracyjne.
     * @details Jest wczytywany ze zmiennych ze skryptu w pythonie o nazwie config.py
     *          w katalogu w którym uruchomiony został program lub ze ścieżki
     *          podanej jako argument wywołania programu.
     *          Możliwe konfiguracje widoczne w pliku config.py w głównym folderze projektu.
     * 
     */
    struct Config
    {
        /**
         * @brief Czy krzyżykiem steruje bot.
         * 
         */
        bool white_is_bot = true;
        /**
         * @brief Czy kółkiem steruje bot.
         * 
         */
        bool black_is_bot = true;
        /**
         * @brief Głębokość przeszukiwania drzewa gry.
         * 
         */
        int depth = 8;

        /**
         * @brief Spróbuj wczytać plik konfiguracyjny z argumentów wywołania programu.
         * 
         */
        static std::optional<Config> try_from_args(int argc, char *argv[]);
    };

} // namespace checkers
