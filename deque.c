#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct 
{
    /* data */
    int x;
    int y;
}Point;

typedef struct 
{
    /* data */
    Point *points;
    int head_index;  // Index of the head of the queue
    int tail_index;  // Index of the tail of the queue
    int max_size;    // Maximum size of the queue
    int count;       // Current number of elements in the queue
}Queue;

Queue *queue_init(int size);
void queue_free(Queue *q);
bool queue_is_empty(const Queue *q);
bool queue_is_full(const Queue *q);
bool queue_push(Queue *q, Point p);
bool queue_pop(Queue *q, Point *p);
bool queue_head_peek(const Queue *q, Point *p);
void queue_clear(Queue *q);

Queue *queue_init(int size)
{
    if(size <= 0) return NULL;  // Check for valid size
    Queue *q = (Queue *)malloc(sizeof(Queue));  //Allocate memory for the queue
    if(!q) return NULL;  // Check if memory allocation was successful
    q->points = (Point *)malloc(sizeof(Point) * size);  // Allocate memory for points
    if(!q->points)
    {
        free(q);  // Free the queue if pooints allocation fails
        return NULL;
    }

    q->head_index = 0;  // Initialize head index
    q->tail_index = 0; // Initialize tail index
    q->max_size = size; // Set maximum size of the queue
    q->count = 0; // Initialize count of elements in the queue

    return q; // Return the initialized queue
}

void queue_free(Queue *q)
{
    if(q)
    {
        free(q->points); //Free the points array
        free(q); // Free the queue structure
    }
}

bool queue_is_empty(const Queue *q)
{
    return !q || (q->count == 0);// Check if the queue is empty
    // The queue is empty if count is 0 or if the queue pointer is NULL
}

bool queue_is_full(const Queue *q)
{
    return q && (q->count == q->max_size); // Check if the queue is full
    // The queue is full if count equals max_size or if the queue pointer is NULL
}

bool queue_push(Queue *q, Point p)
{
    if(!q || !q->points || queue_is_full(q))
    {
        return false;
    }

    q->points[q->tail_index] = p;
    q->tail_index = (q->tail_index + 1) % q->max_size;
    q->count ++;

    return true;
}

bool queue_pop(Queue *q, Point *p)
{
    if(!q || !p || queue_is_empty(q)) return false; // Check for valid queue and point
    *p = q->points[q->head_index]; // Get the point at the head of the queue
    q->head_index = (q->head_index + 1) % q->max_size; //Move head index forward
    q->count--; // Decrease the count of elements in the queue
    return true;
}

bool queue_head_peek(const Queue *q, Point *p)
{
    if(!q || !p || queue_is_empty(q)) return false; // Check for valid queue and point
    *p = q->points[q->head_index]; // Get the point at the head of the queue
    return true;
}

void queue_clear(Queue *q)
{
    if(q)
    {
        q->head_index = 0; // Reset head index
        q->tail_index = 0; // Reset tail index
        q->count = 0; // Reset count
    }
}
// Helper function to free 2D arrays safely
void free_2d_arrays(bool **visited, int **dist, int m)
{
    if(visited)
    {
        for(int i = 0; i < m; i++)
        {
            free(visited[i]);
        }
        free(visited);
    }
    
    if(dist)
    {
        for(int i = 0; i < m; i++)
        {
            free(dist[i]);
        }
        free(dist);
    }
}

int bfs(int m, int n, Point start, Point end)
{
    // Input validation
    if(m <= 0 || n <= 0) return -1;
    if(start.x < 0 || start.x >= m || start.y < 0 || start.y >= n) return -1;
    if(end.x < 0 || end.x >= m || end.y < 0 || end.y >= n) return -1;

    // Early return if start equals end
    if(start.x == end.x && start.y == end.y) return 0;

    bool **visited = (bool **)malloc( sizeof(bool *) * m);
    int **dist = (int **)malloc(sizeof(int *) * m);

    if(!visited || !dist)
    {
        free_2d_arrays(visited, dist, 0);
        return -1;
    }

    for(int i = 0; i< m ; i++)
    {
        visited[i] = (bool *)calloc(n, sizeof(bool));
        dist[i]= (int *)malloc(sizeof(int)*n);

        if(!visited[i] || !dist[i])
        {
            if(visited[i]) free(visited[i]);
            if(dist[i]) free(dist[i]);
            return -1;
        }

        for(int j = 0; j < n; j++)
        {
            dist[i][j] = -1; // Initialize distance to -1
        }
    }

    Queue *q = queue_init(m * n); // Initialize the queue with maximum size
    if(!q) 
    {
        free_2d_arrays(visited, dist, m);
        return -1;
    }

    visited[start.x][start.y] = true; // Mark the start point as visited
    dist[start.x][start.y] = 0; // Distance to start point is 0
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0,-1, 1};

    queue_push(q, start); // Push the start point into the queue

    while(!queue_is_empty(q))
    {
        Point cur;
        queue_pop(q, &cur); // Pop the current point from the queue

        if(cur.x == end.x && cur.y == end.y) // check if we reached the end point
        {
            // Reached the end point, return the distance
            int distance = dist[cur.x][cur.y];
            queue_free(q);
            free_2d_arrays(visited, dist, m);

            return distance;
        }

        for(int dir = 0; dir < 4; dir++)
        {
            int new_x = cur.x + dx[dir];
            int new_y = cur.y + dy[dir];
            if(new_x >=0 && new_x < m && new_y >= 0 && new_y < n && !visited[new_x][new_y])
            {
                visited[new_x][new_y] = true;
                dist[new_x][new_y] = dist[cur.x][cur.y] + 1;
                Point next = {new_x, new_y};
                queue_push(q, next);
            }
        }
    }

    queue_free(q);
    free_2d_arrays(visited, dist, m);
    return -1;
}



