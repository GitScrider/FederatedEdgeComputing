`timescale 1ns / 1ps

module register_tb;

    // Sinais do módulo
    reg clock;
    reg reset;
    reg [31:0] data;
    wire [31:0] register;

    // Instanciação do módulo
    register uut (
        .clock(clock),
        .reset(reset),
        .data(data),
        .register(register)
    );

    // Geração do clock (período = 10 ns)
    initial begin
        clock = 0;
        forever #5 clock = ~clock; // Inverte o clock a cada 5 ns
    end

    // Estímulos de teste
    initial begin 
        // Inicializa sinais
        reset = 0;
        data = 32'h00000000;

        // Cenário 1: Reset ativo
        #10;
        reset = 1; // Ativa reset
        #10;
        reset = 0; // Desativa reset

        // Cenário 2: Carrega dados no registrador
        #10;
        data = 32'hA5A5A5A5; // Define o dado
        #10;
        data = 32'h5A5A5A5A;

        // Cenário 3: Verifica comportamento com reset durante operação
        #10;
        reset = 1; // Ativa reset novamente
        #10;
        reset = 0; // Desativa reset

        // Finaliza a simulação
        #20;
        $finish;
    end

    // Monitor para acompanhar os sinais
    initial begin
        $monitor("Time: %0t | Reset: %b | Data: %h | Register: %h", $time, reset, data, register);
    end

endmodule
