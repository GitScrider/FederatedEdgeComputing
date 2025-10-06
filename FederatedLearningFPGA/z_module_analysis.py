import struct

def ieee754_hex_to_float(hex_string):
    """Convert IEEE754 hexadecimal string to float"""
    try:
        hex_val = hex_string.replace('h', '').replace('0x', '')
        bytes_val = int(hex_val, 16).to_bytes(4, byteorder='big')
        float_val = struct.unpack('>f', bytes_val)[0]
        return float_val
    except Exception as e:
        return f"Error converting {hex_string}: {e}"

def analyze_z_module_execution():
    """Analyze the Z module execution step by step"""
    
    print("=== Z Module Execution Analysis ===")
    print("Test Case 1: Feedforward with inputs [4.0, 3.0, 2.0, 1.0]")
    print("Weights: All 0.5 (3f000000)")
    print("=" * 50)
    
    # Test inputs and expected calculations
    inputs = [4.0, 3.0, 2.0, 1.0]
    weight = 0.5  # 3f000000
    
    print("\n--- Expected Calculations ---")
    expected_sum = 0
    for i, input_val in enumerate(inputs):
        product = input_val * weight
        expected_sum += product
        print(f"Input {i}: {input_val} × {weight} = {product}")
    print(f"Expected Total Sum: {expected_sum}")
    print(f"Expected Final Result (with bias=0): {expected_sum}")
    
    # Actual execution from test output
    print("\n--- Actual Z Module Execution ---")
    print("From test output:")
    
    execution_steps = [
        {"index": 0, "M1_A1": "3f000000", "A1_Z": "3f000000", "Z": "3f000000", "A2_OUT": "00800000"},
        {"index": 1, "M1_A1": "3f800000", "A1_Z": "3f800000", "Z": "3f800000", "A2_OUT": "3f000000"},
        {"index": 2, "M1_A1": "40000000", "A1_Z": "40000000", "Z": "40000000", "A2_OUT": "3f800000"},
        {"index": 3, "M1_A1": "3fc00000", "A1_Z": "3fc00000", "Z": "3fc00000", "A2_OUT": "40000000"},
        {"index": 4, "result": "40600000"}
    ]
    
    for step in execution_steps:
        if "result" in step:
            print(f"Index {step['index']}: result = {step['result']} = {ieee754_hex_to_float(step['result'])}")
        else:
            m1_a1 = ieee754_hex_to_float(step['M1_A1'])
            a1_z = ieee754_hex_to_float(step['A1_Z'])
            z = ieee754_hex_to_float(step['Z'])
            a2_out = ieee754_hex_to_float(step['A2_OUT'])
            
            print(f"Index {step['index']}: M1_A1={step['M1_A1']}({m1_a1}) | A1_Z={step['A1_Z']}({a1_z}) | Z={step['Z']}({z}) | A2_OUT={step['A2_OUT']}({a2_out})")
    
    print("\n--- Analysis of the Problem ---")
    print("Looking at the execution steps:")
    print("1. Index 0: Z starts at 0, M1_A1 = 0.5 (4.0 × 0.5), A1_Z = 0.5 (0 + 0.5), Z becomes 0.5")
    print("2. Index 1: Z = 0.5, M1_A1 = 1.0 (3.0 × 0.5), A1_Z = 1.0 (0.5 + 1.0), Z becomes 1.0")
    print("3. Index 2: Z = 1.0, M1_A1 = 2.0 (2.0 × 0.5), A1_Z = 2.0 (1.0 + 2.0), Z becomes 2.0")
    print("4. Index 3: Z = 2.0, M1_A1 = 1.5 (1.0 × 0.5), A1_Z = 1.5 (2.0 + 1.5), Z becomes 1.5")
    print("5. Final result = 3.5")
    
    print("\n--- The Issue ---")
    print("The problem is in step 4 (Index 3):")
    print("- Z should be 2.0 from previous step")
    print("- M1_A1 should be 1.5 (1.0 × 0.5)")
    print("- A1_Z should be 3.5 (2.0 + 1.5)")
    print("- But Z becomes 1.5 instead of 3.5")
    print("- This suggests Z is being overwritten with M1_A1 instead of A1_Z")
    
    print("\n--- Root Cause Analysis ---")
    print("Looking at the Z module code:")
    print("Z = A1_Z;  // This should work correctly")
    print("But the test output shows Z = M1_A1 instead of Z = A1_Z")
    print("This suggests a timing issue or the adder A1 is not working correctly")
    
    print("\n--- Verification ---")
    print("Expected final Z value: 5.0")
    print("Actual final Z value: 3.5")
    print("Difference: 1.5 (which is exactly the last M1_A1 value)")
    print("This confirms that Z is being set to M1_A1 instead of A1_Z")

if __name__ == "__main__":
    analyze_z_module_execution()
