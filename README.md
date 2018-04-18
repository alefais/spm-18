# spm-exercises2018
Homework of the Parallel and Distributed Systems: Paradigms and Models course of the Computer Science and Networking Master's Degree @ University of Pisa

| <b>Homework number</b> | <b>Language/Framework/Tool</b> | <b>Description</b> |
| ---------- | ----------------------- | ----------- |
| 1 | C++ | A program that given a stream of std::vector<double> elements applies two functions (`f1(x) = x + 1` and `f2(x) = 2 * x`) over all the items of each vector in the stream. The program must use only C++ base mechanisms and libraries. The computation has to be implemented in a sequential way and in a parallel way. The parallel computation has been implemented as a four stage pipeline on a shared memory multicore: STAGE1 generates a stream of m vectors of n items each (randomly filled), STAGE2 increases all the items in each input vector, STAGE3 doubles all the items in each input vector, STAGE4 prints the input vectors contents on screen. The performance has been measured in terms of scalability, speedup and efficiency. |
