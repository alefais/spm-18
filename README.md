# spm-exercises2018
Homework and final project of the Parallel and Distributed Systems: Paradigms and Models course of the Computer Science and Networking Master's Degree @ University of Pisa.

| <b>Homework number</b> | <b>Language/Framework/Tool</b> | <b>Description</b> |
| ---------- | ----------------------- | ----------- |
| 1 | C++ | A program that given a stream of `std::vector<double>` elements applies two functions (`f1(x) = x + 1` and `f2(x) = 2 * x`) over all the items of each vector in the stream. The program must use only C++ base mechanisms and libraries. The computation has to be implemented in a sequential way and in a parallel way.
The parallel computation has been implemented as a four stage pipeline on a shared memory multicore: 
* *STAGE1* generates a stream of m vectors of n items each (randomly filled), 
* *STAGE2* increases all the items in each input vector, 
* *STAGE3* doubles all the items in each input vector, 
* *STAGE4* prints the input vectors contents on screen. 
The performance has been measured in terms of scalability, speedup and efficiency. |
| 2 | C++ | A program that computes in parallel a set of independent tasks, initially stored in a shared data structure, and delivers results using a second shared data structure. The program has been implemented using only C++ standard mechanisms and threads. An input task is given by an integer number N and the result to be computed is the number of prime numbers included in range [1, N]. The initial set of tasks is picked up randomly in the range [1, 10K]. <br>Two different implementations of the workers has been provided: <ul><li>SEQUENTIAL: a worker first checks in the collection of the results if its current task has already been computed by another worker: if yes its computation is done, otherwise it will look for the biggest task computed since now that is smaller than its task and computes the number of primes in a sub interval of [1, N] only. <br>
...<b>Example:</b> worker 0 receives task 100 and it has to compute the number of prime numbers in the interval [1, 100]; he finds out that task 100 has not been computed yet and starts looking for the biggest already computed task that is smaller than 100 (assume it is 93); he finds that task 93 has already been computed so it will only count the number of prime numbers in the range [94, 100] and its final result will be the sum of the number of prime numbers in [1, 93] and the number of prime numbers in [94, 100].</li><li>SEQUENTIAL OPTIMIZED: each worker implements the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) to compute the number of prime numbers between 1 and the value of its received task.</li></ul> Some delay can be added in the procedure to obtain a task to be computed in order to observe the impact on scalability. <br>Load balancing of the workers has been implemented as well. |
<!---
| 3 | C++ | A program that implements a parallel [Google MapReduce](https://en.wikipedia.org/wiki/MapReduce) pattern using C++ base mechanisms only. The program takes as input two filenames: one is the file that has to be processed and the other is the file used to store the results. Given the input file, a `mapper` function will process lines of the file and produce a set of `<Tkey, Tvalue>` pairs. Given the set of pairs, a `reducer` function will apply a reduce operation to all the pairs with the same key value. All the computation is contained in a method `void compute()` that writes the results in the output file out of the lines of the input file. |
-->
| 3 | C++, FastFlow | A program that finds the prime numbers in a range [n1, n2]. Four implementations have been provided: <ul><li>SEQUENTIAL: sequential computation of the prime numbers in the interval [n1, n2].</li><li>PARALLEL (MASTER-WORKER v1): implementation of the master-worker parallel schema (farm + feedback) where the task granularity is one number in the range [n1, n2]  (the emitter distributes the tasks to the workers, each of them checks if the received number is prime and if so it sends it back to the emitter).</li><li>PARALLEL (MASTER-WORKER v2): implementation of the master-worker parallel schema (farm + feedback) where the task granularity is a sub-interval of the range [n1, n2]  (the emitter distributes the tasks to the workers, each of them computes sequentially on the received sub-interval and sends back to the emitter the list of prime numbers in its sub-range).</li><li>PARALLEL (PARALLEL FOR): implementation of the parallel for pattern where the initial range [n1, n2] is partitioned in chunks each one computed by a worker.</li></ul> The implementation of the parallel versions is done using [FastFlow](https://github.com/fastflow/fastflow). |

## Final project