// Names + ID:
// Adir Edri 206991762    
// Ido Gutman 325314698

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_QUEUE_SIZE 100

// Help Functions 

typedef struct 
{
    int arrival_time;
    int computation_time;
    int remaining_time;
    int finish_time;
    int turnaround_time;
} Process;

typedef struct 
{
    int items[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

void init_queue(Queue *q) 
{
    q->front = -1;
    q->rear = -1;
}

int is_empty(Queue *q) 
{
    return q->front == -1;
}

int is_full(Queue *q) 
{
    return (q->rear + 1) % MAX_QUEUE_SIZE == q->front;
}

void enqueue(Queue *q, int value) 
{
    if (is_full(q)) 
    {
        printf("Queue is full\n");
        return;
    }
    if (is_empty(q)) 
        q->front = 0;

    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->items[q->rear] = value;
}

int dequeue(Queue *q) 
{
    if (is_empty(q)) 
    {
        printf("Queue is empty\n");
        return -1;
    }
    int value = q->items[q->front];
    if (q->front == q->rear) 
    {
        q->front = -1;
        q->rear = -1;
    } 
    else 
        q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    return value;
}

void swap(Process *a, Process *b) 
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void bSort(Process *processes, int n) 
{
    for (int i = 0; i < n - 1; i++) 
        for (int j = 0; j < n - i - 1; j++) 
            if (processes[j].arrival_time > processes[j + 1].arrival_time) 
                swap(&processes[j], &processes[j + 1]);
}

void reset_remaining_times(Process *processes, int n) 
{
    for (int i = 0; i < n; i++) 
    {
        processes[i].remaining_time = processes[i].computation_time;
        processes[i].finish_time = 0;
        processes[i].turnaround_time = 0;
    }
}

void reset_and_execute(void (*scheduler)(Process *, int), Process *processes, int n) 
{
    reset_remaining_times(processes, n);
    scheduler(processes, n);
}

// --------------------------  Algo's ------------------------------

// FCFS Algorithm

void calculate_fcfs(Process *processes, int n) 
{
    int current_time = 0;
    int total_turnaround_time = 0;

    for (int i = 0; i < n; i++) 
    {
        if (current_time < processes[i].arrival_time) 
            current_time = processes[i].arrival_time;

        processes[i].finish_time = current_time + processes[i].computation_time;
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
        total_turnaround_time += processes[i].turnaround_time;
        current_time = processes[i].finish_time;
    }

    printf("FCFS: mean turnaround = %.2f\n", (double)total_turnaround_time / n);
}

// LCFS Nonpreemitive Algorithm

void calculate_lcfs_np(Process *processes, int n) 
{
    int current_time = processes[0].arrival_time + processes[0].computation_time;
    int total_turnaround_time = processes[0].turnaround_time = processes[0].finish_time = current_time - processes[0].arrival_time;

    for (int i = n - 1; i > 0; i--) 
    {
        if (current_time < processes[i].arrival_time) 
            current_time = processes[i].arrival_time;

        current_time += processes[i].computation_time;
        processes[i].finish_time = current_time;
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    printf("LCFS (NP): mean turnaround = %.2f\n", (double)total_turnaround_time / n);
}

// LCFS Preemitive Algorithm

void calculate_lcfs_p(Process *processes, int n) 
{
    int current_time = 0;
    int total_turnaround_time = 0;
    int remaining_processes = n;
    int last_index = -1;

    while (remaining_processes > 0) 
    {
        last_index = -1;  
        for (int i = 0; i < n; i++) 
        {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) 
                last_index = i;  
        }
        if (last_index != -1) 
        {
            processes[last_index].remaining_time--;
            current_time++;

            if (processes[last_index].remaining_time == 0) 
            {
                processes[last_index].finish_time = current_time;
                processes[last_index].turnaround_time = processes[last_index].finish_time - processes[last_index].arrival_time;
                total_turnaround_time += processes[last_index].turnaround_time;
                remaining_processes--;
            }
        } 
        else 
        {
            int next_arrival_time = -1;
            for (int i = 0; i < n; i++) 
            {
                if (processes[i].remaining_time > 0 && (next_arrival_time == -1 || processes[i].arrival_time < next_arrival_time)) 
                    next_arrival_time = processes[i].arrival_time;
            }

            if (next_arrival_time != -1 && next_arrival_time > current_time)
                current_time = next_arrival_time;  
            else
                break;
        }
    }
    printf("LCFS (P): mean turnaround = %.2f\n", (double)total_turnaround_time / n);
}

// RR Algorithm (with quantum = 2)

void calculate_rr(Process *processes, int n, int quantum) 
{
    Queue q;
    init_queue(&q);
    int current_time = 0;
    int total_turnaround_time = 0;
    int remaining_processes = n;
    int i = 0;

    while (remaining_processes > 0) 
    {
        while (i < n && processes[i].arrival_time <= current_time) 
        {
            enqueue(&q, i);
            i++;
        }

        if (!is_empty(&q)) 
        {
            int index = dequeue(&q);
            if (processes[index].remaining_time > quantum) 
            {
                processes[index].remaining_time -= quantum;
                current_time += quantum;
                enqueue(&q, index);
            } 
            else 
            {
                current_time += processes[index].remaining_time;
                processes[index].remaining_time = 0;
                processes[index].finish_time = current_time;
                processes[index].turnaround_time = processes[index].finish_time - processes[index].arrival_time;
                total_turnaround_time += processes[index].turnaround_time;
                remaining_processes--;
            }
        } 
        else 
            current_time++;
    }

    printf("RR: mean turnaround = %.2f\n", (double)total_turnaround_time / n);
}

// SJF Algorithm

void calculate_sjf_p(Process *processes, int n) 
{
    int current_time = 0;
    int total_turnaround_time = 0;
    int remaining_processes = n;
    int shortest_index = -1;

    while (remaining_processes > 0) 
    {
        shortest_index = -1;
        for (int i = 0; i < n; i++) 
        {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) 
            {
                if (shortest_index == -1 || processes[i].remaining_time < processes[shortest_index].remaining_time) 
                    shortest_index = i;
            }
        }

        if (shortest_index != -1) 
        {
            processes[shortest_index].remaining_time--;
            current_time++;

            if (processes[shortest_index].remaining_time == 0) 
            {
                processes[shortest_index].finish_time = current_time;
                processes[shortest_index].turnaround_time = processes[shortest_index].finish_time - processes[shortest_index].arrival_time;
                total_turnaround_time += processes[shortest_index].turnaround_time;
                remaining_processes--;
            }
        } 
        else 
        {
            int next_arrival_time = -1;
            for (int i = 0; i < n; i++) 
            {
                if (processes[i].remaining_time > 0 && (next_arrival_time == -1 || processes[i].arrival_time < next_arrival_time)) 
                    next_arrival_time = processes[i].arrival_time;
            }

            if (next_arrival_time != -1)
                current_time = next_arrival_time;
            else
                break; 
        }
    }

    printf("SJF: mean turnaround = %.2f\n", (double)total_turnaround_time / n);
}

//  -------------------------- Main ------------------------------

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) 
    {
        perror("Failed to open file");
        return 1;
    }

    int n;
    char *line = NULL;
    size_t len = 0;

    if (getline(&line, &len, file) != -1) 
        n = atoi(line);
    else 
    {
        perror("Failed to read number of processes");
        return 1;
    }

    Process *processes = (Process *)malloc(n * sizeof(Process));

    for (int i = 0; i < n; i++) 
    {
        if (getline(&line, &len, file) != -1) 
            sscanf(line, "%d,%d", &processes[i].arrival_time, &processes[i].computation_time);
        else 
        {
            perror("Failed to read process data");
            free(processes);
            return 1;
        }
    }

    bSort(processes, n); 

    reset_and_execute(calculate_fcfs, processes, n);
    reset_and_execute(calculate_lcfs_np, processes, n);
    reset_and_execute(calculate_lcfs_p, processes, n);

    reset_remaining_times(processes, n);
    calculate_rr(processes, n, 2);

    reset_and_execute(calculate_sjf_p, processes, n);

    free(processes);
    free(line);
    fclose(file);

    return 0;
}
