#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
typedef struct 
{
    /* data */
    int x;
    int y;
}Point;

typedef struct 
{
    /* data */
    int x;
    int y;
}ParentPoint;


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

void free_2d_parent_arrays(ParentPoint **parent, int m)
{
    if(parent)
    {
        for(int i = 0; i < m; i++)
        {
            free(parent[i]);
        }
        free(parent);
    }
}
// Helper function to free 2D arrays
void free_2d_bool_array(bool **arr, int rows) {
    if (arr) {
        for (int i = 0; i < rows; i++) {
            if (arr[i]) free(arr[i]);
        }
        free(arr);
    }
}

void free_2d_int_array(int **arr, int rows) {
    if (arr) {
        for (int i = 0; i < rows; i++) {
            if (arr[i]) free(arr[i]);
        }
        free(arr);
    }
}

void free_2d_parent_array(ParentPoint **arr, int rows) {
    if (arr) {
        for (int i = 0; i < rows; i++) {
            if (arr[i]) free(arr[i]);
        }
        free(arr);
    }
}


int bfs_with_path(int m, int n, Point start, Point end, int **grid)
{
    if(m <= 0 || n <= 0 || !grid) return -1;
    if(start.x < 0 || start.x >= m || start.y < 0 || start.y >= n) return -1;
    if(end.x < 0 || end.x >= m || end.y < 0 || end.y >= n) return -1;
    if(grid[start.x][start.y] == 1 || grid[end.x][end.y] == 1) return -1;

    if(start.x == end.x && start.y == end.y)
    {
        printf("Start and end are the same point: (%d, %d)\n", start.x, start.y);
        return 0;
    }

    // Allocate memory for arrays
    bool **visited = (bool **)malloc(sizeof(bool *) * m);
    int **dist = (int **)malloc(sizeof(int *) * m);
    ParentPoint **parent = (ParentPoint **)malloc(sizeof(ParentPoint *) * m);

    for(int i = 0; i < m; i++)
    {
        visited[i] = (bool *)calloc(n, sizeof(bool));
        dist[i] = (int *)malloc(sizeof(int) * n);
        parent[i] = (ParentPoint *)malloc(sizeof(ParentPoint) * n);

        if (!visited[i] || !dist[i] || !parent[i]) {
            free_2d_bool_array(visited, m);
            free_2d_int_array(dist, m);
            free_2d_parent_array(parent, m);
            return -1;
        }

        for(int j = 0; j < n; j++)
        {
            dist[i][j] = -1;
            parent[i][j] = (ParentPoint){-1, -1};
        }
    }

    Queue *q = queue_init(m * n);
    if(!q)
    {
        free_2d_bool_array(visited, m);
        free_2d_int_array(dist, m);
        free_2d_parent_array(parent, m);
        return -1;
    }

    // BFS
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    bool found = false;
    
    visited[start.x][start.y] = true;
    dist[start.x][start.y] = 0;
    queue_push(q, start);

    while(!queue_is_empty(q) && !found)
    {
        Point cur;
        queue_pop(q, &cur);
        
        for(int dir = 0; dir < 4; dir++)
        {
            int new_x = cur.x + dx[dir];
            int new_y = cur.y + dy[dir];

            if(new_x >= 0 && new_x < m && new_y >= 0 && new_y < n && 
               !visited[new_x][new_y] && grid[new_x][new_y] == 0)
            {
                visited[new_x][new_y] = true;
                dist[new_x][new_y] = dist[cur.x][cur.y] + 1;
                parent[new_x][new_y] = (ParentPoint){cur.x, cur.y}; 
                
                if(new_x == end.x && new_y == end.y)
                {
                    found = true;
                    break;
                }

                Point next = {new_x, new_y};
                queue_push(q, next);
            }
        }
    }

    int distance = -1;
    if(found)
    {
        distance = dist[end.x][end.y];
        
        // Reconstruct path
        Point *path = (Point *)malloc(sizeof(Point) * (distance + 1));
        if(path)
        {
            int path_len = 0;
            Point cur = end;

            // FIX: Backtrack correctly
            while(!(cur.x == start.x && cur.y == start.y))
            {
                path[path_len++] = cur;
                ParentPoint par = parent[cur.x][cur.y];
                cur.x = par.x;
                cur.y = par.y;
            }
            path[path_len++] = start; // Add start point

            // Draw grid
            char **draw = (char**)malloc(sizeof(char*) * m);
            if(draw)
            {
                for(int i = 0; i < m; i++)
                {
                    draw[i] = (char*)malloc(sizeof(char) * (n + 1));
                    if (draw[i]) {
                        for (int j = 0; j < n; j++) {
                            if (grid[i][j] == 1) 
                                draw[i][j] = '#';
                            else 
                                draw[i][j] = '.';
                        }
                        draw[i][n] = '\0';
                    }
                }

                // Draw path
                for(int i = 0; i < path_len; i++)
                {
                    int x = path[i].x;
                    int y = path[i].y;
                    if (x >= 0 && x < m && y >= 0 && y < n) {
                        draw[x][y] = '*';
                    }
                }

                // FIX: Use single quotes for char
                if(draw[start.x]) draw[start.x][start.y] = 'S';
                if(draw[end.x]) draw[end.x][end.y] = 'E';
                
                printf("Shortest path length: %d\n", distance);
                printf("Path (from start to end):\n");
                for (int i = path_len - 1; i >= 0; i--) {
                    printf("(%d,%d) ", path[i].x, path[i].y);
                }
                printf("\n\nGrid with path:\n");
                
                for (int i = 0; i < m; i++) {
                    if (draw[i]) {
                        printf("%s\n", draw[i]);
                        free(draw[i]);
                    }
                }
                free(draw);
            }
            free(path);
        }
    }

    // Cleanup
    queue_free(q);
    free_2d_bool_array(visited, m);
    free_2d_int_array(dist, m);
    free_2d_parent_array(parent, m);

    return distance;
}

