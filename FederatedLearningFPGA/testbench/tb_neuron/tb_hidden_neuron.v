`timescale 1ns/1ps

module hidden_neuron_tb;

    // Parâmetro do número de entradas
    parameter NUM_INPUTS = 4;

    // Sinais de teste
    reg [(32*NUM_INPUTS)-1:0] inputdata;
    reg clock;
    wire [31:0] a;

    // Instância do módulo `hidden_neuron`
    hidden_neuron #( 
        .NUM_INPUTS(NUM_INPUTS)
    ) uut (
        .inputdata(inputdata),
        .clock(clock),
        .a(a)
    );

    // Clock de 50 MHz (período de 20 ns)
    initial begin
        clock = 0;
        forever #5 clock = ~clock; // Alterna o clock a cada 10 ns
    end

    // Geração de estímulos
    initial begin
        // Inicialização
        inputdata = 0;

        $display("=== Testbench Start ===");
        //$monitor("Time: %t | State: %b | Output (a): %h", $time, uut.neuron_feedfoward_state, a);

        // Estímulo 1: Alimentar a entrada com valores iniciais
        #15; // Espera o clock estabilizar
        inputdata = {32'h40800000, 32'h40400000, 32'h40000000,32'h3f800000 }; // 4 entradas de 32 bits

        // Esperar alguns ciclos para observar a saída
        #100;

        // Estímulo 2: Alterar os dados de entrada
        // inputdata = {32'h00000005, 32'h00000015, 32'h00000025, 32'h00000035};
        // #200;

        // Finalizar a simulação
        $display("=== Testbench End ===");
        $finish;
    end

endmodule
