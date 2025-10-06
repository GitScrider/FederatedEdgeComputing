`timescale 1ns/1ps

module tb_hidden_neuron;

    // Parâmetro do número de entradas
    parameter NUM_INPUTS = 4;

    // Sinais de teste
    reg clock;
    reg neuron_enable;
    reg [(32*NUM_INPUTS)-1:0] inputdata;
    reg [31:0] alpha;
    reg [31:0] deltafunction_value;
    reg [$clog2(NUM_INPUTS)-1:0] weight_memory_address;
    reg [1:0] neuron_state;
    
    wire [31:0] weight;
    wire [31:0] a;

    // Instância do módulo `hidden_neuron`
    hidden_neuron #( 
        .NUM_INPUTS(NUM_INPUTS)
    ) uut (
        .clock(clock),
        .neuron_enable(neuron_enable),
        .inputdata(inputdata),
        .alpha(alpha),
        .deltafunction_value(deltafunction_value),
        .weight_memory_address(weight_memory_address),
        .neuron_state(neuron_state),
        .weight(weight),
        .a(a)
    );

    // Clock de 50 MHz (período de 20 ns)
    always #10 clock = ~clock;

    // Geração de estímulos
    initial begin
        $display("=== Hidden Neuron Testbench ===");
        $display("Function: Multi-state neuron with feedforward, delta function, and backpropagation");
        $display("States: 00=Feedforward, 01=Delta, 10=Backprop, 11=Idle");
        $display("==================================================");
        
        // Inicialização
        clock = 0;
        neuron_enable = 0;
        inputdata = 0;
        alpha = 0;
        deltafunction_value = 0;
        weight_memory_address = 0;
        neuron_state = 2'b11; // Idle state
        
        #20;
        
        // Test Case 1: Feedforward State (State 00)
        $display("\n--- Test Case 1: Feedforward State ---");
        $display("Testing feedforward calculation with ReLU activation");
        
        neuron_enable = 1;
        neuron_state = 2'b00; // Feedforward state
        inputdata = {32'h40800000, 32'h40400000, 32'h40000000, 32'h3f800000}; // [4.0, 3.0, 2.0, 1.0]
        
        // Wait for feedforward to complete (multiple clock cycles)
        #200;
        $display("Feedforward completed. Output a = %h", a);
        
        // Test Case 2: Delta Function State (State 01)
        $display("\n--- Test Case 2: Delta Function State ---");
        $display("Testing weight reading from memory");
        
        neuron_state = 2'b01; // Delta function state
        weight_memory_address = 2; // Read weight from address 2
        
        #40;
        $display("Delta function completed. Weight read = %h", weight);
        
        // Test Case 3: Backpropagation State (State 10)
        $display("\n--- Test Case 3: Backpropagation State ---");
        $display("Testing weight update with backpropagation");
        
        neuron_state = 2'b10; // Backpropagation state
        alpha = 32'h3dcccccd; // 0.1 (learning rate)
        deltafunction_value = 32'h3f800000; // 1.0
        weight_memory_address = 1; // Update weight at address 1
        
        #40;
        $display("Backpropagation completed. Updated weight = %h", weight);
        
        // Test Case 4: Multiple Feedforward Cycles
        $display("\n--- Test Case 4: Multiple Feedforward Cycles ---");
        $display("Testing multiple feedforward operations");
        
        neuron_state = 2'b00; // Feedforward state
        inputdata = {32'h41200000, 32'h41000000, 32'h40a00000, 32'h40800000}; // [10.0, 8.0, 5.0, 4.0]
        
        #200;
        $display("Second feedforward completed. Output a = %h", a);
        
        // Test Case 5: Edge Cases
        $display("\n--- Test Case 5: Edge Cases ---");
        $display("Testing with zero inputs and negative values");
        
        inputdata = {32'h00000000, 32'h80000000, 32'hc0000000, 32'h40000000}; // [0.0, -0.0, -2.0, 2.0]
        
        #200;
        $display("Edge case feedforward completed. Output a = %h", a);
        
        // Test Case 6: Disable/Enable Functionality
        $display("\n--- Test Case 6: Disable/Enable Functionality ---");
        $display("Testing neuron enable/disable behavior");
        
        neuron_enable = 0;
        neuron_state = 2'b00;
        inputdata = {32'h40400000, 32'h40400000, 32'h40400000, 32'h40400000}; // All 3.0
        
        #100;
        $display("Neuron disabled. Output a = %h (should remain unchanged)", a);
        
        neuron_enable = 1;
        #100;
        $display("Neuron re-enabled. Output a = %h", a);
        
        // Test Case 7: State Transitions
        $display("\n--- Test Case 7: State Transitions ---");
        $display("Testing rapid state transitions");
        
        neuron_state = 2'b01; // Delta
        weight_memory_address = 0;
        #20;
        
        neuron_state = 2'b10; // Backprop
        weight_memory_address = 3;
        alpha = 32'h3e4ccccd; // 0.2
        deltafunction_value = 32'h40000000; // 2.0
        #20;
        
        neuron_state = 2'b00; // Feedforward
        inputdata = {32'h3f800000, 32'h3f800000, 32'h3f800000, 32'h3f800000}; // All 1.0
        #100;
        
        $display("State transitions completed. Final output a = %h", a);
        
        // Test Case 8: Idle State Reset
        $display("\n--- Test Case 8: Idle State Reset ---");
        $display("Testing idle state reset functionality");
        
        neuron_state = 2'b11; // Idle state
        #40;
        $display("Idle state completed. All internal states should be reset");
        
        $display("\n=== Hidden Neuron Test Complete ===");
        $finish;
    end

    // Monitor changes
    initial begin
        $monitor("Time: %t | State: %b | Enable: %b | Output a: %h | Weight: %h", 
                 $time, neuron_state, neuron_enable, a, weight);
    end

endmodule
