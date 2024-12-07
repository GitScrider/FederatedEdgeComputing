`timescale 1ns/1ns

module tb_ieee754_divider;

    // Definição de parâmetros
    parameter ITER = 3;          // Parâmetro para número de iterações
    parameter USE_ENABLE = 0;    // Parâmetro para uso do enable

    // Sinais de entrada e saída
    reg clock;
    reg [31:0] A, B; 
    wire [31:0] result; 

    // Instância do módulo com múltiplos parâmetros
    ieee754_divider #(
        .ITER(ITER),               // Passa o parâmetro ITER
        .USE_ENABLE(USE_ENABLE)    // Passa o parâmetro USE_ENABLE
    ) uut (
        .clock(clock),
        .A(A),
        .B(B),
        .result(result)
    );

    always #10 clock = ~clock;

    initial begin
        
        clock = 0;

        A = 32'h40400000; // 8.0
        B = 32'h3f800000; // 3.0

        #120;
        $display("Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);


        $finish;

    end

endmodule