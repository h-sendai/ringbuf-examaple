# ring bufferの例題

バッファに整数値をいれていくwriterスレッドと
バッファに入っている整数値を-1に変更するreaderスレッドの実装。

mutex, pthread_cond_wait, pthread_cond_signalを使った実装と、
POSIX semaphoreを使った実装。

- [ringbuf-cond-wait](ringbuf-cond-wait)
- [ringbuf-semaphore](ringbuf-semaphre)
