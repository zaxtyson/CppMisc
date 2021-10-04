#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MP_ALIGNMENT 32
#define MP_PAGE_SIZE 4096

typedef struct mp_large_s
{
    struct mp_large_s *next;
    void *alloc;
} mp_large_t;

typedef struct mp_node_s
{
    char *last;             // 指向当前节点可用内存的起始位置
    char *end;              // 指向当前节点的结尾
    struct mp_node_s *next; // 指向下一个节点
    size_t failed_times;    // 内存分配失败时重试次数
} mp_node_t;

typedef struct mp_pool_s
{
    size_t max;
    struct mp_node_s *current;
    struct mp_large_s *large;
    struct mp_node_s *head;
} mp_pool_t;

void *mp_alloc(mp_pool_t *mp, size_t size);

void *mp_alloc_block(mp_pool_t *mp, size_t size)
{
    char *block;
    int ret = posix_memalign(&block, MP_ALIGNMENT, MP_PAGE_SIZE);
    if (ret != 0)
    {
        return NULL;
    }
    // 每一块空间前有一个 mp_node_s
    mp_node_t *node = (mp_node_t *)block;
    node->next = NULL;
    node->last = block + sizeof(mp_node_t);
    node->end = block + MP_PAGE_SIZE;

    mp_node_t *p, *current = mp->current;
    for (p = current; p->next; p = p->next)
    {
        // nginx 里面是用到的经验值是 4
        if (p->failed_times++ > 4)
        {
            current = p->next;
        }
    }
    p->next = node;
    return block;
}

mp_pool_t *mp_create(size_t size)
{
    // mp
    // ↓                 |<--            memory we can use            -->|
    // |<-- mp_pool_s -->|<-- mp_node_s -->|<-- blank mem to be alloc -->|
    //                   ↑                 ↑                             ↑
    //               mp->head        head->last                 head->end

    mp_pool_t *mp;
    int ret = posix_memalign(&mp, MP_ALIGNMENT, sizeof(mp_pool_t) + size);
    if (ret != 0)
        return NULL;

    mp->large = NULL;
    mp->current = mp->head = (char *)mp + sizeof(mp_pool_t); // 指向 mp_pool_s 后面的空间

    mp->head->next = NULL;
    mp->head->last = (char *)mp + sizeof(mp_pool_t) + sizeof(mp_node_t); // 当前节点可分配内存的起始地址
    mp->head->end = (char *)mp + size;                                   // 当前节点可分配内存的结束地址
    return mp;
}

void *mp_alloc_large(mp_pool_t *mp, size_t size)
{
    char *m;
    int ret = posix_memalign(&m, MP_ALIGNMENT, size);
    if (ret != 0)
        return NULL;

    for (mp_large_t *large = mp->large; large; large = large->next)
    {
        if (large->alloc == NULL)
        {
            large->alloc = m;
            return m;
        }
    }

    mp_large_t *large = mp_alloc(mp, sizeof(mp_large_t));
    large->alloc = m;
    large->next = mp->large;
    return m;
}

void *mp_alloc(mp_pool_t *mp, size_t size)
{
    if (size > MP_PAGE_SIZE - sizeof(mp_node_t))
    {
        return mp_alloc_large(mp, size);
    }

    mp_node_t *node = mp->current;
    char *last = node->last;
    // 当前节点还有能够分配出 size 大小的空间
    if (node->end - last >= size)
    {
        node->last += size; // 更新当前节点 last 位置
        return last;
    }
}