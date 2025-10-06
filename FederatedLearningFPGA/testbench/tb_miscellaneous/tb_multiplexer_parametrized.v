`timescale 1ns/1ps

module tb_multiplexer_parametrized;

    // Parâmetros
    parameter NUM_INPUTS = 4;    // Número de entradas
    parameter DATA_WIDTH = 32;  // Largura de cada entrada

    // Sinais do módulo
    reg [NUM_INPUTS*DATA_WIDTH-1:0] in; // Entradas achatadas
    reg [$clog2(NUM_INPUTS)-1:0] sel;   // Sinal de seleção
    wire [DATA_WIDTH-1:0] out;          // Saída do MUX

    // Instância do módulo sob teste
    multiplexer_parametrized #(
        .NUM_INPUTS(NUM_INPUTS),
        .DATA_WIDTH(DATA_WIDTH)
    ) uut (
        .in(in),
        .sel(sel),
        .out(out)
    );

    // Inicialização dos sinais e teste
    initial begin
        // Inicializa os sinais
        in = 0;
        sel = 0;

        // Monitoramento dos sinais
        $monitor("Time: %0t | sel: %0d | out: %h", $time, sel, out);

        // Atribui valores para as entradas achatadas
        in = {32'hAAAA_BBBB, 32'hCCCC_DDDD, 32'hEEEE_FFFF, 32'h1234_5678}; // 4 entradas de 32 bits

        // Testa cada valor de `sel`
        #5 sel = 2'b00; // Deve selecionar a primeira entrada (32'hAAAA_BBBB)
        #5 sel = 2'b01; // Deve selecionar a segunda entrada (32'hCCCC_DDDD)
        #5 sel = 2'b10; // Deve selecionar a terceira entrada (32'hEEEE_FFFF)
        #5 sel = 2'b11; // Deve selecionar a quarta entrada (32'h1234_5678)

        // Finaliza a simulação
        #5 $finish;
    end

endmodule
