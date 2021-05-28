#include <stdio.h>
#include <limits.h>
#include <math.h>

int hereustic(int x, int y, int x_end, int y_end) {
    //Using manhattan distance
    return abs(x - x_end) + abs(y - y_end);
}

void getLowestFScore(int prioQueue[30][2], int front, int end, int fScore[5][6], int *x, int *y, int *index) {
    int lowestFScore = INT_MAX, lowestIndex = 0;

    //find the lowest f score of the nodes from the prioQueue
    for(int i = front; i < end; i++) {
        if(fScore[prioQueue[i][0]][prioQueue[i][1]] < lowestFScore) {
            lowestIndex = i;
            lowestFScore = fScore[prioQueue[i][0]][prioQueue[i][1]];
        }
    }

    //returning the index and x, y of the nodes.
    (*x) = prioQueue[lowestIndex][0];
    (*y) = prioQueue[lowestIndex][1];
    (*index) = lowestIndex;
}

void removePrioQueue(int prioQueue[30][2], int *front, int *end, int index) {

    //Removing from the prioQueue by shifting to the left all the elements
    for(int i = index; i < (*end) - 1; i++) {
        prioQueue[i][0] = prioQueue[i + 1][0];
        prioQueue[i][1] = prioQueue[i + 1][1];
    }

    (*end)--;
}

//Node is valid if x, y are inside table bounds (0-4, 0-5) and is not a wall (table[x][y] != 1).
int checkValid(int table[5][6], int *x, int *y) {
    if((*x) < 0 || (*x) >= 5 || (*y) < 0 || (*y) >= 6) {
        (*x) = -1;
        (*y) = -1;

        return 0;
    } else if(table[(*x)][(*y)] == 1) {
        (*x) = -1;
        (*y) = -1;

        return 0;
    }

    return 1;
}

//Check if given node (x, y) is in the prioQueue
int isInPrioQueue(int x, int y, int prioQueue[30][2], int front, int end) {
    for(int i = front; i < end; i++)
        if(prioQueue[i][0] == x && prioQueue[i][1] == y)
            return 1;

    return 0;
} 

//Add a given node to the end of the prioQueue
void addToPrioQueue(int x, int y, int prioQueue[30][2], int *front, int *end) {
    prioQueue[(*end)][0] = x;
    prioQueue[(*end)][1] = y;

    (*end)++;
}

void calculateNeighbour(int table[5][6], int gScore[5][6], int fScore[5][6], 
                        int prioQueue[30][2], int *front, int *end, int cameFrom[5][6][2],
                        int curr_x, int curr_y, int x, int y, int x_end, int y_end) {
    checkValid(table, &x, &y);

    if(x == -1) return; //Check if neighbour is valid

    int tentativeGScore = gScore[curr_x][curr_y] + 1;

    //printf("\nChecking neighbour (%d %d). Tentative gScore: %d, actual gScore: %d ", x, y, tentativeGScore, gScore[x][y]);

    if(tentativeGScore < gScore[x][y]) {
        //printf("| Swapping.\nCame from cell: (%d %d)", curr_x, curr_y);
        cameFrom[x][y][0] = curr_x;
        cameFrom[x][y][1] = curr_y;

        gScore[x][y] = tentativeGScore;
        fScore[x][y] = tentativeGScore + hereustic(x, y, x_end, y_end);

        if(!isInPrioQueue(x, y, prioQueue, (*front), (*end))) {
            //printf(" | Adding to Prio Queue.");
            addToPrioQueue(x, y, prioQueue, front, end);
        }
    }
}

//Reconstruct the path once found, by walking back through cameFrom (First printed will be the end node, and last will be start node)
void reconstructPath(int table[5][6], int cameFrom[5][6][2], int x, int y, int end_x, int end_y) {
    while(checkValid(table, &cameFrom[x][y][0], &cameFrom[x][y][1])) {
        printf("(%d %d) <- ", x, y);
        x = cameFrom[x][y][0];
        y = cameFrom[x][y][1];
    }

    printf("(%d %d)", end_x, end_y);
}

void aSearch(int table[5][6], int x_start, int y_start, int x_end, int y_end) {
    if(x_start == x_end && y_start == y_end) {
        printf("The start is the end!"); //Inexistent path (start_node = end_node).
        return;
    }

    int prioQueue[30][2], queueFront = 0, queueEnd = 0; 
    prioQueue[0][0] = x_start; prioQueue[0][1] = y_start; queueEnd++; //Add the start node to the prioQueue

    int cameFrom[5][6][2];
    int gScore[5][6], fScore[5][6];
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 6; j++) {
            cameFrom[i][j][0] = cameFrom[i][j][1] = -1;
            gScore[i][j] = fScore[i][j] = INT_MAX; //Initiate gScore and fScore with Infinity
        }
    }

    gScore[x_start][y_start] = 0;
    fScore[x_start][y_start] = hereustic(x_start, y_start, x_end, y_end); //Calculate the fScore and gScore of the start node

    while(queueFront != queueEnd) { //loop trough all the nodes in the prioQueue
        int x_current, y_current, index_current;
        getLowestFScore(prioQueue, queueFront, queueEnd, fScore, &x_current, &y_current, &index_current); //Function will return
        //the node with the lowest fScore

        //printf("\nChecking cell: (%d %d). | Front: %d End: %d", x_current, y_current, queueFront, queueEnd);

        if(x_current == x_end && y_current == y_end) { //current_node == end_node, we found our road, let's walk back now
            printf("\nFound road.\n");
            reconstructPath(table, cameFrom, x_current, y_current, x_start, y_start);
            return;
        }

        removePrioQueue(prioQueue, &queueFront, &queueEnd, index_current); //Remove the current node from the prioQueue


        //Check the neighbours of our node
        calculateNeighbour(table, gScore, fScore, prioQueue, &queueFront, &queueEnd, cameFrom, x_current, y_current, x_current + 1, y_current, x_end, y_end);
        calculateNeighbour(table, gScore, fScore, prioQueue, &queueFront, &queueEnd, cameFrom, x_current, y_current, x_current - 1, y_current, x_end, y_end);
        calculateNeighbour(table, gScore, fScore, prioQueue, &queueFront, &queueEnd, cameFrom, x_current, y_current, x_current, y_current + 1, x_end, y_end);
        calculateNeighbour(table, gScore, fScore, prioQueue, &queueFront, &queueEnd, cameFrom, x_current, y_current, x_current, y_current - 1, x_end, y_end);
    }

    printf("Can't find a road"); //Impossible to find a road
}

void createTable(int table[5][6]) {
    table[0][1] = 1;    table[2][3] = 1;
    table[2][1] = 1;    table[3][4] = 1;
    table[3][1] = 1;    table[4][4] = 1;
}

int main() {
    int table[5][6] = {0};

    createTable(table);
    aSearch(table, 0, 0, 4, 5);

    return 0;
}