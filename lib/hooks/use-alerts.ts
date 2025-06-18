"use client";

import { useState, useEffect } from "react";
import { SystemAlert } from "@/types/sensor";
import { subscribeToAlerts } from "@/lib/firebase/firestore";

export const useAlerts = (options: {
  resolved?: boolean;
  severity?: string;
  limit?: number;
  realTime?: boolean;
} = {}) => {
  const [alerts, setAlerts] = useState<SystemAlert[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (!options.realTime) {
      setLoading(false);
      return;
    }

    setLoading(true);
    setError(null);

    const unsubscribe = subscribeToAlerts(
      (newAlerts) => {
        setAlerts(newAlerts);
        setLoading(false);
      },
      {
        resolved: options.resolved,
        severity: options.severity,
        limit: options.limit
      }
    );

    return () => {
      unsubscribe();
    };
  }, [options.resolved, options.severity, options.limit, options.realTime]);

  const unresolvedAlerts = alerts.filter(alert => !alert.is_resolved);
  const criticalAlerts = alerts.filter(alert => alert.severity === 'critical');
  const highAlerts = alerts.filter(alert => alert.severity === 'high');

  return { 
    alerts, 
    unresolvedAlerts,
    criticalAlerts,
    highAlerts,
    loading, 
    error 
  };
};