// Input validation
bool validate_input(int m, int n, Point start, Point end, int **grid)
{
    if(m <= 0 || n <= 0 || !grid) return false; // Invalid grid dimensions
    if(start.x < 0 || start.x >= m || start.y < 0 || start.y >= n) return false; // Invalid start point
    if(end.x < 0 || end.x >= m || end.y < 0 || end.y >= n) return false; // Invalid end point
    if(grid[start.x][start.y] == 1 || grid[end.x][end.y] == 1) return false;  // Invalid start or end point
    
    return true;
}

bool init_bfs_arrays(int m, int n, bool ***visited, int ***dist, ParentPoint ***parent)
{
    if(m <= 0 || n <= 0) return false;

    *visited = (bool **)malloc(sizeof(bool *) * m);
    *dist = (int **)malloc(sizeof(int *) * m);
    *parent = (ParentPoint **)malloc(sizeof(ParentPoint *) * m);

    if(!*visited || !*dist || !*parent) return false;

    for(int i = 0; i < m; i++)
    {
        (*visited)[i] = (bool *)calloc(n, sizeof(bool));
        (*dist)[i] = (int *)malloc(sizeof(int)*n);
        (*parent)[i] = (ParentPoint *)malloc(sizeof(ParentPoint)*n);

        if ((!(*parent)[i]) || !(*dist)[i] || !(*visited)[i]) {
            free_2d_bool_array(*visited, m);
            free_2d_int_array(*dist, m);
            free_2d_parent_array(*parent, m);
            return false;
        }

        for(int j = 0; j < n; j++)
        {
            (*dist)[i][j] = -1; // Initialize distance to -1
            (*parent)[i][j] = (ParentPoint){-1, -1}; // Initialize the default parent
        }
    }

    return true;
}


