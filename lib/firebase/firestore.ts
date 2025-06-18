import { 
  collection, 
  onSnapshot, 
  query, 
  orderBy, 
  limit, 
  where,
  Timestamp 
} from "firebase/firestore";
import { db } from "./config";
import { SensorData, SystemAlert } from "@/types/sensor";

export const subscribeToSensorData = (
  callback: (data: SensorData[]) => void,
  options: {
    potId?: string;
    limit?: number;
    category?: string;
  } = {}
) => {
  let q = query(
    collection(db, 'sensor_data'),
    orderBy('timestamp', 'desc')
  );

  if (options.potId && options.potId !== 'all') {
    q = query(q, where('pot_id', '==', options.potId));
  }

  if (options.category && options.category !== 'all') {
    q = query(q, where('fuzzy_result.category', '==', options.category));
  }

  if (options.limit) {
    q = query(q, limit(options.limit));
  }

  return onSnapshot(q, (snapshot) => {
    const data: SensorData[] = snapshot.docs.map(doc => {
      const docData = doc.data();
      return {
        id: doc.id,
        timestamp: docData.timestamp instanceof Timestamp 
          ? docData.timestamp.toDate() 
          : new Date(docData.timestamp),
        pot_id: docData.pot_id,
        pot_name: docData.pot_name,
        soil_condition: docData.soil_condition,
        has_microplastic: docData.has_microplastic,
        moisture: docData.moisture,
        temperature: docData.temperature,
        uv_intensity: docData.uv_intensity,
        fuzzy_result: docData.fuzzy_result,
        device_id: docData.device_id,
        battery_level: docData.battery_level,
        signal_strength: docData.signal_strength,
        created_by: docData.created_by,
        notes: docData.notes
      };
    });
    
    callback(data);
  });
};

export const subscribeToAlerts = (
  callback: (alerts: SystemAlert[]) => void,
  options: {
    resolved?: boolean;
    severity?: string;
    limit?: number;
  } = {}
) => {
  let q = query(
    collection(db, 'system_alerts'),
    orderBy('created_at', 'desc')
  );

  if (options.resolved !== undefined) {
    q = query(q, where('is_resolved', '==', options.resolved));
  }

  if (options.severity && options.severity !== 'all') {
    q = query(q, where('severity', '==', options.severity));
  }

  if (options.limit) {
    q = query(q, limit(options.limit));
  }

  return onSnapshot(q, (snapshot) => {
    const alerts: SystemAlert[] = snapshot.docs.map(doc => {
      const docData = doc.data();
      return {
        id: doc.id,
        type: docData.type,
        severity: docData.severity,
        title: docData.title,
        message: docData.message,
        pot_id: docData.pot_id,
        sensor_data_id: docData.sensor_data_id,
        is_resolved: docData.is_resolved,
        resolved_at: docData.resolved_at instanceof Timestamp 
          ? docData.resolved_at.toDate() 
          : docData.resolved_at ? new Date(docData.resolved_at) : undefined,
        resolved_by: docData.resolved_by,
        created_at: docData.created_at instanceof Timestamp 
          ? docData.created_at.toDate() 
          : new Date(docData.created_at)
      };
    });
    
    callback(alerts);
  });
};

export const subscribeToLatestSensorData = (
  callback: (data: { [potId: string]: SensorData }) => void
) => {
  const potIds = ['pot_1', 'pot_2', 'pot_3', 'pot_4'];
  const unsubscribers: (() => void)[] = [];

  const latestData: { [potId: string]: SensorData } = {};

  potIds.forEach(potId => {
    const q = query(
      collection(db, 'sensor_data'),
      where('pot_id', '==', potId),
      orderBy('timestamp', 'desc'),
      limit(1)
    );

    const unsubscribe = onSnapshot(q, (snapshot) => {
      if (!snapshot.empty) {
        const doc = snapshot.docs[0];
        const docData = doc.data();
        
        latestData[potId] = {
          id: doc.id,
          timestamp: docData.timestamp instanceof Timestamp 
            ? docData.timestamp.toDate() 
            : new Date(docData.timestamp),
          pot_id: docData.pot_id,
          pot_name: docData.pot_name,
          soil_condition: docData.soil_condition,
          has_microplastic: docData.has_microplastic,
          moisture: docData.moisture,
          temperature: docData.temperature,
          uv_intensity: docData.uv_intensity,
          fuzzy_result: docData.fuzzy_result,
          device_id: docData.device_id,
          battery_level: docData.battery_level,
          signal_strength: docData.signal_strength,
          created_by: docData.created_by,
          notes: docData.notes
        };
      }
      
      callback({ ...latestData });
    });

    unsubscribers.push(unsubscribe);
  });

  // Return a function to unsubscribe from all listeners
  return () => {
    unsubscribers.forEach(unsub => unsub());
  };
};