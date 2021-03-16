# PSZT-P_Warcaby
Prosta gra w warcaby w wersji rosyjskiej. Gracz może grać z komputerem, który wykorzytuje drzewo gry lub z innym graczem wykonując ruch na zmianę na jednej maszynie.

## Wymagania
#### Linux
- Biblioteka Boost (1.71+) wykrywana przez CMake
- Python (3.8+) wykrywany przez CMake
#### Windows
- Biblioteka Boost wykrywana przez kompilator (np. vcpkg zintegrowane z MSBuild)
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
./bin/Debug/pszt_warcaby.exe [sciezka/do/pliku/konfiguracyjnego]
```
#### Linux
``` sh
./bin/pszt_warcaby [sciezka/do/pliku/konfiguracyjnego]
```
## Generowanie i uruchomienie testow
Flaga BUILD_TESTS w CMakeLists.txt kontroluje uruchamianie testów przy budowie
``` cmake
set(BUILD_TESTS True)
```
Po zbudowaniu z flaga BUILD_TESTS:
#### Windows
``` sh
cd tests
../build/Debug/tests.exe
```
#### Linux
``` sh
cd tests
../build/tests
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
