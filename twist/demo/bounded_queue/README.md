# Bounded Queue

## Run

```shell
git clone https://gitlab.com/Lipovsky/twist.git
cd twist
mkdir -p build/sim && cd build/sim
cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 -DTWIST_FAULTY=ON -DTWIST_SIM=ON -DTWIST_SIM_ISOLATION=ON -DTWIST_EXAMPLES=ON ../..
make twist_demo_bounded_queue
./demo/bounded_queue/twist_demo_bounded_queue
```
