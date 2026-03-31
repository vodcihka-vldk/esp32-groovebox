import { Project, Channel, Step, ParameterLock } from '@shared/schemas';

export class JSONConverter {
  // Конвертация веб-формата в ArduinoJson формат
  static webToArduino(webProject: Project): string {
    const arduinoProject = {
      name: webProject.name,
      bpm: webProject.bpm,
      masterVolume: webProject.masterVolume,
      patterns: webProject.patterns.map((pattern) =>
        pattern.tracks.map((track) =>
          track.steps.map((step) => step.active)
        )
      ),
      channels: webProject.patterns[0].tracks.map((track) => track.channel),
      dsp: webProject.dsp
    };
    
    return JSON.stringify(arduinoProject, null, 2);
  }
  
  // Конвертация ArduinoJson формата в веб-формат
  static arduinoToWeb(arduinoJson: string): Project {
    const arduinoProject = JSON.parse(arduinoJson);
    
    return {
      version: "1.0",
      name: arduinoProject.name,
      bpm: arduinoProject.bpm,
      masterVolume: arduinoProject.masterVolume,
      swing: 50,
      patterns: Array.from({ length: 8 }, (_, pIndex) => ({
        id: `pattern_${pIndex}`,
        name: `Pattern ${pIndex + 1}`,
        length: '64',
        swing: 50,
        tracks: Array.from({ length: 8 }, (_, tIndex) => ({
          id: `track_${tIndex}`,
          name: `Track ${tIndex + 1}`,
          sampleId: `sample_${arduinoProject.channels[tIndex].sample}`,
          channel: arduinoProject.channels[tIndex],
          steps: Array.from({ length: 64 }, (_, sIndex) => ({
            active: arduinoProject.patterns[pIndex][tIndex][sIndex]
          })),
          parameterLocks: []
        }))
      })),
      mixer: {
        channels: arduinoProject.channels.map((channel: any, index: number) => ({
          id: `track_${index}`,
          volume: channel.volume,
          pan: 0,
          mute: false,
          solo: false
        }))
      },
      dsp: arduinoProject.dsp,
      metadata: {
        createdAt: new Date().toISOString(),
        updatedAt: new Date().toISOString()
      }
    };
  }
  
  // Валидация JSON для ArduinoJson
  static validateArduinoJson(json: string): { valid: boolean; errors: string[] } {
    const errors: string[] = [];
    
    try {
      const data = JSON.parse(json);
      
      // Проверка обязательных полей
      if (!data.name) errors.push('Missing required field: name');
      if (!data.bpm) errors.push('Missing required field: bpm');
      if (typeof data.bpm !== 'number' || data.bpm < 60 || data.bpm > 300) {
        errors.push('Invalid BPM value (must be 60-300)');
      }
      
      // Проверка треков
      if (!Array.isArray(data.channels)) {
        errors.push('Channels must be an array');
      } else {
        data.channels.forEach((channel: any, index: number) => {
          if (!channel.id) errors.push(`Channel ${index}: missing id`);
          if (typeof channel.volume !== 'number' || channel.volume < 0 || channel.volume > 100) {
            errors.push(`Channel ${index}: invalid volume (must be 0-100)`);
          }
          if (typeof channel.pan !== 'number' || channel.pan < 0 || channel.pan > 100) {
            errors.push(`Channel ${index}: invalid pan (must be 0-100)`);
          }
        });
      }
      
    } catch (error) {
      errors.push('Invalid JSON format');
    }
    
    return {
      valid: errors.length === 0,
      errors
    };
  }
  
  // Генерация шаблона для ArduinoJson
  static generateArduinoTemplate(): string {
    const template = {
      name: "pattern_001",
      bpm: 120,
      masterVolume: 80,
      patterns: Array.from({ length: 8 }, (_, trackIndex) => ({
        id: `track_${trackIndex}`,
        name: `Track ${trackIndex + 1}`,
        sampleId: "",
        muted: false,
        solo: false,
        volume: 80,
        pan: 50,
        steps: Array.from({ length: 64 }, (_, stepIndex) => ({
          active: false,
          velocity: 100,
          note: 60 + trackIndex * 12,
          length: 16,
          offset: 0
        }))
      })),
      parameterLocks: []
    };
    
    return JSON.stringify(template, null, 2);
  }
}
