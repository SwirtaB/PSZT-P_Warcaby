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

#include <optional>

#include "../include/View.hpp"

using namespace checkers;

/**
 * @brief Konstruktor widoku z podaną konfiguracją.
 * @details Komunikuje się z widokiem za pomocą podanego wspólnego pośrednika.
 * 
 * @param config - konifuracja używana przy tworzeniu widoku.
 * @param messageQueues_ - uchwyt do kolejki komunikatów.
 */
View::View(std::shared_ptr<MessageQueues> messageQueues_)
    : Application(400, 400, "Checkers", false)
    , messageQueues(std::move(messageQueues_))
    , lastState()
{
//    set_window_size(400, 400);
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
    ImGui::SetWindowFontScale(get_font_scale());

    check_for_new_state();

    if (lastState)
    {
        GameProgressEnum &progress = lastState.value().gameProgressEnum;
        BoardState &boardState = lastState.value().boardState;
        std::optional<Coord> &selected = lastState->selectedField;

        if (progress == PLAYING) {

            ImGui::Columns(8, nullptr, false);

            for (int y = 7; y >= 0; --y)
            {
                for (int x = 0; x < 8; ++x)
                {
                    ImGui::PushID("b");

                    bool is_selected = selected && selected.value().x == x && selected.value().y == y;
                    if (board_button(boardState.fields[x][y], is_selected, (x + y) % 2 == 0))
                    {
                        send_player_input(PlayerInputMessage(SELECT, x, y));
                    }
                    ImGui::NextColumn();
                }
            }
            for (int i = 0; i < 64; ++i) {
                ImGui::PopID();
            }

        } else if (progress == WHITE_WON) {
            ImGui::Text("White Won!");
            ImGui::NewLine();
            if (ImGui::Button("Exit")) {
                send_player_input(PlayerInputMessage(EXIT, 0, 0));
                quit();
            }
        } else if (progress == BLACK_WON) {
            ImGui::Text("Black Won!");
            ImGui::NewLine();
            if (ImGui::Button("Exit")) {
                send_player_input(PlayerInputMessage(EXIT, 0, 0));
                quit();
            }
        } else if (progress == TIE) {
            ImGui::Text("Tie!");
            ImGui::NewLine();
            if (ImGui::Button("Exit")) {
                send_player_input(PlayerInputMessage(EXIT, 0, 0));
                quit();
            }
        }
    }

    ImGui::End();
}

/**
 * @brief Oblicza współczynnik skalowania tekstu w zależności od rozmiaru okna.
 * 
 * @return float - wspołczynnik skalowania tekstu.
 */
float View::get_font_scale() const
{
    float min_dimension = std::min(get_window_size().x, get_window_size().y);
    return min_dimension / 200;
}

ImVec2 View::get_button_size() {
    return ImVec2(45.0f, 45.0f);
}

/**
 * @brief Wyświetla przycisk na planszy w podanym stanie.
 * 
 * @param state - aktualna plansza.
 * @return true - gdy przycisk został poprawnie wyświetlony.
 * @return false - w przypadku błędu.
 */
bool View::board_button(std::optional<PieceEnum> &pawn, bool is_selected, bool is_dark) const
{
    if (is_selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
    } else {
        if (is_dark) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        }
    }

    bool pressed = false;
    if (!pawn.has_value()) {
        pressed = ImGui::Button("", get_button_size());
    } else {
        switch (pawn.value())
        {
            case BLACK_PAWN:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                pressed = ImGui::Button("o", get_button_size());
                ImGui::PopStyleColor();
                break;
            case BLACK_QUEEN:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                pressed = ImGui::Button("Q", get_button_size());
                ImGui::PopStyleColor();
                break;
            case WHITE_PAWN:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                pressed = ImGui::Button("o", get_button_size());
                ImGui::PopStyleColor();
                break;
            case WHITE_QUEEN:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
                pressed = ImGui::Button("Q", get_button_size());
                ImGui::PopStyleColor();
                break;
            default:
                ImGui::Button("Error", get_button_size());
        }
    }

    ImGui::PopStyleColor();
    return pressed;
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