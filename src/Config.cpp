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
#include <fstream>

using namespace checkers;


/**
 * @brief Spróbuj stworzyć plik konfiguracyjny z argumentów wywołania programu.
 *
 * @param argc - ilość argumentów wywołania programu
 * @param argv - tablica argumentów wywołania
 * @return std::optional<Config> - zwraca strukturę przechowującą dane konfiguracyjne lub nullopt jeśli nie dało się jej utworzyć
 */
std::optional<Config> Config::try_from_args(int argc, char *argv[])
{
    Config config;
    if (argc % 2 == 0) return std::nullopt;

    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]) == "--wbot") {
            if (std::string(argv[i+1]) == "true") {
                config.whiteIsBot = true;
            } else if (std::string(argv[i+1]) == "false") {
                config.whiteIsBot = false;
            }
        } else if (std::string(argv[i]) == "--bbot") {
            if (std::string(argv[i+1]) == "true") {
                config.blackIsBot = true;
            } else if (std::string(argv[i+1]) == "false") {
                config.blackIsBot = false;
            }
        } else if (std::string(argv[i]) == "--wdepth") {
            try {
                config.whiteBotDepth = std::stoi(std::string(argv[i + 1]));
            } catch (std::exception &) {
                return std::nullopt;
            }
        } else if (std::string(argv[i]) == "--bdepth") {
            try {
                config.blackBotDepth = std::stoi(std::string(argv[i + 1]));
            } catch (std::exception &) {
                return std::nullopt;
            }
        } else if (std::string(argv[i]) == "--wheuristic") {
            if (std::string(argv[i+1]) == "basic") {
                config.whiteBotHeuristic = BASIC;
            } else if (std::string(argv[i+1]) == "a_basic") {
                config.whiteBotHeuristic = A_BASIC;
            } else if (std::string(argv[i+1]) == "board_aware") {
                config.whiteBotHeuristic = BOARD_AWARE;
            } else {
                return std::nullopt;
            }
        } else if (std::string(argv[i]) == "--bheuristic") {
            if (std::string(argv[i+1]) == "basic") {
                config.blackBotHeuristic = BASIC;
            } else if (std::string(argv[i+1]) == "a_basic") {
                config.blackBotHeuristic = A_BASIC;
            } else if (std::string(argv[i+1]) == "board_aware") {
                config.blackBotHeuristic = BOARD_AWARE;
            } else {
                return std::nullopt;
            }
        } else if (std::string(argv[i]) == "--log") {
            try {
                std::ofstream f(argv[i+1]);
                f.close();
            } catch (std::exception &_e) {
                (void)_e;
                return std::nullopt;
            }
            config.logPath = std::string(argv[i + 1]);
        } else if (std::string(argv[i]) == "--gui") {
            if (std::string(argv[i+1]) == "true") {
                config.showGUI = true;
            } else if (std::string(argv[i+1]) == "false") {
                config.showGUI = false;
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    return config;
}