// Test function for BFS implementation
int main()
{
    printf("=== BFS Algorithm Testing ===\n\n");
    
    // Test 1: Normal case - 3x3 grid
    Point start1 = {0, 0};
    Point end1 = {2, 2};
    int result1 = bfs(3, 3, start1, end1);
    printf("Test 1 - 3x3 grid (0,0) to (2,2): %d (Expected: 4)\n", result1);
    
    // Test 2: Same point
    Point start2 = {1, 1};
    Point end2 = {1, 1};
    int result2 = bfs(3, 3, start2, end2);
    printf("Test 2 - Same point (1,1) to (1,1): %d (Expected: 0)\n", result2);
    
    // Test 3: Adjacent points
    Point start3 = {0, 0};
    Point end3 = {0, 1};
    int result3 = bfs(3, 3, start3, end3);
    printf("Test 3 - Adjacent (0,0) to (0,1): %d (Expected: 1)\n", result3);
    
    // Test 4: L-shaped path
    Point start4 = {0, 0};
    Point end4 = {1, 1};
    int result4 = bfs(3, 3, start4, end4);
    printf("Test 4 - L-shaped (0,0) to (1,1): %d (Expected: 2)\n", result4);
    
    // Test 5: Invalid bounds - start point
    Point start5 = {-1, 0};
    Point end5 = {1, 1};
    int result5 = bfs(3, 3, start5, end5);
    printf("Test 5 - Invalid start (-1,0): %d (Expected: -1)\n", result5);
    
    // Test 6: Invalid bounds - end point
    Point start6 = {0, 0};
    Point end6 = {3, 3};
    int result6 = bfs(3, 3, start6, end6);
    printf("Test 6 - Invalid end (3,3): %d (Expected: -1)\n", result6);
    
    // Test 7: Invalid grid size
    Point start7 = {0, 0};
    Point end7 = {1, 1};
    int result7 = bfs(0, 0, start7, end7);
    printf("Test 7 - Invalid grid size: %d (Expected: -1)\n", result7);
    
    // Test 8: Large grid corner to corner
    Point start8 = {0, 0};
    Point end8 = {4, 4};
    int result8 = bfs(5, 5, start8, end8);
    printf("Test 8 - 5x5 grid (0,0) to (4,4): %d (Expected: 8)\n", result8);
    
    printf("\n=== Queue Testing ===\n");
    
    // Test Queue functions
    Queue *q = queue_init(5);
    printf("Queue initialized: %s\n", q ? "Success" : "Failed");
    printf("Is empty: %s\n", queue_is_empty(q) ? "Yes" : "No");
    printf("Is full: %s\n", queue_is_full(q) ? "Yes" : "No");
    
    // Push some points
    Point p1 = {1, 2};
    Point p2 = {3, 4};
    queue_push(q, p1);
    queue_push(q, p2);
    printf("After pushing 2 points - Is empty: %s\n", queue_is_empty(q) ? "Yes" : "No");
    
    // Peek and pop
    Point peeked;
    if(queue_head_peek(q, &peeked)) {
        printf("Peeked point: (%d, %d)\n", peeked.x, peeked.y);
    }
    
    Point popped;
    if(queue_pop(q, &popped)) {
        printf("Popped point: (%d, %d)\n", popped.x, popped.y);
    }
    
    queue_free(q);
    printf("Queue freed successfully\n");
    
    return 0;
}