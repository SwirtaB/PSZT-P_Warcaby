//
// Created by swirta on 20.11.2020.
//

#define BOOST_TEST_MODULE ClassTests

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "../include/Config.hpp"
#include "../include/Field.hpp"
#include "../include/MessageQueues.hpp"
#include "../include/Controller.hpp"
#include "../include/BotMove.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <optional>
#include <thread>
#include <cstdio>

#include <boost/python.hpp>

namespace py = boost::python;

using namespace ox;

/// BOOST PYTHON /////////////

BOOST_AUTO_TEST_CASE(boost_python_test)
{
    std::cout << "---BOOST.PYTHON TEST---" << std::endl;

    Py_Initialize();
    py::object main_module = py::import("__main__");
    py::object main_namespace = main_module.attr("__dict__");
    int res = 0;
    try
    {
        py::object noneType = py::exec("res = 3", main_namespace);
        res = py::extract<int>(main_namespace["res"]);
    }
    catch (py::error_already_set const &)
    {
        PyErr_Print();
    }
    BOOST_CHECK_EQUAL(res, 3);
}

//! BOOST PYTHON /////////////

/// CONFIG ///////////////////

BOOST_AUTO_TEST_CASE(Config_read_test)
{
    std::cout << "---PYTHON CONFIG READ TEST---" << std::endl;

    std::ifstream ifs = std::move(std::ifstream("no_file_test_config.py"));
    std::string no_file_config_string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    std::optional<Config> o_config = Config::try_from_script(no_file_config_string.c_str());
    BOOST_CHECK_EQUAL(o_config.has_value(), false);

    ifs = std::move(std::ifstream("worng_data_file_test_config.py"));
    std::string worng_data_config_string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    o_config = Config::try_from_script(no_file_config_string.c_str());
    BOOST_CHECK_EQUAL(o_config.has_value(), false);

    ifs = std::move(std::ifstream("test_config.py"));
    std::string config_string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    o_config = Config::try_from_script(config_string.c_str());

    BOOST_REQUIRE_EQUAL(o_config.has_value(), true);
    BOOST_CHECK_EQUAL(o_config.value().bot_tactic, GAME_TREE);
    BOOST_CHECK_EQUAL(o_config.value().depth, 10);
    BOOST_CHECK_EQUAL(o_config.value().start, CROSS);
    BOOST_CHECK_EQUAL(o_config.value().circle_is_bot, true);
    BOOST_CHECK_EQUAL(o_config.value().cross_is_bot, false);
    BOOST_CHECK_EQUAL(o_config.value().save_path, "save.txt");
    BOOST_CHECK_EQUAL(o_config.value().ui_size, SMALL);
}

//! CONFIG ///////////////////

/// FIELD ////////////////////

BOOST_AUTO_TEST_CASE(Field_manipulation_test)
{
    std::cout << "---FIELD MANIPULATION TEST---" << std::endl;

    Field field;
    BOOST_CHECK_EQUAL(field.is_empty(), true);
    BOOST_CHECK_EQUAL(field.is_circled(), false);
    BOOST_CHECK_EQUAL(field.is_crossed(), false);
    BOOST_CHECK_EQUAL(field.get_state(), EMPTY);

    Field field2(CIRCLED);
    BOOST_CHECK_EQUAL(field2.is_empty(), false);
    BOOST_CHECK_EQUAL(field2.is_circled(), true);
    BOOST_CHECK_EQUAL(field2.is_crossed(), false);
    BOOST_CHECK_EQUAL(field2.get_state(), CIRCLED);

    Field field3(CROSSED);
    BOOST_CHECK_EQUAL(field3.is_empty(), false);
    BOOST_CHECK_EQUAL(field3.is_circled(), false);
    BOOST_CHECK_EQUAL(field3.is_crossed(), true);
    BOOST_CHECK_EQUAL(field3.get_state(), CROSSED);

    Field field4 = field2;
    BOOST_CHECK_EQUAL(field4.is_empty(), false);
    BOOST_CHECK_EQUAL(field4.is_circled(), true);
    BOOST_CHECK_EQUAL(field4.is_crossed(), false);
    BOOST_CHECK_EQUAL(field4.get_state(), CIRCLED);

    field4.empty();
    BOOST_CHECK_EQUAL(field4.is_empty(), true);
    BOOST_CHECK_EQUAL(field4.is_circled(), false);
    BOOST_CHECK_EQUAL(field4.is_crossed(), false);
    BOOST_CHECK_EQUAL(field4.get_state(), EMPTY);

    field4.circle();
    BOOST_CHECK_EQUAL(field4.is_empty(), false);
    BOOST_CHECK_EQUAL(field4.is_circled(), true);
    BOOST_CHECK_EQUAL(field4.is_crossed(), false);
    BOOST_CHECK_EQUAL(field4.get_state(), CIRCLED);

    field4.cross();
    BOOST_CHECK_EQUAL(field4.is_empty(), false);
    BOOST_CHECK_EQUAL(field4.is_circled(), false);
    BOOST_CHECK_EQUAL(field4.is_crossed(), true);
    BOOST_CHECK_EQUAL(field4.get_state(), CROSSED);

    BOOST_CHECK_EQUAL(field2.is_empty(), false);
    BOOST_CHECK_EQUAL(field2.is_circled(), true);
    BOOST_CHECK_EQUAL(field2.is_crossed(), false);
    BOOST_CHECK_EQUAL(field2.get_state(), CIRCLED);
}

