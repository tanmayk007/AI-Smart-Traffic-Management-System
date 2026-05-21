/* ============================================================
 *  traffic.c
 *  AI-Based Smart Traffic Management System
 *  Implementation of all modules.
 * ============================================================ */

#include "traffic.h"

/* ============================================================
 *                     I/O   H E L P E R S
 * ============================================================ */

void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* discard */ }
}

/* Read a bounded integer with re-prompt on bad input */
int readInt(const char *prompt, int minVal, int maxVal)
{
    int  value;
    int  ok;
    char term;

    while (1) {
        printf("%s", prompt);
        ok = scanf("%d%c", &value, &term);
        if (ok == 2 && (term == '\n' || term == ' ' || term == '\t')
            && value >= minVal && value <= maxVal) {
            return value;
        }
        printf("   [!] Invalid input. Please enter an integer in [%d..%d].\n",
               minVal, maxVal);
        clearInputBuffer();
    }
}

/* Read a line of text safely (handles leading whitespace) */
void readString(const char *prompt, char *buf, int size)
{
    int  c;
    int  i = 0;

    printf("%s", prompt);
    /* skip leading whitespace/newlines */
    while ((c = getchar()) != EOF && (c == ' ' || c == '\t' || c == '\n')) { }
    while (c != EOF && c != '\n' && i < size - 1) {
        buf[i++] = (char)c;
        c = getchar();
    }
    buf[i] = '\0';
    if (i == 0) {
        strncpy(buf, "Unnamed", size - 1);
        buf[size - 1] = '\0';
    }
}

void pauseScreen(void)
{
    int c;
    printf("\n   Press ENTER to continue...");
    /* Wait for one newline. If stdin has buffered chars (rare), drain them. */
    while ((c = getchar()) != '\n' && c != EOF) { /* discard */ }
}

void printLine(char ch, int n)
{
    int i;
    for (i = 0; i < n; i++) putchar(ch);
    putchar('\n');
}

void printBanner(const char *title)
{
    printLine('=', 70);
    printf("   %s\n", title);
    printLine('=', 70);
}

const char *congestionLabel(int level)
{
    switch (level) {
        case CONG_LOW:    return "LOW";
        case CONG_MEDIUM: return "MEDIUM";
        case CONG_HIGH:   return "HIGH";
        default:          return "UNKNOWN";
    }
}

/* ============================================================
 *                  A U T H E N T I C A T I O N
 * ============================================================ */

int loginScreen(void)
{
    char user[NAME_LEN];
    char pass[NAME_LEN];
    int  attempts;

    for (attempts = 1; attempts <= 3; attempts++) {
        printBanner("        SMART  TRAFFIC  MANAGEMENT  --  LOGIN");
        printf("\n   Attempt %d of 3\n\n", attempts);

        readString("   Username : ", user, NAME_LEN);
        readString("   Password : ", pass, NAME_LEN);

        if (strcmp(user, ADMIN_USER) == 0 && strcmp(pass, ADMIN_PASS) == 0) {
            printf("\n   [OK] Login successful. Welcome, %s!\n", user);
            pauseScreen();
            return 1;
        }
        printf("\n   [X]  Invalid credentials.\n\n");
    }
    printf("\n   Too many failed attempts. Exiting...\n");
    return 0;
}

/* ============================================================
 *                  F I L E   P E R S I S T E N C E
 * ============================================================ */

int loadData(Junction arr[], int *count)
{
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        *count = 0;
        return 0;
    }
    *count = (int)fread(arr, sizeof(Junction), MAX_JUNCTIONS, fp);
    fclose(fp);
    return 1;
}

int saveData(Junction arr[], int count)
{
    FILE *fp;
    /* ensure data/ exists; if running first time, create it */
    #if defined(_WIN32)
        system("if not exist data mkdir data >nul 2>&1");
    #else
        if (system("mkdir -p data >/dev/null 2>&1") != 0) { /* ignore */ }
    #endif

    fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("   [!] ERROR: cannot open %s for writing.\n", DATA_FILE);
        return 0;
    }
    fwrite(arr, sizeof(Junction), (size_t)count, fp);
    fclose(fp);
    return 1;
}

