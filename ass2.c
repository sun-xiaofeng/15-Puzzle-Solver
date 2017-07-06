#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 4
#define MAX_DIRECTIONS 4
#define INFINITY 1000


typedef enum {
	UP = 1, 
	DOWN, 
	LEFT, 
	RIGHT
} Direction;

typedef struct {
	int state[SIZE][SIZE];
	int cost;
	double evaluation;
	Direction previousMove;
} Node;

typedef struct {
	int x;
	int y;
} Point;


int   findHeuristic(int[][SIZE]);
int   findManhattanDistance(Point, Point);
int   findPossibleMoves(int[][SIZE], Direction[], Point);
void  readData(FILE *, int[][SIZE]);
void  printPuzzle(int[][SIZE]);
void  solvePuzzle(int[][SIZE]);
void  swap(int *, int *);
Point createPoint(int, int);
Point findBlankSpace(int[][SIZE]);
Node  createNode(int[][SIZE]);
bool  search(int[][SIZE]);
bool  ida(Node *, int, int *, long *, long *);
Node  moveTile(int[][SIZE], Direction, Point);
Direction getInverseDirection(Direction);

void readData(FILE *fp, int state[][SIZE]) {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (fscanf(fp, "%d", &state[i][j]) != 1) {
				printf("File reading error.");
				exit(EXIT_FAILURE);
			}
		}
	}
}

int findHeuristic(int state[][SIZE]) {
	int i, j;
	int heuristic = 0;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (state[i][j] == 0)
				continue;
			Point curr = createPoint(j, i);
			Point dest = createPoint(state[i][j] % SIZE, state[i][j] / SIZE);
			heuristic += findManhattanDistance(curr, dest);
		}
	}
	return heuristic;
}

Point createPoint(int x, int y) {
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

Point findBlankSpace(int state[][SIZE]) {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (state[i][j] == 0) {
				return createPoint(j, i);
			}
		}
	}
	printf("Error: Blank space not found.\n");
	return createPoint(0, 0);
}

int findManhattanDistance(Point first, Point second) {
	return abs(first.x - second.x) + abs(first.y - second.y);
}

Node createNode(int state[][SIZE]) {
	Node node;
	node.cost = 0;
	node.evaluation = 0;
	node.previousMove = 0;
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			node.state[i][j] = state[i][j];
		}
	}
	return node;
}

bool search(int state[][SIZE]) {
	int threshold = findHeuristic(state);
	long generatedNode = 0L;
	long expandedNode = 0L;

	bool found = false;
	Node initialState = createNode(state);
	initialState.evaluation = threshold;

	printf("Initial State: \n");
	printPuzzle(state);
	printf("Initial Estimate = %d\n", threshold);
	printf("Threshold = ");

	int start = clock();

	do {
		printf("%d ", threshold);
		int newThreshold = INFINITY;
		found = ida(&initialState, threshold, &newThreshold, 
			&generatedNode, &expandedNode);
		if (!found) {
			threshold = newThreshold;
		}
	} while (!found);

	double t = 1.0 * (clock() - start) / CLOCKS_PER_SEC;

	printf("Generated = %ld\n"
		   "Expanded = %ld\n"
		   "Time = %.2f\n"
		   "Expanded/Second = %d\n",
		    generatedNode, expandedNode, t, (int) (expandedNode / t));

	return found;
}

bool ida(Node *current, int threshold, int *newThreshold, 
	long *generatedNode, long *expandedNode) {

	Direction directions[MAX_DIRECTIONS];
	Point nextMove = findBlankSpace(current->state);
	
	int numDirecions = findPossibleMoves(current->state, directions, nextMove);
	int i;
	for (i = 0; i < numDirecions; i++) {
		if (getInverseDirection(directions[i]) != current->previousMove) {
			*generatedNode += 1;
			Node nextState = moveTile(current->state, directions[i], nextMove);
			int heuristic = findHeuristic(nextState.state);
			nextState.cost = current->cost + 1;
			nextState.evaluation = nextState.cost + heuristic;
			if (nextState.evaluation > threshold) {
				if (nextState.evaluation < *newThreshold) {
					*newThreshold = nextState.evaluation;
				}
			} else {
				*expandedNode += 1;
				if (heuristic == 0) {
					printf("\nSolution = %d\n", nextState.cost);
					return true;
				}
				bool found = ida(&nextState, threshold, newThreshold, 
					generatedNode, expandedNode);
				if (found) {
					return true;
				}
			}
		}
	}
	return false;
}

int findPossibleMoves(int state[][SIZE], Direction directions[], Point position) {
	int i = 0;
	if (position.y > 0) {
		directions[i++] = UP;
	}
	if (position.y < SIZE - 1) {
		directions[i++] = DOWN;
	}
	if (position.x > 0) {
		directions[i++] = LEFT;
	}
	if (position.x < SIZE - 1) {
		directions[i++] = RIGHT;
	}
	return i;
}

Node moveTile(int state[][SIZE], Direction direction, Point start) {
	Node nextState = createNode(state);
	Point dest = start;
	switch (direction) {
		case UP:
			--dest.y;
			nextState.previousMove = UP;
			break;
		case DOWN:
			++dest.y;
			nextState.previousMove = DOWN;
			break;
		case LEFT:
			--dest.x;
			nextState.previousMove = LEFT;
			break;
		case RIGHT:
			++dest.x;
			nextState.previousMove = RIGHT;
			break;
		default:
			printf("Error: Invalid direction.");
			break;
	}
	swap(&nextState.state[start.y][start.x], &nextState.state[dest.y][dest.x]);
	return nextState;
}

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

Direction getInverseDirection(Direction direction) {
	switch (direction) {
		case UP:
			return DOWN;
		case DOWN:
			return UP;
		case LEFT:
			return RIGHT;
		case RIGHT:
			return LEFT;
		default:
			return 0;
	}
}

void solvePuzzle(int state[][SIZE]) {
	search(state);
}

void printPuzzle(int state[][SIZE]) {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			printf("%d ", state[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s datafile\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		printf("Error: Cannot open the file.\n");
		exit(EXIT_FAILURE);
	}

	int initialState[SIZE][SIZE];
	readData(fp, initialState);
	fclose(fp);

	solvePuzzle(initialState);

	return 0;
}

