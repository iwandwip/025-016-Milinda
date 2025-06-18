"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { z } from "zod";
import { toast } from "sonner";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Switch } from "@/components/ui/switch";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { useAuth } from "@/lib/hooks/use-auth";
import { User, Save, Camera } from "lucide-react";

const profileSchema = z.object({
  name: z.string().min(2, "Nama minimal 2 karakter"),
  email: z.string().email("Email tidak valid"),
  theme: z.enum(['light', 'dark', 'system']),
  notifications: z.boolean(),
  language: z.enum(['id', 'en']),
});

type ProfileFormData = z.infer<typeof profileSchema>;

export default function ProfileForm() {
  const { user } = useAuth();
  const [loading, setLoading] = useState(false);

  const {
    register,
    handleSubmit,
    setValue,
    watch,
    formState: { errors },
  } = useForm<ProfileFormData>({
    resolver: zodResolver(profileSchema),
    defaultValues: {
      name: user?.name || "",
      email: user?.email || "",
      theme: user?.preferences?.theme || 'system',
      notifications: user?.preferences?.notifications || true,
      language: user?.preferences?.language || 'id',
    },
  });

  const watchedTheme = watch("theme");
  const watchedNotifications = watch("notifications");

  const onSubmit = async (data: ProfileFormData) => {
    setLoading(true);
    try {
      // Here you would update the user profile in Firebase
      // await updateUserProfile(user!.id, data);
      
      console.log("Profile update data:", data);
      toast.success("Profil berhasil diperbarui!");
    } catch (error: any) {
      toast.error("Gagal memperbarui profil: " + error.message);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-6">
      {/* Profile Header */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center space-x-2">
            <User className="w-5 h-5" />
            <span>Informasi Profil</span>
          </CardTitle>
        </CardHeader>
        <CardContent>
          <div className="flex items-center space-x-6">
            <div className="relative">
              <div className="w-20 h-20 bg-blue-100 dark:bg-blue-900 rounded-full flex items-center justify-center">
                <User className="w-10 h-10 text-blue-600 dark:text-blue-300" />
              </div>
              <Button
                size="sm"
                className="absolute bottom-0 right-0 h-6 w-6 rounded-full p-0"
              >
                <Camera className="w-3 h-3" />
              </Button>
            </div>
            <div className="space-y-2">
              <div>
                <h3 className="text-lg font-semibold">{user?.name}</h3>
                <p className="text-sm text-gray-600 dark:text-gray-400">{user?.email}</p>
              </div>
              <div className="flex items-center space-x-2">
                <Badge 
                  className={
                    user?.role === 'admin' 
                      ? "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-300"
                      : "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-300"
                  }
                >
                  {user?.role === 'admin' ? 'Administrator' : 'User'}
                </Badge>
                <span className="text-xs text-gray-500">
                  Bergabung: {user?.createdAt && new Date(user.createdAt).toLocaleDateString('id-ID')}
                </span>
              </div>
            </div>
          </div>
        </CardContent>
      </Card>

      {/* Profile Form */}
      <Card>
        <CardHeader>
          <CardTitle>Edit Profil</CardTitle>
        </CardHeader>
        <CardContent>
          <form onSubmit={handleSubmit(onSubmit)} className="space-y-6">
            <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
              <div className="space-y-2">
                <Label htmlFor="name">Nama Lengkap</Label>
                <Input
                  id="name"
                  {...register("name")}
                  className={errors.name ? "border-red-500" : ""}
                />
                {errors.name && (
                  <p className="text-sm text-red-500">{errors.name.message}</p>
                )}
              </div>

              <div className="space-y-2">
                <Label htmlFor="email">Email</Label>
                <Input
                  id="email"
                  type="email"
                  {...register("email")}
                  className={errors.email ? "border-red-500" : ""}
                  disabled // Email biasanya tidak bisa diubah
                />
                {errors.email && (
                  <p className="text-sm text-red-500">{errors.email.message}</p>
                )}
                <p className="text-xs text-gray-500">
                  Email tidak dapat diubah. Hubungi administrator jika perlu mengubah email.
                </p>
              </div>
            </div>

            <div className="space-y-4">
              <h4 className="text-lg font-medium">Preferensi</h4>
              
              <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
                <div className="space-y-2">
                  <Label htmlFor="theme">Tema</Label>
                  <Select 
                    value={watchedTheme} 
                    onValueChange={(value) => setValue("theme", value as any)}
                  >
                    <SelectTrigger>
                      <SelectValue placeholder="Pilih tema" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="light">Terang</SelectItem>
                      <SelectItem value="dark">Gelap</SelectItem>
                      <SelectItem value="system">Sistem</SelectItem>
                    </SelectContent>
                  </Select>
                </div>

                <div className="space-y-2">
                  <Label htmlFor="language">Bahasa</Label>
                  <Select 
                    value={watch("language")} 
                    onValueChange={(value) => setValue("language", value as any)}
                  >
                    <SelectTrigger>
                      <SelectValue placeholder="Pilih bahasa" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="id">Bahasa Indonesia</SelectItem>
                      <SelectItem value="en">English</SelectItem>
                    </SelectContent>
                  </Select>
                </div>
              </div>

              <div className="flex items-center justify-between">
                <div className="space-y-0.5">
                  <Label htmlFor="notifications">Notifikasi</Label>
                  <p className="text-sm text-gray-600 dark:text-gray-400">
                    Terima notifikasi email untuk alert sistem
                  </p>
                </div>
                <Switch
                  id="notifications"
                  checked={watchedNotifications}
                  onCheckedChange={(checked) => setValue("notifications", checked)}
                />
              </div>
            </div>

            <div className="flex justify-end space-x-4">
              <Button type="button" variant="outline">
                Batal
              </Button>
              <Button type="submit" disabled={loading}>
                <Save className="w-4 h-4 mr-2" />
                {loading ? "Menyimpan..." : "Simpan Perubahan"}
              </Button>
            </div>
          </form>
        </CardContent>
      </Card>
    </div>
  );
}