module test_adder;
    reg [31:0] A, B;
    wire [31:0] result;
    
    ieee754_adder #(32) adder (
        .A(A),
        .B(B),
        .result(result)
    );
    
    initial begin
        $display("=== IEEE754 Adder Test ===");
        
        // Test 1: 0.5 + 1.0 = 1.5
        A = 32'h3f000000; // 0.5
        B = 32'h3f800000; // 1.0
        #10;
        $display("Test 1: %h + %h = %h (Expected: 1.5)", A, B, result);
        
        // Test 2: 1.5 + 1.0 = 2.5
        A = 32'h3fc00000; // 1.5
        B = 32'h3f800000; // 1.0
        #10;
        $display("Test 2: %h + %h = %h (Expected: 2.5)", A, B, result);
        
        // Test 3: 2.5 + 1.5 = 4.0
        A = 32'h40200000; // 2.5
        B = 32'h3fc00000; // 1.5
        #10;
        $display("Test 3: %h + %h = %h (Expected: 4.0)", A, B, result);
        
        // Test 4: 4.0 + 2.0 = 6.0
        A = 32'h40800000; // 4.0
        B = 32'h40000000; // 2.0
        #10;
        $display("Test 4: %h + %h = %h (Expected: 6.0)", A, B, result);
        
        $finish;
    end
endmodule
