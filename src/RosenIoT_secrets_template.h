\
#pragma once
// *** Fyll i dina SSID och lösenord och spara som RosenIoT_secrets.h ***
// Lägg INTE upp RosenIoT_secrets.h på GitHub.

static const char* ssidList[] = {"MySSID1", "MySSID2"};
static const char* passList[] = {"MyPass1", "MyPass2"};
static const int numNetworks = sizeof(ssidList)/sizeof(ssidList[0]);