/* ============================================================
 *                    A I    E N G I N E
 * ============================================================ */

int classifyCongestion(int vehicles)
{
    if (vehicles < LOW_THRESHOLD)        return CONG_LOW;
    else if (vehicles < HIGH_THRESHOLD)  return CONG_MEDIUM;
    else                                 return CONG_HIGH;
}

/* Rule-based expert system: decide green-light duration */
int computeSignalTime(const Junction *j)
{
    int t;

    if (j->emergencyActive) return EMERGENCY_TIME;

    if (j->vehicleCount < LOW_THRESHOLD)        t = 20;
    else if (j->vehicleCount < HIGH_THRESHOLD)  t = 40;
    else                                        t = 70;

    if (j->peakHour) t += 15;

    if (t < MIN_GREEN_TIME) t = MIN_GREEN_TIME;
    if (t > MAX_GREEN_TIME) t = MAX_GREEN_TIME;
    return t;
}

void runAiOptimization(Junction arr[], int count)
{
    int i;

    if (count == 0) {
        printf("\n   [i] No junctions to optimize. Add some first.\n");
        return;
    }

    printBanner("        A I   S I G N A L   O P T I M I Z A T I O N");
    printf("\n   %-5s %-25s %-10s %-10s %-12s\n",
           "ID", "Junction", "Vehicles", "Signal(s)", "Congestion");
    printLine('-', 70);

    for (i = 0; i < count; i++) {
        arr[i].congestionLevel = classifyCongestion(arr[i].vehicleCount);
        arr[i].signalTime      = computeSignalTime(&arr[i]);

        printf("   %-5d %-25s %-10d %-10d %-12s\n",
               arr[i].id, arr[i].name, arr[i].vehicleCount,
               arr[i].signalTime, congestionLabel(arr[i].congestionLevel));
    }
    printLine('-', 70);
    printf("   [OK] AI optimization complete for %d junction(s).\n", count);
    saveData(arr, count);
}

void detectCongestion(Junction arr[], int count)
{
    int i, high = 0;

    if (count == 0) {
        printf("\n   [i] No data available.\n");
        return;
    }

    printBanner("        T R A F F I C   C O N G E S T I O N   R E P O R T");
    printf("\n");

    for (i = 0; i < count; i++) {
        arr[i].congestionLevel = classifyCongestion(arr[i].vehicleCount);
        printf("   [%d] %-25s -> %-6s ",
               arr[i].id, arr[i].name, congestionLabel(arr[i].congestionLevel));

        switch (arr[i].congestionLevel) {
            case CONG_HIGH:
                printf("*** ALERT: Heavy congestion! ***\n");
                high++;
                break;
            case CONG_MEDIUM:
                printf(" (Moderate flow)\n");
                break;
            default:
                printf(" (Smooth flow)\n");
        }
    }
    printLine('-', 70);
    printf("   Junctions with HIGH congestion : %d / %d\n", high, count);
    if (high > 0) {
        printf("   >> AI Suggestion: Activate alternate-route advisories\n");
        printf("                     and increase green-time at hotspots.\n");
    } else {
        printf("   >> AI Suggestion: Traffic is flowing normally.\n");
    }
    saveData(arr, count);
}

void emergencyOverride(Junction arr[], int count)
{
    int id, idx;

    if (count == 0) {
        printf("\n   [i] No junctions registered.\n");
        return;
    }

    printBanner("        E M E R G E N C Y   O V E R R I D E");
    id  = readInt("\n   Enter Junction ID for emergency override: ", 1, 99999);
    idx = searchById(arr, count, id);

    if (idx == -1) {
        printf("\n   [!] Junction ID %d not found.\n", id);
        return;
    }
    arr[idx].emergencyActive = 1;
    arr[idx].signalTime      = EMERGENCY_TIME;

    printf("\n   [!!] EMERGENCY OVERRIDE ACTIVATED at '%s'\n", arr[idx].name);
    printf("   -> Green signal locked for %d seconds.\n", EMERGENCY_TIME);
    printf("   -> All adjacent junctions set to RED (simulated).\n");
    printf("   -> Ambulance / Fire / Police corridor cleared.\n");
    saveData(arr, count);
}

