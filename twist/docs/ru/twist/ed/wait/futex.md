# Twist-_ed_

## Futex

Заголовочный файл: `twist/ed/wait/futex.hpp`

Для блокирующего ожидания вместо методов `wait` / `notify_{one,all}` у `std::atomic` нужно использовать
свободные функции из пространства имен `twist::ed::futex::`:
- `Wait` / `WaitTimed`
- `PrepareWake` + `Wake{One,All}`

### Пример

[examples/futex/main.cpp](/examples/futex/main.cpp)

```cpp
class OneShotEvent {
 public:
  void Wait() {
    if (fired_.load() == 0) {
      // Запарковать текущий поток, 
      // если в атомике fired_ находится значение 0
      twist::ed::futex::Wait(fired_, /*old=*/0);
    }  
  }

  void Fire() {
    // NB: _До_ записи в fired_
    auto wake_key = twist::ed::futex::PrepareWake(fired_);

    fired_.store(1);
    // Разбудить все потоки, ждущие в futex::Wait на fired_
    twist::ed::futex::WakeAll(wake_key);
  }

 private:
  // Wait работает только с atomic-ом для типа uint32_t
  twist::ed::std::atomic<uint32_t> fired_{0};
};
```

### Только Linux?

Здесь `futex` – собирательное имя для механизма блокирующего ожидания, который предоставляет
конкретная операционная система:

- `futex` для Linux,
- `ulock` для Darwin,
- `WaitOnAddress` для Windows.

### Операции

Обозначим через `SysWait` и `SysWake` системные вызовы для парковки / пробуждения потока в данной операционной системе.

#### Wait

```cpp
uint32_t Wait(atomic<uint32_t>& atom, uint32_t old, std::memory_order mo = std::memory_order::seq_cst) {
  uint32_t curr;
  
  do {
    SysWait(Addr(atom), old);  // Здесь Addr(atom) – взятие адреса атомика
  } while ((curr = atom.load(mo)) == old);
  
  return curr;  // Новое значение
}
```

#### Wake

`Wake` – двухфазный:

1) Сначала (_до_ записи в атомик, которая предшествует пробуждению) с помощью `PrepareWake` фиксируется ключ (`WakeKey`) для адресации системной очереди ожидания для атомика (фактически – адрес ячейки памяти):

```cpp
WakeKey PrepareWake(atomic<uint32_t>& atom) {
  return {Addr(atom)};
}
````

2) Затем (_после_ записи в атомик) вызывается `Wake{One,All}` с адресом, зафиксированном на первом шаге, который и будит ждущие потоки.

```cpp
void WakeOne(WakeKey key) {
  SysWake(key.addr, 1);
}
```

Обратите внимание: `PrepareWake` следует выполнять **до записи** в атомик.

См. http://wg21.link/P2616.

#### WaitTimed

С помощью `futex::WaitTimed` можно встать в очередь ожидания с таймаутом:

```cpp
bool [woken, new_value] = futex::WaitTimed(state, kNotReady, 1s);
```

Если по истечении таймаута поток не был разбужен вызовом `WakeOne` / `WakeAll`, то он сам покинет очередь ожидания фьютекса.

Вызов возвращает пару (`woken`, `new_value`).

`woken` будет равен
- `true`, если 1) поток был разбужен с помощью вызовов `WakeOne` / `WakeAll` **или** 2) произошел spurious wakeup
- `false`, если истек таймаут

Между таймаутом и явным пробуждением есть гонка, так что `false` означает **лишь** то, что таймаут **точно истек**. Пробудился ли поток по этому таймауту или был разбужен явным `Wake{One, All}` – неизвестно.

Если `woken == true`, то в `new_value` будет находиться новое значение атомика (отличное от `old`).

## `atomic<uint64_t>`

Функции `Wait`, `WaitTimed` и `PrepareWake` могут работать не только с `atomic_uint32_t`,
но и отдельно с младшей / старшей половиной `atomic_uint64_t`.

Нужная половина `atomic_uint64_t` адресуется с помощью функций `RefLowHalf` / `RefHighHalf`.

### Пример

```cpp
// Будем использовать старшие биты state для хранения флага
twist::ed::std::atomic_uint64_t state{0};

const uint64_t kSetFlag = (uint64_t)1 << 32;

// Ссылка для ожидания
auto flag_ref = twist::ed::futex::RefHighHalf(state);

twist::ed::std::thread t([&, flag_ref] {
  auto wake_key = twist::ed::futex::PrepareWake(flag_ref);
  state.fetch_or(kSetFlag);
  twist::ed::futex::WakeOne(wake_key);
});

// Wait возвращает новое значение атомика
uint64_t s = twist::ed::futex::Wait(flag_ref, 1);

auto f = s >> 32;
assert(f == 1);

t.join();
```