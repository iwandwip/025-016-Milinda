export interface SensorReading {
  value: number;
  category: string;
}

export interface MoistureReading extends SensorReading {
  category: 'basah' | 'sedang' | 'kering';
}

export interface TemperatureReading extends SensorReading {
  category: 'rendah' | 'normal' | 'tinggi';
}

export interface UVReading extends SensorReading {
  category: 'sedikit' | 'sedang' | 'banyak';
}

export interface FuzzyResult {
  percentage: number;
  category: 'rendah' | 'sedang' | 'tinggi';
  confidence: number;
}

export interface SensorData {
  id: string;
  timestamp: Date;
  pot_id: string;
  pot_name: string;
  soil_condition: 'kering' | 'basah';
  has_microplastic: boolean;
  
  moisture: MoistureReading;
  temperature: TemperatureReading;
  uv_intensity: UVReading;
  fuzzy_result: FuzzyResult;
  
  device_id: string;
  battery_level?: number;
  signal_strength?: number;
  
  created_by?: string;
  notes?: string;
}

export interface SystemAlert {
  id: string;
  type: 'high_microplastic' | 'sensor_error' | 'battery_low' | 'system_maintenance';
  severity: 'low' | 'medium' | 'high' | 'critical';
  title: string;
  message: string;
  pot_id?: string;
  sensor_data_id?: string;
  is_resolved: boolean;
  resolved_at?: Date;
  resolved_by?: string;
  created_at: Date;
}

export interface DeviceStatus {
  device_id: string;
  name: string;
  location: string;
  is_online: boolean;
  last_seen: Date;
  battery_level: number;
  firmware_version: string;
  wifi_signal: number;
  uptime: number;
  error_count: number;
  last_error?: string;
  maintenance_due?: Date;
}