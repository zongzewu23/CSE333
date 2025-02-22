# Command Collection

### Compile a C program
```
gcc -Wall -g -std=c17 -o ex0 ex0.c
g++ -Wall -g -std=c++17 -o ex8 ex8.cc
```

### Enter GDB
```
gdb ./test_suite
break HashTableReader.cc:lineNumber
run --gtest_filter=Test_HashTableReader.*
```

### Clone a Git repository
```
git clone git@gitlab.cs.washington.edu:cse333-25wi-students/cse333-25wi-xyzzy.git
```

### Run a program with Valgrind to check for memory leaks
```
valgrind --leak-check=full ./hello_world
```

### Run a custom Python linter
```
../cpplint.py --clint hello_world.c
../cpplint.py ex8.cc
```

### Create and push a Git tag
```
git tag hw1-final
git push --tags
```
### pwd
```
cd /homes/iws/zongzewu/CSE333/HW/cse333-25wi-zongzewu/hw2
```

