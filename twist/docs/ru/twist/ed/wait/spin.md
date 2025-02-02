# Twist-_ed_

## `SpinWait`

Заголовочный файл: `<twist/ed/wait/spin.hpp>`

Предназначен для _адаптивного_ _активного ожидания_ (_busy waiting_) в спинлоках и мьютексах.

### Пример

[examples/spin/main.cpp](/examples/spin/main.cpp)

```cpp
void SpinLock::Lock() {
  // Одноразовый!
  // Для каждого нового цикла ожидания в каждом потоке 
  // нужно заводить собственный экземпляр SpinWait
  twist::ed::SpinWait spin_wait;
  while (locked_.exchange(true)) {
    // У SpinWait определен operator()
    spin_wait();  // Exponential backoff
  }
}
```

### `ConsiderParking`

`SpinWait` может подсказать потоку, что активное ожидание затянулось и стоит рассмотреть парковку:

```cpp
void Mutex::Lock() {
  twist::ed::SpinWait spin_wait;
  
  while (!TryLock()) {
    if (spin_wait.ConsiderParking()) {
      ParkWaiter();
    } else {
      spin_wait();  // Активное ожидание
    }
  }
}
```

Цикл из этого сниппета можно написать зеркально, используя метод `KeepSpinning`:

```cpp
bool SpinWait::KeepSpinning() const noexcept {
  return !ConsiderParking();
}
```