bool run_bfs(int m, int n, Point start, Point end, int **grid, bool **visited, int **dist, ParentPoint **parent)
{
    Queue *q = queue_init(m * n);
    if(!q)
    {
        return false;
    }

    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};

    visited[start.x][start.y] = true;
    queue_push(q, start);
    dist[start.x][start.y] = 0;

    while(!queue_is_empty(q))
    {
        Point cur;
        queue_pop(q, &cur);

        for(int i = 0; i < 8; i++)
        {
            int new_x = cur.x + dx[i];
            int new_y = cur.y + dy[i];

            if(new_x >= 0 && new_x < m && new_y >= 0 && new_y < n && !visited[new_x][new_y] && grid[new_x][new_y] == 0)
            {
                visited[new_x][new_y] = true;
                dist[new_x][new_y] = dist[cur.x][cur.y] + 1;
                parent[new_x][new_y] = (ParentPoint){cur.x, cur.y};

                if(new_x == end.x && new_y == end.y)
                {
                    queue_free(q);
                    return true; // Found the end point
                }

                Point next = {new_x, new_y};
                queue_push(q, next);
            }
        }
    }

    queue_free(q);

    return false;
}
Point* reconstruct_path(Point start, Point end, ParentPoint **parent, int distance, int *path_len)
{
    Point *path = (Point *)malloc(sizeof(Point) * (distance + 1));
    if (!path) return NULL;

    int len = 0;
    Point cur = end;

    while (!(cur.x == start.x && cur.y == start.y)) {
        path[len++] = cur;
        ParentPoint par = parent[cur.x][cur.y];
        cur = (Point){par.x, par.y};
    }
    path[len++] = start;

    *path_len = len;
    return path;
}

void print_path_and_grid(int m, int n, int **grid, Point *path, int path_len, 
                         Point start, Point end, int distance)
{
    const char* direction_names[] = {
        "North", "South", "East", "West", 
        "NorthEast", "NorthWest", "SouthEast", "SouthWest"
    };
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};

    char **draw = (char**)malloc(sizeof(char*) * m);
    for (int i = 0; i < m; i++) {
        draw[i] = (char*)malloc(sizeof(char) * (n + 1));
        for (int j = 0; j < n; j++) {
            draw[i][j] = (grid[i][j] == 1 ? '#' : '.');
        }
        draw[i][n] = '\0';
    }

    for (int i = 0; i < path_len; i++) {
        int x = path[i].x;
        int y = path[i].y;
        draw[x][y] = '*';
    }
    draw[start.x][start.y] = 'S';
    draw[end.x][end.y] = 'E';

    printf("8-DIRECTION BFS RESULT:\n");
    printf("Shortest path length: %d steps\n", distance);
    printf("Path coordinates (from start to end):\n");

    for (int i = path_len - 1; i >= 0; i--) {
        printf("(%d,%d)", path[i].x, path[i].y);
        if (i > 0) {
            int dx_move = path[i-1].x - path[i].x;
            int dy_move = path[i-1].y - path[i].y;
            for (int d = 0; d < 8; d++) {
                if (dx[d] == dx_move && dy[d] == dy_move) {
                    printf(" --%s--> ", direction_names[d]);
                    break;
                }
            }
        }
    }
    printf("\n\nGrid visualization:\n");
    for (int i = 0; i < m; i++) {
        printf("%s\n", draw[i]);
        free(draw[i]);
    }
    free(draw);
}

