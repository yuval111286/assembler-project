

DATANEG:   .data  -1, -2, -3, -11, -33, -123, -511, -512


m1:         .mat   [2][3]  -1, -2, -3,  -4, -5, -6



START:      mov   #-5, r3            
            add   #-12, r4           
            sub   #-1, r4            
            cmp   #-7, #-2           
            prn   #-9                

            
            mov   #-8, DATA_NEG

            

            
            cmp   #-512, r0
            prn   #-511

            
            add   #-33, m1[r1][r0]

            stop
