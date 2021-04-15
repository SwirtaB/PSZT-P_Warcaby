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
    std::optional<Config> config = Config::try_from_args(argc, argv);
    if (!config.has_value())
    {
        std::cerr << "Config error!" << std::endl;
        return 1;
    }

    std::shared_ptr<MessageQueues> message_queues = std::make_shared<MessageQueues>();

    if (config.value().showGUI) {
        auto controller = std::make_unique<Controller>(config.value(), message_queues);

        std::thread controller_thread([&controller]() {
            controller->run();
        });
        View(message_queues).run();

        controller_thread.join();
    } else {
        Controller(config.value(), message_queues).run();
    }

    return 0;
}
