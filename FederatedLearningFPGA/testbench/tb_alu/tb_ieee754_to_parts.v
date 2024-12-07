`timescale 1ns / 1ps

module tb_ieee754_to_parts;

    // Definição dos sinais de entrada e saída
    reg [31:0] ieee754;
    wire [31:0] integer_part;
    wire [31:0] fractional_part;
    wire sign;

    // Instancia o módulo que está sendo testado
    ieee754_to_parts uut (
        .ieee754(ieee754),
        .integer_part(integer_part),
        .fractional_part(fractional_part),
        .sign(sign)
    );

    initial begin
        // Inicializa a entrada com o valor de teste
        ieee754 = 32'b01000000111100000000000000000000; // 6.0 em IEEE 754

        // Aguarda um tempo para o módulo processar
        #10;

        // Exibe os resultados no console
        $display("IEEE 754: %b", ieee754);
        $display("Parte Inteira: %b (%d)", integer_part, integer_part);
        $display("Parte Fracionada: %b (%d)", fractional_part,fractional_part);
        $display("Sinal: %b", sign);

        // Termina a simulação
        $finish;
    end

endmodule
