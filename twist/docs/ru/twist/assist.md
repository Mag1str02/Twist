# Assist

Аннотации для проверок, подсказки для симулятора / планировщика.

Директория: [`twist/assist/`](/source/twist/assist)

## Инварианты

Заголовок: `twist/assist/assert.hpp`

```cpp
// Где-то в примитиве синхронизации
auto state = state_.exchange(0);
TWIST_ASSERT(state != 0);
```

## Память

Заголовок: `twist/assist/memory.hpp`

### `MemoryAccess`

Явная инструментация обращения к диапазону динамической памяти, выделенной через
`new`.

```cpp
twist::assist::MemoryAccess(addr, size);
```

Проверка выполняется только в режиме симуляции с изоляцией памяти. В других режимах – no-op.

### `Access(T*)`

`Access(T* ptr)` ~ `MemoryAccess(ptr, sizeof(T))`

```cpp
if (top_.compare_exchange_weak(top, top->next)) {
  twist::assist::Access(top);
  delete top;
}
```

### `Ptr`

Обертка для указателя на динамически аллоцированный (т.е. с помощью `new`) объект,
которая проверяет адресуемость этого объекта при обращении к нему через `operator*` и
`operator->`.

```cpp
twist::assist::Ptr<Widget> w = new Widget{42};

// Ptr<T> повторяет интерфейс указателя 

// explicit operator bool
if (w) {
  // operator-> выполняет проверку MemoryAccess(ptr, sizeof(T))
  w->Foo();
}

// operator* – MemoryAccess
(*w).Bar();

// Ptr<T> – не умный указатель и не управляет памятью
delete w.raw;
```

Лучше конструировать `Ptr` прямо в точке обращения, тогда он захватит source location:

```cpp
Widget* w = new Widget{11};

twist::assist::Ptr(w)->Bar();  // Захватываем source location обращения
```

Source location для обращения через уже существующий `Ptr<T>` можно захватить с помощью `operator ()`:

```cpp
twist::assist::Ptr<Widget> w = new Widget{7};

// operator() - захват текущего source location
w()->Baz();
```

См. [демо с лок-фри стеком Трейбера](/demo/treiber_stack/treiber_stack.hpp).

### `New`

Позволяет запомнить контекст (source location) аллокации для отчета об утечках памяти в симуляции.

```cpp
// Вместо new Widget{}
Widget* w = twist::assist::New<Widget>();

// Для удаления - обычный delete
delete w;
```

## Гонки

Заголовок: `twist/assist/shared.hpp`

Аннотация для разделяемой переменной, обращения к которой должны быть упорядочено
синхронизацией (мьютексами, атомиками, формально – через [happens-before](https://eel.is/c++draft/intro.races#def:happens_before))

```cpp
// Узел лок-фри очереди Michael-Scott
struct Node {
  twist::assist::Shared<T> datum;
  twist::ed::std::atomic<Node> next;
};
```

### API

```cpp
twist::assist::Shared<int> var{0};

twist::ed::std::thread t([&var] {
  *var = 1;  // operator* – запись
});

int val = var;  // operator T – чтение

t.join();
```

Вместо перегруженных операторов рекомендуется использовать методы `Write` и `Read`: они захватывают контекст
в исходном коде, который потом попадет в сообщение об ошибке.

```cpp
var.Write(1);  // Захватываем source location записи

int val = var.Read();  // Захватываем source location чтения
```

См. [демо с лок-фри стеком Трейбера](/demo/treiber_stack/treiber_stack.hpp).

## Preemption

Заголовочный файл: `twist/assist/preempt.hpp`

### `PreemptionPoint`

_Twist_ внедряет сбои / перехватывает управление только в точках обращения к примитивам синхронизации.

С помощью функции `void PreemptionPoint()` можно запросить переключение в произвольной точке вашего кода.

### `NoPreemptionGuard`

`NoPreemptionGuard` рекомендует рантайму Twist не внедрять сбои / не прерывать
потоки в своем лексическом скоупе:

```cpp
{
  twist::assist::NoPreemptionGuard hint;

  // Например, цикл чтения из локальной очереди воркера
  // при воровстве задач в work-stealing планировщике
}
```

Подсказка помогает Twist не тратить время на переключения, которые не окажутся полезными (по мнению пользователя)
для проверки корректности.

## Случайность

Заголовок: `twist/assist/random.hpp`

### `Choice`

`DfsScheduler` перебирает все возможные решения о планировании потоков, но иногда
для полноты тестирования нужно перебирать еще и случайные числа, которые генерирует пользователь.

Практический пример – выбор случайной жертвы для воровства в work-stealing планировщике задач.

Стандартная библиотека использует `random_device` как генератор случайности,
но в точке обращения к нему невозможно понять, из какого диапазона 
пользователь хочет получить случайное число.

`twist::assist::Choice` – это API для генерации случайных чисел из небольшого диапазона.

### Пример

```cpp
// Детерминированная псевдо-случайность
twist::ed::std::random_device rd{};

twist::assist::Choice choice{rd};

// Диапазон значений – [0, alts)
size_t a = choice(/*alts=*/3);

// Диапазон значений – [lo, hi)
size_t b = choice(/*lo=*/1, /*hi=*/5);
```

### Потоки

Для сборки с потоками `Choice` использует переданный `random_device` для инициализации [вихря Мерсенна](https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine), который, в свою очередь,
использует для генерации случайных индексов.

## Pruning

Заголовочный файл: `twist/assist/prune.hpp`

Подсказка для model checker: текущее исполнение пошло по не-перспективному (для обнаружения ошибки)
пути, так что все поддерево расписаний с текущим префиксом может быть обрезано.

Следует применять с большой осторожностью!

## Fiber

Заголовочный файл: `twist/assist/fiber.hpp`

Если пользователь использует переключения контекста для реализации кооперативной многозадачности
в виде файберов / stackful сопрограмм, то он должен аннотировать эти переключения для рантайма _Twist_.

Пример аннотации для сопрограммы:

```cpp
class Coroutine {
 public:
  Coroutine(Body body) {
    // Выделяем стек, инициализируем контекст исполнения, ...
    
    // Здесь stack_view - диапазон памяти стека файбера
    // Зная границы стека, Twist сможет выполнять больше проверок
    twist::assist::NewFiber(&fiber_, stack_view);
  }
  
  void Resume() {
    // _До_ переключения контекста
    // Вызов возвращает FiberHandle, ссылающийся на текущий (на момент вызова) файбер
    caller_fiber_ = twist::assist::SwitchToFiber(fiber_.Handle());
    
    // Переключаем контекст
  }
  
  void Suspend() {
    // Игнорируем возвращаемое значение, это fiber_.Handle()
    twist::assist::SwitchToFiber(caller_fiber_);
    
    // Переключаем контекст
  } 
  
 private:
  // Процедура пользователя, стек, контексты исполнения, ...
  
  // Служебные поля
  twist::assist::Fiber fiber_;
  twist::assist::FiberHandle caller_fiber_;
};
```

Здесь

- `Fiber` – объект, время жизни которого совпадает со временем жизни файбера; используется _Twist_
для хранения служебной информации, связанной с файбером пользователя,
- `FiberHandle` – указатель на `Fiber`.

Вызов `SwitchToFiber` возвращает `FiberHandle` для остановленного файбера.

Для каждого потока _Twist_ сам заводит служебный экземпляр `Fiber`.

### Стек

До того, как стек будет подготовлен к запуску файбера, можно "установить" его с помощью `twist::assist::ResetFiberStack(stack_view)`, где `stack_view` – диапазон памяти стека в вид `std::span<std::byte>`.