BOOST_AUTO_TEST_CASE(FieldBoard_manipulation_test)
{
    std::cout << "---FIELD BOARD MANIPULATION TEST---" << std::endl;

    FieldBoard fb;
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            BOOST_REQUIRE_EQUAL(fb.get_field_state(x, y), EMPTY);
        }
    }
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), false);
    BOOST_REQUIRE_EQUAL(fb.check_state(), PLAYING);

    fb.cross_field(1, 2);
    BOOST_REQUIRE_EQUAL(fb.get_field_state(1, 2), CROSSED);
    fb.circle_field(2, 2);
    BOOST_REQUIRE_EQUAL(fb.get_field_state(2, 2), CIRCLED);
    fb.empty_field(1, 2);
    BOOST_REQUIRE_EQUAL(fb.get_field_state(1, 2), EMPTY);
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), false);
    BOOST_REQUIRE_EQUAL(fb.check_state(), PLAYING);

    fb.reset_game_fields();
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            BOOST_REQUIRE_EQUAL(fb.get_field_state(x, y), EMPTY);
        }
    }
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), false);
    BOOST_REQUIRE_EQUAL(fb.check_state(), PLAYING);

    fb.cross_field(0, 0);
    fb.cross_field(1, 0);
    fb.cross_field(2, 0);
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), false);
    BOOST_REQUIRE_EQUAL(fb.check_state(), CROSS_WON);

    fb.reset_game_fields();
    fb.circle_field(2, 0);
    fb.circle_field(1, 1);
    fb.circle_field(0, 2);
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), false);
    BOOST_REQUIRE_EQUAL(fb.check_state(), CIRCLE_WON);

    fb.reset_game_fields();
    fb.circle_field(0, 0);
    fb.circle_field(1, 0);
    fb.cross_field(2, 0);
    fb.circle_field(0, 1);
    fb.cross_field(1, 1);
    fb.circle_field(2, 1);
    fb.cross_field(0, 2);
    fb.cross_field(1, 2);
    fb.circle_field(2, 2);
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), true);
    BOOST_REQUIRE_EQUAL(fb.check_state(), CROSS_WON);

    fb.reset_game_fields();
    fb.circle_field(0, 0);
    fb.circle_field(1, 0);
    fb.cross_field(2, 0);
    fb.cross_field(0, 1);
    fb.cross_field(1, 1);
    fb.circle_field(2, 1);
    fb.circle_field(0, 2);
    fb.cross_field(1, 2);
    fb.circle_field(2, 2);
    BOOST_REQUIRE_EQUAL(fb.is_board_full(), true);
    BOOST_REQUIRE_EQUAL(fb.check_state(), TIE);
}