int bfs_with_path_8dir_new(int m, int n, Point start, Point end, int **grid)
{
    if(!validate_input(m, n, start, end, grid)) return -1;

    if(start.x == end.x && start.y == end.y)
    {
        printf("Start and end are the same point: (%d, %d)\n", start.x, start.y);
        return 0;
    }

    bool **visited;
    int **dist;
    ParentPoint **parent;

    if(!init_bfs_arrays(m, n, &visited, &dist, &parent)) return false;
    bool found = run_bfs(m, n, start, end, grid, visited, dist, parent);

    int distance = -1;

    if(found)
    {
        distance = dist[end.x][end.y];
        int path_len;
        Point *path = reconstruct_path(start, end, parent, distance, &path_len);
        print_path_and_grid(m, n, grid, path, path_len, start, end, distance);
        free(path);
    }

    free_2d_bool_array(visited, m);
    free_2d_int_array(dist, m);
    free_2d_parent_array(parent, m);

    return found ? distance : -1;
}
// Assume all your original BFS code is included here
// BFS with 8 directions (4 cardinal + 4 diagonal)
int bfs_with_path_8dir(int m, int n, Point start, Point end, int **grid)
{
    if(m <= 0 || n <= 0 || !grid) return -1;
    if(start.x < 0 || start.x >= m || start.y < 0 || start.y >= n) return -1;
    if(end.x < 0 || end.x >= m || end.y < 0 || end.y >= n) return -1;
    if(grid[start.x][start.y] == 1 || grid[end.x][end.y] == 1) return -1;

    if(start.x == end.x && start.y == end.y)
    {
        printf("Start and end are the same point: (%d, %d)\n", start.x, start.y);
        return 0;
    }

    // Allocate memory for arrays
    bool **visited = (bool **)malloc(sizeof(bool *) * m);
    int **dist = (int **)malloc(sizeof(int *) * m);
    ParentPoint **parent = (ParentPoint **)malloc(sizeof(ParentPoint *) * m);

    for(int i = 0; i < m; i++)
    {
        visited[i] = (bool *)calloc(n, sizeof(bool));
        dist[i] = (int *)malloc(sizeof(int) * n);
        parent[i] = (ParentPoint *)malloc(sizeof(ParentPoint) * n);

        if (!visited[i] || !dist[i] || !parent[i]) {
            free_2d_bool_array(visited, m);
            free_2d_int_array(dist, m);
            free_2d_parent_array(parent, m);
            return -1;
        }

        for(int j = 0; j < n; j++)
        {
            dist[i][j] = -1;
            parent[i][j] = (ParentPoint){-1, -1};
        }
    }

    Queue *q = queue_init(m * n);
    if(!q)
    {
        free_2d_bool_array(visited, m);
        free_2d_int_array(dist, m);
        free_2d_parent_array(parent, m);
        return -1;
    }

    // 8 DIRECTIONS: 4 cardinal + 4 diagonal
    // Order: N, S, E, W, NE, NW, SE, SW
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};
    
    const char* direction_names[] = {
        "North", "South", "East", "West", 
        "NorthEast", "NorthWest", "SouthEast", "SouthWest"
    };
    
    bool found = false;
    
    visited[start.x][start.y] = true;
    dist[start.x][start.y] = 0;
    queue_push(q, start);

    while(!queue_is_empty(q) && !found)
    {
        Point cur;
        queue_pop(q, &cur);
        
        // Try all 8 directions
        for(int dir = 0; dir < 8; dir++)
        {
            int new_x = cur.x + dx[dir];
            int new_y = cur.y + dy[dir];

            if(new_x >= 0 && new_x < m && new_y >= 0 && new_y < n && 
               !visited[new_x][new_y] && grid[new_x][new_y] == 0)
            {
                visited[new_x][new_y] = true;
                dist[new_x][new_y] = dist[cur.x][cur.y] + 1;
                parent[new_x][new_y] = (ParentPoint){cur.x, cur.y};
                
                if(new_x == end.x && new_y == end.y)
                {
                    found = true;
                    break;
                }

                Point next = {new_x, new_y};
                queue_push(q, next);
            }
        }
    }

    int distance = -1;
    if(found)
    {
        distance = dist[end.x][end.y];
        
        // Reconstruct path
        Point *path = (Point *)malloc(sizeof(Point) * (distance + 1));
        if(path)
        {
            int path_len = 0;
            Point cur = end;

            // Backtrack to find path
            while(!(cur.x == start.x && cur.y == start.y))
            {
                path[path_len++] = cur;
                ParentPoint par = parent[cur.x][cur.y];
                cur.x = par.x;
                cur.y = par.y;
            }
            path[path_len++] = start;

            // Draw grid with path
            char **draw = (char**)malloc(sizeof(char*) * m);
            if(draw)
            {
                for(int i = 0; i < m; i++)
                {
                    draw[i] = (char*)malloc(sizeof(char) * (n + 1));
                    if (draw[i]) {
                        for (int j = 0; j < n; j++) {
                            if (grid[i][j] == 1) 
                                draw[i][j] = '#';
                            else 
                                draw[i][j] = '.';
                        }
                        draw[i][n] = '\0';
                    }
                }

                // Mark the path
                for(int i = 0; i < path_len; i++)
                {
                    int x = path[i].x;
                    int y = path[i].y;
                    if (x >= 0 && x < m && y >= 0 && y < n) {
                        draw[x][y] = '*';
                    }
                }

                // Mark start and end
                if(draw[start.x]) draw[start.x][start.y] = 'S';
                if(draw[end.x]) draw[end.x][end.y] = 'E';
                
                printf("8-DIRECTION BFS RESULT:\n");
                printf("Shortest path length: %d steps\n", distance);
                printf("Path coordinates (from start to end):\n");
                
                // Show path with movement directions
                for (int i = path_len - 1; i >= 0; i--) {
                    printf("(%d,%d)", path[i].x, path[i].y);
                    
                    // Show direction of movement
                    if (i > 0) {
                        int prev_x = path[i].x;
                        int prev_y = path[i].y;
                        int next_x = path[i-1].x;
                        int next_y = path[i-1].y;
                        
                        int move_dx = next_x - prev_x;
                        int move_dy = next_y - prev_y;
                        
                        // Find which direction was used
                        for (int d = 0; d < 8; d++) {
                            if (dx[d] == move_dx && dy[d] == move_dy) {
                                printf(" --%s--> ", direction_names[d]);
                                break;
                            }
                        }
                    }
                }
                printf("\n\nGrid visualization:\n");
                
                for (int i = 0; i < m; i++) {
                    if (draw[i]) {
                        printf("%s\n", draw[i]);
                        free(draw[i]);
                    }
                }
                free(draw);
            }
            free(path);
        }
    }

    // Cleanup
    queue_free(q);
    free_2d_bool_array(visited, m);
    free_2d_int_array(dist, m);
    free_2d_parent_array(parent, m);

    return distance;
}


