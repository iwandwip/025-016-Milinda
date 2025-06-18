import { NextRequest, NextResponse } from 'next/server';
import { collection, getDocs, doc, updateDoc, query, orderBy, where } from 'firebase/firestore';
import { db } from '@/lib/firebase/config';

// GET /api/alerts - Fetch system alerts
export async function GET(request: NextRequest) {
  try {
    const { searchParams } = new URL(request.url);
    const resolved = searchParams.get('resolved');
    const severity = searchParams.get('severity');
    const limit = searchParams.get('limit');

    let q = query(
      collection(db, 'system_alerts'),
      orderBy('created_at', 'desc')
    );

    // Apply filters
    if (resolved !== null) {
      q = query(q, where('is_resolved', '==', resolved === 'true'));
    }

    if (severity && severity !== 'all') {
      q = query(q, where('severity', '==', severity));
    }

    const querySnapshot = await getDocs(q);
    let alerts = querySnapshot.docs.map(doc => ({
      id: doc.id,
      ...doc.data(),
      created_at: doc.data().created_at.toDate().toISOString(),
      resolved_at: doc.data().resolved_at ? doc.data().resolved_at.toDate().toISOString() : null
    }));

    // Apply limit after fetching (for simplicity)
    if (limit) {
      alerts = alerts.slice(0, parseInt(limit));
    }

    // Get counts by severity
    const counts = {
      total: alerts.length,
      unresolved: alerts.filter(a => !a.is_resolved).length,
      critical: alerts.filter(a => a.severity === 'critical').length,
      high: alerts.filter(a => a.severity === 'high').length,
      medium: alerts.filter(a => a.severity === 'medium').length,
      low: alerts.filter(a => a.severity === 'low').length
    };

    return NextResponse.json({
      success: true,
      data: alerts,
      counts
    });

  } catch (error: any) {
    console.error('Error fetching alerts:', error);
    return NextResponse.json(
      { 
        success: false, 
        error: 'Failed to fetch alerts',
        message: error.message 
      },
      { status: 500 }
    );
  }
}

// PUT /api/alerts - Update alert status (resolve/unresolve)
export async function PUT(request: NextRequest) {
  try {
    const body = await request.json();
    const { alertId, isResolved, resolvedBy } = body;

    if (!alertId) {
      return NextResponse.json(
        { success: false, error: 'Alert ID is required' },
        { status: 400 }
      );
    }

    const updateData: any = {
      is_resolved: isResolved,
      resolved_at: isResolved ? new Date() : null
    };

    if (resolvedBy) {
      updateData.resolved_by = resolvedBy;
    }

    await updateDoc(doc(db, 'system_alerts', alertId), updateData);

    return NextResponse.json({
      success: true,
      message: `Alert ${isResolved ? 'resolved' : 'reopened'} successfully`
    });

  } catch (error: any) {
    console.error('Error updating alert:', error);
    return NextResponse.json(
      { 
        success: false, 
        error: 'Failed to update alert',
        message: error.message 
      },
      { status: 500 }
    );
  }
}