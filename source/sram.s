@ Author: Ian Rudy
@ basic functions to load and store a single unsigned integer in the specified area of gba sram
@ (void) StoreValue(u32 data);

        .ARM
        .ALIGN  
        .GLOBL  StoreValue
        .type   StoreValue,%function @can use  STT_FUNC in place of %function

StoreValue:
        @ r0 is the int we are storing
        stmfd   sp!,{r2-r4}
        mov     r4, #0xE000000       @ r4 is address of sram
        add     r4, r4, r1, LSL #2      @ lsl #2 multiplies r1 by 4. Then adds this to r4 (i.e. offset r4 by r1 bytes)

        @strb stores least significant byte of register in specified memory location
        strb    r0, [r4]            @ stores first byte of int in first sram location
        mov     r0, r0, LSR #8      @ Logical shift right the input 8 bits so the least significant byte is now the second byte
        strb    r0, [r4, #1]        @ stores second byte of int in second sram location
        mov     r0, r0, LSR #8
        strb    r0, [r4, #2]
        mov     r0, r0, LSR #8
        strb    r0, [r4, #3]


        ldmfd   sp!,{r2-r4}
        bx      lr


@------------------------------------------------------------------------------------------

        @ (u32) LoadValue();

        .ARM
        .ALIGN  
        .GLOBL  LoadValue
        .type   LoadValue,%function @can use  STT_FUNC in place of %function

LoadValue:
        @ r0 is return value and input memory location offset modifier
        stmfd   sp!,{r1-r4}
        mov     r4, #0xE000000          @Sram memory address
        add     r4, r4, r0, LSL #2      @ lsl #2 multiplies r0 by 4. Then adds this to r4 (i.e. offset r4 by r0 bytes)

        mov     r0, #0

        ldrb    r2, [r4, #3]            @ldrb loads bytes, orr logical ors them to r0 (i.e. taking bytes, converting to int, returning int)
        orr     r0, r0, r2, LSL #24
        ldrb    r2, [r4, #2]
        orr     r0, r0, r2, LSL #16
        ldrb    r2, [r4, #1]
        orr     r0, r0, r2, LSL #8
        ldrb    r2, [r4]
        orr     r0, r0, r2
        


        ldmfd   sp!,{r1-r4}
        bx      lr