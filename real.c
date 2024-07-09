#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_TASKS 10

typedef struct {
    char name[10];
    int period;
    int burst;
    int remaining;
    int deadline;
    int start_time;
    int executed;
    int deadline_miss;
    int killed;
} Task;

void init_tasks(Task tasks[], int num_tasks) {
    for (int i = 0; i < num_tasks; i++) {
        tasks[i].remaining = tasks[i].burst;
        tasks[i].deadline = tasks[i].period;
        tasks[i].start_time = 0;
        tasks[i].executed = 0;
        tasks[i].deadline_miss = 0;
        tasks[i].killed = 0;
    }
}

void execute_rate_monotonic(Task tasks[], int num_tasks, int total_time) {
    FILE *output_file = fopen("rate.out", "w");
    fprintf(output_file, "EXECUTION BY RATE\n");

    init_tasks(tasks, num_tasks);

    int time = 0, idle_time = 0;
    Task *current_task = NULL;

    while (time < total_time) {
        for (int i = 0; i < num_tasks; i++) {
            if (time >= tasks[i].deadline) {
                tasks[i].deadline_miss += (tasks[i].remaining > 0);
                tasks[i].deadline += tasks[i].period;
                tasks[i].remaining = tasks[i].burst;
                if (current_task == &tasks[i]) {
                    fprintf(output_file, "[%s] for %d units - L\n", tasks[i].name, time - tasks[i].start_time);
                    tasks[i].start_time = time;
                    tasks[i].killed ++;
                }
            }
        }

        Task *task_to_run = NULL;
        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i].remaining > 0 && (task_to_run == NULL || tasks[i].period < task_to_run->period)) {
                task_to_run = &tasks[i];
            }
        }

        if (task_to_run != NULL) {
            if (current_task != task_to_run) {
                if (current_task != NULL && time > current_task->start_time) {
                    fprintf(output_file, "[%s] for %d units - H\n", current_task->name, time - current_task->start_time);
                }
                current_task = task_to_run;
                current_task->start_time = time;
            }
            
            current_task->remaining = current_task->remaining - 1;

            if (idle_time != 0) {
                fprintf(output_file, "idle for %d units\n", idle_time);
                idle_time = 0;
            }

            if (current_task->remaining == 0) {
                fprintf(output_file, "[%s] for %d units - F\n", current_task->name, time - current_task->start_time + 1);
                current_task->executed++;
                current_task = NULL;
            }

        } else {
            idle_time++;
            if (current_task && idle_time == 0) {
                fprintf(output_file, "[%s] for %d units - H\n", current_task->name, time - current_task->start_time);
                current_task = NULL;
            }
        }

        time++;
    }

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].remaining > 0 && tasks[i].remaining < tasks[i].burst) {
            fprintf(output_file, "[%s] for %d units - K\n", tasks[i].name, time - tasks[i].start_time);
            tasks[i].killed = 1;
        }
    }

    fprintf(output_file, "\nLOST DEADLINES\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(output_file, "[%s] %d\n", tasks[i].name, tasks[i].deadline_miss);
    }

    fprintf(output_file, "\nCOMPLETE EXECUTION\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(output_file, "[%s] %d\n", tasks[i].name, tasks[i].executed);
    }

    fprintf(output_file, "\nKILLED\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(output_file, "[%s] %d\n", tasks[i].name, tasks[i].killed);
    }

    fclose(output_file);
}


void execute_earliest_deadline_first(Task tasks[], int num_tasks, int total_time) {
    FILE *output_file = fopen("edf.out", "w");
    fprintf(output_file, "EXECUTION BY EDF\n");

    init_tasks(tasks, num_tasks);

    int time = 0, idle_time = 0;
    Task *current_task = NULL;

    while (time < total_time) {
        for (int i = 0; i < num_tasks; i++) {
            if (time >= tasks[i].deadline) {
                if (tasks[i].remaining > 0) {
                    tasks[i].deadline_miss++;
                    if (current_task == &tasks[i]) {
                        fprintf(output_file, "[%s] for %d units - L\n", tasks[i].name, time - tasks[i].start_time);
                        tasks[i].start_time = time;
                    }
                }
                tasks[i].remaining = tasks[i].burst;
                tasks[i].deadline += tasks[i].period;
            }
        }

        Task *task_to_run = NULL;
        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i].remaining > 0 && (task_to_run == NULL || tasks[i].deadline < task_to_run->deadline)) {
                task_to_run = &tasks[i];
            }
        }

        if (task_to_run != NULL) {
            if (current_task != task_to_run) {
                if (current_task != NULL && time > current_task->start_time) {
                    fprintf(output_file, "[%s] for %d units - H\n", current_task->name, time - current_task->start_time);
                }
                current_task = task_to_run;
                current_task->start_time = time;
            }

           current_task->remaining = current_task->remaining - 1;

            if (idle_time > 0) {
                fprintf(output_file, "idle for %d units\n", idle_time);
                idle_time = 0;
            }

            if (current_task->remaining == 0) {
                fprintf(output_file, "[%s] for %d units - F\n", current_task->name, time - current_task->start_time + 1);
                current_task->executed++;
                current_task = NULL;
            }
        } else {
            idle_time++;
            if (current_task != NULL) {
                current_task = NULL;
            }
        }

        time++;
    }

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].remaining > 0 && tasks[i].remaining < tasks[i].burst) {
            tasks[i].killed = 1; 
            fprintf(output_file, "[%s] for %d units - K\n", tasks[i].name, tasks[i].burst - tasks[i].remaining);
        }
    }

    fprintf(output_file, "\nLOST DEADLINES\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(output_file, "[%s] %d\n", tasks[i].name, tasks[i].deadline_miss);
    }

    fprintf(output_file, "\nCOMPLETE EXECUTION\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(output_file, "[%s] %d\n", tasks[i].name, tasks[i].executed);
    }

    fprintf(output_file, "\nKILLED\n");
    for (int i = 0; i < num_tasks; i++) {
        fprintf(output_file, "[%s] %d\n", tasks[i].name, tasks[i].killed);
    }

    fclose(output_file);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Use: %s <input_file>\n", argv[0]);
        return 1;
    }

    char *algoritmo = argv[0];

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Enable to open file: %s\n", argv[1]);
        return 1;
    }

    int total_time, num_tasks = 0;
    Task tasks[MAX_TASKS];

    fscanf(file, "%d", &total_time);

    while (fscanf(file, "%s %d %d", tasks[num_tasks].name, &tasks[num_tasks].period, &tasks[num_tasks].burst) == 3) {
        num_tasks++;
    }

    fclose(file);
    init_tasks(tasks, num_tasks);

    if (strcmp(algoritmo, "./rate") == 0) {
        execute_rate_monotonic(tasks, num_tasks, total_time);
    } else if (strcmp(algoritmo, "./edf") == 0 ) {
        execute_earliest_deadline_first(tasks, num_tasks, total_time);
    }

    return 0;
}
