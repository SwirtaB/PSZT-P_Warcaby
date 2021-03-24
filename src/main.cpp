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

#include <thread>

#include "../include/View.hpp"
#include "../include/Controller.hpp"

using namespace checkers;

int main(int argc, char *argv[])
{
    std::optional<Config> o_config = Config::try_from_args(argc, argv);
    if (!o_config)
    {
        std::cerr << "Config error!" << std::endl;
        return 1;
    }

    std::shared_ptr<MessageQueues> message_queues = std::make_shared<MessageQueues>();

    auto controller = std::make_unique<Controller>(o_config.value(), message_queues);
    std::thread controller_thread([&controller]() {
        controller->run();
    });

    { // w bloku zeby wywolac destruktor view po zakonczeniu view.run()
        View view(message_queues);
        view.run();
    }

    controller_thread.join();
    return 0;
}
