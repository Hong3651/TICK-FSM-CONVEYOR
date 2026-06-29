#include <stdio.h>

typedef enum { STATE_IDLE, STATE_TRANSFER, STATE_SORT, STATE_ERROR } ConveyorState;

int main(void) {
    ConveyorState current = STATE_SORT;

    printf("STATE_IDLE=%d\n", STATE_IDLE);
    printf("STATE_SORT=%d\n", STATE_SORT);
    printf("current=%d\n", current);

    return 0;
}
