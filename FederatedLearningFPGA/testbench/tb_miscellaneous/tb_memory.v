`timescale 1ns / 1ps

module memory_tb;

    parameter WORDS = 16; // Testando com uma memória de 16 palavras

    // Sinais do módulo
    reg clock;
    reg reset;
    reg write_enable;
    reg [31:0] write_data;
    reg [$clog2(WORDS)-1:0] address;
    wire [31:0] read_data;

    // Instanciação do módulo de memória
    memory_parametrized #(WORDS) uut (
        .clock(clock),
        .reset(reset),
        .write_enable(write_enable),
        .write_data(write_data),
        .address(address),
        .read_data(read_data)
    );

    // Geração de clock (período = 10 ns)
    initial begin
        clock = 0;
        forever #5 clock = ~clock;
    end

    // Estímulos de teste
    initial begin
        // Inicializa sinais
        reset = 1;
        write_enable = 0;
        write_data = 32'b0;
        address = 0;

        // Aplica reset
        #10 reset = 0;

        // Escreve na memória em diferentes endereços
        #10 write_enable = 1; address = 3; write_data = 32'hDEADBEEF; // Escreve em endereço 3
        #10 write_enable = 1; address = 5; write_data = 32'hCAFEBABE; // Escreve em endereço 5

        // Lê da memória
        #10 write_enable = 0; address = 3; // Lê endereço 3
        #10 address = 5; // Lê endereço 5

        // Finaliza a simulação
        #20 $finish;
    end

    // Monitor para acompanhar os sinais
    initial begin
        $monitor("Time: %0t | Reset: %b | Write_Enable: %b | Address: %0d | Write_Data: %h | Read_Data: %h",
                 $time, reset, write_enable, address, write_data, read_data);
    end

endmodule
