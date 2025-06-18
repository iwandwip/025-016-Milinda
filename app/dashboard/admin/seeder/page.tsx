"use client";

import { useState } from 'react';
import { Button } from '@/components/ui/button';
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Textarea } from '@/components/ui/textarea';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from '@/components/ui/select';
import { Badge } from '@/components/ui/badge';
import { toast } from 'sonner';
import { Database, Plus, Trash2, Play, RefreshCw } from 'lucide-react';

interface UserData {
  email: string;
  password: string;
  displayName: string;
  role: 'user' | 'admin';
}

interface SeederResult {
  email: string;
  status: 'success' | 'failed';
  uid?: string;
  error?: string;
}

export default function SeederPage() {
  const [users, setUsers] = useState<UserData[]>([
    { email: '', password: '', displayName: '', role: 'user' }
  ]);
  const [bulkData, setBulkData] = useState('');
  const [loading, setLoading] = useState(false);
  const [results, setResults] = useState<SeederResult[]>([]);

  const addUser = () => {
    setUsers([...users, { email: '', password: '', displayName: '', role: 'user' }]);
  };

  const removeUser = (index: number) => {
    setUsers(users.filter((_, i) => i !== index));
  };

  const updateUser = (index: number, field: keyof UserData, value: string) => {
    const updated = [...users];
    updated[index] = { ...updated[index], [field]: value };
    setUsers(updated);
  };

  const parseBulkData = () => {
    try {
      const lines = bulkData.trim().split('\n');
      const parsedUsers: UserData[] = [];

      for (const line of lines) {
        if (line.trim()) {
          const [email, password, displayName, role] = line.split(',').map(s => s.trim());
          if (email && password) {
            parsedUsers.push({
              email,
              password,
              displayName: displayName || '',
              role: (role === 'admin' ? 'admin' : 'user') as 'user' | 'admin'
            });
          }
        }
      }

      if (parsedUsers.length > 0) {
        setUsers(parsedUsers);
        setBulkData('');
        toast.success(`Parsed ${parsedUsers.length} users from bulk data`);
      }
    } catch (error) {
      toast.error('Error parsing bulk data. Please check the format.');
    }
  };

  const runSeeder = async () => {
    const validUsers = users.filter(user => user.email && user.password);
    
    if (validUsers.length === 0) {
      toast.error('Please add at least one user with email and password');
      return;
    }

    try {
      setLoading(true);
      setResults([]);
      
      const response = await fetch('/api/admin/seeder', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ users: validUsers })
      });

      if (!response.ok) throw new Error('Seeder request failed');
      
      const data = await response.json();
      setResults(data.results);
      
      const successCount = data.results.filter((r: SeederResult) => r.status === 'success').length;
      const failCount = data.results.filter((r: SeederResult) => r.status === 'failed').length;
      
      toast.success(`Seeder completed: ${successCount} success, ${failCount} failed`);
    } catch (error: any) {
      toast.error('Error running seeder: ' + error.message);
    } finally {
      setLoading(false);
    }
  };

  const clearAll = () => {
    setUsers([{ email: '', password: '', displayName: '', role: 'user' }]);
    setResults([]);
    setBulkData('');
  };

  return (
    <div className="p-6 space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold text-gray-900 dark:text-white">Data Seeder</h1>
          <p className="text-gray-600 dark:text-gray-400">Buat akun pengguna secara batch</p>
        </div>
        <div className="flex gap-2">
          <Button variant="outline" onClick={clearAll}>
            <Trash2 className="w-4 h-4 mr-2" />
            Clear All
          </Button>
          <Button onClick={runSeeder} disabled={loading}>
            {loading ? (
              <RefreshCw className="w-4 h-4 mr-2 animate-spin" />
            ) : (
              <Play className="w-4 h-4 mr-2" />
            )}
            Run Seeder
          </Button>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Manual Input */}
        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Plus className="w-5 h-5" />
              Manual Input
            </CardTitle>
            <CardDescription>
              Tambah user satu per satu secara manual
            </CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            {users.map((user, index) => (
              <div key={index} className="p-4 border rounded-lg space-y-3">
                <div className="flex items-center justify-between">
                  <span className="text-sm font-medium">User {index + 1}</span>
                  {users.length > 1 && (
                    <Button
                      size="sm"
                      variant="ghost"
                      onClick={() => removeUser(index)}
                    >
                      <Trash2 className="w-4 h-4" />
                    </Button>
                  )}
                </div>
                
                <div className="grid grid-cols-2 gap-3">
                  <div>
                    <Label>Email</Label>
                    <Input
                      type="email"
                      value={user.email}
                      onChange={(e) => updateUser(index, 'email', e.target.value)}
                      placeholder="user@example.com"
                    />
                  </div>
                  <div>
                    <Label>Password</Label>
                    <Input
                      type="password"
                      value={user.password}
                      onChange={(e) => updateUser(index, 'password', e.target.value)}
                      placeholder="Min 6 characters"
                    />
                  </div>
                </div>
                
                <div className="grid grid-cols-2 gap-3">
                  <div>
                    <Label>Display Name</Label>
                    <Input
                      value={user.displayName}
                      onChange={(e) => updateUser(index, 'displayName', e.target.value)}
                      placeholder="John Doe"
                    />
                  </div>
                  <div>
                    <Label>Role</Label>
                    <Select
                      value={user.role}
                      onValueChange={(value) => updateUser(index, 'role', value)}
                    >
                      <SelectTrigger>
                        <SelectValue />
                      </SelectTrigger>
                      <SelectContent>
                        <SelectItem value="user">User</SelectItem>
                        <SelectItem value="admin">Admin</SelectItem>
                      </SelectContent>
                    </Select>
                  </div>
                </div>
              </div>
            ))}
            
            <Button onClick={addUser} variant="outline" className="w-full">
              <Plus className="w-4 h-4 mr-2" />
              Add Another User
            </Button>
          </CardContent>
        </Card>

        {/* Bulk Input */}
        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Database className="w-5 h-5" />
              Bulk Input
            </CardTitle>
            <CardDescription>
              Import multiple users dengan format CSV
            </CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            <div>
              <Label>CSV Format</Label>
              <div className="text-xs text-gray-500 mb-2">
                Format: email,password,displayName,role
              </div>
              <Textarea
                value={bulkData}
                onChange={(e) => setBulkData(e.target.value)}
                placeholder={`user1@example.com,password123,John Doe,user
user2@example.com,password456,Jane Smith,admin
user3@example.com,password789,Bob Wilson,user`}
                className="min-h-[200px] font-mono text-sm"
              />
            </div>
            
            <Button onClick={parseBulkData} variant="outline" className="w-full">
              Parse Bulk Data
            </Button>
          </CardContent>
        </Card>
      </div>

      {/* Results */}
      {results.length > 0 && (
        <Card>
          <CardHeader>
            <CardTitle>Seeder Results</CardTitle>
            <CardDescription>
              Status hasil pembuatan akun
            </CardDescription>
          </CardHeader>
          <CardContent>
            <div className="space-y-2">
              {results.map((result, index) => (
                <div key={index} className="flex items-center justify-between p-3 border rounded">
                  <div>
                    <span className="font-medium">{result.email}</span>
                    {result.uid && (
                      <span className="text-xs text-gray-500 ml-2">UID: {result.uid}</span>
                    )}
                    {result.error && (
                      <div className="text-xs text-red-600 mt-1">{result.error}</div>
                    )}
                  </div>
                  <Badge variant={result.status === 'success' ? 'default' : 'destructive'}>
                    {result.status}
                  </Badge>
                </div>
              ))}
            </div>
          </CardContent>
        </Card>
      )}
    </div>
  );
}