// gcc -o thread_pool thread_pool.c -lpthread && ./thread_pool

// 线程池管理了一个 Worker 队列和一个 Job 队列
// 一个 Worker 工作在一个线程中, 死循环运行, 当 job 队列不为空, 就从其中取
// 一个 job 执行. 如果为空则条件等待, 直到有新的 job 加入或者被要求停止运行
// 线程池大小: cpu核心数*2 + 磁盘数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define LIST_ADD(list, item) \
    item->prev = NULL;       \
    item->next = list;       \
    list = item;

#define LIST_REMOVE(list, item)        \
    if (item->prev)                    \
        item->prev->next = item->next; \
    if (item->next)                    \
        item->next->prev = item->prev; \
    if (item == list)                  \
        list = item->next;             \
    item->prev = item->next = NULL;

typedef struct Job
{
    void (*func)(struct Job *job); // 待执行的函数
    void *data_ptr;                // 执行所需的参数
    struct Worker *worker;         // 所属的 worker
    struct Job *prev;
    struct Job *next;
} job_t;

typedef struct Worker
{
    pthread_t thread;      // 线程
    char *worker_name;     // worker 的名字
    int running;           // 有任务正在运行嘛
    int terminate;         // 是否终止运行
    struct ThreadPool *tp; // 所属的线程池指针
    struct Worker *prev;
    struct Worker *next;
} worker_t;

typedef struct ThreadPool
{
    const char *name;            // 线程池名字
    struct Worker *workers;      // worker 队列
    struct Job *jobs;            // 正在等待中的任务队列
    int waiting_jobs_num;        // 排队等待的 job 数量
    int total_workers_num;       // worker 总数量
    int running_workers_num;     // 运行中的 worker 数量
    int terminate_workers_num;   // 即将停止的 worker 数量
    pthread_mutex_t jobs_mtx;    // 操作 jobs 队列时使用
    pthread_mutex_t workers_mtx; // 操作 workers 队列时使用
    pthread_cond_t jobs_cond;    // worker 条件等待时使用
} thread_pool_t;

/**
 * @brief worker 循环
 * 
 * @param ptr worker 指针
 * @return void* 
 */
void *thread_pool_cycle(void *ptr)
{
    worker_t *worker = (worker_t *)ptr;
    while (worker->terminate == 0)
    {
        // 如果没有 job 可以执行, 条件等待直到被唤醒
        pthread_mutex_lock(&worker->tp->jobs_mtx);
        if (worker->tp->waiting_jobs_num == 0)
        {
            printf("[%s] waiting for job...\n", worker->worker_name);
            pthread_cond_wait(&worker->tp->jobs_cond, &worker->tp->jobs_mtx);
            printf("[%s] wake up! terminate=%d, running=%d\n", worker->worker_name, worker->terminate, worker->running);
        }

        job_t *job = worker->tp->jobs; // 队头的 job
        if (job == NULL)
        {
            pthread_mutex_unlock(&worker->tp->jobs_mtx);
            continue;
        }

        // 拿走一个 job
        LIST_REMOVE(worker->tp->jobs, job);
        job->worker = worker; // 标注该 job 所属的 worker
        worker->tp->waiting_jobs_num--;
        pthread_mutex_unlock(&worker->tp->jobs_mtx);

        worker->running = 1; // 标注 worker 正在运行

        pthread_mutex_lock(&worker->tp->workers_mtx);
        worker->tp->running_workers_num++;
        pthread_mutex_unlock(&worker->tp->workers_mtx);

        job->func(job); // 执行 job 的任务

        // 正常执行完成
        worker->running = 0;
        pthread_mutex_lock(&worker->tp->workers_mtx);
        worker->tp->running_workers_num--;
        pthread_mutex_unlock(&worker->tp->workers_mtx);
    }

    // worker 退出循环, 清理内存, 退出线程
    printf("[%s] CLOSE!!!\n", worker->worker_name);

    pthread_mutex_lock(&worker->tp->workers_mtx);
    LIST_REMOVE(worker->tp->workers, worker);
    worker->tp->total_workers_num--;
    worker->tp->terminate_workers_num--;
    pthread_mutex_unlock(&worker->tp->workers_mtx);

    free(worker->worker_name);
    free(worker);
    pthread_exit(NULL);
}

/**
 * @brief 给线程池添加 worker
 * 
 * @param tp 线程池指针
 * @param worker_num 新增 worker 数量
 * @return int 正常返回 0, 错误返回负数
 */
int thread_pool_add_worker(thread_pool_t *tp, int worker_num)
{
    if (worker_num < 0)
        worker_num = 0;

    for (int i = 0; i < worker_num; i++)
    {
        worker_t *worker = (worker_t *)malloc(sizeof(worker_t));
        if (worker == NULL)
            return -1;

        memset(worker, 0, sizeof(worker_t));
        worker->tp = tp;

        // 给 worker 设置个名字, 接着初始化的序号来
        int name_len = strlen(tp->name) + 6;
        worker->worker_name = (char *)malloc(name_len);
        snprintf(worker->worker_name, name_len, "%s-%d", tp->name, tp->total_workers_num);

        // 给 worker 分配一个线程
        int ret = pthread_create(&worker->thread, NULL, thread_pool_cycle, (void *)worker);
        if (ret != 0)
        {
            free(worker);
            return -2;
        }

        pthread_mutex_lock(&tp->workers_mtx);
        printf("[%s] add worker %s\n", tp->name, worker->worker_name);
        LIST_ADD(tp->workers, worker); // 添加 worker
        tp->total_workers_num++;
        pthread_mutex_unlock(&tp->workers_mtx);
    }
    return 0;
}

