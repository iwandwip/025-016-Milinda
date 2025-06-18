import { NextRequest, NextResponse } from 'next/server';
import { collection, getDocs, addDoc, query, orderBy, limit, where } from 'firebase/firestore';
import { db } from '@/lib/firebase/config';

// GET /api/sensor-data - Fetch sensor data with optional filters
export async function GET(request: NextRequest) {
  try {
    const { searchParams } = new URL(request.url);
    const potId = searchParams.get('pot_id');
    const limitParam = searchParams.get('limit');
    const category = searchParams.get('category');
    const startDate = searchParams.get('start_date');
    const endDate = searchParams.get('end_date');

    let q = query(
      collection(db, 'sensor_data'),
      orderBy('timestamp', 'desc')
    );

    // Apply filters
    if (potId && potId !== 'all') {
      q = query(q, where('pot_id', '==', potId));
    }

    if (category && category !== 'all') {
      q = query(q, where('fuzzy_result.category', '==', category));
    }

    if (startDate) {
      q = query(q, where('timestamp', '>=', new Date(startDate)));
    }

    if (endDate) {
      q = query(q, where('timestamp', '<=', new Date(endDate)));
    }

    if (limitParam) {
      q = query(q, limit(parseInt(limitParam)));
    }

    const querySnapshot = await getDocs(q);
    const data = querySnapshot.docs.map(doc => ({
      id: doc.id,
      ...doc.data(),
      timestamp: doc.data().timestamp.toDate().toISOString()
    }));

    return NextResponse.json({
      success: true,
      data,
      count: data.length
    });

  } catch (error: any) {
    console.error('Error fetching sensor data:', error);
    return NextResponse.json(
      { 
        success: false, 
        error: 'Failed to fetch sensor data',
        message: error.message 
      },
      { status: 500 }
    );
  }
}

// POST /api/sensor-data - Add new sensor data (from ESP32 devices)
export async function POST(request: NextRequest) {
  try {
    const body = await request.json();
    
    // Validate required fields
    const requiredFields = ['pot_id', 'moisture', 'temperature', 'uv_intensity', 'device_id'];
    for (const field of requiredFields) {
      if (!body[field]) {
        return NextResponse.json(
          { 
            success: false, 
            error: `Missing required field: ${field}` 
          },
          { status: 400 }
        );
      }
    }

    // Process fuzzy logic here (simplified example)
    const fuzzyResult = calculateFuzzyLogic(
      body.moisture.value,
      body.temperature.value,
      body.uv_intensity.value
    );

    const sensorData = {
      ...body,
      fuzzy_result: fuzzyResult,
      timestamp: new Date(),
      created_at: new Date()
    };

    const docRef = await addDoc(collection(db, 'sensor_data'), sensorData);

    // Check if alert should be triggered
    if (fuzzyResult.category === 'tinggi') {
      await triggerAlert(docRef.id, body.pot_id, fuzzyResult.percentage);
    }

    return NextResponse.json({
      success: true,
      id: docRef.id,
      message: 'Sensor data added successfully'
    });

  } catch (error: any) {
    console.error('Error adding sensor data:', error);
    return NextResponse.json(
      { 
        success: false, 
        error: 'Failed to add sensor data',
        message: error.message 
      },
      { status: 500 }
    );
  }
}

// Simplified fuzzy logic calculation
function calculateFuzzyLogic(moisture: number, temperature: number, uv: number) {
  // This is a simplified version - in real implementation, use proper fuzzy logic
  let score = 0;
  let factors = 0;

  // Moisture contribution (higher moisture might indicate more microplastic retention)
  if (moisture > 60) {
    score += 30;
    factors++;
  } else if (moisture > 40) {
    score += 20;
    factors++;
  } else {
    score += 10;
    factors++;
  }

  // Temperature contribution
  if (temperature > 30) {
    score += 25;
    factors++;
  } else if (temperature > 20) {
    score += 15;
    factors++;
  } else {
    score += 5;
    factors++;
  }

  // UV contribution
  if (uv > 0.7) {
    score += 20;
    factors++;
  } else if (uv > 0.4) {
    score += 15;
    factors++;
  } else {
    score += 10;
    factors++;
  }

  const percentage = Math.min(score, 100);
  let category: 'rendah' | 'sedang' | 'tinggi';

  if (percentage >= 70) {
    category = 'tinggi';
  } else if (percentage >= 40) {
    category = 'sedang';
  } else {
    category = 'rendah';
  }

  return {
    percentage,
    category,
    confidence: 0.85 // Static confidence for demo
  };
}

// Function to trigger alerts
async function triggerAlert(sensorDataId: string, potId: string, percentage: number) {
  try {
    const alert = {
      type: 'high_microplastic',
      severity: percentage > 80 ? 'critical' : 'high',
      title: 'Deteksi Mikroplastik Tinggi',
      message: `Terdeteksi mikroplastik ${percentage.toFixed(1)}% pada ${potId}`,
      pot_id: potId,
      sensor_data_id: sensorDataId,
      is_resolved: false,
      created_at: new Date()
    };

    await addDoc(collection(db, 'system_alerts'), alert);
  } catch (error) {
    console.error('Error triggering alert:', error);
  }
}