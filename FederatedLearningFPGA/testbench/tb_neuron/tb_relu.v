`timescale 1ns/1ns

module tb_relu;

    reg [31:0] Z;
    wire [31:0] a;

    // Instantiate the ReLU module
    relu uut (
        .Z(Z),
        .a(a)
    );

    // Test cases
    initial begin
        $display("=== ReLU Activation Function Testbench ===");
        $display("Time | Input (Z) | Output (a) | Expected | Status");
        $display("-----|-----------|------------|----------|--------");
        
        // Test 1: Positive number
        #10 Z = 32'h40400000; // 3.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h40400000, 
                   (a == 32'h40400000) ? "PASS" : "FAIL");
        
        // Test 2: Negative number
        #10 Z = 32'hC0400000; // -3.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h00000000, 
                   (a == 32'h00000000) ? "PASS" : "FAIL");
        
        // Test 3: Zero
        #10 Z = 32'h00000000; // 0.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h00000000, 
                   (a == 32'h00000000) ? "PASS" : "FAIL");
        
        // Test 4: Small positive number
        #10 Z = 32'h3F800000; // 1.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h3F800000, 
                   (a == 32'h3F800000) ? "PASS" : "FAIL");
        
        // Test 5: Small negative number
        #10 Z = 32'hBF800000; // -1.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h00000000, 
                   (a == 32'h00000000) ? "PASS" : "FAIL");
        
        // Test 6: Large positive number
        #10 Z = 32'h41200000; // 10.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h41200000, 
                   (a == 32'h41200000) ? "PASS" : "FAIL");
        
        // Test 7: Large negative number
        #10 Z = 32'hC1200000; // -10.0
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h00000000, 
                   (a == 32'h00000000) ? "PASS" : "FAIL");
        
        // Test 8: Very small positive number
        #10 Z = 32'h3A83126F; // 0.001
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h3A83126F, 
                   (a == 32'h3A83126F) ? "PASS" : "FAIL");
        
        // Test 9: Very small negative number
        #10 Z = 32'hBA83126F; // -0.001
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h00000000, 
                   (a == 32'h00000000) ? "PASS" : "FAIL");
        
        // Test 10: Positive infinity
        #10 Z = 32'h7F800000; // +inf
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h7F800000, 
                   (a == 32'h7F800000) ? "PASS" : "FAIL");
        
        // Test 11: Negative infinity
        #10 Z = 32'hFF800000; // -inf
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h00000000, 
                   (a == 32'h00000000) ? "PASS" : "FAIL");
        
        // Test 12: NaN
        #10 Z = 32'h7FFFFFFF; // NaN
        #5 $display("%4t | %h | %h | %h | %s", $time, Z, a, 32'h7FFFFFFF, 
                   (a == 32'h7FFFFFFF) ? "PASS" : "FAIL");
        
        $display("=== ReLU Test Complete ===");
        $finish;
    end

    // Monitor changes
    initial begin
        $monitor("Time: %t | Z: %h | a: %h", $time, Z, a);
    end

endmodule 