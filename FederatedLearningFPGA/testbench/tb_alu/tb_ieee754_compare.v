`timescale 1ns/1ps

module tb_ieee754_compare;

    // Inputs
    reg [31:0] A;
    reg [31:0] B;

    // Output
    wire result;

    // Instância do módulo
    ieee754_compare uut (
        .A(A),
        .B(B),
        .result(result)
    );

    // Procedimento de teste
    initial begin
        $display("Início do Testbench");
        $monitor("Time: %0t | A: %d | B: %d | Result: %b", $time, A, B, result);

        // Caso 1: A > B (positivos)
        A = 32'h40480000; // 3.125 (positiva)
        B = 32'h40400000; // 3.0 (positiva)
        #10;

        // Caso 2: A < B (positivos)
        A = 32'h40400000; // 3.0 (positiva)
        B = 32'h40480000; // 3.125 (positiva)
        #10;

        // Caso 3: A e B iguais
        A = 32'h40480000; // 3.125
        B = 32'h40480000; // 3.125
        #10;

        // Caso 4: A > B (negativos)
        A = 32'hC0400000; // -3.0 (negativa)
        B = 32'hC0480000; // -3.125 (negativa)
        #10;

        // Caso 5: A < B (negativos)
        A = 32'hC0480000; // -3.125 (negativa)
        B = 32'hC0400000; // -3.0 (negativa)
        #10;

        // Caso 6: Sinais diferentes (A positivo, B negativo)
        A = 32'h40400000; // 3.0 (positiva)
        B = 32'hC0400000; // -3.0 (negativa)
        #10;

        // Caso 7: Sinais diferentes (A negativo, B positivo)
        A = 32'hC0400000; // -3.0 (negativa)
        B = 32'h40400000; // 3.0 (positiva)
        #10;

        // Fim do teste
        $display("Fim do Testbench");
        $finish;
    end

endmodule
