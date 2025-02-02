# Twist-_ed_

## `std`

В [`twist/ed/std`](/source/twist/ed/std) / пространстве имен `twist::ed::std::` собраны замены примитивам синхронизации и функциям из стандартной библиотеки.

Примитивы из `twist::ed::std::` повторяют интерфейсы и сохраняют поведение примитивов из `::std::`, так что вы можете пользоваться документацией https://en.cppreference.com/w/ и **не думать**<sup>†</sup> про _Twist_.

### Примитивы синхронизации

Вместо `std::thread`, `std::atomic`, `std::mutex`, `std::condition_variable` и других примитивов из [Concurrency support library](https://en.cppreference.com/w/cpp/thread)
необходимо использовать одноименные примитивы из пространства имен `twist::ed::std::`.

Заголовочные файлы меняются по следующему принципу:
`#include <atomic>` → `#include <twist/ed/std/atomic.hpp>`

Использование примитивов из `::std::` приведет к неопределенному поведению в тестах, будьте внимательны!

#### Локи

При этом можно использовать `std::lock_guard` и `std::unique_lock` (но только в связке с `twist::ed::std::mutex`), это не примитивы синхронизации, а RAII для более безопасного использования мьютекса.

### Планировщик

Заголовочный файл: `twist/ed/std/thread.hpp`

#### `yield`

Вместо `std::this_thread::yield` нужно использовать `twist::ed::std::this_thread::yield`.

А еще лучше использовать `twist::ed::SpinWait` из заголовочного файла `<twist/ed/wait/spin.hpp>`.

#### `sleep_for`

Вместо `std::this_thread::sleep_for` нужно использовать `twist::ed::std::this_thread::sleep_for`.

### Поддержаны

- `atomic<T>`, `atomic_flag` (`atomic.hpp`)
- `mutex`, `timed_mutex` (`mutex.hpp`)
- `shared_mutex` (`shared_mutex.hpp`)
- `condition_variable` (`condition_variable.hpp`)
- `thread` (`thread.hpp`)
- `this_thread::` (`thread.hpp`)
    - `yield`
    - `sleep_for`, `sleep_until`
    - `get_id`
- `random_device` (`random.hpp`)
- `chrono::` (`chrono.hpp`)
    - `system_clock`
    - `steady_clock`
    - `high_resolution_clock`

#### <sup>†</sup> Отступления от `std`

- `atomic` намеренно не поддерживает методы `wait` / `notify` в пользу [более безопасного API](/docs/ru/twist/ed/wait/futex.md)
    - Включить поддержку `wait` можно с помощью флага CMake `TWIST_ATOMIC_WAIT=ON`
- `atomic` намеренно не поддерживает типы данных, размер которых превосходит `sizeof(std::uintptr_t)`
- default constructor для `atomic` помечен как `deprecated`: предпочитайте явную инициализацию
