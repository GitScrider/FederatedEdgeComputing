`timescale 1ns/1ns

module tb_Z;

    parameter NUM_INPUTS = 4;
    
    reg clock;
    reg reset;
    reg enable;
    reg [31:0] neuron_input;
    reg [31:0] weight;
    reg [31:0] bias;
    wire [31:0] result;

    // Instantiate the Z module
    Z #(NUM_INPUTS) uut (
        .clock(clock),
        .reset(reset),
        .enable(enable),
        .neuron_input(neuron_input),
        .weight(weight),
        .bias(bias),
        .result(result)
    );

    // Clock generation
    always #5 clock = ~clock;

    // Test stimulus
    initial begin
        $display("=== Z Module Testbench (Weighted Sum Calculator) ===");
        $display("NUM_INPUTS = %d", NUM_INPUTS);
        $display("Expected: Z = Σ(weight × input) + bias");
        $display("==================================================");
        
        // Initialize
        clock = 0;
        reset = 1;
        enable = 0;
        neuron_input = 0;
        weight = 0;
        bias = 0;
        
        // Reset
        #10 reset = 0;
        #10;
        
        // Test Case 1: Simple calculation
        $display("\n--- Test Case 1: Simple Weighted Sum ---");
        $display("Inputs: [1.0, 2.0, 3.0, 4.0]");
        $display("Weight: 2.0");
        $display("Bias: 1.0");
        $display("Expected: 2.0×1.0 + 2.0×2.0 + 2.0×3.0 + 2.0×4.0 + 1.0 = 21.0");
        
        enable = 1;
        bias = 32'h3F800000; // 1.0
        weight = 32'h40000000; // 2.0
        
        // Input 1
        neuron_input = 32'h3F800000; // 1.0
        #10;
        
        // Input 2
        neuron_input = 32'h40000000; // 2.0
        #10;
        
        // Input 3
        neuron_input = 32'h40400000; // 3.0
        #10;
        
        // Input 4
        neuron_input = 32'h40800000; // 4.0
        #10;
        
        // Wait for result
        #10;
        $display("Result: %h (Expected: ~21.0)", result);
        
        // Test Case 2: All zeros
        $display("\n--- Test Case 2: All Zeros ---");
        $display("Inputs: [0.0, 0.0, 0.0, 0.0]");
        $display("Weight: 1.0");
        $display("Bias: 0.0");
        $display("Expected: 0.0");
        
        enable = 0;
        #10;
        enable = 1;
        bias = 32'h00000000; // 0.0
        weight = 32'h3F800000; // 1.0
        
        // Input 1
        neuron_input = 32'h00000000; // 0.0
        #10;
        
        // Input 2
        neuron_input = 32'h00000000; // 0.0
        #10;
        
        // Input 3
        neuron_input = 32'h00000000; // 0.0
        #10;
        
        // Input 4
        neuron_input = 32'h00000000; // 0.0
        #10;
        
        // Wait for result
        #10;
        $display("Result: %h (Expected: 0.0)", result);
        
        // Test Case 3: Negative values
        $display("\n--- Test Case 3: Negative Values ---");
        $display("Inputs: [-1.0, -2.0, -3.0, -4.0]");
        $display("Weight: 1.5");
        $display("Bias: -0.5");
        $display("Expected: 1.5×(-1.0) + 1.5×(-2.0) + 1.5×(-3.0) + 1.5×(-4.0) + (-0.5) = -15.5");
        
        enable = 0;
        #10;
        enable = 1;
        bias = 32'hBF000000; // -0.5
        weight = 32'h3FC00000; // 1.5
        
        // Input 1
        neuron_input = 32'hBF800000; // -1.0
        #10;
        
        // Input 2
        neuron_input = 32'hC0000000; // -2.0
        #10;
        
        // Input 3
        neuron_input = 32'hC0400000; // -3.0
        #10;
        
        // Input 4
        neuron_input = 32'hC0800000; // -4.0
        #10;
        
        // Wait for result
        #10;
        $display("Result: %h (Expected: ~-15.5)", result);
        
        // Test Case 4: Mixed positive/negative
        $display("\n--- Test Case 4: Mixed Values ---");
        $display("Inputs: [1.0, -1.0, 0.5, -0.5]");
        $display("Weight: 0.5");
        $display("Bias: 0.0");
        $display("Expected: 0.5×1.0 + 0.5×(-1.0) + 0.5×0.5 + 0.5×(-0.5) + 0.0 = 0.0");
        
        enable = 0;
        #10;
        enable = 1;
        bias = 32'h00000000; // 0.0
        weight = 32'h3F000000; // 0.5
        
        // Input 1
        neuron_input = 32'h3F800000; // 1.0
        #10;
        
        // Input 2
        neuron_input = 32'hBF800000; // -1.0
        #10;
        
        // Input 3
        neuron_input = 32'h3F000000; // 0.5
        #10;
        
        // Input 4
        neuron_input = 32'hBF000000; // -0.5
        #10;
        
        // Wait for result
        #10;
        $display("Result: %h (Expected: ~0.0)", result);
        
        // Test Case 5: Reset during calculation
        $display("\n--- Test Case 5: Reset During Calculation ---");
        $display("Testing reset functionality during computation");
        
        enable = 0;
        #10;
        enable = 1;
        bias = 32'h3F800000; // 1.0
        weight = 32'h3F800000; // 1.0
        
        // Input 1
        neuron_input = 32'h3F800000; // 1.0
        #10;
        
        // Input 2
        neuron_input = 32'h40000000; // 2.0
        #10;
        
        // Reset in middle
        reset = 1;
        #10;
        reset = 0;
        #10;
        
        $display("Reset applied - calculation should be interrupted");
        
        $display("\n=== Z Module Test Complete ===");
        $finish;
    end

    // Monitor changes
    initial begin
        $monitor("Time: %t | Enable: %b | Index: %d | Input: %h | Weight: %h | Result: %h", 
                 $time, enable, uut.index, neuron_input, weight, result);
    end

endmodule
