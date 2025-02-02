# Tracing

[OpenTelemetry / Observability Primer](https://opentelemetry.io/docs/concepts/observability-primer/)

## Модель

Сущности:

- _Scope_
- _Domain_
- _Context_
- _Event_
- _Attribute_

_Scope_ и _Domain_ моделируют ортогональные измерения:
- операция / компонент или
- (в тексте программы) метод (точнее, лексический скоуп) / класс  
- (в исполнении программы) фрейм в стеке вызовов / объект в памяти или
- (в физическом смысле) время / пространство
  
У каждого _scope_ (за исключением корневого) есть родительский _scope_.

Каждый _scope_ связан с некоторым _domain_.

Все _scope_ одного потока образуют систему вложенных отрезков.

C каждым _scope_ связаны метаданные – _context_ – которые автоматически наследуются от родительского _scope_.

У каждого _domain_ и _scope_ есть _имя_.

_Event_ находится на "пересечении" _scope_ и _domain_.

Сущности _scope_, _domain_ и _event_ могут быть снабжены набором _атрибутов_.

Атрибут имеет _имя_ и _значение_ (`uint`, `bool`, указатель, строка, unit)

## Классы

### `Domain`

Заголовок: `twist/trace/domain.hpp`

Должен быть полем класса-компонента.

В конструкторе нужно явно задать имя. Имя – строковый литерал, сам `Domain` не аллоцирует динамическую память.

### `Scope`

Заголовок: `twist/trace/scope.hpp`

Должен быть размещен на стеке и привязан к лексическому скоупу.

Автоматически связывается с родительским _scope_.

В конструкторе можно задать _domain_. Если он не задан, то наследуется 
_domain_ родительского _scope_.

В конструкторе можно задать имя. Если имя не задано явно, то используется 
имя текущей функции / метода класса.

Имя – строковый литерал, сам `Scope` не аллоцирует динамическую память.

### Атрибуты

Заголовки: `twist/trace/attr/`

- `Uint` – беззнаковое целое
- `Bool` – флаг
- `Ptr` – указатель
- `Str` – строковый литерал
- `Unit` – метка  
- `Fmt` – форматированная строка

## Логирование

### `Scope`

```cpp
class TicketLock {
 public:  
  void Lock() {
    // Связываем scope с domain, имя scope - имя текущего метода - Lock
    twist::trace::Scope lock{ticket_lock_};
  
    Ticket ticket = next_free_ticket_.fetch_add(1);
    lock.Note("My ticket = {}", ticket);
  
    {
      // Вложенный scope, domain наследуется от родителя - scope `lock`
      twist::trace::Scope wait{"WaitLoop"};
      // Добавляем к scope `wait` атрибут – ticket ждущего потока
      twist::trace::attr::Uint ticket_attr{"ticket", ticket};
      wait.LinkAttr(ticket_attr);

      while (ticket != owner_ticket_.load()) {
        wait.Here();  // ~ wait.Note("Here");
      }
    }
  }
  
  // ...
 
 private:
  // ...
  
  twist::trace::Domain ticket_lock_{"TicketLock"};
}  
```

### `Note`

Заголовок: `twist/trace/note.hpp`

```cpp
// Находим текущий Scope и добавляем в него событие
twist::trace::Note("{}, {}", "Hello", "world");
```

### Visibility

С каждым _scope_ и _domain_ связан целочисленный параметр _visibility_, который влияет на видимость связанных _event_.

У _scope_ параметр _visibility_ принадлежит _context_ и наследуется от родительского _scope_.

_Visibility_ для _event_ определяется как _visibility_(_scope_) + _visibility_(_domain_).

_Event_ считается наблюдаемым, если его _visibility_ > 0.

## Примеры

- [demo/treiber_stack](/demo/treiber_stack/treiber_stack.hpp)
- [example/trace](/examples/trace/main.cpp)


## Use Cases

### Примитивы синхронизации

Некоторые примитивы синхронизации допускают корректные сценарии применения,
в которых примитив разрушается до завершения операции над ним (т.е. время жизни примитива меньше, чем
время жизни фрейма вызова операции).

В таких случаях стоит делать `Domain` статическим.

### Stackful Fibers

Чтобы трассировка корректно работала с файберами, пользователю **необходимо** аннотировать переключения контекста.

См. [Assist](/docs/ru/twist/assist.md).

### Stackless Coroutines

`Scope` **нельзя создавать** в [stackless coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
