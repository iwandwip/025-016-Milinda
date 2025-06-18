"use client";

import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { 
  Thermometer, 
  Droplets, 
  Sun, 
  AlertTriangle,
  TrendingUp,
  TrendingDown,
  Minus
} from "lucide-react";

interface MetricCardProps {
  title: string;
  value: string | number;
  unit?: string;
  category: string;
  trend?: 'up' | 'down' | 'stable';
  trendValue?: string;
  icon: React.ReactNode;
  categoryColor: string;
}

function MetricCard({ 
  title, 
  value, 
  unit, 
  category, 
  trend, 
  trendValue, 
  icon, 
  categoryColor 
}: MetricCardProps) {
  const getTrendIcon = () => {
    switch (trend) {
      case 'up': return <TrendingUp className="w-3 h-3" />;
      case 'down': return <TrendingDown className="w-3 h-3" />;
      default: return <Minus className="w-3 h-3" />;
    }
  };

  const getTrendColor = () => {
    switch (trend) {
      case 'up': return 'text-green-600';
      case 'down': return 'text-red-600';
      default: return 'text-gray-600';
    }
  };

  return (
    <Card>
      <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
        <CardTitle className="text-sm font-medium">{title}</CardTitle>
        <div className="w-4 h-4 text-muted-foreground">
          {icon}
        </div>
      </CardHeader>
      <CardContent>
        <div className="flex items-center justify-between">
          <div>
            <div className="text-2xl font-bold">
              {value}
              {unit && <span className="text-sm font-normal text-muted-foreground ml-1">{unit}</span>}
            </div>
            <Badge className={categoryColor}>
              {category}
            </Badge>
          </div>
          {trend && trendValue && (
            <div className={`flex items-center space-x-1 text-xs ${getTrendColor()}`}>
              {getTrendIcon()}
              <span>{trendValue}</span>
            </div>
          )}
        </div>
      </CardContent>
    </Card>
  );
}

export default function MetricsCards() {
  // Mock data - dalam implementasi nyata, data ini akan dari Firebase/real-time API
  const metrics = [
    {
      title: "Kelembapan Tanah",
      value: 42,
      unit: "%",
      category: "Sedang",
      trend: "down" as const,
      trendValue: "2%",
      icon: <Droplets className="w-4 h-4" />,
      categoryColor: "bg-yellow-100 text-yellow-800 hover:bg-yellow-200"
    },
    {
      title: "Suhu Lingkungan",
      value: 28,
      unit: "°C",
      category: "Normal",
      trend: "up" as const,
      trendValue: "1.2°C",
      icon: <Thermometer className="w-4 h-4" />,
      categoryColor: "bg-green-100 text-green-800 hover:bg-green-200"
    },
    {
      title: "Intensitas UV",
      value: 0.65,
      unit: "V",
      category: "Sedang",
      trend: "stable" as const,
      trendValue: "0%",
      icon: <Sun className="w-4 h-4" />,
      categoryColor: "bg-yellow-100 text-yellow-800 hover:bg-yellow-200"
    },
    {
      title: "Deteksi Mikroplastik",
      value: 67,
      unit: "%",
      category: "Tinggi",
      trend: "up" as const,
      trendValue: "5%",
      icon: <AlertTriangle className="w-4 h-4" />,
      categoryColor: "bg-red-100 text-red-800 hover:bg-red-200"
    }
  ];

  return (
    <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-4">
      {metrics.map((metric, index) => (
        <MetricCard key={index} {...metric} />
      ))}
    </div>
  );
}