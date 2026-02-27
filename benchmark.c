/*
 * Benchmark for the fido protocol.
 *
 * Sends batches of 'set' queries then 'read' queries and measures
 * throughput. Each batch is 240 queries x 5 bytes = 1200 bytes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define QUERIES_PER_BATCH 240
#define QUERY_SIZE        5
#define RESPONSE_SIZE     (QUERIES_PER_BATCH * QUERY_SIZE)
#define BATCHES           1250
#define WARMUP_BATCHES    50
#define NUM_RUNS          5
#define HOST              "0.0.0.0"
#define PORT              8030

struct message {
  unsigned int offset;
  char val;
};

static double now(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static int recv_exact(int sock, void *buf, int nbytes)
{
  int total = 0;
  int n;

  while (total < nbytes) {
    n = recv(sock, (char *)buf + total, nbytes - total, 0);
    if (n <= 0)
      return -1;
    total += n;
  }
  return total;
}

static int cmp_double(const void *a, const void *b)
{
  double da = *(const double *)a;
  double db = *(const double *)b;
  if (da < db) return -1;
  if (da > db) return 1;
  return 0;
}

static double median(double *arr, int n)
{
  double *tmp;
  double result;

  tmp = malloc(n * sizeof(double));
  memcpy(tmp, arr, n * sizeof(double));
  qsort(tmp, n, sizeof(double), cmp_double);

  if (n % 2 == 0)
    result = (tmp[n / 2 - 1] + tmp[n / 2]) / 2.0;
  else
    result = tmp[n / 2];

  free(tmp);
  return result;
}

static double mean(double *arr, int n)
{
  double sum = 0.0;
  int i;

  for (i = 0; i < n; i++)
    sum += arr[i];
  return sum / n;
}

static double min_val(double *arr, int n)
{
  double m = arr[0];
  int i;

  for (i = 1; i < n; i++)
    if (arr[i] < m) m = arr[i];
  return m;
}

static double max_val(double *arr, int n)
{
  double m = arr[0];
  int i;

  for (i = 1; i < n; i++)
    if (arr[i] > m) m = arr[i];
  return m;
}

static int run_benchmark(char cmd, int *queries_out, double *elapsed_out,
                         double *batch_times)
{
  int sock;
  struct sockaddr_in addr;
  char send_buf[RESPONSE_SIZE];
  char recv_buf[RESPONSE_SIZE];
  struct message *msg;
  int a, b, queries;
  double t_start, t_batch;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  inet_pton(AF_INET, HOST, &addr.sin_addr);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
    close(sock);
    return -1;
  }

  /* Warmup -- discard these timings */
  for (a = 1; a <= WARMUP_BATCHES; a++) {
    for (b = 1; b <= QUERIES_PER_BATCH; b++) {
      msg = (struct message *)(send_buf + (b - 1) * QUERY_SIZE);
      msg->offset = a * b;
      msg->val = cmd;
    }
    send(sock, send_buf, RESPONSE_SIZE, 0);
    if (recv_exact(sock, recv_buf, RESPONSE_SIZE) < 0) {
      fprintf(stderr, "connection lost during warmup\n");
      close(sock);
      return -1;
    }
  }

  /* Timed run */
  queries = 0;
  t_start = now();

  for (a = 1; a <= BATCHES; a++) {
    for (b = 1; b <= QUERIES_PER_BATCH; b++) {
      msg = (struct message *)(send_buf + (b - 1) * QUERY_SIZE);
      msg->offset = a * b;
      msg->val = cmd;
    }

    t_batch = now();
    send(sock, send_buf, RESPONSE_SIZE, 0);
    if (recv_exact(sock, recv_buf, RESPONSE_SIZE) < 0) {
      fprintf(stderr, "connection lost during benchmark\n");
      close(sock);
      return -1;
    }
    batch_times[a - 1] = now() - t_batch;
    queries += QUERIES_PER_BATCH;
  }

  *elapsed_out = now() - t_start;
  *queries_out = queries;

  close(sock);
  return 0;
}

static void run_suite(const char *label, char cmd)
{
  double run_results[NUM_RUNS];
  double batch_times[BATCHES];
  double elapsed, qps;
  int queries, run;

  printf("=== %s benchmark (cmd='%c') ===\n\n", label, cmd);

  for (run = 1; run <= NUM_RUNS; run++) {
    if (run_benchmark(cmd, &queries, &elapsed, batch_times) < 0) {
      fprintf(stderr, "%s benchmark failed on run %d\n", label, run);
      return;
    }

    qps = queries / elapsed;
    run_results[run - 1] = qps;

    printf("Run %d: %d queries in %.3fs (%.0f queries/sec)\n",
           run, queries, elapsed, qps);
    printf("  batch latency -- min: %.4fs  max: %.4fs  avg: %.4fs  median: %.4fs\n",
           min_val(batch_times, BATCHES),
           max_val(batch_times, BATCHES),
           mean(batch_times, BATCHES),
           median(batch_times, BATCHES));
  }

  printf("\n%s summary over %d runs:\n", label, NUM_RUNS);
  printf("  median throughput: %.0f queries/sec\n",
         median(run_results, NUM_RUNS));
  printf("  min: %.0f  max: %.0f\n\n",
         min_val(run_results, NUM_RUNS),
         max_val(run_results, NUM_RUNS));
}

int main(void)
{
  run_suite("Set", 's');
  run_suite("Get", 'r');
  return 0;
}
