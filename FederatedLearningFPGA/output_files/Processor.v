module multiplex(
    input  [31:0] in0,   // Entrada 0
    input  [31:0] in1,   // Entrada 1
    input  sel,               // Selecionador
    output reg [31:0] out // Saída
);

    always @(*) begin
        case (sel)
            1'b0: out = in0;  // Se sel for 0, escolhe in0
            1'b1: out = in1;  // Se sel for 1, escolhe in1
            default: out = 31'b0; // Caso não especificado, saída é 0
        endcase
    end

endmodule

module control_unit (
    input [3:0] operation_code,
    output reg jump, 
    output reg branch, 
    output reg memory_write, 
    output reg memory_ALU, 
    output reg ALUadr,
    output reg [2:0] ALUop,
    output reg write_back,
    output reg ALUscr
);

    always @(*) begin
        case (operation_code)
            // ctrl
            4'b0000: begin
                jump <= 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 0;
                ALUscr = 0;
            end
            // addi
            4'b0001: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 1;
                ALUscr = 1;
            end
            // subi
            4'b0010: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b001;
                write_back = 1;
                ALUscr = 1;
            end
            // muli
            4'b0011: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b010;
                write_back = 1;
                ALUscr = 1;
            end
            // divi
            4'b0100: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b011;
                write_back = 1;
                ALUscr = 1;
            end
            // add
            4'b0101: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 1;
                ALUscr = 0;
            end
            // sub
            4'b0110: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b001;
                write_back = 1;
                ALUscr = 0;
            end
            // mul
            4'b0111: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b010;
                write_back = 1;
                ALUscr = 0;
            end
            // div
            4'b1000: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b011;
                write_back = 1;
                ALUscr = 0;
            end
            // slt
            4'b1001: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b100;
                write_back = 0;
                ALUscr = 0;
            end
            // beqz
            4'b1010: begin
                jump = 0;
                branch = 1;
                memory_write = 0; // Corrigido
                memory_ALU = 0;
                ALUadr = 1;
                ALUop = 3'b101;
                write_back = 0;
                ALUscr = 0;
            end
            // sw
            4'b1011: begin
                jump = 0;
                branch = 0;
                memory_write = 1;
                memory_ALU = 0;
                ALUadr = 1;
                ALUop = 3'b000;
                write_back = 0;
                ALUscr = 0;
            end
            // lw
            4'b1100: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 1;
                ALUadr = 1;
                ALUop = 3'b000;
                write_back = 1;
                ALUscr = 0;
            end
            // j
            4'b1101: begin
                jump = 1;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 0;
                ALUscr = 0;
            end
            // NA (1110)
            4'b1110: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 0;
                ALUscr = 0;
            end
            // NA (1111)
            4'b1111: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 0;
                ALUscr = 0;
            end
            // Default case
            default: begin
                jump = 0;
                branch = 0;
                memory_write = 0;
                memory_ALU = 0;
                ALUadr = 0;
                ALUop = 3'b000;
                write_back = 0;
                ALUscr = 0;
            end
        endcase
    end

endmodule


module program_counter (input clock,
								input [7:0] direct_input_address,  
								input jump,
								input ALUscr_signal,
								input ALU_zero_signal,
								output reg [7:0] output_address
								);


	wire signal;

   initial begin
		output_address = 8'b00000000; // Inicializa com 0x00
   end

	assign signal = (ALUscr_signal & ALU_zero_signal) | jump;
								
	always @(posedge clock) begin
	
		case (signal)
			1'b0: output_address = output_address +1;  // Se sel for 0, escolhe in0
			1'b1: output_address = direct_input_address;
		endcase

	end

endmodule

