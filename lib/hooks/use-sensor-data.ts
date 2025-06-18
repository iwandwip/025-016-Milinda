"use client";

import { useState, useEffect } from "react";
import { SensorData } from "@/types/sensor";
import { subscribeToSensorData, subscribeToLatestSensorData } from "@/lib/firebase/firestore";

export const useSensorData = (options: {
  potId?: string;
  limit?: number;
  category?: string;
  realTime?: boolean;
} = {}) => {
  const [data, setData] = useState<SensorData[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (!options.realTime) {
      setLoading(false);
      return;
    }

    setLoading(true);
    setError(null);

    const unsubscribe = subscribeToSensorData(
      (newData) => {
        setData(newData);
        setLoading(false);
      },
      {
        potId: options.potId,
        limit: options.limit,
        category: options.category
      }
    );

    return () => {
      unsubscribe();
    };
  }, [options.potId, options.limit, options.category, options.realTime]);

  return { data, loading, error };
};

export const useLatestSensorData = () => {
  const [latestData, setLatestData] = useState<{ [potId: string]: SensorData }>({});
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    setLoading(true);
    setError(null);

    const unsubscribe = subscribeToLatestSensorData((data) => {
      setLatestData(data);
      setLoading(false);
    });

    return () => {
      unsubscribe();
    };
  }, []);

  return { latestData, loading, error };
};