FUNC LABEL 0
    MOV STK A VAL 123
    REF REG 0 STK A
    REF REG 1 STK A
    MOV STK B VAL 215
    REF REG 1 STK B
    PRINT REG 0
    PRINT REG 1
    RET
