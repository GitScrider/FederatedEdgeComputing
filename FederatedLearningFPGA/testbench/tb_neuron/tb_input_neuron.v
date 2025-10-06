`timescale 1ns/1ns

module tb_input_neuron;

    reg clock;
    reg neuron_enable;
    reg [31:0] inputdata;
    wire [31:0] outputdata;

    // Instantiate the input_neuron module
    input_neuron uut (
        .clock(clock),
        .neuron_enable(neuron_enable),
        .inputdata(inputdata),
        .outputdata(outputdata)
    );

    // Clock generation
    always #5 clock = ~clock;

    // Test stimulus
    initial begin
        $display("=== Input Neuron Testbench (Data Latch) ===");
        $display("Function: Holds input data when enabled");
        $display("Behavior: outputdata = inputdata when neuron_enable = 1");
        $display("=============================================");
        
        // Initialize
        clock = 0;
        neuron_enable = 0;
        inputdata = 0;
        
        #10;
        
        // Test Case 1: Basic data latching
        $display("\n--- Test Case 1: Basic Data Latching ---");
        $display("Testing enable/disable functionality");
        
        // Set input but keep disabled
        inputdata = 32'h40400000; // 3.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h (should be 0)", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Enable and check output
        neuron_enable = 1;
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h (should match input)", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Test Case 2: Multiple data changes
        $display("\n--- Test Case 2: Multiple Data Changes ---");
        $display("Testing data updates while enabled");
        
        inputdata = 32'h3F800000; // 1.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        inputdata = 32'h40000000; // 2.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        inputdata = 32'h41200000; // 10.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Test Case 3: Disable functionality
        $display("\n--- Test Case 3: Disable Functionality ---");
        $display("Testing that output holds when disabled");
        
        neuron_enable = 0;
        inputdata = 32'hC0400000; // -3.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h (should hold previous value)", 
                 $time, neuron_enable, inputdata, outputdata);
        
        inputdata = 32'hBF800000; // -1.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h (should still hold previous value)", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Test Case 4: Re-enable functionality
        $display("\n--- Test Case 4: Re-enable Functionality ---");
        $display("Testing that output updates when re-enabled");
        
        neuron_enable = 1;
        #10;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h (should update to current input)", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Test Case 5: Edge cases
        $display("\n--- Test Case 5: Edge Cases ---");
        $display("Testing special IEEE-754 values");
        
        // Zero
        inputdata = 32'h00000000; // 0.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h (0.0) | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Negative zero
        inputdata = 32'h80000000; // -0.0
        #10;
        $display("Time: %t | Enable: %b | Input: %h (-0.0) | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Infinity
        inputdata = 32'h7F800000; // +inf
        #10;
        $display("Time: %t | Enable: %b | Input: %h (+inf) | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Negative infinity
        inputdata = 32'hFF800000; // -inf
        #10;
        $display("Time: %t | Enable: %b | Input: %h (-inf) | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // NaN
        inputdata = 32'h7FFFFFFF; // NaN
        #10;
        $display("Time: %t | Enable: %b | Input: %h (NaN) | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        // Test Case 6: Rapid enable/disable
        $display("\n--- Test Case 6: Rapid Enable/Disable ---");
        $display("Testing rapid state changes");
        
        inputdata = 32'h40400000; // 3.0
        
        neuron_enable = 0;
        #5;
        neuron_enable = 1;
        #5;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        neuron_enable = 0;
        #5;
        neuron_enable = 1;
        #5;
        $display("Time: %t | Enable: %b | Input: %h | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
        
        $display("\n=== Input Neuron Test Complete ===");
        $finish;
    end

    // Monitor changes
    initial begin
        $monitor("Time: %t | Enable: %b | Input: %h | Output: %h", 
                 $time, neuron_enable, inputdata, outputdata);
    end

endmodule 