void clearEmergencies(Junction arr[], int count)
{
    int i, cleared = 0;
    for (i = 0; i < count; i++) {
        if (arr[i].emergencyActive) {
            arr[i].emergencyActive = 0;
            arr[i].signalTime      = computeSignalTime(&arr[i]);
            cleared++;
        }
    }
    if (cleared == 0)
        printf("\n   [i] No active emergencies to clear.\n");
    else
        printf("\n   [OK] Cleared %d emergency override(s).\n", cleared);
    saveData(arr, count);
}

/* ============================================================
 *                     C R U D    O P E R A T I O N S
 * ============================================================ */

void addJunction(Junction arr[], int *count)
{
    Junction j;
    int dup, i;

    if (*count >= MAX_JUNCTIONS) {
        printf("\n   [!] Capacity full (%d). Cannot add more.\n", MAX_JUNCTIONS);
        return;
    }

    printBanner("        A D D   N E W   J U N C T I O N");
    j.id = readInt("\n   Junction ID (1-99999) : ", 1, 99999);

    dup = searchById(arr, *count, j.id);
    if (dup != -1) {
        printf("   [!] ID %d already exists ('%s').\n",
               j.id, arr[dup].name);
        return;
    }

    readString("   Junction name        : ", j.name, NAME_LEN);
    j.vehicleCount    = readInt("   Current vehicle count: ", 0, 1000);
    j.peakHour        = readInt("   Peak hour? (1=yes, 0=no): ", 0, 1);
    j.emergencyActive = 0;
    j.congestionLevel = classifyCongestion(j.vehicleCount);
    j.signalTime      = computeSignalTime(&j);

    i = *count;
    arr[i] = j;
    (*count)++;

    printf("\n   [OK] Junction '%s' added successfully.\n", j.name);
    printf("        AI-assigned signal time = %d s, Congestion = %s\n",
           j.signalTime, congestionLabel(j.congestionLevel));
    saveData(arr, *count);
}

void displayAll(Junction arr[], int count)
{
    int i;

    if (count == 0) {
        printf("\n   [i] No junctions registered yet. Use 'Add Junction'.\n");
        return;
    }

    printBanner("        R E G I S T E R E D   J U N C T I O N S");
    printf("\n   %-5s %-25s %-9s %-10s %-10s %-6s\n",
           "ID", "Name", "Vehicles", "Signal(s)", "Status", "Peak");
    printLine('-', 70);

    for (i = 0; i < count; i++) {
        printf("   %-5d %-25s %-9d %-10d %-10s %-6s\n",
               arr[i].id,
               arr[i].name,
               arr[i].vehicleCount,
               arr[i].signalTime,
               arr[i].emergencyActive ? "EMERGENCY" :
                   congestionLabel(arr[i].congestionLevel),
               arr[i].peakHour ? "Yes" : "No");
    }
    printLine('-', 70);
    printf("   Total junctions: %d\n", count);
}

void updateVehicleCount(Junction arr[], int count)
{
    int id, idx, v, peak;

    if (count == 0) {
        printf("\n   [i] No junctions to update.\n");
        return;
    }
    printBanner("        U P D A T E   V E H I C L E   C O U N T");

    id  = readInt("\n   Enter Junction ID: ", 1, 99999);
    idx = searchById(arr, count, id);
    if (idx == -1) {
        printf("   [!] Junction ID %d not found.\n", id);
        return;
    }

    printf("   Current: %s | vehicles = %d | peak = %s\n",
           arr[idx].name, arr[idx].vehicleCount,
           arr[idx].peakHour ? "Yes" : "No");

    v    = readInt("   New vehicle count (0-1000): ", 0, 1000);
    peak = readInt("   Peak hour? (1=yes, 0=no)  : ", 0, 1);

    arr[idx].vehicleCount    = v;
    arr[idx].peakHour        = peak;
    arr[idx].congestionLevel = classifyCongestion(v);
    arr[idx].signalTime      = computeSignalTime(&arr[idx]);

    printf("\n   [OK] Updated. New signal time = %d s, Congestion = %s\n",
           arr[idx].signalTime, congestionLabel(arr[idx].congestionLevel));
    saveData(arr, count);
}

