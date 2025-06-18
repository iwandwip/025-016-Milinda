import { NextRequest, NextResponse } from 'next/server';
import { collection, getDocs, query, orderBy, where } from 'firebase/firestore';
import { db } from '@/lib/firebase/config';

// GET /api/export - Export sensor data to CSV
export async function GET(request: NextRequest) {
  try {
    const { searchParams } = new URL(request.url);
    const format = searchParams.get('format') || 'csv';
    const potId = searchParams.get('pot_id');
    const startDate = searchParams.get('start_date');
    const endDate = searchParams.get('end_date');
    const category = searchParams.get('category');

    // Build query
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

    const querySnapshot = await getDocs(q);
    const data = querySnapshot.docs.map(doc => {
      const docData = doc.data();
      return {
        id: doc.id,
        timestamp: docData.timestamp.toDate(),
        pot_id: docData.pot_id,
        pot_name: docData.pot_name,
        moisture_value: docData.moisture?.value || 0,
        moisture_category: docData.moisture?.category || '',
        temperature_value: docData.temperature?.value || 0,
        temperature_category: docData.temperature?.category || '',
        uv_value: docData.uv_intensity?.value || 0,
        uv_category: docData.uv_intensity?.category || '',
        microplastic_percentage: docData.fuzzy_result?.percentage || 0,
        microplastic_category: docData.fuzzy_result?.category || '',
        device_id: docData.device_id,
        battery_level: docData.battery_level || 0,
        signal_strength: docData.signal_strength || 0
      };
    });

    if (format === 'csv') {
      return generateCSV(data);
    } else if (format === 'json') {
      return NextResponse.json({
        success: true,
        data,
        exported_at: new Date().toISOString(),
        total_records: data.length
      });
    } else {
      return NextResponse.json(
        { success: false, error: 'Unsupported format. Use csv or json.' },
        { status: 400 }
      );
    }

  } catch (error: any) {
    console.error('Error exporting data:', error);
    return NextResponse.json(
      { 
        success: false, 
        error: 'Failed to export data',
        message: error.message 
      },
      { status: 500 }
    );
  }
}

function generateCSV(data: any[]) {
  const headers = [
    'ID',
    'Timestamp',
    'Pot ID',
    'Pot Name',
    'Moisture (%)',
    'Moisture Category',
    'Temperature (°C)',
    'Temperature Category',
    'UV (V)',
    'UV Category',
    'Microplastic (%)',
    'Microplastic Category',
    'Device ID',
    'Battery Level (%)',
    'Signal Strength (dBm)'
  ];

  const csvRows = [headers.join(',')];

  for (const row of data) {
    const values = [
      row.id,
      row.timestamp.toISOString(),
      row.pot_id,
      `"${row.pot_name}"`, // Quoted to handle commas in names
      row.moisture_value.toFixed(2),
      row.moisture_category,
      row.temperature_value.toFixed(2),
      row.temperature_category,
      row.uv_value.toFixed(3),
      row.uv_category,
      row.microplastic_percentage.toFixed(2),
      row.microplastic_category,
      row.device_id,
      row.battery_level.toFixed(0),
      row.signal_strength.toFixed(0)
    ];
    csvRows.push(values.join(','));
  }

  const csvContent = csvRows.join('\n');
  const timestamp = new Date().toISOString().split('T')[0];
  const filename = `mikroplastik_data_${timestamp}.csv`;

  return new NextResponse(csvContent, {
    status: 200,
    headers: {
      'Content-Type': 'text/csv',
      'Content-Disposition': `attachment; filename="${filename}"`,
      'Cache-Control': 'no-cache'
    }
  });
}