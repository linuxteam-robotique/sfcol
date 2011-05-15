#include <unistd.h>
#include <sys/time.h>

double	ms_time(void)
{
  static struct timeval tod;
  gettimeofday(&tod, NULL);
  return ((double) tod.tv_sec * 1000.0 + (double) tod.tv_usec / 1000.0);
}
