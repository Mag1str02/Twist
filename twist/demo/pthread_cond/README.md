# `pthread_cond`

## Run

```shell
git clone https://gitlab.com/Lipovsky/twist.git
cd twist
mkdir -p build/sim-release && cd build/sim-release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17 -DTWIST_FAULTY=ON -DTWIST_SIM=ON -DTWIST_SIM_ISOLATION=ON -DSURE_EXCEPTIONS=OFF -DTWIST_EXAMPLES=ON ../..
make twist_demo_pthread_cond
./demo/pthread_cond/twist_demo_pthread_cond
```

## References

- [`pthread_cond_wait.c` source code](https://sourceware.org/git/?p=glibc.git;a=blob;f=nptl/pthread_cond_wait.c;h=806c432d13497c912fbacec0b2591fe92be42f07;hb=HEAD)
- [Bug 25847 - pthread_cond_signal failed to wake up pthread_cond_wait due to a bug in undoing stealing](https://sourceware.org/bugzilla/show_bug.cgi?id=25847)
- [Using TLA+ in the Real World to Understand a Glibc Bug](https://probablydance.com/2020/10/31/using-tla-in-the-real-world-to-understand-a-glibc-bug/) by Malte Skarupke
  - [TLA+ spec](https://github.com/skarupke/glibc_cv_tla_plus/tree/main)
