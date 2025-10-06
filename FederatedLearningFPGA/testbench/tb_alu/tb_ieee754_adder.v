`timescale 1ns / 1ps

module tb_ieee754_adder;

    reg [31:0] A, B;
    wire [31:0] result;

    // Instância do módulo ieee754_adder
    ieee754_adder #(32) uut (
        .A(A),
        .B(B),
        .result(result)
    );

    // Clock Generator


    initial begin


        // Testes
        $display("Iniciando os testes...");

        // Teste 1: A = 3.2, B = 4.2
        A = 32'b0_10000000_10011001100110011001100;  // 3.2
        B = 32'b0_10000001_00001100110011001100110;  // 4.2
        #40; // Espera dois ciclos de clock
        $display("Teste 1: A = 3.2, B = 4.2, Resultado = %h", result);

        // Teste 2: A = 0.66, B = 0.51
        A = 32'b0_01111110_01010001111010111000010;  // 0.66
        B = 32'b0_01111110_00000101000111101011100;  // 0.51
        #40; 
        $display("Teste 2: A = 0.66, B = 0.51, Resultado = %h", result);

        // Teste 3: A = -0.5, B = -6.4
        A = 32'b1_01111110_00000000000000000000000;  // -0.5
        B = 32'b1_10000001_10011001100110011001100;  // -6.4
        #40;
        $display("Teste 3: A = -0.5, B = -6.4, Resultado = %h", result);

        // Teste 4: A = -0.5, B = 6.4
        A = 32'b1_01111110_00000000000000000000000;  // -0.5
        B = 32'b0_10000001_10011001100110011001100;  //  6.4
        #40;
        $display("Teste 4: A = -0.5, B = 6.4, Resultado = %h", result);

        // Teste 5: Valores em hexadecimal
        A = 32'h4034b4b5; // 2.8235295
        B = 32'hbf70f0f1; // -0.9411765
        #40;
        $display("Teste 5: A = 2.8235295, B = -0.9411765, Resultado = %h",result);

        $finish;
    end
endmodule
