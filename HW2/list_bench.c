#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

struct list_node {
    int val;
    struct list_node *next;
};

volatile long long sink = 0;

/* Fisher–Yates shuffle */
static void shuffle(size_t *idx, size_t n)
{
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)(rand() % (i + 1));
        size_t t = idx[i];
        idx[i] = idx[j];
        idx[j] = t;
    }
}

/* 建立隨機走訪順序的 linked list */
struct list_node *build_random_list(size_t n, struct list_node **base_out)
{
    struct list_node *nodes = malloc(n * sizeof(struct list_node));
    if (!nodes) {
        perror("malloc");
        exit(1);
    }

    size_t *idx = malloc(n * sizeof(size_t));
    if (!idx) {
        perror("malloc");
        exit(1);
    }

    for (size_t i = 0; i < n; i++) {
        nodes[i].val = (int)i;
        idx[i] = i;
    }

    shuffle(idx, n);

    for (size_t i = 0; i + 1 < n; i++)
        nodes[idx[i]].next = &nodes[idx[i + 1]];

    nodes[idx[n - 1]].next = NULL;

    struct list_node *head = &nodes[idx[0]];
    free(idx);

    *base_out = nodes;
    return head;
}

/* fast-slow pointer */
struct list_node *middle_fast_slow(struct list_node *head)
{
    struct list_node *slow = head;
    struct list_node *fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/* two-pass */
struct list_node *middle_two_pass(struct list_node *head)
{
    struct list_node *cur = head;
    size_t n = 0;

    while (cur) {
        n++;
        cur = cur->next;
    }

    cur = head;
    for (size_t i = 0; i < n / 2; i++)
        cur = cur->next;

    return cur;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <N> <mode>\n", argv[0]);
        printf("mode = 0 fast_slow\n");
        printf("mode = 1 two_pass\n");
        return 1;
    }

    size_t N = strtoull(argv[1], NULL, 10);
    int mode = atoi(argv[2]);

    srand(1);

    struct list_node *base = NULL;
    struct list_node *head = build_random_list(N, &base);

    struct list_node *mid;

    if (mode == 0)
        mid = middle_fast_slow(head);
    else
        mid = middle_two_pass(head);

    sink += mid->val;

    printf("sink=%lld\n", sink);

    free(base);
    return 0;
}