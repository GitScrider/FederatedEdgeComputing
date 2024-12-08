module relu (
    input [31:0] Z,
    output reg [31:0] a
);

initial begin
    a = 32'b0;
end 

always @(*) begin
    if(Z[31]) begin
        a = 32'b0;
    end else begin
        a = Z;
    end
end

endmodule

//5 clk
module Z #(parameter NUM_INPUTS = 4) (
    input clock,
    input reset,
    input enable,
    input [31:0] neuron_input, 
    input [31:0] weight,
    input [31:0] bias,
    output reg [31:0] result
);

    // Registradores e fios internos
    reg [31:0] Z;
    reg [31:0] index; // Correção da vírgula
    wire [31:0] A1_Z, M1_A1, A2_OUT; // Declaração de A2_OUT

    // Instância do multiplicador IEEE754
    ieee754_multiplier M1 (
        .A(weight),          // Corrigido de W para weight
        .B(neuron_input),
        .result(M1_A1)
    );

    // Instância dos somadores IEEE754
    ieee754_adder #(32) A1 (
        .A(Z),
        .B(M1_A1),
        .result(A1_Z)
    );

    ieee754_adder #(32) A2 (
        .A(Z),
        .B(bias),
        .result(A2_OUT)
    );


    initial begin 
        Z=0;
        index = 0;
    end

    // Bloco síncrono
    always @(posedge clock) begin
        if (enable) begin
            if (index < NUM_INPUTS) begin
                Z = A1_Z;   
                $display("Index: %d | M1_A1 = %h | A1_Z = %h | Z = %h | A2_OUT = %h",index,M1_A1,A1_Z,Z,A2_OUT);
                index = index + 1; 

            end else begin
                
                result = A2_OUT;
                $display("Index: %d | result = %h",index,result);

                Z = 0;             
                index = 0;

            end
        end
    end

endmodule

module hidden_neuron #(parameter NUM_INPUTS = 4) (
    input [(32*NUM_INPUTS)-1:0] inputdata,
    input clock,
    output reg [31:0] a
);

    integer z_counter,clk;

    wire [31:0] MUX_Z, MEM_Z, Z_RELU, RELU_OUT;

    reg [1:0] neuron_feedfoward_state;
    reg write_enable;
    reg enable_Z; // Novo sinal de enable para o módulo Z
    reg [31:0] bias;
    reg [$clog2(NUM_INPUTS)-1:0] data_counter;

    multiplexer_parametrized #(
        .NUM_INPUTS(NUM_INPUTS),
        .DATA_WIDTH(32)
    ) mux (
        .in(inputdata),
        .sel(data_counter),
        .out(MUX_Z)
    );

    memory_parametrized #(
        .WORDS(NUM_INPUTS)
    ) mem (
        .clock(clock),
        .write_enable(write_enable),
        .address(data_counter),
        .read_data(MEM_Z)
    );

    Z #(
        .NUM_INPUTS(NUM_INPUTS)
    ) Z (
        .clock(clock),
        .enable(enable_Z), // Conectando o enable
        .neuron_input(MUX_Z),
        .weight(MEM_Z),
        .bias(bias),
        .result(Z_RELU)
    );

    relu relu(
        .Z(Z_RELU),
        .a(RELU_OUT)
    );

    initial begin
        bias = 0;

        write_enable = 0;
        neuron_feedfoward_state = 0;
        enable_Z = 0; // Inicializando o enable em 0
        //z_counter = 0;
    end

    always @(posedge clock) begin
        case (neuron_feedfoward_state)
            2'b00: begin
                $display("State 0: Initializing...");

                $display("State: %d | MUX_Z = %h | MEM_Z = %h | Z_RELU = %h | RELU_OUT = %h",
                         neuron_feedfoward_state, MUX_Z, MEM_Z, Z_RELU, RELU_OUT);
                
                neuron_feedfoward_state = 2'b01;
                data_counter = 0;
                z_counter = 0;            
            end
            2'b01: begin
                $display("State: %d | MUX_Z = %h | MEM_Z = %h | Z_RELU = %h | RELU_OUT = %h",
                         neuron_feedfoward_state, MUX_Z, MEM_Z, Z_RELU, RELU_OUT);

                enable_Z = 1; // Ativa o módulo Z
                neuron_feedfoward_state = 2'b10;
            end

            2'b10: begin
                $display("State: %d | zcounter %d | MUX_Z = %h | MEM_Z = %h | Z_RELU = %h | RELU_OUT = %h",
                         neuron_feedfoward_state, z_counter, MUX_Z, MEM_Z, Z_RELU, RELU_OUT);
                
                if (z_counter <= NUM_INPUTS) begin
                    z_counter = z_counter + 1;
                    data_counter = data_counter + 1;
                end else begin
                    enable_Z = 0; // Desativa o módulo Z após finalizar
                    neuron_feedfoward_state = neuron_feedfoward_state + 1;
                end
            end
            2'b11: begin
                    a = RELU_OUT;
                    $display("State: %d | RELU_OUT = %h | a = %h ",neuron_feedfoward_state, RELU_OUT, a);
                    neuron_feedfoward_state = 2'b00; // Reinicia o estado
            end
            default: begin
                $display("Default: Invalid state encountered.");
                neuron_feedfoward_state = 2'b00;
            end
        endcase
    end
endmodule


module input_neuron (
    input [31:0] inputdata,
    input clock,
    input enable,
    output reg [31:0] outputdata 
);

    initial begin
        outputdata=0;
    end

    always @(posedge clock) begin
        if(enable) begin
            outputdata = inputdata;
        end
    end

endmodule