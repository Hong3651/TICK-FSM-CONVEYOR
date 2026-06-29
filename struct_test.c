#include <stdio.h>
typedef enum { STATE_IDLE, STATE_TRANSFER, STATE_SORT, STATE_ERROR } ConveyorState;

typedef struct {
    ConveyorState state;
    int motor;
    int error_code;
} Conveyor;


void print_hello() {
    printf("Hello dino~\n");
};

void turn_on_motor(Conveyor *c) {
    c->motor = 1;
}

int main(void) {
    Conveyor c;

    c.state = STATE_IDLE;
    turn_on_motor(&c);
    
    c.error_code = 0;
    
    print_hello();
    printf("state=%d, motor=%d, error_code=%d\n", c.state, c.motor, c.error_code);

    return 0;
}