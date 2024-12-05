import RPi.GPIO as GPIO
import time
import pandas as pd
import csv

ct = 10000
instructionSet = {
    "00000000": "BRK",       # Break
    "00000001": "ORA",       # Logical OR Accumulator
    "00000101": "ORA",       # Logical OR Accumulator
    "00000110": "ASL",       # Arithmetic Shift Left
    "00001000": "PHP",       # Push Processor Status
    "00001001": "ORA",       # Logical OR Accumulator
    "00001010": "ASL",       # Arithmetic Shift Left
    "00001101": "ORA",       # Logical OR Accumulator
    "00001110": "ASL",       # Arithmetic Shift Left
    "00010000": "BPL",       # Branch if Positive
    "00010001": "ORA",       # Logical OR Accumulator
    "00010101": "ORA",       # Logical OR Accumulator
    "00010110": "ASL",       # Arithmetic Shift Left
    "00011000": "CLC",       # Clear Carry Flag
    "00011001": "ORA",       # Logical OR Accumulator
    "00011101": "ORA",       # Logical OR Accumulator
    "00011110": "ASL",       # Arithmetic Shift Left
    "00100000": "JSR",       # Jump to Subroutine
    "00100001": "AND",       # Logical AND Accumulator
    "00100100": "BIT",       # Bit Test
    "00100101": "AND",       # Logical AND Accumulator
    "00100110": "ROL",       # Rotate Left
    "00101000": "PLP",       # Pull Processor Status
    "00101001": "AND",       # Logical AND Accumulator
    "00101010": "ROL",       # Rotate Left
    "00101100": "BIT",       # Bit Test
    "00101101": "AND",       # Logical AND Accumulator
    "00101110": "ROL",       # Rotate Left
    "00110000": "BMI",       # Branch if Minus
    "00110001": "AND",       # Logical AND Accumulator
    "00110101": "AND",       # Logical AND Accumulator
    "00110110": "ROL",       # Rotate Left
    "00111000": "SEC",       # Set Carry Flag
    "00111001": "AND",       # Logical AND Accumulator
    "00111101": "AND",       # Logical AND Accumulator
    "00111110": "ROL",       # Rotate Left
    "01000000": "RTI",       # Return from Interrupt
    "01000001": "EOR",       # Logical Exclusive OR
    "01000101": "EOR",       # Logical Exclusive OR
    "01000110": "LSR",       # Logical Shift Right
    "01001000": "PHA",       # Push Accumulator
    "01001001": "EOR",       # Logical Exclusive OR
    "01001010": "LSR",       # Logical Shift Right
    "01001100": "JMP",       # Jump
    "01001101": "EOR",       # Logical Exclusive OR
    "01001110": "LSR",       # Logical Shift Right
    "01010000": "BCC",       # Branch if Carry Clear
    "01010001": "EOR",       # Logical Exclusive OR
    "01010101": "EOR",       # Logical Exclusive OR
    "01010110": "LSR",       # Logical Shift Right
    "01011000": "CLI",       # Clear Interrupt Disable
    "01011001": "EOR",       # Logical Exclusive OR
    "01011101": "EOR",       # Logical Exclusive OR
    "01011110": "LSR",       # Logical Shift Right
    "01100000": "RTS",       # Return from Subroutine
    "01100001": "ADC",       # Add with Carry
    "01100100": "STZ",       # Store Zero
    "01100101": "ADC",       # Add with Carry
    "01100110": "ROR",       # Rotate Right
    "01101000": "PLA",       # Pull Accumulator
    "01101001": "ADC",       # Add with Carry
    "01101010": "ROR",       # Rotate Right
    "01101100": "JMP",       # Jump
    "01101101": "ADC",       # Add with Carry
    "01101110": "ROR",       # Rotate Right
    "01110000": "BVS",       # Branch if Overflow Set
    "01110001": "ADC",       # Add with Carry
    "01110101": "ADC",       # Add with Carry
    "01110110": "ROR",       # Rotate Right
    "01111000": "SEI",       # Set Interrupt Disable
    "01111001": "ADC",       # Add with Carry
    "01111101": "ADC",       # Add with Carry
    "01111110": "ROR",       # Rotate Right
    "10000000": "BRA",       # Branch Always
    "10000001": "STA",       # Store Accumulator
    "10000100": "STY",       # Store Y Register
    "10000101": "STA",       # Store Accumulator
    "10000110": "STX",       # Store X Register
    "10001000": "DEY",       # Decrement Y Register
    "10001010": "TXA",       # Transfer X to Accumulator
    "10001100": "STY",       # Store Y Register
    "10001101": "STA",       # Store Accumulator
    "10001110": "STX",       # Store X Register
    "10010000": "BCC",       # Branch if Carry Clear
    "10010001": "STA",       # Store Accumulator
    "10010100": "STY",       # Store Y Register
    "10010101": "STA",       # Store Accumulator
    "10010110": "STX",       # Store X Register
    "10011000": "TYA",       # Transfer Y to A
    "10011001": "STA",       # Store Accumulator
    "10011010": "TXS",       # Transfer X to Stack Pointer
    "10011101": "STA",       # Store Accumulator
    "10100000": "LDY",       # Load Y Register
    "10100001": "LDA",       # Load Accumulator
    "10100010": "LDX",       # Load X Register
    "10100100": "LDY",       # Load Y Register
    "10100101": "LDA",       # Load Accumulator
    "10100110": "LDX",       # Load X Register
    "10101000": "TAY",       # Transfer A to Y
    "10101001": "LDA",       # Load Accumulator
    "10101010": "TAX",       # Transfer A to X
    "10101100": "LDY",       # Load Y Register
    "10101101": "LDA",       # Load Accumulator
    "10101110": "LDX",       # Load X Register
    "10110000": "BCS",       # Branch if Carry Set
    "10110001": "LDA",       # Load Accumulator
    "10110100": "LDY",       # Load Y Register
    "10110101": "LDA",       # Load Accumulator
    "10110110": "LDX",       # Load X Register
    "10111000": "CLV",       # Clear Overflow Flag
    "10111001": "LDA",       # Load Accumulator
    "10111010": "TSX",       # Transfer Stack Pointer to X
    "10111100": "LDY",       # Load Y Register
    "10111101": "LDA",       # Load Accumulator
    "10111110": "LDX",       # Load X Register
    "11000000": "CPY",       # Compare Y with Accumulator
    "11000001": "CMP",       # Compare Accumulator
    "11000100": "CPY",       # Compare Y with Accumulator
    "11000101": "CMP",       # Compare Accumulator
    "11000110": "DEC",       # Decrement
    "11001000": "INY",       # Increment Y Register
    "11001001": "CMP",       # Compare Accumulator
    "11001010": "DEX",       # Decrement X Register
    "11001100": "CPY",       # Compare Y with Accumulator
    "11001101": "CMP",       # Compare Accumulator
    "11001110": "DEC",       # Decrement
    "11010000": "BNE",       # Branch if Not Equal
    "11010001": "CMP",       # Compare Accumulator
    "11010101": "CMP",       # Compare Accumulator
    "11010110": "DEC",       # Decrement
    "11011000": "CLD",       # Clear Decimal Mode
    "11011001": "CMP",       # Compare Accumulator
    "11011101": "CMP",       # Compare Accumulator
    "11011110": "DEC",       # Decrement
    "11100000": "CPX",       # Compare X with Accumulator
    "11100001": "SBC",       # Subtract with Borrow
    "11100010": "CPX",       # Compare X with Accumulator
    "11100011": "SBC",       # Subtract with Borrow
    "11100100": "CPX",       # Compare X with Accumulator
    "11100101": "SBC",       # Subtract with Borrow
    "11100110": "INC",       # Increment
    "11101000": "INX",       # Increment X Register
    "11101001": "SBC",       # Subtract with Borrow
    "11101010": "NOP",       # No Operation
    "11101100": "CPX",       # Compare X with Accumulator
    "11101101": "SBC",       # Subtract with Borrow
    "11101110": "INC",       # Increment
    "11110000": "BEQ",       # Branch if Equal
    "11110001": "SBC",       # Subtract with Borrow
    "11110010": "SBC",       # Subtract with Borrow
    "11110011": "SBC",       # Subtract with Borrow
    "11110100": "SBC",       # Subtract with Borrow
    "11110101": "SBC",       # Subtract with Borrow
    "11110110": "INC",       # Increment
    "11111000": "SED",       # Set Decimal Mode
    "11111001": "SBC",       # Subtract with Borrow
    "11111010": "SBC",       # Subtract with Borrow
    "11111101": "SBC",       # Subtract with Borrow
    "11111110": "INC"        # Increment
}

