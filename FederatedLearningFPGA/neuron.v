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
        if (index < NUM_INPUTS) begin
            $display("Index: %d | M1_A1 = %h | A1_Z = %h | M1_A1 = %h | A2_OUT = %h",index,M1_A1,A1_Z,M1_A1,A2_OUT);
            Z = A1_Z;   
            index = index + 1; 
        end else begin
            result = A2_OUT;   
            Z = 0;             
            index = 0;         
        end
    end

endmodule


module hidden_neuron #(parameter NUM_INPUTS) (
    input [(32*NUM_INPUTS)-1:0] inputdata,
    input clock,
    output reg a,
);

    wire [31:0] MUX_Z, MEM_Z, Z_RELU, RELU_OUT;

    reg write_enable;
    reg [31:0] bias,
    reg [$clog2(NUM_INPUTS)-1:0]counter;


    multiplexer_parametrized #(
        .NUM_INPUTS(NUM_INPUTS),   
        .DATA_WIDTH(32)) 
    mux (
        .in(inputdata),  
        .sel(counter),    
        .out(MUX_Z)        
    );

    memory_parametrized #(
        .WORDS(NUM_INPUTS)) 
    mem (
        //.clock(),                          
        .write_enable(write_enable),      
        //input [31:0] write_data, 
        .address(counter), 
        .read_data(MEM_Z),
    );

    Z #(
        .NUM_INPUTS(NUM_INPUTS)) 
    Z (
        .clock(clock), 
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
        counter = 0;
        write_enable = 0;
    end

    always @(posedge clock) begin

        case() 

            2'b00: begin

            end
            2'b00

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