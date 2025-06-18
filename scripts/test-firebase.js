const admin = require('firebase-admin');

async function testFirebase() {
  try {
    console.log('🔥 Testing Firebase Admin Connection...\n');
    
    const serviceAccount = require('../admin/serviceAccount.json');
    console.log('✅ Service account loaded');
    console.log(`📋 Project ID: ${serviceAccount.project_id}`);
    console.log(`📧 Client Email: ${serviceAccount.client_email}`);
    
    // Initialize with explicit credentials
    admin.initializeApp({
      credential: admin.credential.cert({
        projectId: serviceAccount.project_id,
        clientEmail: serviceAccount.client_email,
        privateKey: serviceAccount.private_key.replace(/\\n/g, '\n'), // Fix newlines
      }),
      projectId: serviceAccount.project_id
    });
    
    console.log('✅ Firebase Admin initialized');
    
    // Test connection
    const auth = admin.auth();
    const result = await auth.listUsers(1);
    console.log('✅ Firebase Auth connection successful');
    
    const firestore = admin.firestore();
    await firestore.collection('test').doc('test').get();
    console.log('✅ Firestore connection successful');
    
    console.log('\n🎉 All Firebase services are working!');
    
  } catch (error) {
    console.error('❌ Firebase test failed:', error.message);
    
    if (error.message.includes('private_key')) {
      console.log('\n💡 Try regenerating your service account key from Firebase Console');
    }
  }
  
  process.exit(0);
}

testFirebase();