// Test result structure
typedef struct {
    int test_id;
    char test_name[100];
    int expected_4dir;
    int expected_8dir;
    int actual_4dir;
    int actual_8dir;
    bool passed;
    char notes[200];
} TestResult;

// Test statistics
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double avg_execution_time;
} TestStats;

// Helper function to create a test grid
int** create_test_grid(int m, int n, const int* pattern) {
    int** grid = malloc(sizeof(int*) * m);
    for(int i = 0; i < m; i++) {
        grid[i] = malloc(sizeof(int) * n);
        for(int j = 0; j < n; j++) {
            grid[i][j] = pattern[i * n + j];
        }
    }
    return grid;
}


// Helper function to free test grid
void free_test_grid(int** grid, int m) {
    if(grid) {
        for(int i = 0; i < m; i++) {
            free(grid[i]);
        }
        free(grid);
    }
}

// Helper function to print test grid
void print_test_grid(int** grid, int m, int n, Point start, Point end) {
    printf("Test Grid (%dx%d):\n", m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            if(i == start.x && j == start.y) 
                printf("S ");
            else if(i == end.x && j == end.y) 
                printf("E ");
            else if(grid[i][j] == 1) 
                printf("# ");
            else 
                printf(". ");
        }
        printf("\n");
    }
    printf("\n");
}

// Test Case 1: Basic path finding (simple straight line)
TestResult test_basic_straight_path() {
    TestResult result = {1, "Basic Straight Path", 3, 3, 0, 0, false, ""};
    
    int pattern[5][5] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
    
    int** grid = create_test_grid(5, 5, (int*)pattern);
    Point start = {0, 0};
    Point end = {0, 3};
    
    printf("Test 1: Basic Straight Path\n");
    print_test_grid(grid, 5, 5, start, end);
    
    result.actual_4dir = bfs_with_path(5, 5, start, end, grid);
    result.actual_8dir = bfs_with_path_8dir_new(5, 5, start, end, grid);
    
    result.passed = (result.actual_4dir == result.expected_4dir && 
                    result.actual_8dir == result.expected_8dir);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Expected 4dir=%d, 8dir=%d; Got 4dir=%d, 8dir=%d", 
                result.expected_4dir, result.expected_8dir,
                result.actual_4dir, result.actual_8dir);
    }
    
    free_test_grid(grid, 5);
    return result;
}

