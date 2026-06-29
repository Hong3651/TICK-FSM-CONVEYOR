#include <stdio.h>      // printf, scanf 등 입출력 도구
#include <string.h>     // strcmp (글자 비교) 도구

/* ── 상태 목록 (enum) ──
 * 장비는 항상 이 8개 중 한 상태에 있다. (Day 2부터 전환 로직 추가)
 * 지금은 IDLE만 사용. */
typedef enum {
    STATE_IDLE,
    STATE_DETECT,
    STATE_TRANSFER,
    STATE_SETTLING,
    STATE_SORT,
    STATE_COMPLETE,
    STATE_ERROR,
    STATE_EMERGENCY
} ConveyorState;

/* ── 장비 한 대 (struct) ──
 * 장비의 모든 정보를 한 덩어리(봉투)로 묶는다. */
typedef struct {
    ConveyorState state;   // 현재 상태
    int motor;             // 모터: 켜짐 1 / 꺼짐 0
    int error_code;        // 에러 코드: 0 = 정상
} Conveyor;

/* ── 상태 번호를 읽기 좋은 글자로 바꿔주는 함수 ──
 * status 출력 때 "0" 대신 "IDLE"로 보이게 하려는 용도. */
const char *state_name(ConveyorState s) {
    switch (s) {
        case STATE_IDLE:      return "IDLE";
        case STATE_DETECT:    return "DETECT";
        case STATE_TRANSFER:  return "TRANSFER";
        case STATE_SETTLING:  return "SETTLING";
        case STATE_SORT:      return "SORT";
        case STATE_COMPLETE:  return "COMPLETE";
        case STATE_ERROR:     return "ERROR";
        case STATE_EMERGENCY: return "EMERGENCY";
        default:              return "UNKNOWN";
    }
}

/* ── 현재 장비 상태를 출력하는 함수 ──
 * 원본을 고치진 않고 읽기만 하지만, 큰 struct 복사를 피하려고
 * 포인터로 받는다. (제어 코드의 일반적인 습관) */
void print_status(const Conveyor *c) {
    printf("[STATUS] state=%s, motor=%d, error_code=%d\n",
           state_name(c->state), c->motor, c->error_code);
}

int main(void) {
    Conveyor c;            // 장비 봉투 하나 생성

    // ── 초기화: 변수는 만들면 꼭 초기값을 넣는다 (쓰레기값 방지) ──
    c.state = STATE_IDLE;
    c.motor = 0;
    c.error_code = 0;

    char cmd[20];          // 사용자가 친 명령을 담을 글자 상자

    printf("=== Conveyor Control Simulator (Day 1) ===\n");
    printf("commands: status, exit\n");

    // ── 명령 입력 루프 ──
    while (1) {                       // 무한 반복 (exit 만나면 빠져나감)
        printf("> ");                 // 프롬프트 출력
        if (scanf("%19s", cmd) != 1)  // 단어 하나 입력받기 (최대 19글자)
            break;                    // 입력이 끊기면(Ctrl+Z 등) 종료

        if (strcmp(cmd, "status") == 0) {
            print_status(&c);         // 원본 주소를 넘김 → 함수가 c를 읽음
        } else if (strcmp(cmd, "exit") == 0) {
            printf("bye.\n");
            break;                    // 루프 탈출 → 프로그램 종료
        } else {
            printf("unknown command: %s\n", cmd);
        }
    }

    return 0;
}
