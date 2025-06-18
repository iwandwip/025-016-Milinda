"use client";

import { useState } from "react";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { 
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import { Calendar } from "@/components/ui/calendar";
import {
  Popover,
  PopoverContent,
  PopoverTrigger,
} from "@/components/ui/popover";
import { CalendarIcon, Search, Download, Eye } from "lucide-react";
import { format } from "date-fns";
import { id } from "date-fns/locale";
import { cn } from "@/lib/utils";

// Mock data
const generateMockHistoryData = () => {
  const data = [];
  const pots = [
    { id: 'pot_1', name: 'Tanah Kering', hasPlastic: false },
    { id: 'pot_2', name: 'Tanah Basah', hasPlastic: false },
    { id: 'pot_3', name: 'Tanah Kering + Mikroplastik', hasPlastic: true },
    { id: 'pot_4', name: 'Tanah Basah + Mikroplastik', hasPlastic: true },
  ];

  for (let i = 0; i < 100; i++) {
    const pot = pots[Math.floor(Math.random() * pots.length)];
    const timestamp = new Date(Date.now() - Math.random() * 7 * 24 * 60 * 60 * 1000);
    
    data.push({
      id: `data_${i + 1}`,
      timestamp,
      pot_id: pot.id,
      pot_name: pot.name,
      has_microplastic: pot.hasPlastic,
      moisture: {
        value: Math.random() * 80 + 10,
        category: ['basah', 'sedang', 'kering'][Math.floor(Math.random() * 3)]
      },
      temperature: {
        value: Math.random() * 15 + 20,
        category: ['rendah', 'normal', 'tinggi'][Math.floor(Math.random() * 3)]
      },
      uv_intensity: {
        value: Math.random() * 0.8 + 0.2,
        category: ['sedikit', 'sedang', 'banyak'][Math.floor(Math.random() * 3)]
      },
      fuzzy_result: {
        percentage: Math.random() * 100,
        category: ['rendah', 'sedang', 'tinggi'][Math.floor(Math.random() * 3)]
      },
      device_id: `ESP32_00${Math.floor(Math.random() * 3) + 1}`,
      battery_level: Math.random() * 40 + 60
    });
  }

  return data.sort((a, b) => b.timestamp.getTime() - a.timestamp.getTime());
};

export default function DataTable() {
  const [data, setData] = useState(generateMockHistoryData());
  const [filteredData, setFilteredData] = useState(data);
  const [currentPage, setCurrentPage] = useState(1);
  const [itemsPerPage] = useState(10);
  
  // Filters
  const [searchTerm, setSearchTerm] = useState("");
  const [selectedPot, setSelectedPot] = useState<string>("all");
  const [selectedCategory, setSelectedCategory] = useState<string>("all");
  const [dateFrom, setDateFrom] = useState<Date>();
  const [dateTo, setDateTo] = useState<Date>();

  // Apply filters
  const applyFilters = () => {
    let filtered = [...data];

    // Search filter
    if (searchTerm) {
      filtered = filtered.filter(item => 
        item.pot_name.toLowerCase().includes(searchTerm.toLowerCase()) ||
        item.device_id.toLowerCase().includes(searchTerm.toLowerCase())
      );
    }

    // Pot filter
    if (selectedPot !== "all") {
      filtered = filtered.filter(item => item.pot_id === selectedPot);
    }

    // Category filter
    if (selectedCategory !== "all") {
      filtered = filtered.filter(item => item.fuzzy_result.category === selectedCategory);
    }

    // Date range filter
    if (dateFrom) {
      filtered = filtered.filter(item => item.timestamp >= dateFrom);
    }
    if (dateTo) {
      filtered = filtered.filter(item => item.timestamp <= dateTo);
    }

    setFilteredData(filtered);
    setCurrentPage(1);
  };

  // Reset filters
  const resetFilters = () => {
    setSearchTerm("");
    setSelectedPot("all");
    setSelectedCategory("all");
    setDateFrom(undefined);
    setDateTo(undefined);
    setFilteredData(data);
    setCurrentPage(1);
  };

  // Export to CSV
  const exportToCSV = () => {
    const headers = [
      'Timestamp',
      'Pot',
      'Kelembapan (%)',
      'Kategori Kelembapan',
      'Suhu (°C)',
      'Kategori Suhu',
      'UV (V)',
      'Kategori UV',
      'Mikroplastik (%)',
      'Kategori Mikroplastik',
      'Device ID',
      'Battery (%)'
    ];

    const csvContent = [
      headers.join(','),
      ...filteredData.map(row => [
        format(row.timestamp, 'yyyy-MM-dd HH:mm:ss'),
        row.pot_name,
        row.moisture.value.toFixed(2),
        row.moisture.category,
        row.temperature.value.toFixed(2),
        row.temperature.category,
        row.uv_intensity.value.toFixed(3),
        row.uv_intensity.category,
        row.fuzzy_result.percentage.toFixed(2),
        row.fuzzy_result.category,
        row.device_id,
        row.battery_level?.toFixed(0) || 'N/A'
      ].join(','))
    ].join('\n');

    const blob = new Blob([csvContent], { type: 'text/csv' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `mikroplastik_data_${format(new Date(), 'yyyy-MM-dd')}.csv`;
    a.click();
    window.URL.revokeObjectURL(url);
  };

  // Pagination
  const totalPages = Math.ceil(filteredData.length / itemsPerPage);
  const startIndex = (currentPage - 1) * itemsPerPage;
  const endIndex = startIndex + itemsPerPage;
  const currentData = filteredData.slice(startIndex, endIndex);

  const getCategoryBadge = (category: string, type: 'mikroplastik' | 'kelembapan' | 'suhu' | 'uv') => {
    const baseClass = "text-xs";
    
    if (type === 'mikroplastik') {
      switch (category) {
        case 'tinggi': return <Badge className={`${baseClass} bg-red-100 text-red-800`}>Tinggi</Badge>;
        case 'sedang': return <Badge className={`${baseClass} bg-yellow-100 text-yellow-800`}>Sedang</Badge>;
        case 'rendah': return <Badge className={`${baseClass} bg-green-100 text-green-800`}>Rendah</Badge>;
      }
    }
    
    return <Badge variant="outline" className={baseClass}>{category}</Badge>;
  };

  return (
    <div className="space-y-6">
      {/* Filters */}
      <div className="bg-white dark:bg-gray-900 p-4 rounded-lg border space-y-4">
        <div className="flex items-center justify-between">
          <h3 className="text-lg font-semibold">Filter Data</h3>
          <div className="flex space-x-2">
            <Button variant="outline" onClick={resetFilters}>
              Reset
            </Button>
            <Button onClick={applyFilters}>
              <Search className="w-4 h-4 mr-2" />
              Terapkan Filter
            </Button>
            <Button onClick={exportToCSV} variant="outline">
              <Download className="w-4 h-4 mr-2" />
              Export CSV
            </Button>
          </div>
        </div>
        
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-5 gap-4">
          <div>
            <label className="text-sm font-medium mb-2 block">Pencarian</label>
            <Input
              placeholder="Cari pot atau device..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
            />
          </div>
          
          <div>
            <label className="text-sm font-medium mb-2 block">Pot</label>
            <Select value={selectedPot} onValueChange={setSelectedPot}>
              <SelectTrigger>
                <SelectValue placeholder="Pilih pot" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="all">Semua Pot</SelectItem>
                <SelectItem value="pot_1">Tanah Kering</SelectItem>
                <SelectItem value="pot_2">Tanah Basah</SelectItem>
                <SelectItem value="pot_3">Tanah Kering + Mikroplastik</SelectItem>
                <SelectItem value="pot_4">Tanah Basah + Mikroplastik</SelectItem>
              </SelectContent>
            </Select>
          </div>

          <div>
            <label className="text-sm font-medium mb-2 block">Kategori</label>
            <Select value={selectedCategory} onValueChange={setSelectedCategory}>
              <SelectTrigger>
                <SelectValue placeholder="Pilih kategori" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="all">Semua Kategori</SelectItem>
                <SelectItem value="rendah">Rendah</SelectItem>
                <SelectItem value="sedang">Sedang</SelectItem>
                <SelectItem value="tinggi">Tinggi</SelectItem>
              </SelectContent>
            </Select>
          </div>

          <div>
            <label className="text-sm font-medium mb-2 block">Dari Tanggal</label>
            <Popover>
              <PopoverTrigger asChild>
                <Button
                  variant="outline"
                  className={cn(
                    "w-full justify-start text-left font-normal",
                    !dateFrom && "text-muted-foreground"
                  )}
                >
                  <CalendarIcon className="mr-2 h-4 w-4" />
                  {dateFrom ? format(dateFrom, "dd MMM yyyy", { locale: id }) : "Pilih tanggal"}
                </Button>
              </PopoverTrigger>
              <PopoverContent className="w-auto p-0" align="start">
                <Calendar
                  mode="single"
                  selected={dateFrom}
                  onSelect={setDateFrom}
                  initialFocus
                />
              </PopoverContent>
            </Popover>
          </div>

          <div>
            <label className="text-sm font-medium mb-2 block">Sampai Tanggal</label>
            <Popover>
              <PopoverTrigger asChild>
                <Button
                  variant="outline"
                  className={cn(
                    "w-full justify-start text-left font-normal",
                    !dateTo && "text-muted-foreground"
                  )}
                >
                  <CalendarIcon className="mr-2 h-4 w-4" />
                  {dateTo ? format(dateTo, "dd MMM yyyy", { locale: id }) : "Pilih tanggal"}
                </Button>
              </PopoverTrigger>
              <PopoverContent className="w-auto p-0" align="start">
                <Calendar
                  mode="single"
                  selected={dateTo}
                  onSelect={setDateTo}
                  initialFocus
                />
              </PopoverContent>
            </Popover>
          </div>
        </div>
      </div>

      {/* Results Info */}
      <div className="flex items-center justify-between">
        <p className="text-sm text-gray-600 dark:text-gray-400">
          Menampilkan {startIndex + 1}-{Math.min(endIndex, filteredData.length)} dari {filteredData.length} hasil
        </p>
      </div>

      {/* Table */}
      <div className="bg-white dark:bg-gray-900 rounded-lg border overflow-hidden">
        <Table>
          <TableHeader>
            <TableRow>
              <TableHead>Waktu</TableHead>
              <TableHead>Pot</TableHead>
              <TableHead>Kelembapan</TableHead>
              <TableHead>Suhu</TableHead>
              <TableHead>UV</TableHead>
              <TableHead>Mikroplastik</TableHead>
              <TableHead>Device</TableHead>
              <TableHead>Battery</TableHead>
              <TableHead>Aksi</TableHead>
            </TableRow>
          </TableHeader>
          <TableBody>
            {currentData.map((row) => (
              <TableRow key={row.id}>
                <TableCell className="font-mono text-sm">
                  {format(row.timestamp, 'dd/MM/yyyy HH:mm', { locale: id })}
                </TableCell>
                <TableCell>
                  <div>
                    <div className="font-medium">{row.pot_name}</div>
                    {row.has_microplastic && (
                      <Badge variant="outline" className="text-xs mt-1">
                        + Mikroplastik
                      </Badge>
                    )}
                  </div>
                </TableCell>
                <TableCell>
                  <div className="space-y-1">
                    <div className="font-medium">{row.moisture.value.toFixed(1)}%</div>
                    {getCategoryBadge(row.moisture.category, 'kelembapan')}
                  </div>
                </TableCell>
                <TableCell>
                  <div className="space-y-1">
                    <div className="font-medium">{row.temperature.value.toFixed(1)}°C</div>
                    {getCategoryBadge(row.temperature.category, 'suhu')}
                  </div>
                </TableCell>
                <TableCell>
                  <div className="space-y-1">
                    <div className="font-medium">{row.uv_intensity.value.toFixed(2)}V</div>
                    {getCategoryBadge(row.uv_intensity.category, 'uv')}
                  </div>
                </TableCell>
                <TableCell>
                  <div className="space-y-1">
                    <div className="font-medium">{row.fuzzy_result.percentage.toFixed(1)}%</div>
                    {getCategoryBadge(row.fuzzy_result.category, 'mikroplastik')}
                  </div>
                </TableCell>
                <TableCell>
                  <div className="font-mono text-sm">{row.device_id}</div>
                </TableCell>
                <TableCell>
                  <div className="flex items-center space-x-2">
                    <div className={cn(
                      "w-2 h-2 rounded-full",
                      (row.battery_level || 0) > 20 ? "bg-green-500" : "bg-red-500"
                    )}></div>
                    <span className="text-sm">{row.battery_level?.toFixed(0)}%</span>
                  </div>
                </TableCell>
                <TableCell>
                  <Button variant="ghost" size="sm">
                    <Eye className="w-4 h-4" />
                  </Button>
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </div>

      {/* Pagination */}
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-2">
          <Button
            variant="outline"
            size="sm"
            onClick={() => setCurrentPage(prev => Math.max(prev - 1, 1))}
            disabled={currentPage === 1}
          >
            Previous
          </Button>
          <div className="flex items-center space-x-1">
            {Array.from({ length: Math.min(5, totalPages) }, (_, i) => {
              const page = i + 1;
              return (
                <Button
                  key={page}
                  variant={currentPage === page ? "default" : "outline"}
                  size="sm"
                  onClick={() => setCurrentPage(page)}
                >
                  {page}
                </Button>
              );
            })}
          </div>
          <Button
            variant="outline"
            size="sm"
            onClick={() => setCurrentPage(prev => Math.min(prev + 1, totalPages))}
            disabled={currentPage === totalPages}
          >
            Next
          </Button>
        </div>
        <p className="text-sm text-gray-600 dark:text-gray-400">
          Halaman {currentPage} dari {totalPages}
        </p>
      </div>
    </div>
  );
}