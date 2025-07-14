set build_dir=build
set std=17

cmake -S . -B %build_dir% -G "MinGW Makefiles"
cmake --build %build_dir%

for %%G in (build\*.exe) do (
    "%%G"
)