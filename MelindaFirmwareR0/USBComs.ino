void usbCommunicationCallback(const String& dataRecv) {
  String dataHeader = usbSerial.getString(dataRecv, 0, "#");
  String dataValue = usbSerial.getString(dataRecv, 1, "#");
  dataHeader.toUpperCase();

  debug.startPrint(LOG_COMS);
  debug.continuePrint("dataRecv", dataRecv, LOG_COMS);
  debug.endPrint(LOG_COMS, true);

  if (dataHeader == "R") ESP.restart();

  if (dataHeader == "LOG_INFO") debug.isLogLevelEnabled(LOG_INFO) ? debug.disableLogLevel(LOG_INFO) : debug.enableLogLevel(LOG_INFO);
  if (dataHeader == "LOG_SENSOR") debug.isLogLevelEnabled(LOG_SENSOR) ? debug.disableLogLevel(LOG_SENSOR) : debug.enableLogLevel(LOG_SENSOR);
  if (dataHeader == "LOG_COMS") debug.isLogLevelEnabled(LOG_COMS) ? debug.disableLogLevel(LOG_COMS) : debug.enableLogLevel(LOG_COMS);

  // Firebase RTDB
  if (dataHeader == "RTDB_SET_VALUE") firebaseRTDBState = RTDB_SET_VALUE;
  if (dataHeader == "RTDB_SET_VALUE_JSON") firebaseRTDBState = RTDB_SET_VALUE_JSON;
  if (dataHeader == "RTDB_SET_VALUE_PERIODIC") firebaseRTDBState = RTDB_SET_VALUE_PERIODIC;
  if (dataHeader == "RTDB_GET_VALUE") firebaseRTDBState = RTDB_GET_VALUE;
  if (dataHeader == "RTDB_GET_VALUE_JSON") firebaseRTDBState = RTDB_GET_VALUE_JSON;
  if (dataHeader == "RTDB_GET_VALUE_PERIODIC") firebaseRTDBState = RTDB_GET_VALUE_PERIODIC;

  // Firebase Firestore
  if (dataHeader == "FIRESTORE_CREATE") firebaseFirestoreState = FIRESTORE_CREATE;
  if (dataHeader == "FIRESTORE_READ") firebaseFirestoreState = FIRESTORE_READ;
  if (dataHeader == "FIRESTORE_UPDATE") firebaseFirestoreState = FIRESTORE_UPDATE;
  if (dataHeader == "FIRESTORE_DELETE") firebaseFirestoreState = FIRESTORE_DELETE;

  // Firebase Mesagging
  if (dataHeader == "MESSAGING_SEND") firebaseMessagingState = MESSAGING_SEND;
}