module arithmetic_logic_unit (
    input clock,
    input reset,
    input [2:0] ALU_code_operation,
    input [31:0] data_in1,
    input [31:0] data_in2,
    output reg zero_signal,  // Adicionado reg aqui
    output reg [31:0] output_data  // Adicionado reg aqui
);

    always @(posedge clock or posedge reset) begin
        if (reset) begin
            output_data <= 32'b0;
            zero_signal <= 1'b0;  // Inicializa zero_signal no reset
        end else begin
            case (ALU_code_operation)
                3'b000: output_data <= data_in1 + data_in2;
                3'b001: output_data <= data_in1 - data_in2;
                3'b010: output_data <= data_in1 + data_in2; // Esta operação é repetida
                3'b011: output_data <= data_in1 + data_in2; // Esta operação é repetida
                3'b101: begin
                    output_data <= (data_in1 < data_in2) ? 32'b1 : 32'b0; // Atualizado para saída de 32 bits
                end
                default: output_data <= data_in1 + data_in2; // Uma operação padrão
            endcase
            
            // Atualiza zero_signal
            if (data_in1 == data_in2) begin
                zero_signal <= 1'b1;
            end else begin
                zero_signal <= 1'b0;
            end
        end
    end
endmodule

module data_cache (    
	 input clk,            // Clock
    input rst,            // Reset
    input we,             // Write Enable
    input [7:0] addr,     // Endereço de 8 bits (256 posições de memória)
    input [31:0] data_in, // Dados de entrada (escrita de 32 bits)
    output reg [31:0] data_out // Dados de saída (leitura de 32 bits)
);

    // Definindo uma memória com 256 palavras de 32 bits
    reg [31:0] memory [0:255];

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            // Resetar a memória para 0
            integer i;
            for (i = 0; i < 256; i = i + 1) begin
                memory[i] <= 32'b0;
            end
            data_out <= 32'b0;
        end else if (we) begin
            // Escrita na memória
            memory[addr] <= data_in;
        end else begin
            // Leitura da memória
            data_out <= memory[addr];
        end
    end
endmodule


module instruction_cache (
    input clk,            // Clock
    input rst,            // Reset           
    input [7:0] addr,     // Endereço de 8 bits (256 posições de memória)
    output reg [31:0] data_out // Dados de saída (leitura de 32 bits)
);

    // Definindo uma memória com 256 palavras de 32 bits
    reg [31:0] memory [0:255];

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            // Resetar a memória para 0
            integer i;
            for (i = 0; i < 256; i = i + 1) begin
                memory[i] <= 32'b0;
            end
            data_out <= 32'b0;
        end else begin
            data_out <= memory[addr];
        end
    end
endmodule

