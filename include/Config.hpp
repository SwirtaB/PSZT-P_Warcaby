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

namespace ox
{
    /** \enum ConfigStart
     * @brief Który gracz ma pierwszy ruch.
     * 
     */
    enum ConfigStart
    {
        CROSS,
        CIRCLE
    };
    /** \enum ConfigBotTactic
     * @brief Z jakiej taktyki korzystają boty.
     * 
     */
    enum ConfigBotTactic
    {
        HEURISTIC,
        GAME_TREE
    };

    /** \enum ConfigUiSize
     * @brief Rozmiar interfejsu gracza.
     * 
     */
    enum ConfigUiSize
    {
        SMALL,
        MEDIUM,
        LARGE,
        RESIZABLE,
    };

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
        bool cross_is_bot;
        /**
         * @brief Czy kółkiem steruje bot.
         * 
         */
        bool circle_is_bot;
        /**
         * @brief Który gracz rozpoczyna grę.
         * 
         */
        ConfigStart start;
        /**
         * @brief Jaką taktyką posługują się boty.
         * 
         */
        ConfigBotTactic bot_tactic;
        /**
         * @brief Rozmiar interfejsu gracza.
         * 
         */
        ConfigUiSize ui_size;
        /**
         * @brief Głębokość przeszukiwania drzewa w przypadku taktyki GAME_TREE botów.
         * 
         */
        int depth;
        /**
         * @brief Ścieżka do pliku z zapisaną grą.
         * 
         */
        std::string save_path;

        /**
         * @brief Spróbuj wczytać plik konfiguracyjny z tekstu skryptu python.
         * 
         */
        static std::optional<Config> try_from_script(const char *config_string);
    };

} // namespace ox
