# Сборка

| `constexpr` флаг (`twist/build.hpp`) | Compile definition | Описание |
| --- | --- | --- |
| `kSim` | `__TWIST_BUILD_SIM__` | Детерминированная симуляция |
| `kIsolatedSim` | `__TWIST_BUILD_SIM_ISOLATION__` | Детерминированная симуляция с изоляцией памяти |
| `kThr` | `__TWIST_BUILD_THR__` | Потоки |
| `kPlain` | `__TWIST_BUILD_PLAIN__` | Сборка без вмешательства Twist |
| `kTwisted` | `__TWIST_BUILD_TWISTED__` или `__TWISTED__` | Сброка для тестов: потоки с fault injection или детерминированная симуляция |
