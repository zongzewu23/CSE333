# Command Collection

### Compile a C program
```
gcc -Wall -g -std=c17 -o ex0 ex0.c
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
```

### Create and push a Git tag
```
git tag hw1-final
git push --tags
```

