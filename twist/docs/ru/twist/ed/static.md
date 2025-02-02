# Twist-_ed_

## `static`

Примеры: [examples/static/main.cpp](/examples/static/main.cpp)

### Переменные

Заголовочный файл: `twist/ed/static/var.hpp`

Локальные и глобальные статические переменные должны быть «завернуты» в
макрос `TWISTED_STATIC(T, name)`:

```cpp
void Foo() {
  // Локальная статическая переменная
  // ~ static Widget w;
  TWISTED_STATIC(Widget, w);
  
  // Обращение к объекту – через операторы * и ->
  (*w).Foo();
  w->Bar();
}
```

Статические константы примитивных типов аннотировать не нужно:

```cpp
static const size_t kBufferSize = 128;
```

### Поля

```cpp
// .hpp

class Gadget {
 public:
  void Bar() {
    w->Baz();
  }
  
 private:
  // ~ static Widget w;
  TWISTED_STATIC_MEMBER_DECLARE(Widget, w);
};
```

```cpp
// .cpp

// ~ Widget Gadget::w;
TWISTED_STATIC_MEMBER_DEFINE(Gadget, Widget, w);
```

## `static thread_local`

Примеры: [examples/thread_local/main.cpp](/examples/thread_local/main.cpp)

#### Произвольное значение

Заголовочный файл: `twist/ed/static/thread_local/var.hpp`

Вместо `static thread_local T name` нужно использовать макрос `TWISTED_STATIC_THREAD_LOCAL(T, name)`:

```cpp
// ~ static thread_local Widget w;
TWISTED_STATIC_THREAD_LOCAL(Widget, w);

// Вызов метода Foo – через operator-> (как при работе с std::optional)
w->Foo();

// Получение ссылки на объект – через operator* (как при работе с std::optional)
(*w).Foo();

// Взятие адреса – operator&
Widget* w_ptr = &w;
w_ptr->Foo();
```

#### Указатель

Заголовочный файл: `twist/ed/static/thread_local/ptr.hpp`

Вместо `static thread_local T* name` нужно использовать макрос `TWISTED_STATIC_THREAD_LOCAL_PTR(T, name)`.

```cpp
// ~ static thread_local Widget* w = nullptr;
TWISTED_STATIC_THREAD_LOCAL_PTR(Widget, w);

w = &widget;

// Повторяет интерфейс указателя
w->Bar();
```

##### Инициализация

```cpp
// ~ static thead_local Gadget* g = &gadget;
TWISTED_STATIC_THREAD_LOCAL_PTR_INIT(Gadget, g, &gadget);
```

#### Поля

Заголовочный файл: `twist/ed/static/thread_local/member.hpp`

- `TWISTED_STATIC_THREAD_LOCAL_MEMBER_DECLARE(T, name)` – объявление статического thread-local члена класса
- `TWISTED_STATIC_THREAD_LOCAL_MEMBER_DEFINE(K, T, name)` – определение, где `K` – имя класса со статическим полем, `T` – тип статического поля
