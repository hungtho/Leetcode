#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct
{
    /* data */
    int id; // Task ID
    int priority; // Task priority
}Task;

typedef struct 
{
    /* data */
    Task *tasks; // Array of tasks
    int size; // Max size of the hea
    int count; // Current number of elements in the heap
    Task firstTask; // Highest priority task
}MaxHeap;

MaxHeap *min_heap_init(int size)
{
    if(size <= 0) return NULL; // Check for valid size

    MaxHeap *hp = (MaxHeap*)malloc(sizeof(MaxHeap));
    if(!hp) return NULL; // Check if memory allocation was successful
    hp->tasks = (Task*)malloc(sizeof(Task) * size); // Allocate memory for heap tasks

    if(!hp->tasks)
    {
        free(hp); // Free the heap if tasks allocation fails
        return NULL;
    }

    hp->size = size;// Set max size of the heap
    hp->count  = 0; // Set current number of elements in the heap
    hp->firstTask = (Task){-1, -1}; // Initialize the first task

    return hp; // Return the initialized heap
}

void heap_free(MaxHeap *hp)
{
    if(hp)
    {
        free(hp->tasks); // Free the tasks array
        free(hp); // Free the heap structure
    }
}

void heap_clear(MaxHeap *hp)
{
    if(hp)
    {
        hp->count = 0; // Reset the count of elements in the heap
    }
}

bool heap_is_empty(const MaxHeap *hp)
{
    return !hp || (hp->count == 0);
}

bool heap_is_full(const MaxHeap *hp)
{
    return hp && (hp->count == hp->size);
}

bool heap_push(MaxHeap *hp, Task task)
{
    if(!hp || hp->tasks == NULL) return false;

    if(hp->count == hp->size)
    {
        hp->size *= 2;
        hp->tasks = (Task*)realloc(hp->tasks, sizeof(Task) * hp->size);
        if(!hp->tasks) return false;
    }

    if(hp->count == 0 || task.priority < hp->firstTask.priority)
        hp->firstTask = task;

    // Insert the new value into the heap
    hp->tasks[hp->count] = task;
    hp->count++;

    // Restore the heap property
    int i = hp->count - 1;
    while(i > 0)
    {
        int parent = (i - 1) / 2;
        if(hp->tasks[parent].priority >= hp->tasks[i].priority) break;

        // Swap parent and current
        Task temp = hp->tasks[parent];
        hp->tasks[parent] = hp->tasks[i];
        hp->tasks[i] = temp;
        i = parent;
    }

    return true;
}

Task heap_pop(MaxHeap *hp)
{
    if(!hp || hp->count == 0) return (Task){-1, -1}; // Check for valid heap and elements

    Task root = hp->tasks[0]; // Get the root element
    hp->count--;

    // Move the last element to the root
    hp->tasks[0] = hp->tasks[hp->count];

    // Restore the heap property
    int i = 0;
    while(2 * i + 1 < hp->count)
    {
        int child = 2 * i + 1;
        if(child + 1 < hp->count && hp->tasks[child + 1].priority > hp->tasks[child].priority)
            child++;

        if(hp->tasks[i].priority >= hp->tasks[child].priority) break;

        // Swap parent and child
        Task temp = hp->tasks[i];
        hp->tasks[i] = hp->tasks[child];
        hp->tasks[child] = temp;
        i = child;
    }

    // Reset firstTask
    if(hp->count == 0)
    {
        hp->firstTask = (Task){-1,-1};
    }    
    else if(root.priority == hp->firstTask.priority && hp->count > 0) {
        Task newBest = hp->tasks[0];
        for(int j=1;j<hp->count;j++)
            if(hp->tasks[j].priority < newBest.priority)
                newBest = hp->tasks[j];
        hp->firstTask = newBest;
    }

    return root;
}

Task heap_peek(const MaxHeap *hp)
{
    if(!hp || hp->count == 0) return (Task){-1, -1}; // Check for valid heap and elements
    return hp->tasks[0]; // Return the root element
}