BOOST_AUTO_TEST_CASE(FieldBoard_serialization_test)
{
    std::cout << "---FIELD BOARD SERIALIZATION TEST---" << std::endl;

    std::stringstream s;

    FieldBoard fb;
    fb.circle_field(0, 0);
    fb.circle_field(1, 0);
    fb.cross_field(2, 0);
    fb.cross_field(0, 1);
    fb.cross_field(1, 1);
    fb.circle_field(2, 1);
    fb.circle_field(0, 2);
    fb.cross_field(1, 2);
    fb.circle_field(2, 2);

    fb.write(s);
    std::optional<FieldBoard> fb2 = FieldBoard::try_read(s);
    BOOST_REQUIRE_EQUAL(fb2.has_value(), true);
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            BOOST_REQUIRE_EQUAL(fb.get_field_state(x, y), fb2.value().get_field_state(x, y));
        }
    }
}

//! FIELD ////////////////////

/// MESSAGE_QUEUES ///////////

BOOST_AUTO_TEST_CASE(MessageQueues_test)
{
    std::cout << "---MESSAGE QUEUES TEST---" << std::endl;

    MessageQueues mq;

    PlayerInputMessage pim(INPUT, 1, 2);
    PlayerInputMessage *recieved;
    std::thread handle([&] {
        recieved = new PlayerInputMessage(mq.wait_for_player_input());
    });
    mq.send_player_input(pim);
    handle.join();
    BOOST_REQUIRE_EQUAL(pim.messageType, recieved->messageType);
    BOOST_REQUIRE_EQUAL(pim.x, recieved->x);
    BOOST_REQUIRE_EQUAL(pim.y, recieved->y);
    delete recieved;

    FieldBoard fb;
    fb.circle_field(0, 0);
    fb.circle_field(1, 0);
    fb.cross_field(2, 0);
    fb.cross_field(0, 1);
    fb.cross_field(1, 1);
    fb.circle_field(2, 1);
    fb.circle_field(0, 2);
    fb.cross_field(1, 2);
    fb.circle_field(2, 2);
    GameStateMessage gsm(TIE, fb);
    BOOST_REQUIRE_EQUAL(mq.check_for_game_state().has_value(), false);
    mq.send_game_state(gsm);
    std::optional<GameStateMessage> recieved2 = mq.check_for_game_state();
    BOOST_REQUIRE_EQUAL(recieved2.has_value(), true);
    BOOST_REQUIRE_EQUAL(gsm.gameStateEnum, recieved2.value().gameStateEnum);
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            BOOST_REQUIRE_EQUAL(gsm.fieldBoard.get_field_state(x, y), recieved2.value().fieldBoard.get_field_state(x, y));
        }
    }
}

//! MESSAGE_QUEUES ///////////

/// BOT_MOVE /////////////////

BOOST_AUTO_TEST_CASE(BotMove_test)
{
    std::cout << "---BOT MOVE TEST---" << std::endl;

    FieldBoard fb;
    fb.cross_field(0, 0);
    fb.circle_field(2, 1);
    fb.cross_field(0, 2);
    fb.circle_field(0, 1);
    fb.cross_field(1, 1);
    std::pair<int, int> move = bot::bot_move(fb, CIRCLE_PLAYER, HEURISTIC, 10);
    BOOST_CHECK_EQUAL(fb.get_field_state(move.first, move.second), EMPTY);
    std::pair<int, int> move2 = bot::bot_move(fb, CIRCLE_PLAYER, GAME_TREE, 10);
    BOOST_CHECK_EQUAL(fb.get_field_state(move2.first, move2.second), EMPTY);

    fb.circle_field(2, 2);
    fb.cross_field(1, 0);
    fb.circle_field(1, 2);
    std::pair<int, int> move3 = bot::bot_move(fb, CROSS_PLAYER, HEURISTIC, 10);
    BOOST_CHECK_EQUAL(move3.first, 2);
    BOOST_CHECK_EQUAL(move3.second, 0);
    std::pair<int, int> move4 = bot::bot_move(fb, CROSS_PLAYER, GAME_TREE, 10);
    BOOST_CHECK_EQUAL(move4.first, 2);
    BOOST_CHECK_EQUAL(move4.second, 0);
}

//! BOT_MOVE /////////////////

