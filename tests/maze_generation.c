#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>

#define DEFAULT_SIZE 10

// definitions for the cell state
#define N 0x01
#define E 0x02
#define S 0x04
#define W 0x08
#define START 0x10
#define END 0x20
#define VISITED 0x40
// final bit is unused

typedef struct cell{
    unsigned char state;

    unsigned int x_pos, y_pos;

    struct cell *n, *e, *s, *w;
} cell;

unsigned char cell_value(cell *c);

wchar_t print_cell(cell *c);

void print_maze(cell **maze, size_t size);

void gen_maze(cell **maze, cell **stack, size_t stack_size);

cell *random_neighbor(cell **maze, cell *src);

bool visited(cell *c); 

int getopt(int argc, char * const argv[], const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char **argv){
    setlocale(LC_ALL, "en_US.UTF-8");

    size_t size = DEFAULT_SIZE;

    int c;

    while((c=getopt(argc, argv, "s:")) != -1){
        switch(c){
            case 's':
                size = atoi(optarg);
                break;
            case '?':
            default:
                abort();
        }
    }

    srand(time(NULL));

    // allocate the maze as a 2D array, size*size
    cell **maze = malloc(size * sizeof(cell*));
    for(int i = 0; i < size; i++) {
        // allocate row
        maze[i] = malloc(size * sizeof(cell));
    }
    for(int i = 0; i < size; i++) {
        // populate row cells
        for(int j = 0; j < size; j++) {
            maze[i][j].state = 0x00;
            maze[i][j].x_pos = j;
            maze[i][j].y_pos = i;
            // initialize pointers to neighbors
            maze[i][j].n = (i == 0    ? NULL : &maze[i-1][j]);
            maze[i][j].e = (j == size-1 ? NULL : &maze[i][j+1]);
            maze[i][j].s = (i == size-1 ? NULL : &maze[i+1][j]);
            maze[i][j].w = (j == 0    ? NULL : &maze[i][j-1]);

        }
    }

#ifdef DEBUG
    cell tmp;
    for(unsigned char i = 1; i <= 0x0F; i++) {
        tmp.state = i;
        printf("%lc", print_cell(&tmp));
    }
    printf("\n");
#endif

    // allocate cell stack
    cell **stack = malloc(size * size * sizeof(cell*));
    size_t stack_size = 0;

    // pick starting cell at 0,0 
    cell cur = maze[0][0];
    cur.state = START;
    // push the first cell onto the top of the stack
    stack[stack_size] = &cur;
    stack_size++;

    // generate the maze
    gen_maze(maze, stack, stack_size);

    // set opposite corner as endpoint
    maze[size-1][size-1].state += END;

    // print the generated maze
    print_maze(maze, size);

    return EXIT_SUCCESS;
}

void gen_maze(cell **maze, cell **stack, size_t stack_size) {
    cell *cur = stack[stack_size-1];
    // mark current cell as visited
    cur->state += VISITED;

    cell *neighbor = random_neighbor(maze, cur);
    if(neighbor){
        // add neighbor to stack
        stack[stack_size] = neighbor;
        stack_size++;
        // connect current cell with neighbor
        if(neighbor == cur->n){
            cur->state+=N;
            neighbor->state+=S;
        }else if(neighbor == cur->e){
            cur->state+=E;
            neighbor->state+=W;
        }else if(neighbor == cur->s){
            cur->state+=S;
            neighbor->state+=N;
        }else if(neighbor == cur->w){
            cur->state+=W;
            neighbor->state+=E;
        }
    }else if(stack_size > 0){
        // no neighbors, pop off top of stack and recurse
        stack_size--;
        stack[stack_size] = NULL;
    }

    if(!stack_size){
        // if stack is empty, exit
        return;
    }

    // recurse
    gen_maze(maze, stack, stack_size);
}

cell *random_neighbor(cell **maze, cell *src){
    size_t s = 0;
    // array of pointers to hold the neighbors
    cell **neighbors = malloc(4 * sizeof(cell*));
    // north neighbor
    if(src->n && ! visited(src->n)){
        neighbors[s] = src->n;
        s++;
    }
    // east neighbor
    if(src->e && ! visited(src->e)){
        neighbors[s] = src->e;
        s++;
    }
    // south neighbor
     if(src->s && ! visited(src->s)){ 
        neighbors[s] = src->s;
        s++;
    }
    // west neighbor
    if(src->w && ! visited(src->w) ){ 
        neighbors[s] = src->w;
        s++;
    }


    if(s){
        // random unvisited neighbor
        return neighbors[rand() % s];
    }else{
        // no unvisited neighbors
        return NULL;
    }
}

void print_maze(cell **maze, size_t size) {
    for(int i = 0; i < size+2; i++) {
        printf("-");
    }
    printf("\n");

    for(int i = 0; i < size; i++) {
        printf("|");
        for(int j = 0; j < size; j++) {
            printf("%lc", print_cell(&maze[i][j]));
        }
        printf("|\n");
    }
    
    for(int i = 0; i < size+2; i++) {
        printf("-");
    }
    printf("\n");

}

bool visited(cell *c) {
    // the 6th bit of the cell's state is the visited boolean 
    return (c->state && VISITED);
}

unsigned char cell_value(cell *c) {
    // only save the first 6 bits
    return 0x3F & c->state;
}

wchar_t print_cell(cell *c) {
    // symbol of the maze cell    
    wchar_t retval;

    switch(0x0F & c->state) {
        // empty
        case 0x00: retval = 0x0020; break;
        // N
        case N: retval = 0x2579; break;
        // E
        case E: retval = 0x257A; break;
        // NE
        case N + E: retval = 0x2517; break;
        // S
        case S: retval = 0x257B; break;
        // NS
        case N + S: retval = 0x2503; break;
        // ES
        case E + S: retval = 0x250F; break;
        // NES
        case N + E + S: retval = 0x2523; break;
        // W
        case W: retval = 0x2578; break;
        // NW
        case N + W: retval = 0x251B; break;
        // EW
        case E + W: retval = 0x2501; break;
        // NEW
        case N + E + W: retval = 0x253B; break;
        // SW
        case S + W: retval = 0x2513; break;
        // NSW
        case N + S + W: retval = 0x252B; break;
        // ESW
        case E + S + W: retval = 0x2533; break;
        // NESW
        case N + E + S + W: retval = 0x254B; break;
        // error
        default: retval = 0;
    }

    return retval;
}

