N EQU 2048
N_MOD_MASK EQU N-1
	
	AREA data, READWRITE

	EXPORT queue_size
	EXPORT queue

	ALIGN 4
queue_tail DCD 0
queue_head DCD 0
queue_size DCD 0
queue SPACE N*4

	AREA |.text|, CODE, READONLY

	EXPORT enqueue
	EXPORT check_and_dequeue

enqueue PROC
    PUSH {r4, r5}
    MOVW r5, #N_MOD_MASK ; r4 <- N mod mask
    LDR r1, _queue
    LDR r2, _queue_tail
    LDR r3, [r2]
    STR r0, [r1, r3]     ; queue[*queue_tail] <- r0
    ADD r3, r3, #4
    LSL r4, r5, #2       ; r4 <- (N mod mask * 4)
    ADD r4, r4, #3       ; lower 2 bits are 00, make them 11 to get mod mask for N * 4
    AND r3, r3, r4
    STR r3, [r2]         ; *queue_tail <- (*queue_tail + 4) % (N * 4)
    LDR r1, _queue_size  ; r1 <- queue_size
L1
    LDREX r2, [r1]       ; r2 <- *queue_size
    ADD r2, r2, #1
    AND r2, r2, r5       ; r2 <- (*queue_size + 1) % N
    STREX r3, r2, [r1]   ; try committing changes ..
    CMP r3, #0           ;   .. and restart if it fails
    BNE L1               ;   ..
    DMB
	POP {r4, r5}
    BX lr
	
	ENDP

check_and_dequeue PROC
    PUSH {r4, r5}
    MOVW r4, #N_MOD_MASK ; r4 <- N mod mask
    LDR r0, _queue_size
L2
    LDREX r1, [r0]       ; r1 <- *queue_size
    MOV r5, r1           ; save *queue_size for later
    CMP r1, #0
    BEQ L3
    SUB r1, r1, #1           
    AND r1, r1, r4       ; r2 <- (*queue_size - 1) % N
L3
    STREX r2, r1, [r0]   ; try committing changes ..
    CMP r2, #0           ;   .. and restart if it fails
    BNE L2               ;   ..
    DMB
    MOV r0, #1           ; r0 <- -2^{31} (the default return value)
    LSL r0, r0, #31
    CMP r5, #0           ; if queue is empty ..
    BEQ L4               ; return immediately
    LDR r1, _queue
    LDR r2, _queue_head
    LDR r3, [r2]
    LDR r0, [r1, r3]     ; r0 <- queue[*queue_head]
    ADD r3, r3, #4
    LSL r4, r4, #2       ; r4 <- (N * 4) mod mask
    ADD r4, r4, #3
    AND r3, r3, r4
    STR r3, [r2]         ; *queue_head <- (*queue_head + 4) % (N * 4)
L4
    POP {r4, r5}
    BX lr
	
	ENDP

; addresses of queue items
	ALIGN 4
_queue_tail DCD queue_tail
_queue_head DCD queue_head
_queue_size DCD queue_size
_queue DCD queue

	END
