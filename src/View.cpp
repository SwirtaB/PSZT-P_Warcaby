/**
 * @file View.cpp
 * @author Maciej Wojno
 * @brief Zawiera defifnicję metod klasy View
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "../include/View.hpp"

using namespace ox;

/**
 * @brief Konstruktor widoku z podaną konfiguracją.
 * @details Komunikuje się z widokiem za pomocą podanego wspólnego pośrednika.
 * 
 * @param config - konifuracja używana przy tworzeniu widoku.
 * @param messageQueues_ - uchwyt do kolejki komunikatów.
 */
View::View(ox::Config &config, std::shared_ptr<MessageQueues> messageQueues_)
    : Application(300, 300, "OX", config.ui_size == RESIZABLE ? true : false), messageQueues(messageQueues_), lastState()
{
    switch (config.ui_size)
    {
    case SMALL:
        set_window_size(300, 300);
        break;

    case MEDIUM:
        set_window_size(600, 600);
        break;

    case LARGE:
        set_window_size(900, 900);
        break;

    case RESIZABLE:
        set_window_size(300, 300);
        break;
    }
}

View::~View()
{
    send_player_input(PlayerInputMessage(EXIT, 0, 0));
}

const int IMGUI_NO_TITLE_BAR = 1 << 0;
const int IMGUI_NO_RESIZE = 1 << 1;
const int IMGUI_NO_MOVE = 1 << 2;
const int IMGUI_NO_SCROLLBAR = 1 << 3;
const int IMGUI_NO_SCROLL_MOUSE = 1 << 4;

const int IMGUI_WINDOW_FLAGS =
    IMGUI_NO_TITLE_BAR |
    IMGUI_NO_RESIZE |
    IMGUI_NO_MOVE |
    IMGUI_NO_SCROLLBAR |
    IMGUI_NO_SCROLL_MOUSE;

/**
 * @brief Funkcja aktualizacji interfejsu gracza.
 * @details Jest wykonywana w pętli, ponieważ jest to interfejs typu immidiate mode GUI
 * 
 */
void View::update()
{
    ImGui::SetNextWindowSize(get_window_size());
    ImGui::SetNextWindowPos(get_window_pos());
    ImGui::Begin("window", nullptr, IMGUI_WINDOW_FLAGS);
    ImGui::SetWindowFontScale(get_font_scale()); // TEST

    check_for_new_state();

    if (lastState.has_value())
    {
        GameStateEnum &state = lastState.value().gameStateEnum;
        FieldBoard &fields = lastState.value().fieldBoard;

        switch (state)
        {
        case PREPARING:
            update_preparing();
            break;

        case LOAD_SAVE_QUESTION:
            update_load_save_question();
            break;

        case PLAYING:
            update_game_in_progress(fields);
            break;

        case CIRCLE_WON:
            update_game_finished(PlayerEnum::CIRCLE_PLAYER);
            break;

        case CROSS_WON:
            update_game_finished(PlayerEnum::CROSS_PLAYER);
            break;

        case TIE:
            update_game_finished(PlayerEnum::NONE);
            break;
        }
    }
    else
    {
        update_preparing();
    }

    ImGui::End();
}

/**
 * @brief Aktualizacja interfejsu w stanie gry PREPARING. 
 * @details Wyświetla pust panel.
 * 
 */
void View::update_preparing()
{
}

/**
 * @brief Aktualizacja interfejsu w stanie gry LOAD_SAVE_QUESTION. 
 * @details Wyświetla pytanie o wczytanie zapisanego stanu gry.
 * 
 */
void View::update_load_save_question()
{
    ImGui::Text("Load saved state?");

    if (ImGui::Button("Yes"))
    {
        send_player_input(PlayerInputMessage(LOAD_SAVE, 0, 0));
    }
    ImGui::PushID("b");
    if (ImGui::Button("No"))
    {
        send_player_input(PlayerInputMessage(NO_LOAD_SAVE, 0, 0));
    }
    ImGui::PopID();
}

/**
 * @brief Aktualizacja interfejsu w stanie gry IN_PROGRESS.
 * @details Wyświetla planszę gry i pozwala graczowi wybierać pole.
 * 
 * @param fields - aktualna plansza.
 */
void View::update_game_in_progress(FieldBoard &fields)
{
    ImGui::Columns(3, nullptr, false);

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            if (field_button(fields.get_field_state(x, y)))
            {
                send_player_input(PlayerInputMessage(INPUT, x, y));
                std::cout << x << " " << y << std::endl;
            }
            if (y != 3)
                ImGui::NextColumn();
            ImGui::PushID("b");
        }
    }
    for (int i = 0; i < 9; ++i)
        ImGui::PopID();
}

/**
 * @brief Aktualizacja interfejsu w stanie gry FINISHED.
 * @details Wyświetla informację o zwycięzcy i pozwala wywołac kolejną rozgrywkę.
 * 
 * @param player - gracz który wygrał.
 */
void View::update_game_finished(PlayerEnum player)
{
    if (player == CIRCLE_PLAYER && ImGui::Button("Circle won!", get_window_size()) || player == CROSS_PLAYER && ImGui::Button("Cross won!", get_window_size()) || player == NONE && ImGui::Button("Tie!", get_window_size()))
    {
        send_player_input(PlayerInputMessage(RESET_GAME, 0, 0));
    }
}

/**
 * @brief Oblicza współczynnik skalowania tekstu w zależności od rozmiaru okna.
 * 
 * @return float - wspołczynnik skalowania tekstu.
 */
float View::get_font_scale() const
{
    float min_dimension = std::min(get_window_size().x, get_window_size().y);
    return min_dimension / 300;
}

/**
 * @brief Wyświetla przycisk na planszy w podanym stanie.
 * 
 * @param state - aktualna plansza.
 * @return true - gdy przycisk został poprawnie wyświetlony.
 * @return false - w przypadku błędu.
 */
bool View::field_button(FieldState state) const
{
    switch (state)
    {
    case EMPTY:
        return ImGui::Button("", get_window_size() / 3);
    case CIRCLED:
        return ImGui::Button("O", get_window_size() / 3);
    case CROSSED:
        return ImGui::Button("X", get_window_size() / 3);
    default:
        return ImGui::Button("Error", get_window_size() / 3);
    }
}

/**
 * @brief Sprawdza czy przyszedł nowy stan od kontrolera.
 * 
 */
void View::check_for_new_state()
{
    auto state = messageQueues->check_for_game_state();
    if (state)
    {
        lastState = state;
    }
}

/**
 * @brief Wysyła akcję gracza do kontrolera.
 * 
 * @param message - akcja wysyłana do kontrolera.
 */
void View::send_player_input(PlayerInputMessage message) const
{
    messageQueues->send_player_input(message);
}