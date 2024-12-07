module multiplexer_parametrized #(
    parameter NUM_INPUTS = 4,   // Número de entradas do MUX (default: 4)
    parameter DATA_WIDTH = 32  // Largura de cada entrada (default: 32 bits)
)(
    input [NUM_INPUTS*DATA_WIDTH-1:0] in,  // Entradas achatadas (N entradas de 32 bits)
    input [$clog2(NUM_INPUTS)-1:0] sel,    // Sinal de seleção (log2(NUM_INPUTS) bits)
    output reg [DATA_WIDTH-1:0] out        // Saída do MUX
);

    // Lógica do MUX
    always @(*) begin
        // Seleciona o bloco correspondente ao índice do seletor
        out = in[sel*DATA_WIDTH +: DATA_WIDTH];
    end

endmodule

module register(
    input clock,
    input reset,
    input [31:0] data,
    output reg [31:0] register
); 

    always @(posedge clock) begin
        if (reset) begin
            register <= 32'b0; // Zera o registrador no reset
        end else begin
            register <= data; // Carrega o dado na borda do clock
        end
    end

endmodule


module memory_parametrized #(
    parameter WORDS = 256 // Número de palavras na memória (default: 256 palavras)
)(
    input clock,             // Clock
    input reset,             // Reset
    input write_enable,      // Sinal para habilitar escrita
    input [31:0] write_data, // Dados para escrita
    input [$clog2(WORDS)-1:0] address, // Endereço da memória
    output reg [31:0] read_data // Dados lidos da memória
);

    // Declaração da memória (array de palavras de 32 bits)
    reg [31:0] mem [0:WORDS-1];

    // Reset: Zera toda a memória (opcional)
    integer i;
    always @(posedge clock) begin
        if (reset) begin
            for (i = 0; i < WORDS; i = i + 1) begin
                mem[i] <= 32'b0;
            end
        end else if (write_enable) begin
            mem[address] <= write_data; // Escreve na memória
        end
    end

    // Leitura da memória
    always @(posedge clock) begin
        if (!write_enable) begin
            read_data <= mem[address]; // Lê a memória
        end
    end

endmodule


module bin_to_7seg (
    input [3:0] binary,    
    output reg [6:0] seg    
);

    always @(*) begin
        case (binary)
            4'b0000: seg = 7'b1000000; // 0
            4'b0001: seg = 7'b1111001; // 1
            4'b0010: seg = 7'b0100100; // 2
            4'b0011: seg = 7'b0110000; // 3
            4'b0100: seg = 7'b0011001; // 4
            4'b0101: seg = 7'b0010010; // 5
            4'b0110: seg = 7'b0000010; // 6
            4'b0111: seg = 7'b1111000; // 7
            4'b1000: seg = 7'b0000000; // 8
            4'b1001: seg = 7'b0010000; // 9
            4'b1010: seg = 7'b0001000; // A
            4'b1011: seg = 7'b0000011; // b
            4'b1100: seg = 7'b1000110; // C
            4'b1101: seg = 7'b0100001; // d
            4'b1110: seg = 7'b0000110; // E
            4'b1111: seg = 7'b0001110; // F
            default: seg = 7'b1111111; // clean display
        endcase
    end

endmodule