module clock_control (
    input clock,
    input reset,
    output reg program_counter_clock,
    output reg register_file_clock,
    output reg ALU_clock,
    output reg data_cache_clock,
    output reg writeback_clock
);

    reg [2:0] clock_counter;

    // Inicializando o clock_counter no reset
    always @(posedge clock or posedge reset) begin
        if (reset) begin
            clock_counter <= 3'b000; 
            // Redefinindo todos os clocks para 0
            program_counter_clock <= 0;
            register_file_clock <= 0;
            ALU_clock <= 0;
            data_cache_clock <= 0;
            writeback_clock <= 0;
        end 
        else begin
            if (clock_counter >= 3'b101) begin
                clock_counter <= 3'b000; // Resetando o contador ao atingir 5
            end 
            else begin
                clock_counter <= clock_counter + 1; 
            end
        end

        // Controle de clocks com base no clock_counter
        case(clock_counter)
            3'b001: begin
                program_counter_clock <= 1;  // Usando atribuição não-bloqueante
                register_file_clock <= 0;
                ALU_clock <= 0;
                data_cache_clock <= 0;
                writeback_clock <= 0;
            end
            3'b010: begin
                program_counter_clock <= 0;
                register_file_clock <= 1;
                ALU_clock <= 0;
                data_cache_clock <= 0;
                writeback_clock <= 0;
            end
            3'b011: begin
                program_counter_clock <= 0;
                register_file_clock <= 0;
                ALU_clock <= 1;
                data_cache_clock <= 0;
                writeback_clock <= 0;
            end
            3'b100: begin
                program_counter_clock <= 0;
                register_file_clock <= 0;
                ALU_clock <= 0;
                data_cache_clock <= 1;
                writeback_clock <= 0;
            end
            3'b101: begin
                program_counter_clock <= 0;
                register_file_clock <= 0;
                ALU_clock <= 0;
                data_cache_clock <= 0;
                writeback_clock <= 1;
            end
            default: begin
                program_counter_clock <= 0;
                register_file_clock <= 0;
                ALU_clock <= 0;
                data_cache_clock <= 0;
                writeback_clock <= 0;
            end
        endcase
    end
endmodule


module register_file (
    input clock_register_file,
    input clock_write_back,
    input reset,
    input write_back,
    input [7:0] RD_address,
    input [7:0] RX_address,
    input [7:0] RY_address,
    input [31:0] data_in,
    output reg [31:0] data_RX,
    output reg [31:0] data_RY
);

    // Definindo um vetor de 256 registros de 32 bits
    reg [31:0] reg_file [0:255];
    
    always @(posedge clock_register_file or posedge reset) begin
        if (reset) begin
            // Resetar a memória para 0
            integer i;
            for (i = 0; i < 256; i = i + 1) begin
                reg_file[i] <= 32'b0;
            end
        end else if (write_back) begin
            // Escrita na memória
            reg_file[RD_address] <= data_in;
        end
    end

    // Leitura da memória
    always @(posedge clock_register_file) begin
        data_RX <= reg_file[RX_address];
        data_RY <= reg_file[RY_address];
    end
endmodule


module processor (
    input clock,
    input reset
);

    wire pcclock; // Corrigi o nome da variável para seguir a nomenclatura
    wire rfclock;
    wire aluclock;
    wire dcclock;
    wire wbclock;

    // Instância do módulo clock_control
    clock_control cc (
        .clock(clock),
        .reset(reset),
        .program_counter_clock(pcclock), // Corrigindo a conexão
        .register_file_clock(rfclock),
        .ALU_clock(aluclock),
        .data_cache_clock(dcclock),
        .writeback_clock(wbclock)
    );

    wire [7:0] pcaddress;

    // Instância do módulo program_counter
    program_counter pc_inst (
        .clock(clock),
        .direct_input_address(icoutput[7:0]),  
        .jump(jump),
        .ALUscr_signal(ALUscr),
        .ALU_zero_signal(zs), // Descomente e conecte se necessário
        .output_address(pcaddress)
    );

    wire [31:0] icoutput;

// Exemplo de instância no módulo processor
		instruction_cache ic (
			 .clk(clock),
			 .rst(reset),
			 .addr(pcaddress), // supondo que pcaddress é a entrada correta
			 .data_out(icoutput) // saída de dados
		);

    wire jump;
    wire branch;
    wire memory_write;
    wire memory_ALU;
    wire ALUadr;
    wire [2:0] ALUop;
    wire write_back;
    wire ALUscr;

    // Instância do módulo control_unit
    control_unit cu (
        .operation_code(icoutput[31:28]),
        .jump(jump), 
        .branch(branch), 
        .memory_write(memory_write), 
        .memory_ALU(memory_ALU), 
        .ALUadr(ALUadr),
        .ALUop(ALUop),
        .write_back(write_back),
        .ALUscr(ALUscr)
    );
	 
	 
//wire [31:0] data_in; // Adicione a definição de data_in
wire [31:0] dRX;
wire [31:0] dRY;

register_file fl(
    .clock_register_file(rfclock),
    .clock_write_back(wbclock),
    .reset(reset),
    .write_back(write_back),
    .RD_address(icoutput[7:0]),
    .RX_address(icoutput[15:8]),
    .RY_address(icoutput[23:16]),
    .data_in(muxalumenoutput), // Assegure-se de conectar data_in
    .data_RX(dRX),
    .data_RY(dRY));

wire zs;
wire [31:0] ALUoutputdata;

arithmetic_logic_unit (
    .clock(aluclock),
    .reset(reset),
    .ALU_code_operation(ALUop),
    .data_in1(dRX),
    //.data_in2,
    .zero_signal(zs),  
    .output_data(ALUoutputdata));
	 
	 
wire [31:0] dcoutput;
	 
data_cache dc(    
	 .clk(dcclock),            // Clock
    .rst(reset),            // Reset
    .we(memory_write),             // Write Enable
    .addr(ALUoutputdata[7:0]),     // Endereço de 8 bits (256 posições de memória)
    .data_in(ALUoutputdata), // Dados de entrada (escrita de 32 bits)
    .data_out(dcoutput));

	wire [31:0] muxalumenoutput;
		
	 multiplex muxalumen(
    .in0(ALUoutputdata),   // Entrada 0
    .in1(dcoutput),   // Entrada 1
    .sel(memory_ALU), // Selecionador
    .out(muxalumenoutput)); // Saída);

endmodule



