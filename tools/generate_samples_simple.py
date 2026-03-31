#!/usr/bin/env python3
"""
Generate simple drum samples without numpy dependency
"""

import math
import struct
import wave

def generate_kick_sample(filename="kick.wav", duration=0.5, sample_rate=44100):
    """Generate kick drum sample"""
    num_samples = int(sample_rate * duration)
    
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)  # Mono
        wav.setsampwidth(2)  # 16-bit
        wav.setframerate(sample_rate)
        
        samples = []
        for i in range(num_samples):
            t = i / sample_rate
            
            # Frequency sweep from 150Hz to 0Hz
            freq = 150 * math.exp(-t * 10)
            
            # Generate sine wave
            sample = math.sin(2 * math.pi * freq * t)
            
            # Apply envelope
            envelope = math.exp(-t * 8)
            sample *= envelope
            
            # Convert to 16-bit
            sample_int = max(-32768, min(32767, int(sample * 32767)))
            samples.append(struct.pack('<h', sample_int))
        
        wav.writeframes(b''.join(samples))

def generate_snare_sample(filename="snare.wav", duration=0.2, sample_rate=44100):
    """Generate snare drum sample"""
    num_samples = int(sample_rate * duration)
    
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        
        samples = []
        for i in range(num_samples):
            t = i / sample_rate
            
            # Tone component
            tone = math.sin(2 * math.pi * 250 * t)
            
            # Noise component
            noise = (hash(str(i)) % 1000 - 500) / 5000.0  # Simple pseudo-random
            
            # Mix tone and noise
            sample = tone + noise
            
            # Apply envelope
            envelope = math.exp(-t * 12)
            sample *= envelope
            
            # Convert to 16-bit
            sample_int = max(-32768, min(32767, int(sample * 32767)))
            samples.append(struct.pack('<h', sample_int))
        
        wav.writeframes(b''.join(samples))

def generate_hihat_sample(filename="hihat.wav", duration=0.1, sample_rate=44100, closed=True):
    """Generate hi-hat sample"""
    num_samples = int(sample_rate * duration)
    
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        
        samples = []
        for i in range(num_samples):
            t = i / sample_rate
            
            # High frequency noise
            noise = (hash(str(i * 13)) % 1000 - 500) / 5000.0
            
            if closed:
                decay = math.exp(-t * 30)  # Fast decay
            else:
                decay = math.exp(-t * 5)   # Slow decay
            
            sample = noise * decay
            
            # Convert to 16-bit
            sample_int = max(-32768, min(32767, int(sample * 32767)))
            samples.append(struct.pack('<h', sample_int))
        
        wav.writeframes(b''.join(samples))

def convert_wav_to_c_array(wav_file, c_file, array_name):
    """Convert WAV file to C array"""
    with wave.open(wav_file, 'r') as wav:
        frames = wav.readframes(-1)
        sample_count = wav.getnframes()
        
        # Write C array
        with open(c_file, 'w') as f:
            f.write(f"// Auto-generated from {wav_file}\n")
            f.write(f"static const int16_t {array_name}[] = {{\n    ")
            
            for i in range(0, len(frames), 2):
                if i > 0 and i % 64 == 0:
                    f.write("\n    ")
                
                sample = struct.unpack('<h', frames[i:i+2])[0]
                f.write(f"{sample}, ")
            
            f.write("\n};\n")
        
        print(f"Generated {c_file} with {sample_count} samples")

def main():
    print("🎵 Generating drum samples for ESP32 Groovebox...")
    
    # Generate WAV files
    generate_kick_sample("kick.wav")
    generate_snare_sample("snare.wav")
    generate_hihat_sample("hihat_closed.wav", closed=True)
    generate_hihat_sample("hihat_open.wav", closed=False)
    
    # Convert to C arrays
    convert_wav_to_c_array("kick.wav", "kick_data.inc", "kick_sample")
    convert_wav_to_c_array("snare.wav", "snare_data.inc", "snare_sample")
    convert_wav_to_c_array("hihat_closed.wav", "hihat_closed_data.inc", "hihat_closed_sample")
    convert_wav_to_c_array("hihat_open.wav", "hihat_open_data.inc", "hihat_open_sample")
    
    print("✅ Sample generation complete!")

if __name__ == "__main__":
    main()
