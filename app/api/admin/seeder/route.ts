import { NextRequest, NextResponse } from 'next/server';
import { adminAuth, adminFirestore } from '@/lib/firebase/admin';

export async function POST(request: NextRequest) {
  try {
    const { users } = await request.json();
    
    if (!users || !Array.isArray(users)) {
      return NextResponse.json({ error: 'Users array is required' }, { status: 400 });
    }

    const results = [];
    
    for (const userData of users) {
      try {
        const { email, password, displayName, role = 'user' } = userData;
        
        if (!email || !password) {
          results.push({ email, status: 'failed', error: 'Email and password required' });
          continue;
        }

        const userRecord = await adminAuth.createUser({
          email,
          password,
          displayName,
          emailVerified: false
        });

        await adminFirestore.collection('users').doc(userRecord.uid).set({
          uid: userRecord.uid,
          email,
          displayName: displayName || '',
          role,
          createdAt: new Date().toISOString()
        });

        results.push({ 
          email, 
          status: 'success', 
          uid: userRecord.uid 
        });
      } catch (error: any) {
        results.push({ 
          email: userData.email, 
          status: 'failed', 
          error: error.message 
        });
      }
    }

    return NextResponse.json({ 
      message: 'Seeder completed',
      results 
    });
  } catch (error) {
    console.error('Error in seeder:', error);
    return NextResponse.json({ error: 'Seeder failed' }, { status: 500 });
  }
}