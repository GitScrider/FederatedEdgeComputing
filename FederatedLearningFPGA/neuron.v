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
    reg [31:0] index;
    wire [31:0] M1_A1, A1_Z, A2_OUT;

    // Instância do multiplicador IEEE754
    ieee754_multiplier M1 (
        .A(weight),
        .B(neuron_input),
        .result(M1_A1)
    );

    // Instância do somador IEEE754 para acumulação
    ieee754_adder #(32) A1 (
        .A(Z),
        .B(M1_A1),
        .result(A1_Z)
    );

    // Instância do somador IEEE754 para bias
    ieee754_adder #(32) A2 (
        .A(Z),
        .B(bias),
        .result(A2_OUT)
    );

    initial begin 
        Z = 0;
        index = 0;
    end

    // Bloco síncrono
    always @(posedge clock) begin
        if (reset) begin
            Z = 0;
            index = 0;
            result = 0;
        end else if (enable) begin
            if (index < NUM_INPUTS) begin
                // Use the IEEE754 adder to accumulate properly
                Z = A1_Z;   
                $display("Index: %d | M1_A1 = %h | A1_Z = %h | Z = %h | A2_OUT = %h", index, M1_A1, A1_Z, Z, A2_OUT);
                index = index + 1; 

            end else if (index == NUM_INPUTS) begin
                // Add bias to the final accumulated Z value
                result = A2_OUT;
                $display("Index: %d | Final Z = %h | Bias = %h | result = %h", index, Z, bias, result);

                Z = 0;             
                index = 0;

            end
        end
    end

endmodule

module input_neuron (
    input clock,
    input neuron_enable,
    input [31:0] inputdata,
    output reg [31:0] outputdata 
);

    initial begin
        outputdata=0;
    end

    always @(posedge clock) begin
        if(neuron_enable) begin
            outputdata = inputdata;
        end
    end

endmodule

module hidden_neuron #(parameter NUM_INPUTS = 4) (
    input clock,
    input neuron_enable,
    input [(32*NUM_INPUTS)-1:0] inputdata,
    input [31:0] alpha,
    input [31:0] deltafunction_value,
    input [$clog2(NUM_INPUTS)-1:0] weight_memory_address,
    input [1:0] neuron_state,
    output reg [31:0] weight,
    output reg [31:0] a
);
    
    integer z_counter,clk;

    wire [31:0] MUX_OUT, MEM_OUT, Z_RELU, RELU_OUT;
    wire [31:0] M1_M2, M2_A1, A1_MEM;

    reg [1:0] neuron_feedfoward_state, neuron_deltafunction_state, neuron_backpropagation_state;
    reg write_enable;
    reg enable_Z; 
    reg [31:0] bias;
    reg [$clog2(NUM_INPUTS)-1:0] data_counter;

    //feedfoward

    multiplexer_parametrized #(
        .NUM_INPUTS(NUM_INPUTS),
        .DATA_WIDTH(32)
    ) mux (
        .in(inputdata),
        .sel(data_counter),
        .out(MUX_OUT)
    );

    memory_parametrized #(
        .WORDS(NUM_INPUTS)
    ) mem (
        .clock(clock),
        .write_enable(write_enable),
        .write_data(A1_MEM),
        .address(data_counter),
        .read_data(MEM_OUT)
    );

    Z #(
        .NUM_INPUTS(NUM_INPUTS)
    ) Z (
        .clock(clock),
        .reset(1'b0), // Reset signal - set to 0 for normal operation
        .enable(enable_Z), 
        .neuron_input(MUX_OUT),
        .weight(MEM_OUT),
        .bias(bias),
        .result(Z_RELU)
    );

    relu relu(
        .Z(Z_RELU),
        .a(RELU_OUT)
    );

    //back propagation

    ieee754_multiplier M1 (
        .A(alpha),
        .B(MUX_OUT),
        .result(M1_M2)
    );

    ieee754_multiplier M2 (
        .A(M1_M2),          
        .B(deltafunction_value),
        .result(M2_A1)
    );

    ieee754_adder #(32) A1 (
        .A(MEM_OUT),
        .B(M2_A1),
        .result(A1_MEM)
    );

    //

    initial begin

        bias = 0;
        weight = 0;

        write_enable = 0;

        neuron_feedfoward_state = 0;
        neuron_deltafunction_state=0;
        neuron_backpropagation_state=0;

        enable_Z = 0; 
    end

    always @(posedge clock) begin
        if(neuron_enable) begin
                case (neuron_state)
                    2'b00: begin //feedfoward
                        case (neuron_feedfoward_state)
                            2'b00: begin
                                $display("State 0: Initializing...");

                                $display("State: %d | MUX_OUT = %h | MEM_OUT = %h | Z_RELU = %h | RELU_OUT = %h",
                                        neuron_feedfoward_state, MUX_OUT, MEM_OUT, Z_RELU, RELU_OUT);
                                
                                neuron_feedfoward_state = 2'b01;
                                data_counter = 0;
                                z_counter = 0;            
                            end
                            2'b01: begin
                                $display("State: %d | MUX_OUT = %h | MEM_OUT = %h | Z_RELU = %h | RELU_OUT = %h",
                                        neuron_feedfoward_state, MUX_OUT, MEM_OUT, Z_RELU, RELU_OUT);

                                enable_Z = 1; // Ativa o módulo Z
                                neuron_feedfoward_state = 2'b10;
                            end

                            2'b10: begin
                                $display("State: %d | zcounter %d | MUX_OUT = %h | MEM_OUT = %h | Z_RELU = %h | RELU_OUT = %h",
                                        neuron_feedfoward_state, z_counter, MUX_OUT, MEM_OUT, Z_RELU, RELU_OUT);
                                
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
                                $display("Default: Invalid feedfoward neuron state encountered.");
                                neuron_feedfoward_state = 2'b00;
                            end
                        endcase

                    end
                    2'b01: begin //deltafunction
                        case (neuron_deltafunction_state)
                            2'b00: begin
                                data_counter = weight_memory_address;
                                neuron_deltafunction_state = 1;
                            end
                            2'b01: begin
                                weight = MEM_OUT;
                                neuron_deltafunction_state= 0;
                            end
                            default: begin
                                $display("Default: Invalid deltafunction neuron state encountered.");
                            end
                        endcase
                    end
                    2'b10: begin //backpropagation
                        case (neuron_backpropagation_state)
                            2'b00: begin
                                data_counter = weight_memory_address;
                                neuron_backpropagation_state = 1;
                                write_enable = 1;
                            end
                            2'b01: begin
                                weight = MEM_OUT;
                                neuron_backpropagation_state= 0;
                            end
                            default: begin
                                $display("Default: Invalid backpropagation neuron state encountered.");
                            end
                        endcase

                    end
                    
                    2'b11: begin //idle state

                        neuron_feedfoward_state = 0;
                        neuron_deltafunction_state = 0;
                        neuron_backpropagation_state = 0;
                        write_enable = 0;

                    end   

                    default: begin
                        $display("Default: Invalid neuron state encountered.");
                    end
                endcase
        end
    end
