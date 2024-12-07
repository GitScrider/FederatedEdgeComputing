`timescale 1ns / 1ps

module tb_Z;

    parameter ITER = 4;

    reg clock;
    reg [31:0] neuron_input, weight, bias;
    wire [31:0] result;

    // Instância do módulo Z
    Z  #(ITER) uut (
        .clock(clock),
        .neuron_input(neuron_input),
        .weight(weight),
        .bias(bias),
        .result(result)
    );

    // Geração de clock
    always #5 clock = ~clock; // Clock com período de 20ns (50 MHz)

    initial begin
        // Inicializa sinais
        clock = 0;
        neuron_input = 0;
        weight = 0;
        bias = 0;
        
        // Caso 1: Primeiro conjunto de entradas

        bias = 32'b00111111100000000000000000000000;        // 1.0

        neuron_input = 32'b01000000000000000000000000000000; // 2.0
        weight = 32'b00111111110000000000000000000000;      // 1.5
        

        #50; // Aguarda 5 ciclos de clock (ITER = 4 + 1 ciclo para bias)
        $display("Caso 1: Resultado esperado com bias somado, Z = %h", result);


        $finish;
    end
endmodule