void deleteJunction(Junction arr[], int *count)
{
    int id, idx, i;

    if (*count == 0) {
        printf("\n   [i] No junctions to delete.\n");
        return;
    }
    printBanner("        D E L E T E   J U N C T I O N");

    id  = readInt("\n   Enter Junction ID to delete: ", 1, 99999);
    idx = searchById(arr, *count, id);
    if (idx == -1) {
        printf("   [!] Junction ID %d not found.\n", id);
        return;
    }

    printf("   Deleting '%s' (ID %d)...\n", arr[idx].name, arr[idx].id);

    for (i = idx; i < *count - 1; i++) {
        arr[i] = arr[i + 1];
    }
    (*count)--;
    printf("   [OK] Deletion complete. Remaining: %d\n", *count);
    saveData(arr, *count);
}

/* ============================================================
 *                  S E A R C H   &   S O R T
 * ============================================================ */

int searchById(Junction arr[], int count, int id)
{
    int i;
    for (i = 0; i < count; i++) {
        if (arr[i].id == id) return i;
    }
    return -1;
}

void searchJunctionMenu(Junction arr[], int count)
{
    int id, idx;

    if (count == 0) {
        printf("\n   [i] No junctions to search.\n");
        return;
    }
    printBanner("        S E A R C H   J U N C T I O N");

    id  = readInt("\n   Enter Junction ID to search: ", 1, 99999);
    idx = searchById(arr, count, id);

    if (idx == -1) {
        printf("   [!] Junction ID %d not found.\n", id);
        return;
    }
    printf("\n   ------ Junction Found ------\n");
    printf("   ID            : %d\n",  arr[idx].id);
    printf("   Name          : %s\n",  arr[idx].name);
    printf("   Vehicles      : %d\n",  arr[idx].vehicleCount);
    printf("   Signal time   : %d s\n", arr[idx].signalTime);
    printf("   Congestion    : %s\n",  congestionLabel(arr[idx].congestionLevel));
    printf("   Peak hour     : %s\n",  arr[idx].peakHour ? "Yes" : "No");
    printf("   Emergency     : %s\n",  arr[idx].emergencyActive ? "ACTIVE" : "off");
}

