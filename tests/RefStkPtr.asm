FUNC LABEL 0
    MOV STK A VAL 45
    MOV STK B VAL 90
    REF STK C STK B
    REF STK D PTR C
    PRINT STK D
    RET
