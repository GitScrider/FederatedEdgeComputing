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

        // Test 1: 8.0 / 1.0
        A = 32'h40400000; // 8.0
        B = 32'h3f800000; // 1.0
        #120;
        $display("Test 1 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 2: 8.0 / 2.0
        A = 32'h40400000; // 8.0
        B = 32'h40000000; // 2.0
        #120;
        $display("Test 2 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 3: 8.0 / 4.0
        A = 32'h40400000; // 8.0
        B = 32'h40800000; // 4.0
        #120;
        $display("Test 3 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 4: 3.0 / 2.0
        A = 32'h40400000; // 3.0
        B = 32'h40000000; // 2.0
        #120;
        $display("Test 4 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 5: 5.0 / 2.0
        A = 32'h40a00000; // 5.0
        B = 32'h40000000; // 2.0
        #120;
        $display("Test 5 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 6: 0.0 / 3.0
        A = 32'h00000000; // 0.0
        B = 32'h40400000; // 3.0
        #120;
        $display("Test 6 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 7: 2.0 / 8.0
        A = 32'h40000000; // 2.0
        B = 32'h40400000; // 8.0
        #120;
        $display("Test 7 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 8: 2.5 / 1.2
        A = 32'h40200000; // 2.5
        B = 32'h3f99999a; // 1.2
        #120;
        $display("Test 8 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 9: 0.75 / 0.5
        A = 32'h3f400000; // 0.75
        B = 32'h3f000000; // 0.5
        #120;
        $display("Test 9 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 10: -8.0 / 2.0
        A = 32'hc1000000; // -8.0
        B = 32'h40000000; // 2.0
        #120;
        $display("Test 10 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 11: 8.0 / -2.0
        A = 32'h41000000; // 8.0
        B = 32'hc0000000; // -2.0
        #120;
        $display("Test 11 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 12: -8.0 / -2.0
        A = 32'hc1000000; // -8.0
        B = 32'hc0000000; // -2.0
        #120;
        $display("Test 12 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        // Test 13: 3.0 / 0.0 (division by zero)
        A = 32'h40400000; // 3.0
        B = 32'h00000000; // 0.0
        #120;
        $display("Test 13 | Time: %t | A: %h | B: %h | Result: %h",$time, A, B, result);

        $finish;

    end

endmodule