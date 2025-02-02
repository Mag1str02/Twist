## Mutex Lock Order

> _Two roads diverged in a yellow wood,_  
> _And sorry I could not travel both_  
>_And be one traveler_
>
> [The Road Not Taken](https://www.poetryfoundation.org/poems/44272/the-road-not-taken) by Robert Frost

## Run

```shell
git clone https://gitlab.com/Lipovsky/twist.git
cd twist
mkdir -p build/sim && cd build/sim
cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 -DTWIST_FAULTY=ON -DTWIST_SIM=ON -DTWIST_SIM_ISOLATION=ON -DTWIST_EXAMPLES=ON ../..
make twist_demo_mutex_lock_order
./demo/mutex_lock_order/twist_demo_mutex_lock_order
```