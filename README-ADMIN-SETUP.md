# 🔐 Admin Setup Guide

## Initial Admin Account Creation

Untuk membuat akun admin pertama, ikuti langkah-langkah berikut:

### 1. Setup Admin Account

Jalankan script setup admin:

```bash
npm run setup-admin
```

atau

```bash
pnpm run setup-admin
```

### 2. Input Data Admin

Script akan meminta input:
- **Email**: Email admin (contoh: admin@example.com)
- **Password**: Password minimal 6 karakter
- **Display Name**: Nama admin (opsional)

### 3. Contoh Penggunaan

```bash
$ npm run setup-admin

🔥 Firebase Admin Setup Script
================================

📝 Enter admin account details:

Email: admin@milinda.com
Password (min 6 chars): admin123
Display Name (optional): Administrator

🚀 Creating admin user...
✅ User created with UID: abc123...
✅ Admin role assigned in Firestore

🎉 SUCCESS! Admin account created successfully!
====================================================
📧 Email: admin@milinda.com
👤 Name: Administrator  
🔑 Role: Administrator
🆔 UID: abc123...
====================================================

💡 You can now login to the dashboard with these credentials.
```

## 🔒 Keamanan

- Script hanya akan membuat admin jika belum ada admin yang terdaftar
- Jika admin sudah ada, script akan memberikan peringatan dan berhenti
- Email yang sudah terdaftar akan diupdate role-nya menjadi admin

## 🚀 Setelah Setup

Setelah admin berhasil dibuat:

1. **Login ke Dashboard**: Gunakan email dan password yang telah dibuat
2. **Akses Admin Panel**: Menu admin akan muncul di sidebar
3. **Kelola Users**: Bisa membuat, edit, dan hapus user lainnya
4. **Data Seeder**: Bisa membuat user secara batch

## 🛠️ Troubleshooting

### Error: Email already exists
Jika email sudah terdaftar, script akan otomatis update role menjadi admin.

### Error: Service account not found
Pastikan file `admin/serviceAccount.json` sudah ada dan valid.

### Error: Firebase project not found
Pastikan project ID di `admin.ts` sesuai dengan project Firebase Anda.

---

**⚠️ PENTING**: Simpan kredensial admin dengan aman dan jangan share ke orang yang tidak berwenang!