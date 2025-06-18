import ProfileForm from "@/components/profile/profile-form";
import PasswordForm from "@/components/profile/password-form";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Activity, Clock, Shield, Settings } from "lucide-react";

export default function ProfilePage() {
  return (
    <div className="p-6 space-y-6">
      {/* Page Header */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-2xl font-bold text-gray-900 dark:text-white">
            Profil & Pengaturan
          </h1>
          <p className="text-gray-600 dark:text-gray-400">
            Kelola informasi akun dan preferensi sistem
          </p>
        </div>
        <div className="flex items-center space-x-2">
          <Badge variant="outline" className="flex items-center space-x-1">
            <Shield className="w-3 h-3" />
            <span>Akun Terverifikasi</span>
          </Badge>
          <Badge className="bg-green-100 text-green-800 flex items-center space-x-1">
            <Activity className="w-3 h-3" />
            <span>Aktif</span>
          </Badge>
        </div>
      </div>

      {/* Account Statistics */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <Card>
          <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
            <CardTitle className="text-sm font-medium">Sesi Login</CardTitle>
            <Activity className="w-4 h-4 text-muted-foreground" />
          </CardHeader>
          <CardContent>
            <div className="text-2xl font-bold">47</div>
            <p className="text-xs text-muted-foreground">
              Bulan ini
            </p>
          </CardContent>
        </Card>

        <Card>
          <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
            <CardTitle className="text-sm font-medium">Waktu Aktif</CardTitle>
            <Clock className="w-4 h-4 text-muted-foreground" />
          </CardHeader>
          <CardContent>
            <div className="text-2xl font-bold">124h</div>
            <p className="text-xs text-muted-foreground">
              30 hari terakhir
            </p>
          </CardContent>
        </Card>

        <Card>
          <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
            <CardTitle className="text-sm font-medium">Data Akses</CardTitle>
            <Settings className="w-4 h-4 text-muted-foreground" />
          </CardHeader>
          <CardContent>
            <div className="text-2xl font-bold">2,847</div>
            <p className="text-xs text-muted-foreground">
              Records diakses
            </p>
          </CardContent>
        </Card>
      </div>

      {/* Profile Form */}
      <ProfileForm />

      {/* Password Form */}
      <PasswordForm />

      {/* Account Security */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center space-x-2">
            <Shield className="w-5 h-5" />
            <span>Keamanan Akun</span>
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="space-y-3">
            <div className="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-800 rounded-lg">
              <div>
                <h4 className="font-medium">Two-Factor Authentication</h4>
                <p className="text-sm text-gray-600 dark:text-gray-400">
                  Tambahan keamanan untuk akun Anda
                </p>
              </div>
              <Badge variant="outline">
                Tidak Aktif
              </Badge>
            </div>

            <div className="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-800 rounded-lg">
              <div>
                <h4 className="font-medium">Login Alerts</h4>
                <p className="text-sm text-gray-600 dark:text-gray-400">
                  Notifikasi email saat login dari device baru
                </p>
              </div>
              <Badge className="bg-green-100 text-green-800">
                Aktif
              </Badge>
            </div>

            <div className="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-800 rounded-lg">
              <div>
                <h4 className="font-medium">Session Timeout</h4>
                <p className="text-sm text-gray-600 dark:text-gray-400">
                  Otomatis logout setelah 30 menit tidak aktif
                </p>
              </div>
              <Badge className="bg-green-100 text-green-800">
                Aktif
              </Badge>
            </div>
          </div>

          <div className="pt-4 border-t">
            <h4 className="font-medium mb-2">Login Terakhir</h4>
            <div className="text-sm text-gray-600 dark:text-gray-400 space-y-1">
              <p>• Hari ini, 09:15 WIB - Chrome di Windows</p>
              <p>• Kemarin, 16:45 WIB - Firefox di Windows</p>
              <p>• 2 hari lalu, 08:30 WIB - Safari di iPhone</p>
            </div>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}