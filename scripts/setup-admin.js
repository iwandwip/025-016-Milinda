const admin = require('firebase-admin');
const readline = require('readline');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

function question(prompt) {
  return new Promise((resolve) => {
    rl.question(prompt, resolve);
  });
}

async function setupAdmin() {
  try {
    console.log('🔥 Firebase Admin Setup Script');
    console.log('================================\n');

    // Initialize Firebase Admin
    if (!admin.apps.length) {
      const serviceAccount = require('../admin/serviceAccount.json');
      
      // Validate service account
      if (!serviceAccount.project_id || !serviceAccount.private_key || !serviceAccount.client_email) {
        throw new Error('Invalid service account credentials');
      }
      
      admin.initializeApp({
        credential: admin.credential.cert({
          projectId: serviceAccount.project_id,
          clientEmail: serviceAccount.client_email,
          privateKey: serviceAccount.private_key.replace(/\\n/g, '\n'), // Fix newlines
        }),
        projectId: serviceAccount.project_id
      });
      
      console.log(`🔗 Connected to Firebase project: ${serviceAccount.project_id}`);
    }

    const auth = admin.auth();
    const firestore = admin.firestore();

    // Check if admin already exists
    try {
      const usersSnapshot = await firestore.collection('users').where('role', '==', 'admin').get();
      if (!usersSnapshot.empty) {
        console.log('❌ Admin user already exists! Setup not needed.');
        const adminUsers = usersSnapshot.docs.map(doc => doc.data().email);
        console.log('Existing admin(s):', adminUsers.join(', '));
        rl.close();
        return;
      }
    } catch (error) {
      console.log('⚠️  Could not check existing admins, proceeding...\n');
    }

    // Get admin details
    console.log('📝 Enter admin account details:\n');
    
    const email = await question('Email: ');
    if (!email || !email.includes('@')) {
      console.log('❌ Please enter a valid email address');
      rl.close();
      return;
    }

    const password = await question('Password (min 6 chars): ');
    if (!password || password.length < 6) {
      console.log('❌ Password must be at least 6 characters');
      rl.close();
      return;
    }

    const displayName = await question('Display Name (optional): ');

    console.log('\n🚀 Creating admin user...');

    // Create user in Firebase Auth
    const userRecord = await auth.createUser({
      email: email,
      password: password,
      displayName: displayName || 'Administrator',
      emailVerified: true
    });

    console.log(`✅ User created with UID: ${userRecord.uid}`);

    // Create user document in Firestore
    await firestore.collection('users').doc(userRecord.uid).set({
      uid: userRecord.uid,
      email: email,
      displayName: displayName || 'Administrator',
      role: 'admin',
      createdAt: new Date().toISOString(),
      setupBy: 'admin-script'
    });

    console.log('✅ Admin role assigned in Firestore');

    console.log('\n🎉 SUCCESS! Admin account created successfully!');
    console.log('====================================================');
    console.log(`📧 Email: ${email}`);
    console.log(`👤 Name: ${displayName || 'Administrator'}`);
    console.log(`🔑 Role: Administrator`);
    console.log(`🆔 UID: ${userRecord.uid}`);
    console.log('====================================================');
    console.log('\n💡 You can now login to the dashboard with these credentials.');
    console.log('🔒 This script will not create another admin if one already exists.\n');

  } catch (error) {
    console.error('❌ Error creating admin:', error.message);
    
    if (error.code === 'auth/email-already-exists') {
      console.log('\n💡 Email already exists. Checking if it has admin role...');
      try {
        const userRecord = await admin.auth().getUserByEmail(email);
        const userDoc = await admin.firestore().collection('users').doc(userRecord.uid).get();
        
        if (userDoc.exists && userDoc.data().role === 'admin') {
          console.log('✅ User already has admin role!');
        } else {
          console.log('⚠️  User exists but not admin. Updating role...');
          await admin.firestore().collection('users').doc(userRecord.uid).set({
            uid: userRecord.uid,
            email: userRecord.email,
            displayName: userRecord.displayName || 'Administrator',
            role: 'admin',
            updatedAt: new Date().toISOString(),
            updatedBy: 'admin-script'
          }, { merge: true });
          console.log('✅ Admin role assigned!');
        }
      } catch (updateError) {
        console.error('❌ Error updating existing user:', updateError.message);
      }
    }
  } finally {
    rl.close();
    process.exit(0);
  }
}

// Run the setup
setupAdmin();