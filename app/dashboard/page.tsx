import MetricsCards from "@/components/dashboard/metrics-cards";
import RealTimeChart from "@/components/dashboard/real-time-chart";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Alert, AlertDescription, AlertTitle } from "@/components/ui/alert";
import { AlertTriangle, CheckCircle, Clock, Wifi } from "lucide-react";

export default function DashboardPage() {
  return (
    <div className="p-6 space-y-6">
      {/* Page Header */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-2xl font-bold text-gray-900 dark:text-white">
            Dashboard Monitoring
          </h1>
          <p className="text-gray-600 dark:text-gray-400">
            Sistem deteksi mikroplastik pada tanah - Real-time monitoring
          </p>
        </div>
        <div className="flex items-center space-x-2">
          <Badge variant="outline" className="flex items-center space-x-1">
            <Wifi className="w-3 h-3" />
            <span>4 Device Online</span>
          </Badge>
          <Badge className="bg-green-100 text-green-800 flex items-center space-x-1">
            <CheckCircle className="w-3 h-3" />
            <span>System Normal</span>
          </Badge>
        </div>
      </div>

      {/* System Alerts */}
      <div className="space-y-4">
        <Alert className="border-red-200 bg-red-50">
          <AlertTriangle className="h-4 w-4" />
          <AlertTitle>Peringatan Mikroplastik Tinggi</AlertTitle>
          <AlertDescription>
            Pot 3 (Tanah Kering + Mikroplastik) menunjukkan tingkat deteksi 67% - di atas threshold normal.
            <span className="text-xs text-gray-500 ml-2">• 2 menit yang lalu</span>
          </AlertDescription>
        </Alert>

        <Alert className="border-yellow-200 bg-yellow-50">
          <Clock className="h-4 w-4" />
          <AlertTitle>Maintenance Scheduled</AlertTitle>
          <AlertDescription>
            Kalibrasi sensor dijadwalkan besok pukul 14:00 WIB.
            <span className="text-xs text-gray-500 ml-2">• 1 jam yang lalu</span>
          </AlertDescription>
        </Alert>
      </div>

      {/* Metrics Overview */}
      <MetricsCards />

      {/* Real-time Charts */}
      <RealTimeChart />

      {/* Quick Stats */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        <Card>
          <CardHeader>
            <CardTitle className="text-lg">Status Hari Ini</CardTitle>
          </CardHeader>
          <CardContent className="space-y-3">
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">Total Readings</span>
              <span className="font-semibold">1,440</span>
            </div>
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">Alert Triggered</span>
              <span className="font-semibold text-red-600">3</span>
            </div>
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">Uptime</span>
              <span className="font-semibold text-green-600">99.8%</span>
            </div>
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle className="text-lg">Device Status</CardTitle>
          </CardHeader>
          <CardContent className="space-y-3">
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">ESP32_001</span>
              <Badge className="bg-green-100 text-green-800">Online</Badge>
            </div>
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">ESP32_002</span>
              <Badge className="bg-green-100 text-green-800">Online</Badge>
            </div>
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">ESP32_003</span>
              <Badge className="bg-yellow-100 text-yellow-800">Warning</Badge>
            </div>
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle className="text-lg">Rangkuman Mingguan</CardTitle>
          </CardHeader>
          <CardContent className="space-y-3">
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">Avg. Mikroplastik</span>
              <span className="font-semibold">45.2%</span>
            </div>
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">Peak Detection</span>
              <span className="font-semibold">78.5%</span>
            </div>
            <div className="flex justify-between items-center">
              <span className="text-sm text-gray-600">Data Points</span>
              <span className="font-semibold">10,080</span>
            </div>
          </CardContent>
        </Card>
      </div>
    </div>
  );
}