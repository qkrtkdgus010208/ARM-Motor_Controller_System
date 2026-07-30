# DC Motor Control System

> **Interrupt & Finite State Machine(FSM) 기반 DC Motor 제어 시스템**
> 

---

## Rivision History

| **No** |  **수정 사항** |
| --- | --- |
| 0.001 | Initial |
| 0.002 | 기존 사용 함수 목록 추가 |

---

# 0. 목차

1.  **프로젝트 개요**  
1.1 프로젝트 소개  
1.2 개발 목적  
1.3 개발 환경

2.  **하드웨어 구성**

3.  **FSM 설계**  
3.1 상태 전이도  
3.2 UART2 Command

4.  **인터럽트 처리 흐름**  
4.1 버튼 제어  
4.2 UART Interrupt 제어

5.  **전역 변수**  
5.1 type 선언  
5.2 상태 변수  
5.3 플래그 변수  
5.4 데이터 변수

6.  **함수 설계**  
6.1 Interrupt  
6.2 handler  
6.3 state manager 함수  
6.4 Controller 함수

7.  **함수 목록**  
7.1 handler.c  
7.2 key.c  
7.3 motor.c  
7.4 timer.c  
7.5 uart.c

---

# 1. 프로젝트 개요

## 1.1 프로젝트 소개

본 프로젝트는 **STM32 마이크로컨트롤러**를 이용하여 DC Motor를 제어하는 시스템

기존 Polling 방식이 아닌 **Interrupt 기반 이벤트 처리 방식**을 적용하여 CPU 사용률을 줄이고, 버튼 입력에 대한 빠른 응답성을 확보함

또한 FSM(Finite State Machine)을 적용하여 모터의 동작을 상태(State) 단위로 관리함으로써 유지보수성과 확장성을 높힘

## 1.2 개발 목적

- Interrupt 기반 Event 처리 구현
- FSM(State Machine) 설계
- PWM을 이용한 DC Motor 속도 제어
- Button Short / Long Press 구분
- UART를 이용한 디버깅
- 코드 모듈화

## 1.3 개발 환경

| 항목 | 내용 |
| --- | --- |
| MCU | STM32 |
| Language | C |
| Control Method | Interrupt + FSM |
| Communication | UART |
| PWM | TIM5 |

---

# 2. 하드웨어 구성

| 장치 | 역할 |
| --- | --- |
| STM32 | 메인 제어기 |
| Push Button | 사용자 입력 |
| DC Motor | 구동 장치 |
| TIM5 | PWM 생성 |
| TIM4 | 버튼 입력 시간 체크 |
| UART | UART 인터럽트 및 디버깅 |

---

# 3. FSM 설계

FSM(Finite State Machine)은 모터의 현재 상태를 관리하며, 버튼 이벤트에 따라 상태를 변경

상태는 아래와 같이 구성됨

| State | 설명 |
| --- | --- |
| STOP | 모터 정지 |
| CW | 시계 방향 회전 |
| CCW | 반시계 방향 회전 |

## 3.1 상태 전이도

<img width="851" height="478" alt="f47aa57d-963a-448d-8605-1d81876fa2fa" src="https://github.com/user-attachments/assets/9decde2c-c78c-4cb4-b1eb-f63a54d9150c" />


## 3.2 UART2 Command

| Command | 동작 |
| --- | --- |
| S | Motor Stop |
| F | CW 회전 |
| R | CCW 회전 |
| 1~9 | PWM Duty 변경 |

---

# 4. 인터럽트 처리 흐름

### 4.1 버튼 제어

<img width="502" height="758" alt="image" src="https://github.com/user-attachments/assets/fbcca721-c21e-48a2-b386-4099433bc64f" />


### 4.2 UART Interrupt 제어

<img width="641" height="735" alt="image 1" src="https://github.com/user-attachments/assets/8c5690ae-7e4f-4913-a873-560f1b3ee7ff" />

---

# 5. 전역 변수(Global Variables)

## **5.1 Type 선언**

#### Motor_State_t

```c
typedef enum {
    STOP = 0,
    CW,
    CCW,
} Motor_State_t;
```

#### 상태 설명

| Enum | 설명 |
| --- | --- |
| STOP | 모터 정지 |
| CW | 시계 방향 회전 |
| CCW | 반시계 방향 회전 |

## 5.2 상태 **변수**

| 변수 | 자료형 | 설명 |
| --- | --- | --- |
| prev_motor_state | Motor_State_t | 이전 모터 상태 |
| cur_motor_state | Motor_State_t | 현재 모터 상태 |

## 5.3 플래그 **변수**

| 변수 | 자료형 | 설명 |
| --- | --- | --- |
| Key_Pressed | unsigned char | 키(버튼) 눌림 플래그 |
| Key_Released | unsigned char | 키(버튼) 뗌 플래그 |
| Uart_Data_In | unsigned char | UART 수신 플래그 |
| TIM4_Expired | unsigned char | TIM4 타임아웃 플래그 |

