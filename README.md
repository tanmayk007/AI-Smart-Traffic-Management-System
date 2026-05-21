# 🚦 AI-Based Smart Traffic Management System

A C mini-project that demonstrates a **rule-based AI** for adaptive
traffic-signal control. Built as a single-binary, menu-driven console
application — no external libraries, no special tooling. Compiles
cleanly with `gcc -Wall -Wextra` under C89, C99, C11 and Turbo C.

---

## ✨ Features

- 🧠 **Rule-based AI engine** — computes green-light duration from live
  vehicle counts, peak-hour flag, and emergency status
- 🚨 **Emergency override** — locks a 60-second green corridor for
  ambulances / fire / police
- 📊 **Real-time congestion detection** with hotspot alerts
- 🗂️ **CRUD** — add, display, update, delete, search junctions
- 🔃 **Sort by vehicle density** (bubble sort, descending)
- 💾 **Persistent storage** — binary `.dat` file survives restarts
- 📝 **Daily reports** — human-readable `.txt` export with AI advice
- 🔐 **Login system** — three attempts, `admin / admin123`
- 📈 **Statistics dashboard** — busiest junction, distribution, totals
- 🧪 **Tested** — zero warnings on `gcc -Wall -Wextra -Wpedantic`

## 📁 Project Layout

```
SmartTraffic/
├── src/
│   ├── traffic.h          # struct, constants, prototypes
│   ├── traffic.c          # ~600 lines of implementation
│   └── main.c             # menu loop
├── data/                  # auto-created at runtime
│   ├── traffic_data.dat   # binary state
│   └── daily_report.txt   # text report
├── docs/
│   ├── algorithms.md      # pseudocode + complexity
│   ├── flowchart.md       # ASCII flowcharts
│   ├── viva_questions.md  # 30 Q&A
│   └── execution_guide.md # demo script
├── screenshots/           # 10 PNG screenshots of real runs
├── Makefile
├── .gitignore
└── README.md
```

## 🚀 Quick Start

```bash
unzip SmartTraffic.zip && cd SmartTraffic
make                # or: gcc -Wall -o smarttraffic src/traffic.c src/main.c
./smarttraffic
```

Login: **`admin`** / **`admin123`**

In the menu, choose **`13`** to load 5 demo junctions, then try
`7` (AI optimization) and `8` (congestion detection).

## 🧠 The AI Engine

```c
int computeSignalTime(const Junction *j) {
    if (j->emergencyActive) return 60;        // override
    int t;
    if      (j->vehicleCount <  30) t = 20;   // LOW
    else if (j->vehicleCount <  70) t = 40;   // MEDIUM
    else                            t = 70;   // HIGH
    if (j->peakHour) t += 15;                 // rush hour
    if (t < 15) t = 15;
    if (t > 90) t = 90;
    return t;
}
```

This is **classical symbolic AI** (rule-based expert system) — the same
family used in real first-generation adaptive controllers like SCATS
and SCOOT. It is deterministic, explainable, and runs in O(1) per
junction.

## 🎯 Concepts Covered (C Mini-Project Rubric)

| Concept | Where |
|---|---|
| Arrays | `Junction arr[MAX_JUNCTIONS]` |
| Structures | `typedef struct { ... } Junction;` |
| Pointers | `int *count`, `const Junction *j` |
| Functions | 25+ modular functions across two `.c` files |
| Conditionals | All AI/threshold logic |
| Loops | `for / while` in CRUD, sort, search, report |
| File I/O | Binary `.dat` + text `.txt` |
| Searching | Linear search by ID |
| Sorting | Bubble sort by density |
| String handling | `strcpy`, `strncpy`, `strcmp`, custom `readString` |
| Header file | `traffic.h` with include guard |

## 🛠️ Build Variants

| Compiler / Standard | Command |
|---|---|
| GCC modern | `gcc -Wall -Wextra -std=c99 -o smarttraffic src/traffic.c src/main.c` |
| GCC strict ANSI | `gcc -Wall -Wextra -std=c89 -o smarttraffic src/traffic.c src/main.c` |
| Turbo C | Open `src/main.c`, add `traffic.c`, Compile → Make EXE |
| VS Code | `Ctrl+Shift+B` with the bundled `tasks.json` (optional) |

## 📸 Screenshots

See `screenshots/` for 10 PNG captures of real program runs:

1. Login screen
2. Main menu
3. Display all junctions
4. AI signal optimization
5. Congestion detection
6. Emergency override
7. Search by ID
8. Add new junction
9. System statistics
10. Sort by density

## 🎓 Viva Preparation

`docs/viva_questions.md` contains **30 anticipated viva questions** with
model answers covering project specifics (Why "AI"? thresholds? file
format?) and general C topics (struct vs union, pointers, complexity).

## 📜 License

MIT — free to use, modify, submit.
