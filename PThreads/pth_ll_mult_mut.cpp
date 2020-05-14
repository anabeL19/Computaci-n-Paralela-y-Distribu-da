#include <iostream>
#include <random>
#include <pthread.h>
#include <chrono>

// g++ -g -Wall -o test pth_ll_mult_mut.cpp -lpthread

int Insert(int value, struct ListNode** head_p);
int Member(int value, struct ListNode* head_p);
int Delete(int value, struct ListNode** head_p);
void printList(struct ListNode* head_p);
void* Thread_op(void* arg);

class ListNode
{
    public:
    int data;
    struct ListNode* next;
};

typedef std::chrono::steady_clock::time_point time_pt;

long thread_count;
int c_insert = 10000;
int c_delete = 10000;
int c_member = 8000;//10000;
int c_ops = c_insert + c_delete + c_member;
int op;
std::uniform_int_distribution<std::mt19937::result_type> dist1(1, c_ops);
std::mt19937 rng;

int minimum = 0;
int maximum = 10000;
ListNode* head_pt;
std::uniform_int_distribution<std::mt19937::result_type> dist(minimum, maximum);
pthread_mutex_t mutex;


int main(int argc, char* argv[])
{
    int init_keys = 1000;
    long thread_ops;
    pthread_t* thread_handles;

    thread_count = strtol(argv[1], NULL, 10);
    thread_ops = c_ops / thread_count;
    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));

    head_pt = new ListNode();
    head_pt->data = dist(rng);
    head_pt->next = NULL;
    ListNode* node_ptr;

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < init_keys; ++i) 
        Insert(dist(rng), &head_pt);

    time_pt begin = std::chrono::steady_clock::now();
    for (int thread = 0; thread < thread_count; ++thread) 
        pthread_create(&thread_handles[thread], NULL, Thread_op, (void*) thread_ops);

    for (int thread = 0; thread < thread_count; ++thread) 
        pthread_join(thread_handles[thread], NULL);
        
    time_pt end = std::chrono::steady_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" <<std::endl;
    printf("Number of threads: %ld\n", thread_count);

    pthread_mutex_destroy(&mutex);
    free(thread_handles);

    return 0;
}

void* Thread_op(void* arg)
{
    long thread_ops = (long) arg;
    for (int i = 0; i < thread_ops; ++i) {
        op = dist1(rng);
        if (op <= c_member) {
            pthread_mutex_lock(&mutex);
            Member(dist(rng), head_pt);
            pthread_mutex_unlock(&mutex);
        }
        else if (op > c_member && op <= c_member + c_insert) {
            pthread_mutex_lock(&mutex);
            Insert(dist(rng), &head_pt);
            pthread_mutex_unlock(&mutex);
        }
        else {
            pthread_mutex_lock(&mutex);
            Delete(dist(rng), &head_pt);
            pthread_mutex_unlock(&mutex);
        }
    }
}



int Member(int value, struct ListNode* head_p)
{
    struct ListNode* curr_p = head_p;

    while (curr_p != NULL && curr_p->data < value) 
        curr_p = curr_p->next;

    if (curr_p == NULL || curr_p->data > value) 
        return 0;
    else return 1;
}

int Insert(int value, struct ListNode** head_p)
{
    struct ListNode* curr_p = *head_p;
    struct ListNode* pred_p = NULL;
    struct ListNode* temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = (ListNode*) malloc(sizeof(struct ListNode));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) 
            *head_p = temp_p;
        else pred_p->next = temp_p;
        return 1;
    }
    else return 0;
}

int Delete(int value, struct ListNode** head_p)
{
    struct ListNode* curr_p = *head_p;
    struct ListNode* pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {
            *head_p = curr_p->next;
            free(curr_p);
        }
        else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    }
    else return 0;
}

void printList(struct ListNode* head_p)
{
    while (head_p != NULL) {
        std::cout << head_p->data << " ";
        head_p = head_p->next;
    }    
    std::cout << "\n";
}