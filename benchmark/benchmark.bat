set build_dir=build

cmake -S . -B %build_dir% -G "MinGW Makefiles"
cmake --build %build_dir%

for %%G in (build\*.exe) do (
    "%%G"
)