// Test Case 2: Diagonal advantage test
TestResult test_diagonal_advantage() {
    TestResult result = {2, "Diagonal Advantage", 6, 3, 0, 0, false, ""};
    
    int pattern[] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };
    
    int** grid = create_test_grid(5, 5, (int*)pattern);
    Point start = {0, 0};
    Point end = {3, 3}; // Diagonal movement should be shorter
    
    printf("Test 2: Diagonal Advantage\n");
    print_test_grid(grid, 5, 5, start, end);
    
    result.actual_4dir = bfs_with_path(5, 5, start, end, grid);
    result.actual_8dir = bfs_with_path_8dir_new(5, 5, start, end, grid);
    
    result.passed = (result.actual_4dir == result.expected_4dir && 
                    result.actual_8dir == result.expected_8dir);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Expected 4dir=%d, 8dir=%d; Got 4dir=%d, 8dir=%d", 
                result.expected_4dir, result.expected_8dir,
                result.actual_4dir, result.actual_8dir);
    }
    
    free_test_grid(grid, 5);
    return result;
}

// Test Case 3: No path available
TestResult test_no_path() {
    TestResult result = {3, "No Path Available", -1, -1, 0, 0, false, ""};
    
    int pattern[5][5] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0}
    };
    
    int** grid = create_test_grid(5, 5, (int*)pattern);
    Point start = {0, 0};
    Point end = {0, 4}; // Blocked by wall
    
    printf("Test 3: No Path Available\n");
    print_test_grid(grid, 5, 5, start, end);
    
    result.actual_4dir = bfs_with_path(5, 5, start, end, grid);
    result.actual_8dir = bfs_with_path_8dir_new(5, 5, start, end, grid);
    
    result.passed = (result.actual_4dir == result.expected_4dir && 
                    result.actual_8dir == result.expected_8dir);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Expected 4dir=%d, 8dir=%d; Got 4dir=%d, 8dir=%d", 
                result.expected_4dir, result.expected_8dir,
                result.actual_4dir, result.actual_8dir);
    }
    
    free_test_grid(grid, 5);
    return result;
}

// Test Case 4: Same start and end point
TestResult test_same_point() {
    TestResult result = {4, "Same Start and End", 0, 0, 0, 0, false, ""};
    
    int pattern[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    
    int** grid = create_test_grid(3, 3, (int*)pattern);
    Point start = {1, 1};
    Point end = {1, 1};
    
    printf("Test 4: Same Start and End Point\n");
    print_test_grid(grid, 3, 3, start, end);
    
    result.actual_4dir = bfs_with_path(3, 3, start, end, grid);
    result.actual_8dir = bfs_with_path_8dir_new(3, 3, start, end, grid);
    
    result.passed = (result.actual_4dir == result.expected_4dir && 
                    result.actual_8dir == result.expected_8dir);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Expected 4dir=%d, 8dir=%d; Got 4dir=%d, 8dir=%d", 
                result.expected_4dir, result.expected_8dir,
                result.actual_4dir, result.actual_8dir);
    }
    
    free_test_grid(grid, 3);
    return result;
}

// Test Case 5: Maze with narrow passages
TestResult test_narrow_maze() {
    TestResult result = {5, "Narrow Maze", 14, 9, 0, 0, false, ""};
    
    int pattern[7][7] = {
        {0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0},
        {0, 0, 0, 1, 0, 0, 0},
        {1, 1, 0, 1, 0, 1, 1},
        {0, 0, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 1, 0}
    };
    
    int** grid = create_test_grid(7, 7, (int*)pattern);
    Point start = {0, 0};
    Point end = {6, 6};
    
    printf("Test 5: Narrow Maze\n");
    print_test_grid(grid, 7, 7, start, end);
    
    result.actual_4dir = bfs_with_path(7, 7, start, end, grid);
    result.actual_8dir = bfs_with_path_8dir_new(7, 7, start, end, grid);
    
    result.passed = (result.actual_4dir == result.expected_4dir && 
                    result.actual_8dir == result.expected_8dir);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Expected 4dir=%d, 8dir=%d; Got 4dir=%d, 8dir=%d", 
                result.expected_4dir, result.expected_8dir,
                result.actual_4dir, result.actual_8dir);
    }
    
    free_test_grid(grid, 7);
    return result;
}

