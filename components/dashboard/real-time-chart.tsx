"use client";

import { useState, useEffect } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Badge } from "@/components/ui/badge";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
  BarChart,
  Bar
} from "recharts";

// Mock data generator
const generateMockData = () => {
  const now = new Date();
  const data = [];
  
  for (let i = 23; i >= 0; i--) {
    const time = new Date(now.getTime() - i * 60 * 60 * 1000);
    data.push({
      time: time.toLocaleTimeString('id-ID', { hour: '2-digit', minute: '2-digit' }),
      pot1: Math.random() * 30 + 20, // 20-50%
      pot2: Math.random() * 40 + 30, // 30-70%
      pot3: Math.random() * 30 + 45, // 45-75%
      pot4: Math.random() * 25 + 55, // 55-80%
      temperature: Math.random() * 10 + 20, // 20-30°C
      uv: Math.random() * 0.5 + 0.25, // 0.25-0.75V
    });
  }
  
  return data;
};

const potInfo = [
  { id: 'pot1', name: 'Tanah Kering', color: '#8884d8', hasPlastic: false },
  { id: 'pot2', name: 'Tanah Basah', color: '#82ca9d', hasPlastic: false },
  { id: 'pot3', name: 'Tanah Kering + Mikroplastik', color: '#ffc658', hasPlastic: true },
  { id: 'pot4', name: 'Tanah Basah + Mikroplastik', color: '#ff7300', hasPlastic: true },
];

export default function RealTimeChart() {
  const [data, setData] = useState(generateMockData());
  const [activeTab, setActiveTab] = useState("mikroplastik");

  // Simulate real-time data updates
  useEffect(() => {
    const interval = setInterval(() => {
      setData(prevData => {
        const newData = [...prevData.slice(1)];
        const now = new Date();
        newData.push({
          time: now.toLocaleTimeString('id-ID', { hour: '2-digit', minute: '2-digit' }),
          pot1: Math.random() * 30 + 20,
          pot2: Math.random() * 40 + 30,
          pot3: Math.random() * 30 + 45,
          pot4: Math.random() * 25 + 55,
          temperature: Math.random() * 10 + 20,
          uv: Math.random() * 0.5 + 0.25,
        });
        return newData;
      });
    }, 5000); // Update every 5 seconds

    return () => clearInterval(interval);
  }, []);

  const latestData = data[data.length - 1];

  return (
    <div className="space-y-6">
      {/* Pot Status Cards */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        {potInfo.map((pot) => {
          const value = latestData[pot.id as keyof typeof latestData] as number;
          const category = value > 60 ? 'Tinggi' : value > 40 ? 'Sedang' : 'Rendah';
          const categoryColor = value > 60 ? 'bg-red-100 text-red-800' : 
                               value > 40 ? 'bg-yellow-100 text-yellow-800' : 
                               'bg-green-100 text-green-800';
          
          return (
            <Card key={pot.id}>
              <CardHeader className="pb-2">
                <CardTitle className="text-sm font-medium">{pot.name}</CardTitle>
              </CardHeader>
              <CardContent>
                <div className="space-y-2">
                  <div className="text-2xl font-bold">
                    {value.toFixed(1)}%
                  </div>
                  <div className="flex items-center justify-between">
                    <Badge className={categoryColor}>
                      {category}
                    </Badge>
                    {pot.hasPlastic && (
                      <Badge variant="outline" className="text-xs">
                        + Mikroplastik
                      </Badge>
                    )}
                  </div>
                  <div className="w-full bg-gray-200 rounded-full h-2">
                    <div 
                      className="h-2 rounded-full transition-all duration-500"
                      style={{ 
                        width: `${value}%`,
                        backgroundColor: pot.color
                      }}
                    ></div>
                  </div>
                </div>
              </CardContent>
            </Card>
          );
        })}
      </div>

      {/* Charts */}
      <Card>
        <CardHeader>
          <CardTitle>Monitoring Real-time</CardTitle>
        </CardHeader>
        <CardContent>
          <Tabs value={activeTab} onValueChange={setActiveTab}>
            <TabsList className="grid w-full grid-cols-3">
              <TabsTrigger value="mikroplastik">Deteksi Mikroplastik</TabsTrigger>
              <TabsTrigger value="environment">Lingkungan</TabsTrigger>
              <TabsTrigger value="comparison">Perbandingan</TabsTrigger>
            </TabsList>

            <TabsContent value="mikroplastik" className="mt-6">
              <ResponsiveContainer width="100%" height={400}>
                <LineChart data={data}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis domain={[0, 100]} />
                  <Tooltip 
                    formatter={(value: number) => [`${value.toFixed(1)}%`, 'Mikroplastik']}
                    labelFormatter={(label) => `Waktu: ${label}`}
                  />
                  <Legend />
                  {potInfo.map((pot) => (
                    <Line 
                      key={pot.id}
                      type="monotone" 
                      dataKey={pot.id} 
                      stroke={pot.color} 
                      strokeWidth={2}
                      name={pot.name}
                      connectNulls={false}
                    />
                  ))}
                </LineChart>
              </ResponsiveContainer>
            </TabsContent>

            <TabsContent value="environment" className="mt-6">
              <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
                <div>
                  <h3 className="text-lg font-semibold mb-4">Suhu Lingkungan</h3>
                  <ResponsiveContainer width="100%" height={300}>
                    <LineChart data={data}>
                      <CartesianGrid strokeDasharray="3 3" />
                      <XAxis dataKey="time" />
                      <YAxis domain={[15, 35]} />
                      <Tooltip 
                        formatter={(value: number) => [`${value.toFixed(1)}°C`, 'Suhu']}
                      />
                      <Line 
                        type="monotone" 
                        dataKey="temperature" 
                        stroke="#ff7300" 
                        strokeWidth={2}
                        name="Suhu"
                      />
                    </LineChart>
                  </ResponsiveContainer>
                </div>
                
                <div>
                  <h3 className="text-lg font-semibold mb-4">Intensitas UV</h3>
                  <ResponsiveContainer width="100%" height={300}>
                    <LineChart data={data}>
                      <CartesianGrid strokeDasharray="3 3" />
                      <XAxis dataKey="time" />
                      <YAxis domain={[0, 1]} />
                      <Tooltip 
                        formatter={(value: number) => [`${value.toFixed(2)}V`, 'UV']}
                      />
                      <Line 
                        type="monotone" 
                        dataKey="uv" 
                        stroke="#ffcc00" 
                        strokeWidth={2}
                        name="UV Intensity"
                      />
                    </LineChart>
                  </ResponsiveContainer>
                </div>
              </div>
            </TabsContent>

            <TabsContent value="comparison" className="mt-6">
              <ResponsiveContainer width="100%" height={400}>
                <BarChart data={data.slice(-6)}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis domain={[0, 100]} />
                  <Tooltip 
                    formatter={(value: number) => [`${value.toFixed(1)}%`, 'Mikroplastik']}
                  />
                  <Legend />
                  {potInfo.map((pot) => (
                    <Bar 
                      key={pot.id}
                      dataKey={pot.id} 
                      fill={pot.color} 
                      name={pot.name}
                    />
                  ))}
                </BarChart>
              </ResponsiveContainer>
            </TabsContent>
          </Tabs>
        </CardContent>
      </Card>
    </div>
  );
}