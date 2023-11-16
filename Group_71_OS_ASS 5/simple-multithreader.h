#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>

int size;
int user_main(int argc, char **argv);
void demonstration(std::function<void()> &&lambda)
{
  lambda();
}
//------------------------------------------------------VECTOR.CPP----------------------------------------------------------------------//
typedef struct
{
  int low;
  int high;
  std::function<void(int)> lambda;
} thread_args1;

void vector_sum(int low, int high, std::function<void(int)> lambda)
{
  for (int i = low; i < high; ++i)
  {
    lambda(i);
  }
}
void *func1(void *ptr)
{
  thread_args1 *t = (thread_args1 *)ptr;
  vector_sum(t->low, t->high, t->lambda);
  return NULL;
}

static void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads)
{
  clock_t start = clock();
  pthread_t tid[numThreads];
  thread_args1 args[numThreads];
  int chunk = size / numThreads;
  for (int i = 0; i < numThreads; i++)
  {
    args[i].low = i * chunk;
    args[i].high = (i + 1) * chunk;
    args[i].lambda = lambda;
    pthread_create(&tid[i], nullptr, func1, (void *)&args[i]);
  }
  for (int i = 0; i < numThreads; i++)
  {
    pthread_join(tid[i], nullptr);
  }
  clock_t end = clock();
  float time = ((float)end - start) / CLOCKS_PER_SEC;
  std::cout << "Execution time is " << time << "s" << std::endl;
}
//------------------------------------------------------MATRIX.CPP-----------------------------------------------------------------------//
typedef struct
{
  int low1;
  int low2;
  int high1;
  int high2;
  std::function<void(int, int)> lambda;
} thread_args2;

void matrix_multi(int low1, int high1, int low2, int high2, std::function<void(int, int)> lambda)
{
  for (int i = low1; i < high1; i++)
  {
    for (int j = low2; j < high2; j++)
    {
      lambda(i, j);
    }
  }
}

void *func2(void *ptr)
{
  thread_args2 *t = (thread_args2 *)ptr;
  matrix_multi(t->low1, t->high1, t->low2, t->high2, t->lambda);
  return NULL;
}

static void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads)
{
  clock_t start = clock();
  pthread_t tid[numThreads];
  thread_args2 args[numThreads];
  int chunk = (size) / numThreads;

  for (int i = 0; i < numThreads; i++)
  {
    args[i].low1 = i * chunk;
    args[i].high1 = (i + 1) * chunk;
    args[i].low2 = low2;
    args[i].high2 = high2;
    args[i].lambda = lambda;
    pthread_create(&tid[i],nullptr, func2,(void *)&args[i]);
  }

  for (int i = 0; i < numThreads; i++)
  {
    pthread_join(tid[i], nullptr);
  }
  clock_t end = clock();
  float time = ((float)end - start) / CLOCKS_PER_SEC;
  std::cout << "Execution time is " << time << "s" << std::endl;
}
//---------------------------------------------------------END--------------------------------------------------------------------------//

int main(int argc, char **argv)
{

  std::string exe = argv[0];

  if (exe.find("vector") != std::string::npos)
  {
    size = (argc > 2 ? atoi(argv[2]) : 48000000);
  }
  else if (exe.find("matrix") != std::string::npos)
  {
    size = (argc > 2 ? atoi(argv[2]) : 1024);
  }
  // size = argc > 2 ? atoi(argv[2]) : 48000000;
  // size2 = argc > 2 ? atoi(argv[2]) : 1024;
  /*
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be
   * explicity captured if they are used inside lambda.
   */
  int x = 5, y = 1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/ [/*by value*/ x, /*by reference*/ &y](void)
  {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);

  auto /*name*/ lambda2 = [/*nothing captured*/]()
  {
    std::cout << "====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main