// Test Case 6: Invalid inputs
TestResult test_invalid_inputs() {
    TestResult result = {6, "Invalid Inputs", -1, -1, 0, 0, false, ""};
    
    printf("Test 6: Invalid Inputs\n");
    
    // Test with NULL grid
    Point start = {0, 0};
    Point end = {1, 1};
    
    result.actual_4dir = bfs_with_path(5, 5, start, end, NULL);
    result.actual_8dir = bfs_with_path_8dir_new(5, 5, start, end, NULL);
    
    // Test with invalid dimensions
    int pattern[2][2] = {{0, 0}, {0, 0}};
    int** grid = create_test_grid(2, 2, (int*)pattern);
    
    Point invalid_start = {-1, 0};
    Point invalid_end = {5, 5};
    
    int result_4dir_2 = bfs_with_path(2, 2, invalid_start, invalid_end, grid);
    int result_8dir_2 = bfs_with_path_8dir_new(2, 2, invalid_start, invalid_end, grid);

    result.passed = (result.actual_4dir == -1 && result.actual_8dir == -1 &&
                    result_4dir_2 == -1 && result_8dir_2 == -1);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Invalid input tests failed - should return -1 for all cases");
    }
    
    free_test_grid(grid, 2);
    return result;
}

// Test Case 7: Large grid performance test
TestResult test_large_grid() {
    TestResult result = {7, "Large Grid Performance", -1, -1, 0, 0, false, ""};
    
    printf("Test 7: Large Grid Performance Test (20x20)\n");
    
    // Create a 20x20 grid with some obstacles
    int** grid = (int**)malloc(sizeof(int*) * 20);
    for(int i = 0; i < 20; i++) {
        grid[i] = (int*)malloc(sizeof(int) * 20);
        for(int j = 0; j < 20; j++) {
            // Create some random obstacles (about 20% of the grid)
            grid[i][j] = ((i + j) % 5 == 0 && i != 0 && j != 0 && 
                         i != 19 && j != 19) ? 1 : 0;
        }
    }
    
    Point start = {0, 0};
    Point end = {19, 19};
    
    clock_t start_time = clock();
    result.actual_4dir = bfs_with_path(20, 20, start, end, grid);
    clock_t mid_time = clock();
    result.actual_8dir = bfs_with_path_8dir_new(20, 20, start, end, grid);
    clock_t end_time = clock();
    
    double time_4dir = ((double)(mid_time - start_time)) / CLOCKS_PER_SEC;
    double time_8dir = ((double)(end_time - mid_time)) / CLOCKS_PER_SEC;
    
    result.passed = (result.actual_4dir > 0 && result.actual_8dir > 0);
    
    snprintf(result.notes, sizeof(result.notes), 
            "4dir: %d steps (%.4fs), 8dir: %d steps (%.4fs)", 
            result.actual_4dir, time_4dir, result.actual_8dir, time_8dir);
    
    free_test_grid(grid, 20);
    return result;
}

// Test Case 8: Edge cases - start/end at obstacles
TestResult test_obstacle_positions() {
    TestResult result = {8, "Start/End at Obstacles", -1, -1, 0, 0, false, ""};
    
    int pattern[4][4] = {
        {1, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 1}
    };
    
    int** grid = create_test_grid(4, 4, (int*)pattern);
    Point start = {0, 0}; // Start at obstacle
    Point end = {3, 3};   // End at obstacle
    
    printf("Test 8: Start/End at Obstacles\n");
    print_test_grid(grid, 4, 4, start, end);
    
    result.actual_4dir = bfs_with_path(4, 4, start, end, grid);
    result.actual_8dir = bfs_with_path_8dir_new(4, 4, start, end, grid);

    result.passed = (result.actual_4dir == result.expected_4dir && 
                    result.actual_8dir == result.expected_8dir);
    
    if(!result.passed) {
        snprintf(result.notes, sizeof(result.notes), 
                "Expected 4dir=%d, 8dir=%d; Got 4dir=%d, 8dir=%d", 
                result.expected_4dir, result.expected_8dir,
                result.actual_4dir, result.actual_8dir);
    }
    
    free_test_grid(grid, 4);
    return result;
}

