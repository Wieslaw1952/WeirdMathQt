# WeirdMath

Fraktale typu trójkąta Sierpińskiego rysowane metodą „gry w chaos” (Qt 6, C++17).
Port starszego programu napisanego w C++Builder/VCL.

## Pobieranie

Gotowe paczki są w zakładce [Releases](https://github.com/Wieslaw1952/WeirdMathQt/releases/latest):

- **Windows**: `WeirdMath-<wersja>-setup.exe` (instalator) albo `WeirdMath-<wersja>-win64-portable.zip` (wersja przenośna, rozpakuj i uruchom `WeirdMath.exe`)
- **Linux (Ubuntu/Debian)**: `WeirdMath-<wersja>-Linux.deb`, instalacja: `sudo apt install ./WeirdMath-<wersja>-Linux.deb`
- **Linux (inne)**: `WeirdMath-<wersja>-Linux.tar.gz`, rozpakuj i uruchom `bin/WeirdMath`

Paczki zawierają biblioteki Qt, więc Qt nie musi być zainstalowane.

## Budowanie ze źródeł

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<ścieżka do Qt>/gcc_64 -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && cpack
```
