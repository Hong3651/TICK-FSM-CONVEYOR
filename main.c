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
//--- 입력(센서/버튼)-----------
    int product_sensor;   //입구제품감지 : 있음 1 /없음 0
    int start_button;     //시작버튼 : 눌림 1 / 안눌림 0    


} Conveyor;

/* ── 상태 번호를 읽기 좋은 글자로 바꿔주는 함수 ──
 * status 출력 때 "0" 대신 "IDLE"로 보이게 하려는 용도. */
const char *state_name(ConveyorState s) {
    switch (s) {
        case STATE_IDLE:      return "IDLE";       // 대기 — 제품 투입을 기다림
        case STATE_DETECT:    return "DETECT";     // 감지 — 입구 센서에 제품이 잡힘
        case STATE_TRANSFER:  return "TRANSFER";   // 이송 — 벨트(모터)가 제품을 옮기는 중
        case STATE_SETTLING:  return "SETTLING";   // 안정화 — 위치 도착 후 멈춰 자리잡길 대기
        case STATE_SORT:      return "SORT";       // 분류 — 실린더가 제품을 옆으로 밂
        case STATE_COMPLETE:  return "COMPLETE";   // 완료 — 분류 끝
        case STATE_ERROR:     return "ERROR";      // 오류 — 문제 발생 (reset 전까지 잠금)
        case STATE_EMERGENCY: return "EMERGENCY";  // 비상정지 — 모든 출력 OFF
        default:              return "UNKNOWN";    // 알 수 없음 (예외 안전장치)
    }
}

/* ── 현재 장비 상태를 출력하는 함수 ──
 * 원본을 고치진 않고 읽기만 하지만, 큰 struct 복사를 피하려고
 * 포인터로 받는다. (제어 코드의 일반적인 습관) */
void print_status(const Conveyor *c) {
    printf("[STATUS] state=%s, motor=%d, error_code=%d\n",
           state_name(c->state), c->motor, c->error_code);
}

/* ── 스캔 사이클 1단계: 입력 읽기 ──
 * 실제 PLC라면 여기서 센서/버튼 하드웨어를 읽는다.
 * 우리는 시뮬레이터라 명령(product_on 등)으로 미리 켜둔 입력을 쓴다.
 * 지금은 구조만 — 읽었다고 알리기만 한다. */
void read_inputs(Conveyor *c) {
    printf("  [read]  product_sensor=%d, start_button=%d\n",
           c->product_sensor, c->start_button);
}

/* ── 스캔 사이클 2단계: 판단(상태머신) ──
 * 현재 상태 + 입력을 합쳐 "다음 상태"를 결정한다.
 * 이 프로젝트의 두뇌. (로직은 C단계에서 채움) */
void run_state_machine(Conveyor *c) {
    switch (c->state) {              // 지금 상태가 뭐냐에 따라 갈래길

        case STATE_IDLE:            // 지금 IDLE(대기)이면
            if (c->product_sensor == 1) {    // 제품이 감지됐다면
                c->state = STATE_DETECT;     // → DETECT로 상태 변경
                printf("  [judge] 제품 감지 -> DETECT\n");
            }
            break;                  // 이 갈래 끝 (아래 case로 흘러내리지 않게)

        case STATE_DETECT:          // 지금 DETECT(감지)이면
            c->state = STATE_TRANSFER;       // → 조건 없이 바로 TRANSFER
            printf("  [judge] 이송 시작 -> TRANSFER\n");
            break;

        default:                    // 그 외 상태 (아직 로직 없음)
            printf("  [judge] (%s: 대기 중)\n", state_name(c->state));
            break;
    }
}

/* ── 스캔 사이클 3단계: 출력 쓰기 ──
 * 판단 결과대로 모터/실린더 등을 실제로 켜고 끈다. */
void write_outputs(Conveyor *c) {
    // 출력은 '현재 상태'를 보고 결정한다 (출력 = 상태의 함수)
    if (c->state == STATE_TRANSFER) {
        c->motor = 1;               // 이송 중 → 벨트 모터 ON
    } else {
        c->motor = 0;               // 그 외 → 모터 OFF
    }
    printf("  [write] motor=%d\n", c->motor);
}

/* ── 1 tick = 스캔 사이클 한 바퀴 ──
 * read → judge → write 를 항상 이 순서로 딱 한 번씩. */
void run_tick(Conveyor *c) {
    printf("--- tick ---\n");
    read_inputs(c);
    run_state_machine(c);
    write_outputs(c);
}

int main(void) {
    Conveyor c;            // 장비 봉투 하나 생성

    // ── 초기화: 변수는 만들면 꼭 초기값을 넣는다 (쓰레기값 방지) ──
    c.state = STATE_IDLE;
    c.motor = 0;
    c.error_code = 0;
    c.product_sensor = 0;
    c.start_button = 0;
    char cmd[20];          // 사용자가 친 명령을 담을 글자 상자

    printf("=== Conveyor Control Simulator (Day 2) ===\n");
    printf("commands: product_on, tick, status, exit\n");

    // ── 명령 입력 루프 ──
    while (1) {                       // 무한 반복 (exit 만나면 빠져나감)
        printf("> ");                 // 프롬프트 출력
        if (scanf("%19s", cmd) != 1)  // 단어 하나 입력받기 (최대 19글자)
            break;                    // 입력이 끊기면(Ctrl+Z 등) 종료

        if (strcmp(cmd, "status") == 0) {
            print_status(&c);         // 원본 주소를 넘김 → 함수가 c를 읽음
        } else if (strcmp(cmd, "product_on") == 0) {
            c.product_sensor = 1;     // 입구에 제품 투입 = 센서 ON
            printf("제품 투입됨 (product_sensor=1)\n");
        } else if (strcmp(cmd, "tick") == 0) {
            run_tick(&c);             // 스캔 사이클 한 바퀴 실행
        } else if (strcmp(cmd, "exit") == 0) {
            printf("bye.\n");
            break;                    // 루프 탈출 → 프로그램 종료
        } else {
            printf("unknown command: %s\n", cmd);
        }
    }

    return 0;
}