// Main test runner function
void run_comprehensive_bfs_tests() {
    printf("COMPREHENSIVE BFS TEST SUITE\n");
    printf("================================\n\n");
    
    TestResult tests[8];
    TestStats stats = {0, 0, 0, 0.0};
    
    clock_t total_start = clock();
    
    // Run all tests
    tests[0] = test_basic_straight_path();
    tests[1] = test_diagonal_advantage();
    tests[2] = test_no_path();
    tests[3] = test_same_point();
    tests[4] = test_narrow_maze();
    tests[5] = test_invalid_inputs();
    tests[6] = test_large_grid();
    tests[7] = test_obstacle_positions();
    
    clock_t total_end = clock();
    
    // Calculate statistics
    stats.total_tests = 8;
    for(int i = 0; i < 8; i++) {
        if(tests[i].passed) {
            stats.passed_tests++;
        } else {
            stats.failed_tests++;
        }
    }
    stats.avg_execution_time = ((double)(total_end - total_start)) / CLOCKS_PER_SEC;
    
    // Print results summary
    printf("\nTEST RESULTS SUMMARY\n");
    printf("=======================\n");
    printf("Passed: %d \n", stats.passed_tests);
    printf("Failed: %d \n", stats.failed_tests);
    printf("Success Rate: %.1f%%\n", 
           (float)stats.passed_tests / stats.total_tests * 100);
    printf("Total Execution Time: %.4f seconds\n", stats.avg_execution_time);
    
    printf("\n DETAILED TEST RESULTS\n");
    printf("========================\n");
    
    for(int i = 0; i < 8; i++) {
        printf("Test %d: %s - %s\n", 
               tests[i].test_id, 
               tests[i].test_name, 
               tests[i].passed ? "PASSED" : "FAILED");
        
        printf("  4-dir result: %d, 8-dir result: %d\n", 
               tests[i].actual_4dir, tests[i].actual_8dir);
        
        if(!tests[i].passed && strlen(tests[i].notes) > 0) {
            printf("  Notes: %s\n", tests[i].notes);
        }
        
        if(strlen(tests[i].notes) > 0 && tests[i].passed) {
            printf("  Info: %s\n", tests[i].notes);
        }
        
        printf("\n");
    }
    
    // Performance comparison
    printf("PERFORMANCE ANALYSIS\n");
    printf("=======================\n");
    int total_4dir_advantage = 0;
    int total_8dir_advantage = 0;
    int equal_results = 0;
    
    for(int i = 0; i < 8; i++) {
        if(tests[i].actual_4dir > 0 && tests[i].actual_8dir > 0) {
            if(tests[i].actual_4dir > tests[i].actual_8dir) {
                total_8dir_advantage++;
            } else if(tests[i].actual_4dir < tests[i].actual_8dir) {
                total_4dir_advantage++;
            } else {
                equal_results++;
            }
        }
    }
    
    printf("8-direction found shorter paths: %d times\n", total_8dir_advantage);
    printf("4-direction found shorter paths: %d times\n", total_4dir_advantage);
    printf("Equal path lengths: %d times\n", equal_results);
    
    if(stats.failed_tests == 0) {
        printf("\nALL TESTS PASSED! Your BFS implementation is working correctly.\n");
    } else {
        printf("\nSome tests failed. Please review the implementation.\n");
    }
}

// Function to run a specific comparison test
void run_comparison_test() {
    printf("\nRUNNING COMPARISON TEST\n");
    printf("==========================\n");
    
    int pattern[8][8] = {
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 1, 1, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    
    int** grid = create_test_grid(8, 8, (int*)pattern);
    Point start = {0, 0};
    Point end = {7, 7};
    
    free_test_grid(grid, 8);
}

// Main function for testing
int main() {
    printf("BFS PATHFINDING TEST SUITE\n");
    printf("==========================\n\n");
    
    // Run comprehensive tests
    run_comprehensive_bfs_tests();
    
    // Run comparison test
    run_comparison_test();
    
    return 0;
}