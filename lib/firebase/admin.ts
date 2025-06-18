import admin from 'firebase-admin';

if (!admin.apps.length) {
  const serviceAccount = require('../../admin/serviceAccount.json');
  
  admin.initializeApp({
    credential: admin.credential.cert({
      projectId: serviceAccount.project_id,
      clientEmail: serviceAccount.client_email,
      privateKey: serviceAccount.private_key.replace(/\\n/g, '\n'),
    }),
    projectId: serviceAccount.project_id
  });
}

export const adminAuth = admin.auth();
export const adminFirestore = admin.firestore();

export default admin;