lue = 0
fieldnames = ["instruction","x_value"]

GPIO.cleanup()
pins = [6,13,19,26,12,16,20,21]

print(f"Pins: {pins}")

latch_pin = 5
GPIO.setmode(GPIO.BCM)
for pin in pins:
    GPIO.setup(pin, GPIO.IN)
    
GPIO.setup(latch_pin, GPIO.OUT)
GPIO.output(latch_pin, GPIO.LOW)

x_value = 0
hz = 1000
print(f"Sampling at {hz} hz")

def clear_csv_file(filename, fieldnames):
    """Clear the CSV file and write the header"""
    with open(filename, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()

try:
    with open('data.csv', 'w', newline='') as f: 
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        
        instruction_count = 0  # Add a counter for instructions
        
        while True:
            try:
                value = [GPIO.input(pin) for pin in pins]
                time.sleep(1/hz)
                value = "".join([str(x) for x in value])
                
                if value in instructionSet:
                    code = instructionSet[value]            
                    x_value += 1
                    
                    # Increment instruction count
                    instruction_count += 1
                    
                    if instruction_count % ct == 0:
                        f.close()  # Close the current file
                        clear_csv_file('data.csv', fieldnames)
                        f = open('data.csv', 'a', newline='')
                        writer = csv.DictWriter(f, fieldnames=fieldnames)
                else:
                    continue
                
                info = {
                    "x_value": x_value,
                    "instruction": code
                }
                
                writer.writerow(info)
                f.flush()  # Force write to disk
                
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f"Error: {e}")
                
except Exception as e:
    print(f"Outer exception: {e}")
finally:
    GPIO.cleanup()