## 5.4 데이터 **변수**

| 변수 | 자료형 | 설명 |
| --- | --- | --- |
| prev_motor_speed | unsigned char | 이전 모터 속도 |
| cur_motor_speed | unsigned char | 현재 모터 속도 |
| Uart_Data | unsigned char | UART 수신 데이터 |

---

# 6. 함수 설계

## 6.1 Interrupt

| **함수명** | **EXTI15_10_IRQHandler** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | Key_Pressed, Key_Released |
| **업무** | 키 입력 값에 따라 전역변수 플래그 활성화 |

| **함수명** |  **USART2_IRQHandler** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | Uart_Data, Uart_Data_In |
| **업무** | Uart 입력 값에 따라 입력 값 대입, 플래그 활성화 |

| **함수명** | **TIM4_IRQHandler** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | TIM4_Expired |
| **업무** | Timeout이 발생할 시 전역변수 플래그 활성화  |

## 6.2 handler.c

| **함수명** | **Handler** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | x |
| **업무** | 각 Handler 함수 호출 |

| **함수명** | **Key_Handler** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | Key_Pressed, Key_Released, TIM4_Expired, cur_motor_state |
| **업무** | Key_Pressed, Key_Released, TIM4_Expired 체크해서 TIM4 실행 후 cur_motor_state 변경 |

| **함수명** | **UART_Handler** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | Uart_Data_In, Uart_Data, cur_motor_state |
| **업무** | Uart 입력이 들어오면 cur_motor_state or cur_motor_speed 변경 및 화면 출력 |

## 6.3 state manager 함수

| **함수명** | **State_Management** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | x |
| **업무** | 각 State_Management 함수 호출 |

| **함수명** | **Motor_State_Management** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | prev_motor_state, cur_motor_state, prev_motor_speed, cur_motor_speed |
| **업무** | prev ≠ cur 체크해서 Motor_Controller 함수 및 TIM5_Out_PWM_Genration 함수 호출 |

## 6.4 Controller.c 함수

| **함수명** | **Motor_Controller** |
| --- | --- |
| **parameter** | void |
| **argument** | void |
| **전역변수** | cur_motor_state |
| **업무** | cur_motor_state 값에 따라 모터 제어 |

| **함수명** | **TIM5_Out_PWM_Generation** |
| --- | --- |
| **parameter** | void |
| **argument** | int duty |
| **전역변수** | x |
| **업무** | duty값에 따라 타이머의 ARR값을 변경 후 PWM 파형 생성 |

---

# 7. 함수 목록

## 7.1 handler.c

| 함수 | 설명 |
| --- | --- |
| Key_Handler | 키 관련 함수 처리 |
| UART_Handler | Uart 입력 데이터 함수 처리 |
| Motor_Handler | 모터 조작 함수 처리 |

## 7.2  key.c

| 함수 | 설명 |
| --- | --- |
| Key_Poll_Init | PC13번 키 활성화 |
| Key_Get_Pressed | 키 누를 떄 IDR PC13번 IDR 비트 체크 |
| Key_ISR_Enable | 키 ISR 활성화 |

## 7.3 motor.c

| 함수 | 설명 |
| --- | --- |
| MOTOR_Init | 모터 초기화 |
| MOTOR_Stop | 모터 정지 |
| MOTOR_CW | 모터 정방향 회전 |
| MOTOR_CCW | 모터 역방향 회전 |

## 7.4 timer.c

| 함수 | 설명 |
| --- | --- |
| TIM2_Delay | Timer2를 이용한 대기시간 설정 |
| TIM4_Check_Timeout | Timeout 체크 |
| TIM4_Stop | Timer4 비활성화 |
| TIM4_Interrupt_Enable | Timer4 활성화 및 Tim4_IRQHandler 활성화 |
| TIM5_Out_Init | Timer5 초기화 |
| TIM5_Out_PWM_Generation | duty rate 설정 후 PWM 파형 생성 |
| TIM5_Out_Stop | Timer5 비활성화 |

## 7.5 uart.c

| 함수 | 설명 |
| --- | --- |
| Uart2_Init | Uart2 초기화 |
| Uart2_RX_Interrupt_Enable | USART2_IRQHandler 활성화 |

## 외관 사진
<img width="1498" height="1540" alt="KakaoTalk_20260730_123306885" src="https://github.com/user-attachments/assets/5591f718-8155-47e5-8701-672099166f1e" />

## 동작 영상
|동작 영상 - 모터|
|:---:|
| [![동작 영상 - 모터](https://img.youtube.com/vi/GmYo-FkbSe4/hqdefault.jpg)](https://youtube.com/shorts/GmYo-FkbSe4) |