/// CONTROLLER ///////////////

BOOST_AUTO_TEST_CASE(Controller_play_game_test)
{
    std::cout << "---CONTROLLER PLAY GAME TEST---" << std::endl;
    std::shared_ptr<MessageQueues> testQueuesHandler = std::make_shared<MessageQueues>();

    Config config;
    config.start = ConfigStart::CIRCLE;
    config.circle_is_bot = false;
    config.cross_is_bot = false;

    Controller game(config, testQueuesHandler);
    testQueuesHandler->send_player_input(PlayerInputMessage(INPUT, 1, 1));
    testQueuesHandler->send_player_input(PlayerInputMessage(INPUT, 1, 0));
    testQueuesHandler->send_player_input(PlayerInputMessage(INPUT, 0, 0));
    testQueuesHandler->send_player_input(PlayerInputMessage(INPUT, 0, 2));
    testQueuesHandler->send_player_input(PlayerInputMessage(INPUT, 2, 2));
    testQueuesHandler->send_player_input(PlayerInputMessage(EXIT, 0, 0));
    game.run();

    int recieved = 0;
    while (recieved < 6)
    {
        std::optional<GameStateMessage> gs = testQueuesHandler->check_for_game_state();
        if (gs)
        {
            if (recieved == 0)
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, PREPARING);
            }
            else
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, PLAYING);
            }
            ++recieved;
        }
    }

    std::optional<GameStateMessage> last_gs;
    do
    {
        last_gs = testQueuesHandler->check_for_game_state();
    } while (!last_gs);
    BOOST_REQUIRE_EQUAL(last_gs.value().gameStateEnum, CIRCLE_WON);
}

BOOST_AUTO_TEST_CASE(Controller_save_game_test)
{
    std::cout << "---CONTROLLER SAVE GAME TEST---" << std::endl;

    Config config;
    config.start = ConfigStart::CIRCLE;
    config.circle_is_bot = false;
    config.cross_is_bot = false;
    config.save_path = "test_save";

    std::shared_ptr<MessageQueues> mq = std::make_shared<MessageQueues>();
    remove(config.save_path.c_str());
    Controller game(config, mq);
    mq->send_player_input(PlayerInputMessage(INPUT, 1, 1));
    mq->send_player_input(PlayerInputMessage(INPUT, 1, 0));
    mq->send_player_input(PlayerInputMessage(INPUT, 0, 0));
    mq->send_player_input(PlayerInputMessage(EXIT, 0, 0));
    game.run();

    int recieved = 0;
    while (recieved < 5)
    {
        std::optional<GameStateMessage> gs = mq->check_for_game_state();
        if (gs)
        {
            if (recieved == 0)
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, PREPARING);
            }
            else
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, PLAYING);
            }
            ++recieved;
        }
    }

    std::shared_ptr<MessageQueues> mq2 = std::make_shared<MessageQueues>();
    Controller game2(config, mq2);
    mq2->send_player_input(PlayerInputMessage(LOAD_SAVE, 0, 0));
    mq2->send_player_input(PlayerInputMessage(INPUT, 0, 2));
    mq2->send_player_input(PlayerInputMessage(INPUT, 2, 2));
    mq2->send_player_input(PlayerInputMessage(EXIT, 0, 0));
    game2.run();
    BOOST_REQUIRE_EQUAL(remove(config.save_path.c_str()), 0);

    int recieved2 = 0;
    while (recieved2 < 4)
    {
        std::optional<GameStateMessage> gs = mq2->check_for_game_state();
        if (gs)
        {
            if (recieved2 == 0)
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, PREPARING);
            }
            else if (recieved2 == 1)
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, LOAD_SAVE_QUESTION);
            }
            else
            {
                BOOST_REQUIRE_EQUAL(gs.value().gameStateEnum, PLAYING);
            }
            ++recieved2;
        }
    }

    std::optional<GameStateMessage> last_gs;
    do
    {
        last_gs = mq2->check_for_game_state();
    } while (!last_gs);
    BOOST_REQUIRE_EQUAL(last_gs.value().gameStateEnum, CIRCLE_WON);
}

//! CONTROLLER ///////////////
