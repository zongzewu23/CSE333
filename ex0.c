// name: Zongze Wu
// UW email: zongzewu@uw.edu

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

double calculatePi(int num);

int main(int argc, char **argv)
{
    int num;
    char *pEnd;
    long arg2;

    // Check if there is exactly two arguments
    if (argc != 2)
    {
        fprintf(stderr, "Error: There needs to be exactly two arguments.\n");
        return 1;
    }

    // Convert the argument from string to long using strol
    arg2 = strtol(*(argv + 1), &pEnd, 10);

    // If there are any non-numeric characters left after conversion then print error message
    if (*pEnd != '\0')
    {
        fprintf(stderr, "Error: The input contains non-numeric characters.\n");
        return 2;
    }

    // Check if the number is within the range of int and is non-negative
    if (arg2 < 0 || arg2 > INT_MAX)
    {
        fprintf(stderr, "Error: The input is an Integer that is either too big or too small, make sure it's a positive int.\n");
        return 3;
    }

    num = (int)arg2;

    // Print the result, keeping 20 decimal places
    printf("Our estimate of Pi is %.20lf\n", calculatePi(num));

    return 0;
}

/*
 *   Uses a series to approximate the value of PI.
 */
double calculatePi(int num)
{
    double result = 3.0; // Initial approximation value
    double n = 1.0;      // Used to calculate the denominator in each step
    int count = 1;       // Loop counter

    while (count <= num)
    {
        // If count is even, subtract; otherwise, add. this is an simulation of (-1)^(n+1)
        if (count % 2 == 0)
        {
            result -= (4.0 / (2.0 * n * (2.0 * n + 1) * (2.0 * n + 2)));
        }
        else
        {
            result += (4.0 / (2.0 * n * (2.0 * n + 1) * (2.0 * n + 2)));
        }

        count++;
        n++;
    }

    return result;
}
