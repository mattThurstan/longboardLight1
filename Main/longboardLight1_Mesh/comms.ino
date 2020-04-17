/*----------------------------comms----------------------------*/
void setupMesh() {
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );       // set before init() so that you can see startup messages

  //mesh.init( MESH_NAME, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL );
  mesh.init( MESH_NAME, MESH_PASSWORD, MESH_PORT, WIFI_STA, STATION_CHANNEL );  // endpoint node
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

