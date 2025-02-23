* Ran benchmarks with virtme, don't really have a convenient way of running them bare metal.
* Surprisingly fast and reproducible with virtme
    * Maybe something is broken?

# Baseline

### get

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-get"
Setting up benchmark 'xdp-trait-get'...
Benchmark 'xdp-trait-get' started.
Iter   0 (699.119us): hits   79.305M/s ( 79.305M/prod)
Iter   1 (-109.982us): hits   79.369M/s ( 79.369M/prod)
Iter   2 ( 42.179us): hits   79.357M/s ( 79.357M/prod)
Iter   3 (-76.673us): hits   79.366M/s ( 79.366M/prod)
Iter   4 ( -2.471us): hits   76.800M/s ( 76.800M/prod)
Iter   5 ( 52.849us): hits   77.416M/s ( 77.416M/prod)
Iter   6 (  9.774us): hits   78.299M/s ( 78.299M/prod)
Summary: throughput   78.433 ± 1.121 M ops/s ( 78.433M ops/prod), latency   12.750 ns/op
```

### set

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-set"
Setting up benchmark 'xdp-trait-set'...
Benchmark 'xdp-trait-set' started.
Iter   0 (745.703us): hits   65.791M/s ( 65.791M/prod)
Iter   1 (-186.073us): hits   67.293M/s ( 67.293M/prod)
Iter   2 (-13.797us): hits   69.121M/s ( 69.121M/prod)
Iter   3 (203.146us): hits   69.106M/s ( 69.106M/prod)
Iter   4 (-119.742us): hits   70.578M/s ( 70.578M/prod)
Iter   5 (  8.599us): hits   69.429M/s ( 69.429M/prod)
Iter   6 (-107.321us): hits   69.928M/s ( 69.928M/prod)
Summary: throughput   69.240 ± 1.108 M ops/s ( 69.240M ops/prod), latency   14.443 ns/op
```

### move

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-move"
Setting up benchmark 'xdp-trait-move'...
Benchmark 'xdp-trait-move' started.
Iter   0 (515.650us): hits   79.729M/s ( 79.729M/prod)
Iter   1 (-30.487us): hits   63.592M/s ( 63.592M/prod)
Iter   2 ( 39.185us): hits   63.997M/s ( 63.997M/prod)
Iter   3 (-72.901us): hits   64.005M/s ( 64.005M/prod)
Iter   4 (-71.078us): hits   61.444M/s ( 61.444M/prod)
Iter   5 (129.606us): hits   63.992M/s ( 63.992M/prod)
Iter   6 ( 20.805us): hits   64.499M/s ( 64.499M/prod)
Summary: throughput   63.588 ± 1.091 M ops/s ( 63.588M ops/prod), latency   15.726 ns/op
```

# trait: Replace memcpy calls with inline copies

* Seems to show expected speedup, ~15% everywhere. 
* Jseper's benchmarks showed ~40%, but I think that was on AMD where function calls are particularly expensive with mitigations.

### get

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-get"
Setting up benchmark 'xdp-trait-get'...
Benchmark 'xdp-trait-get' started.
Iter   0 (699.357us): hits   81.863M/s ( 81.863M/prod)
Iter   1 ( 56.250us): hits   87.035M/s ( 87.035M/prod)
Iter   2 ( 39.174us): hits   87.037M/s ( 87.037M/prod)
Iter   3 (-69.097us): hits   87.046M/s ( 87.046M/prod)
Iter   4 (-145.520us): hits   89.613M/s ( 89.613M/prod)
Iter   5 (170.943us): hits   87.025M/s ( 87.025M/prod)
Iter   6 (-153.658us): hits   87.053M/s ( 87.053M/prod)
Summary: throughput   87.467 ± 1.045 M ops/s ( 87.467M ops/prod), latency   11.433 ns/op
```