/**
 * @brief 创建线程池
 * 
 * @param worker_num 初始 worker 数量
 * @param name 线程池名字
 * @return int 正常返回 0, 错误返回负数
 */
int thread_pool_create(thread_pool_t *tp, const char *name, int worker_num)
{
    if (worker_num <= 0)
        worker_num = 1;

    memset(tp, 0, sizeof(thread_pool_t));
    tp->name = name;

    // 初始化条件变量
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    memcpy(&tp->jobs_cond, &cond, sizeof(pthread_cond_t));

    // 初始化 mutex
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&tp->jobs_mtx, &mtx, sizeof(pthread_mutex_t));
    memcpy(&tp->workers_mtx, &mtx, sizeof(pthread_mutex_t));

    return thread_pool_add_worker(tp, worker_num);
}

/**
 * @brief 移除指定数量的 worker, 优先移除空闲的 worker
 * 至少会保留一个 worker 工作
 * 
 * @param tp 线程池指针
 * @param worker_num 要移除的 worker 数量
 */
void thread_pool_remove_workers(thread_pool_t *tp, int worker_num)
{
    pthread_mutex_lock(&tp->workers_mtx);
    // 至少留一个 worker
    int can_terminate = tp->total_workers_num - tp->terminate_workers_num;
    if (worker_num >= can_terminate)
        worker_num = can_terminate - 1;

    printf("%d workers should be removed, %p, next: %p\n", worker_num, tp->workers, tp->workers->next);

    // 优先终止没有运行的 worker
    for (worker_t *worker = tp->workers; worker != NULL; worker = worker->next)
    {
        if (worker_num == 0)
            break;

        // 没被标注为终止且没在运行状态
        if (worker->terminate == 0 && worker->running == 0)
        {
            printf("choose idle worker %s to be removed\n", worker->worker_name);
            worker_num--;
            // worker 被唤醒后发现 terminate == 1 就会退出
            worker->terminate = 1;
            worker->tp->terminate_workers_num++;
        }
    }

    // 如果需要终止更多 worker
    for (worker_t *worker = tp->workers; worker != NULL; worker = worker->next)
    {
        if (worker_num == 0)
            break;

        // 找一些正在工作但未被要求停止的 worker 下手
        if (worker->terminate == 0 && worker->running == 1)
        {
            printf("choose running worker %s to be removed\n", worker->worker_name);
            worker_num--;
            worker->terminate = 1;
            worker->tp->terminate_workers_num++;
        }
    }

    // 唤醒所有还在等待 job 的 worker
    pthread_cond_broadcast(&tp->jobs_cond);
    pthread_mutex_unlock(&tp->workers_mtx);
}

/**
 * @brief 销毁线程池
 * 
 * @param tp 线程池指针
 */
void thread_pool_destroy(thread_pool_t *tp)
{
    // 通知所有 worker 退出
    pthread_mutex_lock(&tp->jobs_mtx);
    for (worker_t *worker = tp->workers; worker != NULL; worker = worker->next)
    {
        worker->terminate = 1;
    }
    // tp->workers = NULL;
    // tp->jobs = NULL;
    pthread_cond_broadcast(&tp->jobs_cond);
    pthread_mutex_unlock(&tp->jobs_mtx);
}

/**
 * @brief 添加任务到线程池
 * 
 * @param tp 线程池指针
 * @param job 任务指针
 */
void thread_pool_add_job(thread_pool_t *tp, job_t *job)
{
    pthread_mutex_lock(&tp->jobs_mtx);
    tp->waiting_jobs_num++;
    LIST_ADD(tp->jobs, job);             // 添加一个 job
    pthread_cond_signal(&tp->jobs_cond); // 唤醒一个正在等待的 worker
    pthread_mutex_unlock(&tp->jobs_mtx);
}

// ----------------------------------------------------------------

void something_todo(job_t *job)
{
    int job_index = *(int *)job->data_ptr;
    int ms = rand() % 10000 + 3000; // 模拟计算耗时 ms
    printf("[%s] processing job %d, time require: %dms\n", job->worker->worker_name, job_index, ms);
    usleep(ms * 1000);
    free(job->data_ptr);
    free(job);
}

int main(int argc, char const *argv[])
{
    printf("## Thread Pool Test\n\n"
           "* 'a' to add a worker\n"
           "* 'r' to remove a worker\n"
           "* 'j' to add a job\n"
           "* 'q' to close thread pool\n"
           "* 'x..x' to do action 'x' n times\n"
           "* 'Enter' to show status\n\n");

    thread_pool_t pool;
    thread_pool_create(&pool, "MyThreadPool", 1);
    int job_index = 0;
    job_t *job;

    for (;;)
    {
        if (pool.total_workers_num == 0)
        {
            printf("all worker exit\n");
            exit(0);
        }

        switch (getchar())
        {
        case 'a':
            thread_pool_add_worker(&pool, 1);
            break;
        case 'r':
            thread_pool_remove_workers(&pool, 1);
            break;
        case 'j':
            job = (job_t *)malloc(sizeof(job_t));
            job->func = something_todo;
            job->data_ptr = malloc(sizeof(int));
            *(int *)job->data_ptr = job_index;
            job_index++;
            thread_pool_add_job(&pool, job);
            break;
        case 'q':
            thread_pool_destroy(&pool);
            break;
        default:
            printf("# total workers: %d, running workers: %d, to be ternimate worker: %d, waiting jobs: %d\n",
                   pool.total_workers_num, pool.running_workers_num, pool.terminate_workers_num, pool.waiting_jobs_num);
        }
    }
}