/* Bubble sort by vehicle density (descending) */
void sortByDensity(Junction arr[], int count)
{
    int i, j;
    Junction tmp;

    if (count < 2) {
        printf("\n   [i] Not enough data to sort.\n");
        return;
    }

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1 - i; j++) {
            if (arr[j].vehicleCount < arr[j + 1].vehicleCount) {
                tmp        = arr[j];
                arr[j]     = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
    printf("\n   [OK] Sorted by vehicle density (highest first).\n");
    displayAll(arr, count);
    saveData(arr, count);
}

/* ============================================================
 *                     R E P O R T I N G
 * ============================================================ */

void generateReport(Junction arr[], int count)
{
    FILE   *fp;
    time_t  now;
    int     i, low = 0, med = 0, high = 0, totalVeh = 0;

    if (count == 0) {
        printf("\n   [i] No data to report.\n");
        return;
    }

    #if defined(_WIN32)
        system("if not exist data mkdir data >nul 2>&1");
    #else
        if (system("mkdir -p data >/dev/null 2>&1") != 0) { /* ignore */ }
    #endif

    fp = fopen(REPORT_FILE, "w");
    if (fp == NULL) {
        printf("   [!] ERROR: cannot create %s\n", REPORT_FILE);
        return;
    }

    now = time(NULL);
    fprintf(fp, "======================================================================\n");
    fprintf(fp, "        AI-BASED  SMART  TRAFFIC  MANAGEMENT  --  DAILY REPORT\n");
    fprintf(fp, "======================================================================\n");
    fprintf(fp, "Generated : %s", ctime(&now));
    fprintf(fp, "Junctions : %d\n", count);
    fprintf(fp, "----------------------------------------------------------------------\n");
    fprintf(fp, "%-5s %-25s %-9s %-10s %-10s\n",
            "ID", "Name", "Vehicles", "Signal(s)", "Congestion");
    fprintf(fp, "----------------------------------------------------------------------\n");

    for (i = 0; i < count; i++) {
        fprintf(fp, "%-5d %-25s %-9d %-10d %-10s\n",
                arr[i].id, arr[i].name, arr[i].vehicleCount,
                arr[i].signalTime, congestionLabel(arr[i].congestionLevel));
        totalVeh += arr[i].vehicleCount;
        switch (arr[i].congestionLevel) {
            case CONG_LOW:    low++;  break;
            case CONG_MEDIUM: med++;  break;
            case CONG_HIGH:   high++; break;
            default: break;
        }
    }
    fprintf(fp, "----------------------------------------------------------------------\n");
    fprintf(fp, "Summary:\n");
    fprintf(fp, "   Total vehicles tracked : %d\n", totalVeh);
    fprintf(fp, "   Average per junction   : %.2f\n",
            count ? (double)totalVeh / count : 0.0);
    fprintf(fp, "   LOW    congestion      : %d\n", low);
    fprintf(fp, "   MEDIUM congestion      : %d\n", med);
    fprintf(fp, "   HIGH   congestion      : %d\n", high);
    fprintf(fp, "----------------------------------------------------------------------\n");
    fprintf(fp, "AI Recommendation:\n");
    if (high > 0)
        fprintf(fp, "   - Deploy traffic personnel at %d hotspot(s).\n", high);
    if (med > 0)
        fprintf(fp, "   - Monitor %d medium-density junction(s) for escalation.\n", med);
    if (high == 0 && med == 0)
        fprintf(fp, "   - All clear. Normal operations recommended.\n");
    fprintf(fp, "======================================================================\n");
    fclose(fp);

    printf("\n   [OK] Report written to '%s'\n", REPORT_FILE);
}

void showStatistics(Junction arr[], int count)
{
    int i, low = 0, med = 0, high = 0, emerg = 0;
    int total = 0, maxV = 0, maxIdx = -1;

    if (count == 0) {
        printf("\n   [i] No data available.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        total += arr[i].vehicleCount;
        if (arr[i].vehicleCount > maxV) {
            maxV   = arr[i].vehicleCount;
            maxIdx = i;
        }
        switch (arr[i].congestionLevel) {
            case CONG_LOW:    low++;  break;
            case CONG_MEDIUM: med++;  break;
            case CONG_HIGH:   high++; break;
            default: break;
        }
        if (arr[i].emergencyActive) emerg++;
    }

    printBanner("        S Y S T E M    S T A T I S T I C S");
    printf("\n   Total junctions       : %d\n", count);
    printf("   Total vehicles        : %d\n", total);
    printf("   Average per junction  : %.2f\n", (double)total / count);
    printf("   Busiest junction      : %s (%d vehicles)\n",
           maxIdx >= 0 ? arr[maxIdx].name : "-", maxV);
    printLine('-', 70);
    printf("   LOW    congestion     : %d\n", low);
    printf("   MEDIUM congestion     : %d\n", med);
    printf("   HIGH   congestion     : %d\n", high);
    printf("   Active emergencies    : %d\n", emerg);
    printLine('=', 70);
}

/* ============================================================
 *                      D E M O   D A T A
 * ============================================================ */

void loadSampleData(Junction arr[], int *count)
{
    int i;
    const char *names[5] = {
        "MG Road Junction", "Park Street", "Ring Road",
        "Airport Road", "Lake View"
    };
    int   ids[5]   = {101, 102, 103, 104, 105};
    int   veh[5]   = { 85,  22,  55,  78,  18};
    int   peak[5]  = {  1,   0,   0,   1,   0};

    *count = 0;
    for (i = 0; i < 5 && *count < MAX_JUNCTIONS; i++) {
        arr[i].id              = ids[i];
        strncpy(arr[i].name, names[i], NAME_LEN - 1);
        arr[i].name[NAME_LEN - 1] = '\0';
        arr[i].vehicleCount    = veh[i];
        arr[i].peakHour        = peak[i];
        arr[i].emergencyActive = 0;
        arr[i].congestionLevel = classifyCongestion(veh[i]);
        arr[i].signalTime      = computeSignalTime(&arr[i]);
        (*count)++;
    }
    saveData(arr, *count);
    printf("\n   [OK] Sample data loaded: %d junctions.\n", *count);
}
