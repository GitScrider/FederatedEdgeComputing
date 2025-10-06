module input_layer #(
    parameter NUM_NEURONS = 4
) (
    input clock,
    input [(32*NUM_NEURONS)-1:0] input_layer,
    input layer_enable,
    output [(32*NUM_NEURONS)-1:0] output_layer
);
    
    input_neuron n0 (
        .inputdata(input_layer[31:0]),
        .clock(clock),
        .neuron_enable(layer_enable),
        .outputdata(output_layer[31:0]) 
    );

    input_neuron n1 (
        .inputdata(input_layer[63:32]),
        .clock(clock),
        .neuron_enable(layer_enable),
        .outputdata(output_layer[63:32]) 
    );

    input_neuron n2 (
        .inputdata(input_layer[95:64]),
        .clock(clock),
        .neuron_enable(layer_enable),
        .outputdata(output_layer[95:64]) 
    );

    input_neuron n3 (
        .inputdata(input_layer[127:96]),
        .clock(clock),
        .neuron_enable(layer_enable),
        .outputdata(output_layer[127:96])
    );

endmodule

module hidden_layer #(
    parameter NUM_NEURONS = 4
) (
    input clock,
    input layer_enable,
    input [31:0] alpha,
    input [(32*NUM_NEURONS)-1:0] input_layer,
    input [$clog2(NUM_NEURONS)-1:0] weight_memory_address,
    input [1:0] layer_state,
    input [(32*NUM_NEURONS)-1:0] deltafunctions_value, 
    output [(32*NUM_NEURONS)-1:0] neurons_weight,
    output [(32*NUM_NEURONS)-1:0] output_layer
);

    hidden_neuron #(
        .NUM_INPUTS(4)
    ) n0 (
        .clock(clock),
        .neuron_enable(layer_enable),
        .inputdata(input_layer),
        .alpha(alpha),
        .deltafunction_value(deltafunctions_value[31:0]),
        .weight_memory_address(weight_memory_address),
        .neuron_state(layer_state),
        .weight(neurons_weight[31:0]),
        .a(output_layer[31:0])
    );

    hidden_neuron #(
        .NUM_INPUTS(4)
    ) n1 (
        .clock(clock),
        .neuron_enable(layer_enable),
        .inputdata(input_layer),
        .alpha(alpha),
        .deltafunction_value(deltafunctions_value[63:32]),
        .weight_memory_address(weight_memory_address),
        .neuron_state(layer_state),
        .weight(neurons_weight[63:32]),
        .a(output_layer[63:32])
    );

    hidden_neuron #(
        .NUM_INPUTS(4)
    ) n2 (
        .clock(clock),
        .neuron_enable(layer_enable),
        .inputdata(input_layer),
        .alpha(alpha),
        .deltafunction_value(deltafunctions_value[95:64]),
        .weight_memory_address(weight_memory_address),
        .neuron_state(layer_state),
        .weight(neurons_weight[95:64]),
        .a(output_layer[95:64])
    );

endmodule

module output_softmax_layer #(
    parameter NUM_NEURONS = 4
)(
    input clock,
    input layer_enable,
    input [31:0] alpha,
    input [(32*NUM_NEURONS)-1:0] input_layer,
    
    input [$clog2(NUM_NEURONS)-1:0] weight_memory_address,
    input [(32*NUM_NEURONS)-1:0] deltafunctions_value, 
    input [1:0] layer_state,
    output [(32*NUM_NEURONS)-1:0] neurons_weight,
    output [(32*NUM_NEURONS)-1:0] output_layer
);

    wire [31:0] ss_n, ns1_ss,ns2_ss,ns3_ss;

    softmax_sum #(.NUM_INPUTS(4)) ss (
        
        .input_data({ns3_ss,ns2_ss,ns1_ss,32'h00000000}),
        .output_data(ss_n)

    );

    softmax_neuron #(.NUM_INPUTS(4)) sn1 (

        .clock(clock),
        .neuron_enable(layer_enable),
        .alpha(alpha),
        .inputdata(input_layer),
        .exp_sum(ss_n),

        .deltafunction_value(deltafunctions_value[31:0]),
        .weight_memory_address(weight_memory_address),
        .neuron_state(layer_state),

        .exp(ns1_ss), 
        .weight(neurons_weight[31:0]),
        .a(output_layer[31:0])

    );

    softmax_neuron #(.NUM_INPUTS(4)) sn2 (

        .clock(clock),
        .neuron_enable(layer_enable),
        .alpha(alpha),
        .inputdata(input_layer),
        .exp_sum(ss_n),

        .deltafunction_value(deltafunctions_value[63:32]),
        .weight_memory_address(weight_memory_address),
        .neuron_state(layer_state),

        .exp(ns2_ss), 
        .weight(neurons_weight[63:32]),
        .a(output_layer[63:32])

    );

        softmax_neuron #(.NUM_INPUTS(4)) sn3 (

        .clock(clock),
        .neuron_enable(layer_enable),
        .alpha(alpha),
        .inputdata(input_layer),
        .exp_sum(ss_n),

        .deltafunction_value(deltafunctions_value[95:64]),
        .weight_memory_address(weight_memory_address),
        .neuron_state(layer_state),

        .exp(ns3_ss), 
        .weight(neurons_weight[95:64]),
        .a(output_layer[95:64])
    );

endmodule

module layer_control #(parameter 
    MAX_NUM_NEURONS = 4,
    NUM_LAYERS = 4
) (

    input clock,
    input [(32*MAX_NUM_NEURONS)-1:0] neurons_weight,
    input [(32*MAX_NUM_NEURONS)-1:0] output_sofmax_layer,
    output reg [$clog2(MAX_NUM_NEURONS)-1:0] weight_memory_address,
    output reg [1:0] layer_state,
    output reg [NUM_LAYERS-1:0] enable_layers,
    output reg [(32*MAX_NUM_NEURONS)-1:0] deltafunction
);
    

    // always @(*) begin

    // end


endmodule