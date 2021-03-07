/**
 * @file Field.hpp
 * @author Bartosz Świrta
 * @brief Zawiera deklaracje klas: Field - reprezentuje pojedyncze pole, FieldBoard - reprezentuje plansze. Zawiera rowniez definicje FieldState - enumerator stanu pola
 * @version 1.0
 * @date 2021-01-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "Utils.hpp"
#include <array>
#include <iostream>
#include <optional>

namespace ox
{
    /** \enum FieldState
     * @brief Możliwe stany pola w grze.
     * 
     */
    enum FieldState
    {
        EMPTY,
        CIRCLED,
        CROSSED,
        FIELD_STATE_ERROR
    };

    /** \class Field
     * @brief Pojedyńcze pole w grze.
     * 
     */
    class Field
    {
    public:
        /**
         * @brief Utworzenie pola o podanym stanie.
         * 
         */
        Field(FieldState = EMPTY);
        /**
         * @brief Wyczyść pole.
         * 
         */
        void empty();
        /**
         * @brief Zaznacz pole kółkiem.
         * 
         */
        void circle();
        /**
         * @brief Zaznacz pole krzyżykiem.
         * 
         */
        void cross();
        /**
         * @brief Czy pole jest puste.
         */
        bool is_empty() const;
        /**
         * @brief Czy na polu jest krzyzyk.
         *
         */
        bool is_crossed() const;
        /**
         * @brief Czy na polu jest kółko.
         *
         */
        bool is_circled() const;
        /**
         * @brief Zwraca stan obiektu
         *
         */
        const FieldState get_state() const;

    private:
        /**
         * @brief Stan pola.
         *
         */
        FieldState fieldState;
    };

    /** \class FieldBoard
     * @brief Reprezentuje plansze do gry o rozmiarze 3x3.
     * 
     */
    class FieldBoard
    {
    public:
        /// Resetowanie planszy - ustawia wszyskie pola na puste.
        void reset_game_fields();
        /// Postawienie krzyżyka na danym polu.
        bool cross_field(int x, int y);
        /// Postawnienie kółka na danym polu.
        bool circle_field(int x, int y);
        /// Wyczyszczenie danego pola.
        bool empty_field(int x, int y);
        /// Zwróć stan danego pola.
        const FieldState get_field_state(int x, int y) const;
        /// Czy plansza jest pełna - żadne pole nie jest puste.
        bool is_board_full() const;
        /// Sprawdź stan planszy - jeden z graczy wygrał / jest remis / gra trwa.
        const GameStateEnum check_state() const;

        /// Spróbuj odczytać zapisany wcześniej stan planszy ze strumienia.
        static std::optional<FieldBoard> try_read(std::istream &);
        /// Zapisz stan planszy do strumienia.
        void write(std::ostream &) const;

    private:
        /// Pola na planszy w tablicy interpretowanej później jako tablica dwuwymiarowa 3x3.
        std::array<Field, 9> fieldArray;
    };

} // namespace ox