Task heap_pop_firsttask(MaxHeap *hp) {
    if(!hp || hp->count == 0) return (Task){-1, -1};
    
    // Store the firstTask to return
    Task result = hp->firstTask;
    
    // Find the index of the firstTask in the heap
    int target_index = -1;
    for(int i = 0; i < hp->count; i++) {
        if(hp->tasks[i].id == hp->firstTask.id && 
           hp->tasks[i].priority == hp->firstTask.priority) {
            target_index = i;
            break;
        }
    }
    
    // If firstTask not found (shouldn't happen), return error
    if(target_index == -1) return (Task){-1, -1};
    
    // Remove the task at target_index
    // Move the last element to fill the gap
    hp->tasks[target_index] = hp->tasks[hp->count - 1];
    hp->count--;
    
    // If we removed the last element, no need to restore heap property
    if(target_index == hp->count) {
        // Update firstTask for remaining elements
        if(hp->count == 0) {
            hp->firstTask = (Task){-1, -1};
        } else {
            // Find new minimum
            Task new_min = hp->tasks[0];
            for(int i = 1; i < hp->count; i++) {
                if(hp->tasks[i].priority < new_min.priority) {
                    new_min = hp->tasks[i];
                }
            }
            hp->firstTask = new_min;
        }
        return result;
    }
    
    // Restore heap property starting from target_index
    // First try to bubble up (if the moved element is larger than its parent)
    int current = target_index;
    while(current > 0) {
        int parent = (current - 1) / 2;
        if(hp->tasks[parent].priority >= hp->tasks[current].priority) break;
        
        // Swap parent and current
        Task temp = hp->tasks[parent];
        hp->tasks[parent] = hp->tasks[current];
        hp->tasks[current] = temp;
        current = parent;
    }
    
    // If we didn't bubble up, try to bubble down
    if(current == target_index) {
        while(2 * current + 1 < hp->count) {
            int child = 2 * current + 1;
            if(child + 1 < hp->count && 
               hp->tasks[child + 1].priority > hp->tasks[child].priority) {
                child++;
            }
            
            if(hp->tasks[current].priority >= hp->tasks[child].priority) break;
            
            // Swap current and child
            Task temp = hp->tasks[current];
            hp->tasks[current] = hp->tasks[child];
            hp->tasks[child] = temp;
            current = child;
        }
    }
    
    // Update firstTask for remaining elements
    if(hp->count == 0) {
        hp->firstTask = (Task){-1, -1};
    } else {
        // Find new minimum among remaining tasks
        Task new_min = hp->tasks[0];
        for(int i = 1; i < hp->count; i++) {
            if(hp->tasks[i].priority < new_min.priority) {
                new_min = hp->tasks[i];
            }
        }
        hp->firstTask = new_min;
    }
    
    return result;
}


// ---  top-k tasks ---
MaxHeap* top_k_tasks(Task *tasks, int n, int k) {
    if(k <= 0 || n <= 0) return NULL;
    MaxHeap *heap = min_heap_init(k);
    if(!heap) return NULL;

    for(int i = 0; i < n; i++) {
        if(heap->count < k) {
            heap_push(heap, tasks[i]);
        } else if(tasks[i].priority < heap->tasks[0].priority) {
            heap_pop(heap);
            heap_push(heap, tasks[i]);
        } else {
            // firstTask 
            if(tasks[i].priority < heap->firstTask.priority)
                heap->firstTask = tasks[i];
        }
    }
    return heap;
}

void print_task(Task t) {
    printf("Task{id=%d, priority=%d}\n", t.id, t.priority);
}


void print_heap(MaxHeap *hp) {
    printf("Heap (count=%d):\n", hp->count);
    for (int i = 0; i < hp->count; i++) {
        printf("  ");
        print_task(hp->tasks[i]);
    }
    printf("FirstTask (best): ");
    print_task(hp->firstTask);
}

// Test
int main() {
    Task tasks[] = {
        {1, 5}, {2, 3}, {3, 8}, {4, 1}, {5, 7},
        {6, 2}, {7, 6}, {8, 4}
    };
    int n = sizeof(tasks) / sizeof(tasks[0]);
    int k = 5;

    printf("  tasks  :\n");
    for (int i = 0; i < n; i++) {
        print_task(tasks[i]);
    }

    printf("top-%d tasks    :\n", k);
    MaxHeap *heap = top_k_tasks(tasks, n, k);

    print_heap(heap);
    
    // Test pop
    printf("\nPop root:\n");
    Task popped = heap_pop(heap);
    printf("Popped: ");
    print_task(popped);
    print_heap(heap);

    Task popped2 = heap_pop(heap);
    printf("Popped: ");
    print_task(popped2);
    print_heap(heap);

    heap_free(heap);

    return 0;
}