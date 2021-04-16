# PSZT-P_Warcaby
Projekt realizowany w ramach przedmiotu PSZT (Podstawy Sztucznej Inteligencji).
Prosta gra w warcaby w wersji rosyjskiej. Gracz może rozgrywać partię z komputerem, który wykorzytuje drzewo gry (algorytm minimax z przycinaniem alpha-beta) lub z innym graczem wykonując ruchy na zmianę na jednej maszynie.

## Wymagania do kompilacji
- Python (3.8+) wykrywany przez CMake

## Budowanie i uruchomienie
Budowanie:
``` sh
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build .
cd ..
```
Uruchomienie:
#### Windows
``` sh
./bin/Debug/pszt_warcaby.exe [opcjonalne parametry wywołania]
```
#### Linux
``` sh
./bin/pszt_warcaby [opcjonalne parametry wywołania]
```
## Parametry wywołania programu
Wszystkie parametry składają się z dwuch członów - opcji oraz przypisywanej jej wartości. \
Program akceptuje następujące parametry wywołania:
- --log (ścieżka do pliku) - ścieżka do pliku w którym zapisane będa statystyki rozgrywki.
- --gui (true/false) - czy uruchamiać widok (przydatne do testów komputer vs komputer).
- --wbot (true/false) - czy graczem białym steruje komputer.
- --bbot (true/false) - czy graczem czarnym steruje komputer.
- --wheuristic (basic/a_basic/board_aware) - heurysytyka którą posługuje się biały komputer.
- --bheuristic (basic/a_basic/board_aware) - heurysytyka którą posługuje się czarny komputer.
- --wdepth (liczba dodatnia) - maksymalna głębokość przesukiwania drzewa gry przez biały komputer.
- --bdepth (liczba dodatnia) - maksymalna głębokość przesukiwania drzewa gry przez czarny komputer.

## Skrypt testujący grę komputera
Skrypt bot_tests.py przeprowadza gry pomiędzy różnymi heurystykami z różnymi ustawieniami głębokości.\
W folderze *match_results* umieszcza surowe logi z gier. \
W folderze *results* umieszcza logi gier z uśrednionym czasem ruchu każdego gracza. \
W folderze *reports* umieszcza raporty dotyczące potyczek konkretnych par heurystyk oraz czasu ruchów heurystyk \
#### Uruchomienie
```
python bot_tests.py [ścieżka do programu warcaby]
```

## Generowanie dokumentacji
Flaga BUILD_DOCS w CMakeLists.txt kontroluje budowanie dokumentacji
``` cmake
set(BUILD_DOCS True)
```
Wygenerowana dokumentacja HTML jest umieszczana w folderze docs
``` sh
cd docs/html
```
Jeśli dokumentacja HTML nie wygenerowała się automatycznie trzeba ręcznie ją wygenerować z wygenerowanego pliku doxyfile w folderze cmake-build za pomocą terminalowego doxygena lub doxywizarda
``` sh
doxygen cmake-build/doxyfile
```
