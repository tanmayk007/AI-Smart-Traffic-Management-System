/* ============================================================
 *  main.c
 *  AI-Based Smart Traffic Management System
 *  Entry point and menu loop.
 *
 *  Build (any platform):
 *      gcc -Wall -Wextra -o smarttraffic src/traffic.c src/main.c
 *  Run:
 *      ./smarttraffic           (Linux / macOS)
 *      smarttraffic.exe         (Windows)
 *
 *  Default login:  admin / admin123
 * ============================================================ */

#include "traffic.h"

static void showSplash(void)
{
    printLine('=', 70);
    printf("                                                                  \n");
    printf("       A I - B A S E D   S M A R T   T R A F F I C                \n");
    printf("            M A N A G E M E N T   S Y S T E M                     \n");
    printf("                                                                  \n");
    printf("                  ( Mini-project in C )                           \n");
    printLine('=', 70);
}

static void showMenu(void)
{
    printf("\n");
    printLine('=', 70);
    printf("                       M A I N    M E N U                        \n");
    printLine('=', 70);
    printf("   [ 1] Add new junction\n");
    printf("   [ 2] Display all junctions\n");
    printf("   [ 3] Update vehicle count\n");
    printf("   [ 4] Delete junction\n");
    printf("   [ 5] Search junction by ID\n");
    printf("   [ 6] Sort junctions by density\n");
    printf("   [ 7] Run AI signal optimization\n");
    printf("   [ 8] Detect traffic congestion\n");
    printf("   [ 9] Emergency override\n");
    printf("   [10] Clear all emergencies\n");
    printf("   [11] Show system statistics\n");
    printf("   [12] Generate daily report\n");
    printf("   [13] Load sample data (demo)\n");
    printf("   [ 0] Exit\n");
    printLine('=', 70);
}

int main(void)
{
    Junction arr[MAX_JUNCTIONS];
    int      count  = 0;
    int      choice;
    int      running = 1;

    showSplash();
    if (!loginScreen()) return 0;

    loadData(arr, &count);
    if (count > 0)
        printf("\n   [i] Loaded %d junction(s) from previous session.\n", count);
    else
        printf("\n   [i] No saved data. Tip: choose option 13 to load demo data.\n");

    while (running) {
        showMenu();
        choice = readInt("   Select an option (0-13): ", 0, 13);

        switch (choice) {
            case 1:  addJunction(arr, &count);          break;
            case 2:  displayAll(arr, count);            break;
            case 3:  updateVehicleCount(arr, count);    break;
            case 4:  deleteJunction(arr, &count);       break;
            case 5:  searchJunctionMenu(arr, count);    break;
            case 6:  sortByDensity(arr, count);         break;
            case 7:  runAiOptimization(arr, count);     break;
            case 8:  detectCongestion(arr, count);      break;
            case 9:  emergencyOverride(arr, count);     break;
            case 10: clearEmergencies(arr, count);      break;
            case 11: showStatistics(arr, count);        break;
            case 12: generateReport(arr, count);        break;
            case 13: loadSampleData(arr, &count);       break;
            case 0:
                saveData(arr, count);
                printBanner("        Thank you for using Smart Traffic System.");
                printf("   Session data saved to '%s'.\n", DATA_FILE);
                printLine('=', 70);
                running = 0;
                break;
            default:
                printf("\n   [!] Invalid option.\n");
        }
        if (running && choice != 0) pauseScreen();
    }
    return 0;
}
