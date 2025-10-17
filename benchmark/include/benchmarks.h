#ifndef COLLO_LOG_BENCHMARKS_H
#define COLLO_LOG_BENCHMARKS_H

const int MessagesPerThread = 1000000;

long long runLongBenchmark(unsigned int nThreads);
long long runDropRateBenchmark(unsigned int nThreads);

#endif // !COLLO_LOG_BENCHMARKS_H