endmodule

module softmax_sum #(parameter NUM_INPUTS = 4) (
    
    input [(32*NUM_INPUTS)-1:0] input_data,
    output reg [31:0] output_data

);

    wire [31:0] A1_A2, A1_A3;

    ieee754_adder #(32) A1 (
        .A(input_data[31:0]),
        .B(input_data[63:32]),
        .result(A1_A2)
    );
        ieee754_adder #(32) A2 (
        .A(A1_A2),
        .B(input_data[95:64]),
        .result(A1_A3)
    );

    initial begin
        output_data = 0;
    end

    always @(*) begin
        output_data = A1_A3;
    end

endmodule

module softmax_neuron #(parameter NUM_INPUTS = 4) (

    input clock,
    input neuron_enable,
    input [31:0] alpha,
    input [(32*NUM_INPUTS)-1:0] inputdata,
    input [31:0] exp_sum,

    input [31:0] deltafunction_value,
    input [$clog2(NUM_INPUTS)-1:0] weight_memory_address,
    input [1:0] neuron_state,

    output[31:0] exp, 
    output reg [31:0] weight,
    output reg [31:0] a

);

    integer z_counter,clk;

    wire [31:0] MUX_OUT, MEM_OUT, Z_E1, E1_D1, D1_OUT;
    wire [31:0] M1_M2, M2_A1, A1_MEM;

    reg [1:0] neuron_feedfoward_state, neuron_deltafunction_state, neuron_backpropagation_state;
    reg write_enable;
    reg enable_Z; 
    reg [31:0] bias;
    reg [$clog2(NUM_INPUTS)-1:0] data_counter;

    //feedfoward

    multiplexer_parametrized #(
        .NUM_INPUTS(NUM_INPUTS),
        .DATA_WIDTH(32)
    ) mux (
        .in(inputdata),
        .sel(data_counter),
        .out(MUX_OUT)
    );

    memory_parametrized #(
        .WORDS(NUM_INPUTS)
    ) mem (
        .clock(clock),
        .write_enable(write_enable),
        .write_data(A1_MEM),
        .address(data_counter),
        .read_data(MEM_OUT)
    );

    Z #(
        .NUM_INPUTS(NUM_INPUTS)
    ) Z (
        .clock(clock),
        .reset(1'b0), // Reset signal - set to 0 for normal operation
        .enable(enable_Z), 
        .neuron_input(MUX_OUT),
        .weight(MEM_OUT),
        .bias(bias),
        .result(Z_E1)
    );

    ieee754_natural_exponential #(.ITER(10)) E1 (
        .clock(clock),
        .A(Z_E1),
        .result(E1_D1)
    );

    reg enable_d1;

    ieee754_divider #(.ITER(4), .USE_ENABLE(0)) D1 (
        .clock(clock),
        .enable(enable_d1),
        .A(E1_D1),
        .B(exp_sum),
        .result(D1_OUT)
    );

    assign exp = E1_D1;

    //back propagation

    ieee754_multiplier M1 (
        .A(alpha),
        .B(MUX_OUT),
        .result(M1_M2)
    );

    ieee754_multiplier M2 (
        .A(M1_M2),          
        .B(deltafunction_value),
        .result(M2_A1)
    );

    ieee754_adder #(32) A1 (
        .A(MEM_OUT),
        .B(M2_A1),
        .result(A1_MEM)
    );

    //

    initial begin

        bias = 0;
        weight = 0;

        write_enable = 0;

        neuron_feedfoward_state = 0;
        neuron_deltafunction_state=0;
        neuron_backpropagation_state=0;

        enable_Z = 0; 
    end

    always @(posedge clock) begin
            if(neuron_enable) begin
                case (neuron_state)
                    2'b00: begin //feedfoward
                        case (neuron_feedfoward_state)
                            2'b00: begin
                                $display("State 0: Initializing...");

                                $display("State: %d | MUX_OUT = %h | MEM_OUT = %h | E1_D1 = %h | D1_OUT = %h",
                                neuron_feedfoward_state, MUX_OUT, MEM_OUT, E1_D1, D1_OUT);
                                    
                                neuron_feedfoward_state = 2'b01;
                                data_counter = 0;
                                z_counter = 0;            
                            end
                            2'b01: begin
                                $display("State: %d | MUX_OUT = %h | MEM_OUT = %h | E1_D1 = %h | D1_OUT = %h",
                                neuron_feedfoward_state, MUX_OUT, MEM_OUT, E1_D1, D1_OUT);

                                enable_Z = 1; // Ativa o módulo Z
                                neuron_feedfoward_state = 2'b10;
                            end

                            2'b10: begin
                                $display("State: %d | zcounter %d | MUX_OUT = %h | MEM_OUT = %h | E1_D1 = %h | D1_OUT = %h",
                                neuron_feedfoward_state, z_counter, MUX_OUT, MEM_OUT, E1_D1, D1_OUT);
                                    
                                if (z_counter <= NUM_INPUTS) begin
                                     z_counter = z_counter + 1;
                                data_counter = data_counter + 1;
                                end else begin
                                    enable_Z = 0; // Desativa o módulo Z após finalizar
                                    neuron_feedfoward_state = neuron_feedfoward_state + 1;
                                end
                            end
                            2'b11: begin
                                a = D1_OUT;
                                $display("State: %d | D1_OUT = %h | a = %h ",neuron_feedfoward_state, D1_OUT, a);
                                neuron_feedfoward_state = 2'b00; // Reinicia o estado
                            end
                            default: begin
                                $display("Default: Invalid feedfoward neuron state encountered.");
                                neuron_feedfoward_state = 2'b00;
                            end
                        endcase

                    end
                    2'b01: begin //deltafunction
                        case (neuron_deltafunction_state)
                            2'b00: begin
                                data_counter = weight_memory_address;
                                neuron_deltafunction_state = 1;
                            end
                            2'b01: begin
                                weight = MEM_OUT;
                                neuron_deltafunction_state= 0;
                            end
                            default: begin
                                $display("Default: Invalid deltafunction neuron state encountered.");
                            end
                        endcase
                    end
                    2'b10: begin //backpropagation
                        case (neuron_backpropagation_state)
                            2'b00: begin
                                data_counter = weight_memory_address;
                                neuron_backpropagation_state = 1;
                                write_enable = 1;
                            end
                            2'b01: begin
                                weight = MEM_OUT;
                                neuron_backpropagation_state= 0;
                            end
                            default: begin
                                $display("Default: Invalid backpropagation neuron state encountered.");
                            end
                        endcase

                    end
                    2'b11: begin //idle state
                        neuron_feedfoward_state = 0;
                        neuron_deltafunction_state = 0;
                        neuron_backpropagation_state = 0;
                        write_enable = 0;
                    end   

                    default: begin
                        $display("Default: Invalid neuron state encountered.");
                    end

                endcase
            end
        end

endmodule