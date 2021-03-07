/**
 * @file main.cpp
 * @author Maciej Wojno
 * @brief Punkt wejscia do programu. Zawiera funkcje main, ktora uruchamia kolejne moduly programu.
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <string>
#include <fstream>
#include <optional>
#include <iostream>
#include <thread>

#include "../include/View.hpp"
#include "../include/Controller.hpp"
#include "../include/MessageQueues.hpp"
#include "../include/Config.hpp"

using namespace ox;

std::optional<Config> try_load_config(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    std::optional<Config> o_config = try_load_config(argc, argv);
    if (!o_config)
    {
        std::cerr << "Config error!" << std::endl;
        return 1;
    }

    std::shared_ptr<MessageQueues> message_queues = std::make_shared<MessageQueues>();

    auto model = std::make_unique<Controller>(o_config.value(), message_queues);
    std::thread model_thread([&model]() {
        model->run();
    });

    { // w bloku zeby wywolac destruktor view po zakonczeniu view.run()
        View view(o_config.value(), message_queues);
        view.run();
    }

    model_thread.join();
    return 0;
}

/**
 * @brief Spróbuj wczytać plik konfiguracyjny z podanej lub domyślnej ścieżki.
 * 
 * @param argc 
 * @param argv 
 * @return std::optional<Config> - struktura przechowujaca dane wczytane z pliku konfiguracyjnego
 */
std::optional<Config> try_load_config(int argc, char *argv[])
{
    std::ifstream ifs;
    if (argc == 2)
    {
        ifs = std::move(std::ifstream(argv[1]));
    }
    else
    {
        ifs = std::move(std::ifstream("config.py"));
    }
    std::string config_string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return Config::try_from_script(config_string.c_str());
}