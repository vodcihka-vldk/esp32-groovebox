# Sample Generation Script for ESP32 Groovebox

import numpy as np
import wave
import os

def generate_kick_sample(filename="kick.wav", duration=0.5, sample_rate=44100):
    """Generate kick drum sample"""
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    
    # Frequency sweep from 150Hz to 40Hz
    freq = np.exp(np.linspace(np.log(150), np.log(40), len(t)))
    
    # Generate sine wave with frequency sweep
    signal = np.sin(2 * np.pi * freq * t)
    
    # Apply envelope
    envelope = np.exp(-t * 8)  # Fast decay
    signal = signal * envelope
    
    # Normalize to 16-bit
    signal = (signal * 32767).astype(np.int16)
    
    # Save as WAV
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)  # Mono
        wav.setsampwidth(2)  # 16-bit
        wav.setframerate(sample_rate)
        wav.writeframes(signal.tobytes())
    
    print(f"Generated {filename}: {duration}s, {sample_rate}Hz")

def generate_snare_sample(filename="snare.wav", duration=0.2, sample_rate=44100):
    """Generate snare drum sample"""
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    
    # Tone component
    tone = np.sin(2 * np.pi * 250 * t)
    
    # Noise component
    noise = np.random.normal(0, 0.1, len(t))
    
    # Mix tone and noise
    signal = tone + noise
    
    # Apply envelope
    envelope = np.exp(-t * 12)
    signal = signal * envelope
    
    # Normalize to 16-bit
    signal = (signal * 32767).astype(np.int16)
    
    # Save as WAV
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(signal.tobytes())
    
    print(f"Generated {filename}: {duration}s, {sample_rate}Hz")

def generate_hihat_sample(filename="hihat.wav", duration=0.1, sample_rate=44100, closed=True):
    """Generate hi-hat sample"""
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    
    # High frequency noise
    noise = np.random.normal(0, 0.2, len(t))
    
    # High-pass filter effect (simple)
    if closed:
        decay = np.exp(-t * 30)  # Fast decay for closed
    else:
        decay = np.exp(-t * 5)   # Slow decay for open
    
    signal = noise * decay
    
    # Normalize to 16-bit
    signal = (signal * 32767).astype(np.int16)
    
    # Save as WAV
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(signal.tobytes())
    
    print(f"Generated {filename}: {duration}s, {sample_rate}Hz, closed={closed}")

def generate_tom_sample(filename="tom.wav", duration=0.4, sample_rate=44100):
    """Generate tom sample"""
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    
    # Frequency sweep from 200Hz to 50Hz
    freq = np.exp(np.linspace(np.log(200), np.log(50), len(t)))
    
    # Generate sine wave with frequency sweep
    signal = np.sin(2 * np.pi * freq * t)
    
    # Apply envelope
    envelope = np.exp(-t * 4)
    signal = signal * envelope
    
    # Normalize to 16-bit
    signal = (signal * 32767).astype(np.int16)
    
    # Save as WAV
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(signal.tobytes())
    
    print(f"Generated {filename}: {duration}s, {sample_rate}Hz")

def generate_clap_sample(filename="clap.wav", duration=0.3, sample_rate=44100):
    """Generate clap sample"""
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    
    # Multiple noise bursts for clap effect
    signal = np.zeros_like(t)
    
    # Three noise bursts at different times
    burst_times = [0.0, 0.02, 0.04]
    for burst_time in burst_times:
        burst_start = int(burst_time * sample_rate)
        burst_length = int(0.01 * sample_rate)
        if burst_start + burst_length < len(signal):
            burst = np.random.normal(0, 0.3, burst_length)
            burst_envelope = np.exp(-np.linspace(0, 10, burst_length))
            signal[burst_start:burst_start+burst_length] += burst * burst_envelope
    
    # Apply overall envelope
    envelope = np.exp(-t * 8)
    signal = signal * envelope
    
    # Normalize to 16-bit
    signal = (signal * 32767).astype(np.int16)
    
    # Save as WAV
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(signal.tobytes())
    
    print(f"Generated {filename}: {duration}s, {sample_rate}Hz")

def generate_crash_sample(filename="crash.wav", duration=1.5, sample_rate=44100):
    """Generate crash cymbal sample"""
    t = np.linspace(0, duration, int(sample_rate * duration), False)
    
    # High frequency noise with metallic character
    noise = np.random.normal(0, 0.15, len(t))
    
    # Add some tonal components for metallic sound
    metallic = np.sin(2 * np.pi * 8000 * t) * 0.1
    metallic += np.sin(2 * np.pi * 5000 * t) * 0.05
    
    signal = noise + metallic
    
    # Apply slow envelope
    envelope = np.exp(-t * 2)
    signal = signal * envelope
    
    # Normalize to 16-bit
    signal = (signal * 32767).astype(np.int16)
    
    # Save as WAV
    with wave.open(filename, 'w') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        wav.writeframes(signal.tobytes())
    
    print(f"Generated {filename}: {duration}s, {sample_rate}Hz")

def generate_all_samples():
    """Generate all basic drum samples"""
    print("🎵 Generating drum samples for ESP32 Groovebox...")
    
    samples_dir = "samples"
    if not os.path.exists(samples_dir):
        os.makedirs(samples_dir)
    
    # Generate samples
    generate_kick_sample(f"{samples_dir}/kick.wav")
    generate_snare_sample(f"{samples_dir}/snare.wav")
    generate_hihat_sample(f"{samples_dir}/hihat_closed.wav", closed=True)
    generate_hihat_sample(f"{samples_dir}/hihat_open.wav", closed=False)
    generate_tom_sample(f"{samples_dir}/tom.wav")
    generate_clap_sample(f"{samples_dir}/clap.wav")
    generate_crash_sample(f"{samples_dir}/crash.wav")
    
    print(f"\n✅ All samples generated in {samples_dir}/ directory")
    print("Use wav2c.py to convert them to C arrays for ESP32")

if __name__ == "__main__":
    try:
        import numpy as np
    except ImportError:
        print("Error: numpy required. Install with: pip install numpy")
        sys.exit(1)
    
    generate_all_samples()
