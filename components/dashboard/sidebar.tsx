"use client";

import { useState } from "react";
import Link from "next/link";
import { usePathname } from "next/navigation";
import { cn } from "@/lib/utils";
import { Button } from "@/components/ui/button";
import { 
  LayoutDashboard,
  History,
  User,
  Settings,
  LogOut,
  Menu,
  X,
  Activity,
  AlertTriangle,
  Shield,
  Users,
  Database
} from "lucide-react";
import { useAuth } from "@/lib/hooks/use-auth";
import { logout } from "@/lib/firebase/auth";
import { toast } from "sonner";

const navigation = [
  {
    name: "Dashboard",
    href: "/dashboard",
    icon: LayoutDashboard,
    description: "Overview monitoring"
  },
  {
    name: "Real-time Monitor",
    href: "/dashboard/monitor",
    icon: Activity,
    description: "Live sensor data"
  },
  {
    name: "Riwayat Data",
    href: "/dashboard/history",
    icon: History,
    description: "Data historis"
  },
  {
    name: "Alert System",
    href: "/dashboard/alerts",
    icon: AlertTriangle,
    description: "Sistem peringatan"
  },
  {
    name: "Profil",
    href: "/dashboard/profile",
    icon: User,
    description: "Pengaturan akun"
  },
];

const adminNavigation = [
  {
    name: "User Management",
    href: "/dashboard/admin/users",
    icon: Users,
    description: "Kelola pengguna"
  },
  {
    name: "Data Seeder",
    href: "/dashboard/admin/seeder",
    icon: Database,
    description: "Seeder data user"
  },
];

export default function Sidebar() {
  const [collapsed, setCollapsed] = useState(false);
  const pathname = usePathname();
  const { user } = useAuth();

  const handleLogout = async () => {
    try {
      await logout();
      toast.success("Logout berhasil");
    } catch (error: any) {
      toast.error("Logout gagal: " + error.message);
    }
  };

  return (
    <div className={cn(
      "bg-white dark:bg-gray-900 border-r border-gray-200 dark:border-gray-700 transition-all duration-300",
      collapsed ? "w-16" : "w-64"
    )}>
      <div className="flex h-full flex-col">
        {/* Header */}
        <div className="flex items-center justify-between p-4 border-b border-gray-200 dark:border-gray-700">
          {!collapsed && (
            <div className="flex items-center space-x-2">
              <div className="w-8 h-8 bg-blue-600 rounded-lg flex items-center justify-center">
                <Activity className="w-5 h-5 text-white" />
              </div>
              <span className="font-semibold text-gray-900 dark:text-white">
                Mikroplastik
              </span>
            </div>
          )}
          <Button
            variant="ghost"
            size="sm"
            onClick={() => setCollapsed(!collapsed)}
            className="p-2"
          >
            {collapsed ? <Menu className="w-4 h-4" /> : <X className="w-4 h-4" />}
          </Button>
        </div>

        {/* User Info */}
        {!collapsed && user && (
          <div className="p-4 border-b border-gray-200 dark:border-gray-700">
            <div className="flex items-center space-x-3">
              <div className="w-10 h-10 bg-blue-100 dark:bg-blue-900 rounded-full flex items-center justify-center">
                <User className="w-5 h-5 text-blue-600 dark:text-blue-300" />
              </div>
              <div className="min-w-0 flex-1">
                <p className="text-sm font-medium text-gray-900 dark:text-white truncate">
                  {user.name}
                </p>
                <p className="text-xs text-gray-500 dark:text-gray-400 truncate">
                  {user.email}
                </p>
                <span className={cn(
                  "inline-flex items-center px-2 py-0.5 rounded-full text-xs font-medium",
                  user.role === 'admin' 
                    ? "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-300"
                    : "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-300"
                )}>
                  {user.role === 'admin' ? 'Administrator' : 'User'}
                </span>
              </div>
            </div>
          </div>
        )}

        {/* Navigation */}
        <nav className="flex-1 p-4 space-y-2">
          {navigation.map((item) => {
            const isActive = pathname === item.href;
            return (
              <Link key={item.name} href={item.href}>
                <div className={cn(
                  "flex items-center px-3 py-2 text-sm font-medium rounded-lg transition-colors",
                  isActive
                    ? "bg-blue-100 text-blue-700 dark:bg-blue-900 dark:text-blue-300"
                    : "text-gray-700 dark:text-gray-300 hover:bg-gray-100 dark:hover:bg-gray-800"
                )}>
                  <item.icon className={cn(
                    "flex-shrink-0 w-5 h-5",
                    collapsed ? "mx-auto" : "mr-3"
                  )} />
                  {!collapsed && (
                    <div className="min-w-0 flex-1">
                      <div>{item.name}</div>
                      <div className="text-xs text-gray-500 dark:text-gray-400">
                        {item.description}
                      </div>
                    </div>
                  )}
                </div>
              </Link>
            );
          })}

          {/* Admin Section */}
          {user?.role === 'admin' && (
            <>
              {!collapsed && (
                <div className="pt-4 pb-2">
                  <div className="flex items-center px-3 py-2">
                    <Shield className="w-4 h-4 mr-2 text-red-600 dark:text-red-400" />
                    <span className="text-xs font-semibold text-red-600 dark:text-red-400 uppercase tracking-wider">
                      Admin Panel
                    </span>
                  </div>
                </div>
              )}
              {adminNavigation.map((item) => {
                const isActive = pathname === item.href;
                return (
                  <Link key={item.name} href={item.href}>
                    <div className={cn(
                      "flex items-center px-3 py-2 text-sm font-medium rounded-lg transition-colors",
                      isActive
                        ? "bg-red-100 text-red-700 dark:bg-red-900 dark:text-red-300"
                        : "text-gray-700 dark:text-gray-300 hover:bg-red-50 dark:hover:bg-red-900/20"
                    )}>
                      <item.icon className={cn(
                        "flex-shrink-0 w-5 h-5",
                        collapsed ? "mx-auto" : "mr-3"
                      )} />
                      {!collapsed && (
                        <div className="min-w-0 flex-1">
                          <div>{item.name}</div>
                          <div className="text-xs text-gray-500 dark:text-gray-400">
                            {item.description}
                          </div>
                        </div>
                      )}
                    </div>
                  </Link>
                );
              })}
            </>
          )}
        </nav>

        {/* Logout */}
        <div className="p-4 border-t border-gray-200 dark:border-gray-700">
          <Button
            variant="ghost"
            onClick={handleLogout}
            className={cn(
              "w-full justify-start text-gray-700 dark:text-gray-300 hover:bg-red-50 hover:text-red-700 dark:hover:bg-red-900 dark:hover:text-red-300",
              collapsed && "justify-center"
            )}
          >
            <LogOut className={cn("w-5 h-5", !collapsed && "mr-3")} />
            {!collapsed && "Logout"}
          </Button>
        </div>
      </div>
    </div>
  );
}