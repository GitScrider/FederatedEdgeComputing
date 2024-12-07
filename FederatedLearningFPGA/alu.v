module ieee754_to_parts (
    input [31:0] ieee754,             
    output reg [31:0] integer_part,    
    output reg [31:0] fractional_part,  
    output reg sign                    
);

    reg [7:0] exponent;
    reg [23:0] mantissa;

    always @(*) begin
        sign = ieee754[31];
        exponent = ieee754[30:23] - 8'b01111111; 
        mantissa = {1'b1, ieee754[22:0]}; 

        integer_part = 0;
        fractional_part = 0;
        integer_part [7:0] = exponent;
        fractional_part [23:0] = mantissa;

        if (exponent >= 0) begin
            integer_part = mantissa >> (23 - exponent);
        end

        if (exponent >= 0) begin
            fractional_part = mantissa & ((1 << (23 - exponent)) - 1); 
            fractional_part = fractional_part << (9+exponent);
        end else begin
            fractional_part = mantissa << (-exponent); 
        end
    end

endmodule

module ieee754_compare (input [31:0]A,
                        input [31:0]B,
                        output reg result);

    always @(*) begin
        // compare signs
        if (A[31] != B[31])
            result = ~A[31];  // A is positive (0) -> A >= B -> result = 1

        // compare exponents
        else begin
            if (A[30:23] != B[30:23]) begin
                result = (A[30:23] > B[30:23]) ? 1'b1 : 1'b0;  // A has bigger exponent than B, so it is bigger
                if (A[31]) result = ~result;  // but if A is negative (1), bigger exponent means smaller number
            end
            // compare mantissas
            else begin
                result = (A[22:0] > B[22:0]) ? 1'b1 : 1'b0;  // A has bigger mantissa than B, so it is bigger
                if (A[31]) result = ~result;  // but if A is negative (1), bigger mantissa means smaller number
            end
        end
    end

endmodule


module ieee754_adder #(parameter XLEN=32)
                        (input [XLEN-1:0]A,
                         input [XLEN-1:0]B,
                         output reg  [XLEN-1:0] result);

    reg [31:0] A_swap, B_swap;  // comparison-based swap
    wire [23:0] A_Mantissa = {1'b1, A_swap[22:0]}, B_Mantissa = {1'b1, B_swap[22:0]};  // stored mantissa is 23b, this is {1'b1, mantissa} = 24b long
    wire [7:0] A_Exponent = A_swap[30:23], B_Exponent = B_swap[30:23];
    wire A_sign = A_swap[31], B_sign = B_swap[31];

    reg [23:0] Temp_Mantissa, B_shifted_mantissa;
    reg [22:0] Mantissa;
    reg [7:0] Exponent;
    reg Sign;

    reg [7:0] diff_Exponent;
    reg [32:0] Temp;
    reg carry;
    wire comp;

    integer i;

    // compare absolute values of A, B
    ieee754_compare comp_abs(.A({1'b0, A[30:0]}), .B({1'b0, B[30:0]}), .result(comp));

    always @(*) begin
        // let A >= B (switch numbers if needed)
        A_swap = comp ? A : B;
        B_swap = comp ? B : A;

        // shift B to same exponent (A >= B, exponent diff >= 0)
        diff_Exponent = A_Exponent-B_Exponent;
        B_shifted_mantissa = (B_Mantissa >> diff_Exponent);

        // sum the mantissas (and store potential carry)
        {carry,Temp_Mantissa} = (A_sign ~^ B_sign)? A_Mantissa + B_shifted_mantissa : A_Mantissa - B_shifted_mantissa;
        Exponent = A_Exponent;

        // adjust mantissa to format 1.xxxx (bit 23 is 1)
        if(carry) begin
                Temp_Mantissa = Temp_Mantissa>>1;
                Exponent = (Exponent < 8'hff) ? Exponent + 1 : 8'hff;  // protect exponent overflow
        end
        else if(|Temp_Mantissa != 1'b1) begin  // mantissa contains no 1 or unknown value (result should be 0)
            
                Temp_Mantissa = 0;
        end
        else begin
                // 1st bit is not 1, but there is some 1 in the mantissa (protecting exponent underflow)
                // fixed limit of iterations because Vivado saw this as an infinite loop
                for(i = 0; Temp_Mantissa[23] !== 1'b1 && Exponent > 0 && i < 24; i = i + 1) begin
                    Temp_Mantissa = Temp_Mantissa << 1;
                    Exponent = Exponent - 1;
                end
        end

        result = {A_sign,Exponent,Temp_Mantissa[22:0]};
    end

endmodule

module ieee754_multiplier #(parameter XLEN = 32)(
    input  [XLEN-1:0] A,
    input  [XLEN-1:0] B,
    output [XLEN-1:0] result
);

    // Decomposição do formato IEEE 754
    wire [23:0] A_Mantissa = {1'b1, A[22:0]};  // Mantissa de A com bit implícito
    wire [23:0] B_Mantissa = {1'b1, B[22:0]};  // Mantissa de B com bit implícito
    wire [7:0] A_Exponent = A[30:23];          // Expoente de A
    wire [7:0] B_Exponent = B[30:23];          // Expoente de B
    wire A_sign = A[31];                       // Sinal de A
    wire B_sign = B[31];                       // Sinal de B

    // Variáveis intermediárias
    reg [47:0] Temp_Mantissa;                  // Mantissa multiplicada
    reg [8:0] Temp_Exponent;                   // Expoente com overflow
    reg [7:0] Exponent;                        // Expoente final
    reg [22:0] Mantissa;                       // Mantissa final
    reg Sign;                                  // Sinal final

    // Resultado final
    assign result = {Sign, Exponent, Mantissa};

    always @(*) begin
        // Soma de expoentes com ajuste de bias
        Temp_Exponent = A_Exponent + B_Exponent - 8'd127;

        // Multiplicação das mantissas
        Temp_Mantissa = A_Mantissa * B_Mantissa;

        // Normalização da mantissa
        if (Temp_Mantissa[47]) begin
            Mantissa = Temp_Mantissa[46:24];  // Desloca para a direita
            Temp_Exponent = Temp_Exponent + 1;  // Incrementa o expoente
        end else begin
            Mantissa = Temp_Mantissa[45:23];  // Mantissa já normalizada
        end

        // Prevenção de overflow e underflow no expoente
        if (Temp_Exponent >= 9'd255) begin
            Exponent = 8'hff;  // Overflow (representa infinito)
            Mantissa = 23'd0;  // Mantissa zerada
        end else if (Temp_Exponent <= 9'd0) begin
            Exponent = 8'd0;   // Underflow (representa zero)
            Mantissa = 23'd0;  // Mantissa zerada
        end else begin
            Exponent = Temp_Exponent[7:0];  // Expoente válido
        end

        // Cálculo do sinal
        Sign = A_sign ^ B_sign;
    end

endmodule



module ieee754_divider #( parameter ITER = 4, parameter USE_ENABLE = 0) (
    input wire clock,
    input wire enable,
    input wire [31:0] A,
    input wire [31:0] B,
    output reg [31:0] result
);

    integer index,clk;

    // Sinal interno para controlar o enable
    wire effective_enable = USE_ENABLE ? enable : 1'b1;

    // Fios intermediários para resultados dos módulos
    wire [31:0] M1_A1, A1_Xn, M2_A2, A2_M3, M3_M4, M4_OUT;
    reg [31:0] Xn;

    // Contadores para controlar estados
    reg [2:0] divider_counter;

    // ----Initial value----  B_Mantissa * (2 ^ -1)  * 32 / 17
    ieee754_multiplier M1 (
        .A({1'b0, 8'd126, B[22:0]}),
        .B(32'h3ff0f0f1), 
        .result(M1_A1)
    );

    // 48 / 17  -  abs(M1_A1)
    ieee754_adder #(32) A1 (
        //.clock(enable_a1),
        .A(32'h4034b4b5),
        .B({1'b1, M1_A1[30:0]}),
        .result(A1_Xn)
    );

    ieee754_multiplier M2 (
        .A({1'b0, 8'd126, B[22:0]}),
        .B(Xn),
        .result(M2_A2)
    );

    ieee754_adder #(32) A2 (
        //.clock(enable_a2),
        .A(32'h40000000),
        .B({!M2_A2[31], M2_A2[30:0]}),
        .result(A2_M3)
    );

    ieee754_multiplier M3 (
        .A(Xn),
        .B(A2_M3),
        .result(M3_M4)
    );

    ieee754_multiplier M4 (
        .A(A),
        .B({B[31], (M3_M4[30:23] + 8'd126 - B[30:23]), M3_M4[22:0]}),
        .result(M4_OUT)
    );

    // Inicialização dos sinais
    initial begin
        divider_counter = 0;
        index = 0;
        clk=0;
        result = 0;
    end

    // Controle de estados
    always @(posedge clock) begin
        //$display("  effective_enable: %d",effective_enable);
        if (effective_enable) begin
            case (divider_counter)
                3'b000: begin
                    //$display("  Start Div clock: %d",clk);
                    //$display("  State %d: A = %h | B = %h ",divider_counter,A,B);
                    divider_counter = 1;
                    index=0;
                    clk=1;
                end
                3'b001: begin
                    //$display("  State %d: inA_M1 = %h | inB_M1 = %h -> M1 = %h",divider_counter,{1'b0, 8'd126, B[22:0]}, 32'h3ff0f0f1,M1_A1);
                    //$display("  State %d: inA_A1 = %h | inB_A1 = %h -> A1 = %h",divider_counter,32'h4034b4b5, {1'b1, M1_A1[30:0]}, A1_Xn);
                    Xn = A1_Xn;
                    //$display("  State %d: Xn = %h",divider_counter,Xn);
                    divider_counter = divider_counter + 1;
                    clk=clk+1;
                end
                3'b010: begin
                    //$display("  State %d: index %d",divider_counter,index);
                    if(index<ITER) begin
                        //$display("      Iteration %d: inA_M2 = %h | inB_M2 = %h -> M2 = %h ",index,{1'b0, 8'd126, B[22:0]}, Xn, M2_A2);
                        //$display("      Iteration %d: inA_A2 = %h | inB_A2 = %h -> A2 = %h",index,32'h40000000, {!M2_A2[31], M2_A2[30:0]}, A2_M3);
                        //$display("      Iteration %d: inA_M3 = %h | inB_M3 = %h -> M3 = %h",index,Xn,A2_M3, M3_M4);
                        Xn = M3_M4;
                        //$display("      Iteration %d: Xn = M3_M4 = %h", index, Xn);
                        clk=clk+1;
                    end else begin
                        //$display("  State %d: inA_M4 = %h | inB_M4 = %h -> M4 = %h",divider_counter,A,{B[31], (M3_M4[30:23] + 8'd126 - B[30:23]), M3_M4[22:0]},M4_OUT);
                        result = M4_OUT;
                        //$display("  State %d: result = %h",divider_counter,result);
                        divider_counter = 0 ;
                        clk=clk+1;
                        //$display("  End Div clock: %d",clk);
                    end
                    index = index + 1;
                end
                default: begin
                    //$display("Default State: Div invalid state encountered.");
                    divider_counter=0;
                end
            endcase
        end
    end
endmodule


module ieee754_natural_exponential #(parameter ITER = 4)(
    input clock, 
    input [31:0] A, 
    output reg [31:0] result
);

    integer index_fatorial,index_division,clk;

    wire [31:0] M1_D1, A1_M2, M2_D1, D1_A2, A2_OUT;

    reg div_enable;
    reg [2:0] natural_exponential_counter;
    reg [31:0] pontential_mul, fatorial_count, fatorial_mul, natural_exponential_sum;

    ieee754_multiplier M1 (
        .A(A),
        .B(pontential_mul), 
        .result(M1_D1)
    );
    
    ieee754_adder #(32) A1 (
        .A(32'b00111111100000000000000000000000),
        .B(fatorial_count),
        .result(A1_M2)
    );

    ieee754_multiplier M2 (
        .A(A1_M2),
        .B(fatorial_mul), 
        .result(M2_D1)
    );

    ieee754_divider #(
        .ITER(3),
        .USE_ENABLE(1)) 
    D1(
        .clock(clock),
        .enable(div_enable),
        .A(M1_D1),
        .B(M2_D1),
        .result(D1_A2)
    );
    
    ieee754_adder #(32) A2 (
        .A(D1_A2),
        .B(natural_exponential_sum),
        .result(A2_OUT)
    );

    initial begin
        
        div_enable=0;
        natural_exponential_counter=0;
        natural_exponential_sum = 32'b00111111100000000000000000000000;

    end


    always @(posedge clock) begin

        case (natural_exponential_counter)
            3'b000: begin
                $display("State %d: A = %h ", natural_exponential_counter,A);
                natural_exponential_counter = 1;
                index_fatorial=0;
                index_division = 0;
                clk=1;

                pontential_mul = 32'b00111111100000000000000000000000;
                fatorial_count = 32'b0;
                fatorial_mul = 32'b00111111100000000000000000000000;
                natural_exponential_sum = 32'b00111111100000000000000000000000;
            end
            3'b001: begin
                clk=clk+1;

                $display("State: %d Clock: %d",natural_exponential_counter,clk);
                $display("State: %d inA_M1 = %h | inB_M1 = %h -> M1 = %h",natural_exponential_counter,A,pontential_mul,M1_D1);
                $display("State: %d inA_A1 = %h | inB_A1 = %h -> A1 = %h",natural_exponential_counter,32'b00111111100000000000000000000000,fatorial_count,A1_M2);
                $display("State: %d inA_M2 = %h | inB_M2 = %h -> M2 = %h",natural_exponential_counter,A1_M2,fatorial_mul,M2_D1);
                $display("State: %d inA_D1 = %h | inB_D1 = %h -> D1 = %h",natural_exponential_counter,M1_D1,M2_D1,D1_A2);
                $display("State: %d: inA_D1 = %h | inB_D1 = %h -> A2 = %h",natural_exponential_counter,D1_A2,natural_exponential_sum,A2_OUT);

                natural_exponential_counter = natural_exponential_counter+1;
                div_enable = 1;
            end
            3'b010: begin
                // $display("State: %d inA_M1 = %h | inB_M1 = %h -> M1 = %h",natural_exponential_counter,A,pontential_mul,M1_D1);
                // $display("State: %d inA_A1 = %h | inB_A1 = %h -> A1 = %h",natural_exponential_counter,32'b00111111100000000000000000000000,fatorial_count,A1_M2);
                // $display("State: %d inA_M2 = %h | inB_M2 = %h -> M2 = %h",natural_exponential_counter,A1_M2,fatorial_mul,M2_D1);
                // $display("State: %d inA_D1 = %h | inB_D1 = %h -> D1 = %h",natural_exponential_counter,M1_D1,M2_D1,D1_A2);
                // $display("State: %d: inA_D1 = %h | inB_D1 = %h -> A2 = %h",natural_exponential_counter,D1_A2,natural_exponential_sum,A2_OUT);
                $display("clock: %d\n",clk);
                if (index_fatorial<=ITER) begin
                    
                    if(index_division<ITER*2) begin

                        $display("index_division: %d\n",index_division);
                        clk=clk+1;
                        index_division = index_division + 1;

                    end else begin 

                        div_enable = 0;

                        // pontential_mul = M1_D1;
                        // fatorial_count = A1_M2;
                        // fatorial_mul = M2_D1;
                        // natural_exponential_sum = A2_OUT;

                        index_division=0;
                        index_fatorial = index_fatorial + 1;
                        clk=clk+1;
                        natural_exponential_counter = natural_exponential_counter + 1;

                    end
                end else begin
                    clk=clk+1;
                    result = A2_OUT;
                    $display("State %d: clk = %d",natural_exponential_counter, clk+1);
                    natural_exponential_counter=0;

                end
            end
            3'b011: begin

                pontential_mul = M1_D1;
                fatorial_count = A1_M2;
                fatorial_mul = M2_D1;
                natural_exponential_sum = A2_OUT;

                $display("State: %d inA_M1 = %h | inB_M1 = %h -> M1 = %h",natural_exponential_counter,A,pontential_mul,M1_D1);
                $display("State: %d inA_A1 = %h | inB_A1 = %h -> A1 = %h",natural_exponential_counter,32'b00111111100000000000000000000000,fatorial_count,A1_M2);
                $display("State: %d inA_M2 = %h | inB_M2 = %h -> M2 = %h",natural_exponential_counter,A1_M2,fatorial_mul,M2_D1);
                $display("State: %d inA_D1 = %h | inB_D1 = %h -> D1 = %h",natural_exponential_counter,M1_D1,M2_D1,D1_A2);
                $display("State: %d: inA_D1 = %h | inB_D1 = %h -> A2 = %h",natural_exponential_counter,D1_A2,natural_exponential_sum,A2_OUT);

                natural_exponential_counter = 1;

            end


            default: begin
                $display("Default State: Natural exponential invalid state encountered.");
                natural_exponential_counter=0;
            end
        endcase
    end
endmodule



