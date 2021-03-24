/**
 * @file Config.cpp
 * @author Maciej Wojno
 * @brief Zawiera definicję metody try_from_script.
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "../include/Config.hpp"

#include <optional>
#include <string>

using namespace checkers;

/**
 * @brief Spróbuj wczytać plik konfiguracyjny z tekstu skryptu python.
 * 
 * @param config_str - ciąg znaków z wczytaną konfiguracją
 * @return std::optional<Config> - zwraca strukturę przechowującą dane konfiguracyjne
 */
std::optional<Config> Config::try_from_args(int argc, char *argv[])
{
    Config config;

    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]) == "--bot" && std::string(argv[i+1]) == "white") {
            config.white_is_bot = true;
        } else if (std::string(argv[i]) == "--bot" && std::string(argv[i+1]) == "black") {
            config.black_is_bot = true;
        } else if (std::string(argv[i]) == "--depth") {
            try {
                config.depth = std::stoi(std::string(argv[i + 1]));
            } catch (std::exception &) {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    return config;
}