# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**025-016-Milinda** is an IoT-based microplastic detection system for soil monitoring that combines ESP32 hardware with a Next.js web dashboard. The system uses fuzzy logic algorithms (Mamdani, Sugeno, Tsukamoto) to analyze sensor data and detect microplastic contamination in soil.

## Development Commands

### Web Application
```bash
# Development server
pnpm dev

# Build for production  
pnpm build

# Start production server
pnpm start

# Linting
pnpm lint

# Admin setup (first-time only)
pnpm run setup-admin

# Test Firebase connection
pnpm run test-firebase
```

### Environment Setup
- Copy `.env.example` to `.env.local` and configure Firebase environment variables:
  - `NEXT_PUBLIC_FIREBASE_API_KEY`
  - `NEXT_PUBLIC_FIREBASE_AUTH_DOMAIN`
  - `NEXT_PUBLIC_FIREBASE_PROJECT_ID`
  - `NEXT_PUBLIC_FIREBASE_STORAGE_BUCKET`
  - `NEXT_PUBLIC_FIREBASE_MESSAGING_SENDER_ID`
  - `NEXT_PUBLIC_FIREBASE_APP_ID`
  - `NEXT_PUBLIC_FIREBASE_MEASUREMENT_ID`

- Place Firebase service account JSON at `admin/serviceAccount.json` for admin scripts

## Architecture Overview

### Tech Stack
- **Frontend**: Next.js 15, React 19, TypeScript, Tailwind CSS v4
- **UI Components**: Radix UI with shadcn/ui styling
- **Backend**: Firebase (Auth, Firestore, Realtime Database, Storage)
- **Hardware**: ESP32 with sensor modules
- **Package Manager**: pnpm

### Application Structure

#### Web Application (`/app`)
- **Authentication**: Firebase Auth with role-based access (admin/user)
- **Dashboard**: Real-time monitoring with charts and metrics
- **Admin Panel**: User management and data seeding
- **History**: Data analysis and export functionality
- **Profile**: User preferences and password management

#### Firmware (`/firmware`)
- **MelindaFirmwareR0/**: Production ESP32 firmware
- **TestFuzzyMethods/**: Fuzzy logic testing implementations
- **Backup/**: Sensor calibration and testing utilities
- **Test_Sensor_*/**: Individual sensor testing modules

#### Core Libraries (`/lib`)
- **firebase/**: Firebase configuration and utilities
- **hooks/**: React hooks for auth, sensor data, and alerts
- **utils.ts**: Shared utility functions

### Sensor System

#### Sensor Types
1. **Soil Moisture**: Categories: 'basah' | 'sedang' | 'kering'
2. **Temperature (MLX90614)**: Categories: 'rendah' | 'normal' | 'tinggi' 
3. **UV Intensity (GUVA-S12SD)**: Categories: 'sedikit' | 'sedang' | 'banyak'

#### Fuzzy Logic Implementation
- **Methods**: Mamdani, Sugeno, Tsukamoto comparison
- **Output**: Microplastic percentage and confidence level
- **Categories**: 'rendah' | 'sedang' | 'tinggi'

## Key Data Structures

### SensorData Interface
```typescript
interface SensorData {
  id: string;
  timestamp: Date;
  pot_id: string;
  soil_condition: 'kering' | 'basah';
  has_microplastic: boolean;
  moisture: MoistureReading;
  temperature: TemperatureReading;
  uv_intensity: UVReading;
  fuzzy_result: FuzzyResult;
  device_id: string;
}
```

### Firebase Collections
- **users**: User authentication and role management
- **sensor_data**: Real-time sensor readings
- **devices**: ESP32 device status and configuration
- **alerts**: System notifications and warnings

## Admin Setup Process

1. **Initial Setup**: Run `pnpm run setup-admin` to create first admin account
2. **Admin Features**: User management, data seeding, system monitoring
3. **Role System**: 'admin' | 'user' roles with different permissions

## Firebase Integration

### Services Used
- **Authentication**: User login/registration
- **Firestore**: User data and historical records
- **Realtime Database**: Live sensor data streaming
- **Storage**: File uploads and exports

### Admin SDK
- Service account required at `admin/serviceAccount.json`
- Used for user management and data operations
- Configured in `lib/firebase/admin.ts`

## Hardware Development

### ESP32 Configuration
- **Main Headers**: `Header.h` defines all enabled modules
- **Sensor Integration**: Analog sensors with fuzzy logic processing
- **Communication**: WiFi + Firebase RTDB for real-time data
- **Libraries**: Custom Kinematrix framework for sensor management

### Sensor Calibration
- Interactive calibration modules in `/firmware/Backup/`
- Use `InteractiveSerialAnalogSensorCalibrator` for sensor setup
- Test individual sensors with respective test modules

## Development Workflow

### Frontend Development
1. Use TypeScript strict mode - all components are fully typed
2. Follow shadcn/ui patterns for consistent UI components
3. Use React hooks from `/lib/hooks/` for state management
4. Real-time data via Firebase Realtime Database

### Firmware Development
1. Main firmware in `MelindaFirmwareR0/`
2. Test individual features in respective test directories
3. Use Arduino IDE or PlatformIO for ESP32 development
4. Follow modular approach defined in `Header.h`

### Testing
- No specific test framework configured
- Manual testing through dashboard and sensor modules
- Use admin seeder for bulk data testing

## Important Notes

### Language Usage
- Indonesian language used for sensor categories and UI text
- Documentation mixes English and Indonesian
- Sensor readings use Indonesian terminology

### Security Considerations
- Firebase rules should restrict access based on user roles
- Service account JSON must be kept secure
- Environment variables required for all Firebase operations

### Research Context
- This is academic research project (code: 025-016)
- Focus on fuzzy logic method comparison
- Real-world IoT implementation for environmental monitoring