### set

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-set"
Setting up benchmark 'xdp-trait-set'...
Benchmark 'xdp-trait-set' started.
Iter   0 (863.170us): hits   79.292M/s ( 79.292M/prod)
Iter   1 (-383.369us): hits   81.951M/s ( 81.951M/prod)
Iter   2 (198.998us): hits   79.344M/s ( 79.344M/prod)
Iter   3 (-153.236us): hits   79.042M/s ( 79.042M/prod)
Iter   4 (-41.094us): hits   81.903M/s ( 81.903M/prod)
Iter   5 ( 98.315us): hits   80.942M/s ( 80.942M/prod)
Iter   6 (-105.429us): hits   80.689M/s ( 80.689M/prod)
Summary: throughput   80.640 ± 1.229 M ops/s ( 80.640M ops/prod), latency   12.401 ns/op
```

### move

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-move"
Setting up benchmark 'xdp-trait-move'...
Benchmark 'xdp-trait-move' started.
Iter   0 (1159.709us): hits   66.483M/s ( 66.483M/prod)
Iter   1 (-454.507us): hits   65.670M/s ( 65.670M/prod)
Iter   2 (-11.464us): hits   67.481M/s ( 67.481M/prod)
Iter   3 (-41.552us): hits   69.123M/s ( 69.123M/prod)
Iter   4 ( 78.502us): hits   66.555M/s ( 66.555M/prod)
Iter   5 (-71.909us): hits   66.565M/s ( 66.565M/prod)
Iter   6 ( 50.010us): hits   67.937M/s ( 67.937M/prod)
Summary: throughput   67.217 ± 1.230 M ops/s ( 67.217M ops/prod), latency   14.877 ns/op
```

# trait: Replace memmove calls with inline move

* My inline move is slower than memmove, at least on Intel... work to be done!
* Maybe it's better on AMD where function calls are more expensive with mitigations?

### get

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-get"
Setting up benchmark 'xdp-trait-get'...
Benchmark 'xdp-trait-get' started.
Iter   0 (818.638us): hits   84.411M/s ( 84.411M/prod)
Iter   1 ( -9.979us): hits   85.871M/s ( 85.871M/prod)
Iter   2 ( 93.613us): hits   88.202M/s ( 88.202M/prod)
Iter   3 (-21.239us): hits   87.022M/s ( 87.022M/prod)
Iter   4 (-79.891us): hits   86.667M/s ( 86.667M/prod)
Iter   5 (122.020us): hits   84.870M/s ( 84.870M/prod)
Iter   6 (-222.718us): hits   84.499M/s ( 84.499M/prod)
Summary: throughput   86.187 ± 1.395 M ops/s ( 86.187M ops/prod), latency   11.603 ns/op
```

### set

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-set"
Setting up benchmark 'xdp-trait-set'...
Benchmark 'xdp-trait-set' started.
Iter   0 (645.403us): hits   76.750M/s ( 76.750M/prod)
Iter   1 (-89.919us): hits   79.217M/s ( 79.217M/prod)
Iter   2 ( 75.706us): hits   79.504M/s ( 79.504M/prod)
Iter   3 (-76.505us): hits   79.366M/s ( 79.366M/prod)
Iter   4 (101.672us): hits   76.792M/s ( 76.792M/prod)
Iter   5 (-99.670us): hits   79.368M/s ( 79.368M/prod)
Iter   6 (  7.956us): hits   79.589M/s ( 79.589M/prod)
Summary: throughput   78.972 ± 1.072 M ops/s ( 78.972M ops/prod), latency   12.663 ns/op
```

### move

```
afabre@bobby ~/D/w/linux> vng -r arch/x86/boot/bzImage --memory 2G --exec "cd tools/testing/selftests/bpf; ./bench xdp-trait-move"
Setting up benchmark 'xdp-trait-move'...
Benchmark 'xdp-trait-move' started.
Iter   0 (873.717us): hits   63.445M/s ( 63.445M/prod)
Iter   1 (182.138us): hits   64.488M/s ( 64.488M/prod)
Iter   2 (-175.519us): hits   64.011M/s ( 64.011M/prod)
Iter   3 (-165.466us): hits   64.011M/s ( 64.011M/prod)
Iter   4 (170.223us): hits   63.989M/s ( 63.989M/prod)
Iter   5 ( 37.030us): hits   63.998M/s ( 63.998M/prod)
Iter   6 (-118.297us): hits   65.388M/s ( 65.388M/prod)
Summary: throughput   64.313 ± 0.560 M ops/s ( 64.313M ops/prod), latency   15.549 ns/op
```
