## Requirements
The [FastFlow](https://github.com/fastflow/fastflow) header library.

## Compile and run
A Makefile has been provided for both the versions of the project. The test (the same for the two versions) requires some parameters in order to execute correctly (the usage is shown by invoking ```./main``` without parameters).
<br>Mandatory parameters: * *vector size* * scan type (```i``` means *inclusive*, ```e``` means *exclusive*) * *parallelism degree* (0 for sequential execution)
<br>Optional parameters: * *seed* for the generation of the input vector (otherwise a random seed is used) * *test number* (default is test number one: vector of integers and addition operation)
