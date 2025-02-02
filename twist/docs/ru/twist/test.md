# Test

Утилиты для описания тестовых сценариев.

## `Plate`

Заголовок: `twist/test/plate.hpp`

[Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

Разделяемый ресурс, обращаться к которому можно только эксклюзивно.

```cpp
mutex.lock();
plate.Access();
mutex.unlock();
```

## `Either`

Заголовок: `twist/test/either.hpp`

Развилка в тестовом сценарии:

```cpp
if (twist::test::Either()) {
  mutex.lock();
  {
    // cs
  }
  mutex.unlock();
} else {
  if (mutex.try_lock()) {
    {
      // cs
    }
    mutex.unlock();
  }
}
```

Версия с числом альтернатив:

```cpp
switch (twist::test::Either(3)) {
  case 0:
    // Alt 1
    break;
  case 1:
    // Alt 2
    break;
  case 2:
    // Alt 3
    break;
};
```

При нагрузочном тестировании с потоками или при использовании рандомизированных планировщиков
в симуляции ветка выбирается случайно.

При переборе исполнений с model checker-ом (`sim::sched::DfsScheduler`) перебираются 
**обе** (**все**) ветки. 

Эквивалент для `either` из [PlusCal](https://lamport.azurewebsites.net/tla/p-manual.pdf).

## `WaitGroup`

Заголовок: `twist/test/wg.hpp`

Группа потоков

```cpp
MutexToTest mutex;
twist::test::Plate plate;

twist::test::WaitGroup wg;

// Добавляем 3 потока, каждый из которых исполняет заданную лямбду
// Вызывать `Add` можно несколько раз
wg.Add(3, [&] {
  mutex.lock();
  plate.Access();
  mutex.unlock();
});

// Запускаем потоки и дожидаемся их завершения
wg.Join();
```

Лямбда, которую будет исполнять поток, может опционально получать индекс (отсчитывая от 0) потока:

```cpp
wg.Add(4, [](size_t index) {
  //
});
```

## Lock-Free

Заголовок: `twist/test/lock_free.hpp`

Подсказки для активации проверки гарантии lock-freedom.

```cpp
LockFreeStack<Unit> stack;

twist::test::WaitGroup wg;
wg.Add(3, [&stack] {
  // Скоуп, в котором рантайм может останавливать потоки
  twist::test::LockFreeScope lf;
  
  stack.Push({});
  // Сообщаем рантайму о прогрессе, т.е. о завершении вызова
  twist::test::Progress();
  
  stack.TryPop();
  twist::test::Progress();
})
```

## `Message`

Заголовок: `twist/test/message.hpp`

`Message<T>` – иммутабельные данные для тестирования синхронизации в сценариях message passing

```cpp
LockFreeStack<twist::test::Message<int>> stack;

// Поток-producer
stack.Push(twist::test::Message<int>::New(1));

// Поток-consumer
if (auto message = stack.TryPop()) {  // Пусть TryPop возвращает std::optional<T>
  message->Read();  // Проверка happens-before (в симуляции Twist или с помощью ThreadSanitizer)
}
```