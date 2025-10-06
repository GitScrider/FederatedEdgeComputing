import struct

def ieee754_hex_to_float(hex_string):
    """Convert IEEE754 hexadecimal string to float"""
    try:
        # Remove 'h' suffix if present and convert to int
        hex_val = hex_string.replace('h', '').replace('0x', '')
        # Convert hex to bytes and then to float
        bytes_val = int(hex_val, 16).to_bytes(4, byteorder='big')
        float_val = struct.unpack('>f', bytes_val)[0]
        return float_val
    except Exception as e:
        return f"Error converting {hex_string}: {e}"

def analyze_all_hidden_neuron_tests():
    """Analyze all 8 test cases from the hidden neuron testbench with decimal conversions"""
    
    print("=== Complete Hidden Neuron Test Results Analysis ===")
    print("All 8 Test Cases with IEEE754 to Decimal Conversion")
    print("=" * 60)
    
    # Test inputs from the testbench
    test_inputs = {
        "3f800000": "1.0",
        "40000000": "2.0", 
        "40400000": "3.0",
        "40800000": "4.0",
        "40a00000": "5.0",
        "40c00000": "6.0",
        "40e00000": "7.0",
        "41000000": "8.0",
        "41200000": "10.0",
        "41400000": "12.0",
        "00000000": "0.0",
        "80000000": "-0.0",
        "c0000000": "-2.0",
        "3f000000": "0.5",
        "3fc00000": "1.5",
        "40200000": "2.5",
        "40600000": "3.5",
        "40900000": "4.5",
        "40d00000": "6.5",
        "41180000": "9.5",
        "3dcccccd": "0.1 (learning rate)",
        "3e4ccccd": "0.2 (learning rate)",
        "00800000": "smallest positive denormalized",
        "7f800000": "positive infinity",
        "ff800000": "negative infinity"
    }
    
    print("\n--- Input Values Used in Tests ---")
    for hex_val, description in test_inputs.items():
        decimal_val = ieee754_hex_to_float(hex_val)
        print(f"{hex_val} = {decimal_val} ({description})")
    
    # Complete analysis of all 8 test cases
    print("\n--- Complete Test Cases Analysis ---")
    
    # Test Case 1: Feedforward State
    print("\n** Test Case 1: Feedforward State (State 00) **")
    print("Inputs: [4.0, 3.0, 2.0, 1.0]")
    print("Final Output: 40600000 =", ieee754_hex_to_float("40600000"), "(≈ 3.5)")
    print("Analysis: Initial feedforward calculation with ReLU activation")
    
    # Test Case 2: Delta Function State
    print("\n** Test Case 2: Delta Function State (State 01) **")
    print("Weight Memory Address: 2")
    print("Weight Read: 3f000000 =", ieee754_hex_to_float("3f000000"), "(0.5)")
    print("Analysis: Testing weight reading from memory")
    
    # Test Case 3: Backpropagation State
    print("\n** Test Case 3: Backpropagation State (State 10) **")
    print("Alpha (learning rate): 3dcccccd =", ieee754_hex_to_float("3dcccccd"), "(0.1)")
    print("Delta function value: 3f800000 =", ieee754_hex_to_float("3f800000"), "(1.0)")
    print("Weight Memory Address: 1")
    print("Updated Weight: 3f000000 =", ieee754_hex_to_float("3f000000"), "(0.5)")
    print("Analysis: Testing weight update with backpropagation")
    
    # Test Case 4: Multiple Feedforward Cycles
    print("\n** Test Case 4: Multiple Feedforward Cycles (State 00) **")
    print("Inputs: [10.0, 8.0, 5.0, 4.0]")
    print("Final Output: 41400000 =", ieee754_hex_to_float("41400000"), "(≈ 12.0)")
    print("Analysis: Testing multiple feedforward operations with different inputs")
    
    # Test Case 5: Edge Cases
    print("\n** Test Case 5: Edge Cases (State 00) **")
    print("Inputs: [0.0, -0.0, -2.0, 2.0]")
    print("Final Output: 41180000 =", ieee754_hex_to_float("41180000"), "(≈ 9.5)")
    print("Analysis: Testing with zero inputs and negative values")
    
    # Test Case 6: Disable/Enable Functionality
    print("\n** Test Case 6: Disable/Enable Functionality (State 00) **")
    print("Inputs: [3.0, 3.0, 3.0, 3.0] (all 3.0)")
    print("Neuron Disabled: Output remains 41180000 =", ieee754_hex_to_float("41180000"), "(≈ 9.5)")
    print("Neuron Re-enabled: Output becomes 40c00000 =", ieee754_hex_to_float("40c00000"), "(≈ 6.0)")
    print("Analysis: Testing neuron enable/disable behavior")
    
    # Test Case 7: State Transitions
    print("\n** Test Case 7: State Transitions **")
    print("Rapid transitions: Delta(01) → Backprop(10) → Feedforward(00)")
    print("Inputs: [1.0, 1.0, 1.0, 1.0] (all 1.0)")
    print("Final Output: 40c00000 =", ieee754_hex_to_float("40c00000"), "(≈ 6.0)")
    print("Analysis: Testing rapid state transitions")
    
    # Test Case 8: Idle State Reset
    print("\n** Test Case 8: Idle State Reset (State 11) **")
    print("State: Idle (11)")
    print("Analysis: Testing idle state reset functionality - all internal states should be reset")
    
    # Summary of all outputs
    print("\n--- Summary of All Test Outputs ---")
    outputs = {
        "Test 1 (Feedforward)": "40600000 = 3.5",
        "Test 2 (Delta)": "3f000000 = 0.5 (weight read)",
        "Test 3 (Backprop)": "3f000000 = 0.5 (updated weight)",
        "Test 4 (Multiple FF)": "41400000 = 12.0",
        "Test 5 (Edge Cases)": "41180000 = 9.5",
        "Test 6 (Disable/Enable)": "40c00000 = 6.0 (after re-enable)",
        "Test 7 (State Transitions)": "40c00000 = 6.0",
        "Test 8 (Idle Reset)": "State reset - no output"
    }
    
    for test_name, output in outputs.items():
        print(f"{test_name}: {output}")
    
    print("\n=== Complete Analysis of All 8 Test Cases ===")

if __name__ == "__main__":
    analyze_all_hidden_neuron_tests()
