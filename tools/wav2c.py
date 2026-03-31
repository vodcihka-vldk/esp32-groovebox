#!/usr/bin/env python3
"""
Convert WAV file to C array for ESP32
Usage: python wav2c.py input.wav output.h
"""

import sys
import struct
import wave

def wav_to_c_array(input_file, output_file, array_name="sample_data"):
    try:
        with wave.open(input_file, 'rb') as wav:
            # Get WAV properties
            channels = wav.getnchannels()
            sample_width = wav.getsampwidth()
            frame_rate = wav.getframerate()
            n_frames = wav.getnframes()
            
            print(f"Input: {channels} channels, {sample_width*8} bits, {frame_rate} Hz, {n_frames} frames")
            
            # Read all frames
            frames = wav.readframes(n_frames)
            
            # Convert to mono if stereo
            if channels == 2:
                frames = bytearray()
                wav.rewind()
                for _ in range(n_frames):
                    frame = wav.readframes(1)
                    if len(frame) == 4:  # 16-bit stereo
                        left = struct.unpack('<h', frame[:2])[0]
                        right = struct.unpack('<h', frame[2:])[0]
                        mono = (left + right) // 2
                        frames.extend(struct.pack('<h', mono))
                n_frames = len(frames) // 2
            elif channels != 1:
                raise ValueError("Only mono or stereo supported")
            
            # Convert to 16-bit if needed
            if sample_width != 2:
                if sample_width == 1:  # 8-bit
                    frames = bytes([b * 256 - 128 for b in frames])
                else:
                    raise ValueError("Only 8-bit and 16-bit supported")
            
            # Write C array
            with open(output_file, 'w') as f:
                f.write(f"// Auto-generated from {input_file}\n")
                f.write(f"// {channels} channel(s), {sample_width*8} bits, {frame_rate} Hz\n\n")
                f.write(f"#include <stdint.h>\n\n")
                f.write(f"const uint16_t {array_name}_length = {n_frames};\n")
                f.write(f"const int16_t {array_name}[{n_frames}] = {{\n    ")
                
                # Write samples
                samples_per_line = 16
                for i in range(0, n_frames, 2):
                    if i > 0 and i % samples_per_line == 0:
                        f.write("\n    ")
                    
                    if i + 1 < n_frames:
                        sample1 = struct.unpack('<h', frames[i*2:i*2+2])[0]
                        sample2 = struct.unpack('<h', frames[(i+1)*2:(i+1)*2+2])[0]
                        f.write(f"{sample1}, {sample2}")
                    else:
                        sample1 = struct.unpack('<h', frames[i*2:i*2+2])[0]
                        f.write(f"{sample1}")
                    
                    if i + 2 < n_frames:
                        f.write(", ")
                
                f.write("\n};\n")
            
            print(f"Output: {n_frames} samples written to {output_file}")
            
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python wav2c.py input.wav output.h")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    array_name = input_file.replace('.wav', '').replace('-', '_').replace(' ', '_')
    
    wav_to_c_array(input_file, output_file, array_name)
