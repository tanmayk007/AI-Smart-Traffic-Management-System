/* ============================================================
 *  traffic.h
 *  AI-Based Smart Traffic Management System
 *  Header file: data structures, constants, function prototypes
 * ============================================================ */

#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ---------- Compile-time constants ---------- */
#define MAX_JUNCTIONS     50
#define NAME_LEN          40
#define DATA_FILE         "data/traffic_data.dat"
#define REPORT_FILE       "data/daily_report.txt"
#define ADMIN_USER        "admin"
#define ADMIN_PASS        "admin123"

/* Congestion thresholds (vehicles present) */
#define LOW_THRESHOLD     30
#define HIGH_THRESHOLD    70

/* Signal-time bounds (seconds) */
#define MIN_GREEN_TIME    15
#define MAX_GREEN_TIME    90
#define EMERGENCY_TIME    60

/* Congestion levels */
#define CONG_LOW          0
#define CONG_MEDIUM       1
#define CONG_HIGH         2

/* ---------- Core data structure ---------- */
typedef struct {
    int  id;                    /* unique junction id            */
    char name[NAME_LEN];        /* junction name                 */
    int  vehicleCount;          /* current vehicles present      */
    int  signalTime;            /* AI-computed green time (s)    */
    int  congestionLevel;       /* 0=LOW, 1=MED, 2=HIGH          */
    int  emergencyActive;       /* 1 if emergency override on    */
    int  peakHour;              /* 1 if peak-hour flag set       */
} Junction;

/* ---------- Function prototypes ---------- */

/* I/O helpers */
void   clearInputBuffer(void);
int    readInt(const char *prompt, int minVal, int maxVal);
void   readString(const char *prompt, char *buf, int size);
void   pauseScreen(void);
void   printBanner(const char *title);
void   printLine(char ch, int n);

/* Authentication */
int    loginScreen(void);

/* File persistence */
int    loadData(Junction arr[], int *count);
int    saveData(Junction arr[], int count);

/* Core operations (CRUD) */
void   addJunction(Junction arr[], int *count);
void   displayAll(Junction arr[], int count);
void   updateVehicleCount(Junction arr[], int count);
void   deleteJunction(Junction arr[], int *count);

/* Search & sort */
int    searchById(Junction arr[], int count, int id);          /* returns index or -1 */
void   searchJunctionMenu(Junction arr[], int count);
void   sortByDensity(Junction arr[], int count);

/* AI engine */
int    computeSignalTime(const Junction *j);
int    classifyCongestion(int vehicles);
void   runAiOptimization(Junction arr[], int count);
void   detectCongestion(Junction arr[], int count);
void   emergencyOverride(Junction arr[], int count);
void   clearEmergencies(Junction arr[], int count);

/* Reporting */
void   generateReport(Junction arr[], int count);
void   showStatistics(Junction arr[], int count);

/* Demo data */
void   loadSampleData(Junction arr[], int *count);

/* Utility */
const char *congestionLabel(int level);

#endif /* TRAFFIC_H */
