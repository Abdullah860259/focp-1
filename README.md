# Relief Supply Distribution System

A C++ console program that helps a district administration share limited relief supplies after a natural disaster such as a flood. For each request from an affected area, it asks a few questions, works out a **need score**, chooses a **relief package**, falls back to a smaller package when stock runs short, updates the inventory and saves the request to a CSV file.

Built for **CS 110: Fundamentals of Computer Programming, Assignment 1** (NUST SEECS).

**Repository:** https://github.com/Abdullah860259/focp-1

---

## Group Members

| Name | Contribution |
|---|---|
| Abdullah Anwar | _TODO: e.g. program structure, input engine, dispatch logic_ |
| Hooria | _TODO_: e.g.  Implemented Inventory Summary , Relief Request History , command routing in main.cpp, and updated instructions in output.cpp. |
| Abeera | _TODO_: e.g. |

> **TODO:** Fill in who did what. The rubric checks that this matches the commit history, and each member must be able to explain the whole program in the viva.

---

## Table of Contents

1. [Features](#features)
2. [Requirements](#requirements)
3. [Building and Running](#building-and-running)
4. [Commands](#commands)
5. [Making a Relief Request](#making-a-relief-request)
6. [Need Score Calculation](#need-score-calculation)
7. [Decision / Action Rules](#decision--action-rules)
8. [Relief Packages and Inventory](#relief-packages-and-inventory)
9. [Saved Requests (CSV)](#saved-requests-csv)
10. [File Structure](#file-structure)
11. [Testing](#testing)
12. [Flowchart](#flowchart)
13. [Known Limitations](#known-limitations)
14. [Use of AI](#use-of-ai)

---

## Features

- Colour-coded, table-based console interface that adapts to the terminal width
- Input validation: every answer is checked and asked again until it is valid
- Need score built from 7 factors about the affected area
- Four relief packages, from Full Relief down to Defer / Monitor
- Automatic fallback to a smaller package when any resource runs short, with the missing item printed
- Stock never goes below zero
- Live inventory view
- Every request is saved to `data/requests.csv`

---

## Requirements

- A C++ compiler with **C++11** support (tested with MinGW `g++` 6.3.0 on Windows)
- A terminal that supports ANSI colours (Windows 10/11 Terminal, PowerShell, VS Code terminal, Git Bash, or any Linux/macOS terminal)

---

## Building and Running

Run these commands from the project root (the folder containing `src/` and `include/`).

### Compile

```bash
g++ -std=c++11 -Wall -I ./include ./src/*.cpp -o build
```

| Part | Meaning |
|---|---|
| `-std=c++11` | Use the C++11 standard |
| `-Wall` | Show all common warnings |
| `-I ./include` | Look for header files in the `include` folder |
| `./src/*.cpp` | Compile every source file in `src` |
| `-o build` | Name the program `build` (`build.exe` on Windows) |

This works in PowerShell, Command Prompt and Git Bash. If your shell does not expand `*.cpp`, list the files instead:

```bash
g++ -std=c++11 -Wall -I ./include src/main.cpp src/output.cpp src/inventory.cpp src/engine.cpp src/storage.cpp src/utility.cpp -o build
```

### Run

```bash
./build
```

> Run the program **from the project root**. The CSV file is saved relative to the folder you run it from, so it ends up in `./data/requests.csv`.

---

## Commands

Type a command at the prompt and press **Enter**. Capital letters and spaces are ignored, so `Get Help`, `GET_HELP` and `get_help` all work.

| Command | What it does |
|---|---|
| `get_help` | Make a new relief request for an area |
| `show_inventory` | Show the current stock of every resource |
| `instructions` | Show the instructions screen again |
| `exit` | Quit the program |

Anything else shows an "Invalid command" message.

---

## Making a Relief Request

`get_help` asks these questions in order:

| # | Question | Accepted answers |
|---|---|---|
| 1 | Area name | Any text (cannot be empty) |
| 2 | Damage level | `low` / `medium` / `high` (or `l` / `m` / `h`) |
| 3 | Families affected | Whole number, **10 to 200** |
| 4 | Access route | `open` / `damaged` (or `o` / `d`) |
| 5 | Distance to the area in km | Whole number, **1 to 500**. *Only asked if the route is open* |
| 6 | Water scarcity | `low` / `medium` / `high` |
| 7 | Food availability | `low` / `medium` / `high` |
| 8 | Number of injured people | Whole number, **0 to 1000** |
| 9 | Previous aid received | `yes` / `no` (or `y` / `n`) |

If an answer is invalid (letters instead of a number, out of range, an unknown word), a yellow error is shown and the same question is asked again.

After the questions, the program shows:

1. **Request summary**: everything that was entered, colour-coded by severity
2. **Need assessment**: the need score on a coloured meter and the recommended package
3. **Dispatch**: each package that was tried, why any were refused, and which one was sent
4. **Stock table**: before / used / remaining for every resource
5. **Result**: `DISPATCHED` or `DEFERRED / MONITORING`, with the reason

### Example

```
> get_help
  Area name: Riverside Village
  Damage level (low/medium/high): high
  Families affected (10-200): 150
  Access route (open/damaged): damaged
  Water scarcity (low/medium/high): high
  Food availability (low/medium/high): low
  Number of injured people (0-1000): 40
  Previous aid received (yes/no): no

  Need score: 23 / 25
  DISPATCHED: A. Full Relief
```

---

## Need Score Calculation

```
Need Score = Damage + Families + Route + Water + Food + Injured + Aid
```

| Factor | Rule | Points |
|---|---|---|
| Damage level | Low = 0, Medium = 3, High = 6 (`level × 3`) | 0 – 6 |
| Families affected | 10–50 → 1, 51–100 → 2, 101–200 → 3 | 1 – 3 |
| Access route | Open and ≤ 100 km → 1, Open and > 100 km → 2, Damaged → 3 | 1 – 3 |
| Water scarcity | Low = 0, Medium = 2, High = 4 (`level × 2`) | 0 – 4 |
| Food availability | High = 0, Medium = 2, Low = 4 (`(2 − level) × 2`, *less food means more need*) | 0 – 4 |
| Injured people | 0–100 → 1, 101–500 → 2, 501–1000 → 3 | 1 – 3 |
| Previous aid | Received → 0, Not received → 2 | 0 – 2 |
| **Total** | | **3 – 25** |

### Why these weights

> **TODO:** Explain in your own words why each factor has the weight it has. The rubric requires *every* weight to have a written reason linked to real-world impact. Questions to answer:
> - Why is **damage** the heaviest factor (up to 6)?
> - Why do **water** and **food** (up to 4 each) count more than **families** or **injured people** (up to 3)?
> - Why does a **damaged route** add the most route points, and why does a long distance on an open route still add some?
> - Why does **previous aid** only lower the score by 2 rather than more?

---

## Decision / Action Rules

### 1. Recommend a package from the score

| Need score | Recommended package |
|---|---|
| 20 – 25 | A. Full Relief |
| 12 – 19 | B. Standard Relief |
| 8 – 11 | C. Emergency Kit Only |
| 3 – 7 | D. Defer / Monitor |

### 2. Check stock and fall back if needed

1. If the recommended package can be fully supplied (enough of **all five** resources), send it.
2. If not, print which items are short (e.g. `not enough packs (need 50, have 20)`) and try the next smaller package: **A → B → C → D**.
3. If even C cannot be supplied, the request is **deferred**.
4. Stock is only subtracted once a package is actually sent, so no resource can ever go below zero.

### 3. Deferral reasons

| Situation | Message |
|---|---|
| Score was 3 – 7 | *the need score is too low for a relief package right now* |
| Score was 8+ but no package could be supplied | *not enough resources left for any package* |

### Why these rules

> **TODO:** Explain why you chose 20 / 12 / 8 as the cut-off points, and why falling back to a smaller package is better than refusing the request outright.

---

## Relief Packages and Inventory

### Packages

| Package | Trucks | Supply packs | Water bottles | Food packets | Medicine kits |
|---|---|---|---|---|---|
| A. Full Relief | 2 | 50 | 100 | 50 | 25 |
| B. Standard Relief | 1 | 30 | 60 | 30 | 15 |
| C. Emergency Kit Only | 1 | 10 | 20 | 10 | 5 |
| D. Defer / Monitor | 0 | 0 | 0 | 0 | 0 |

Defined in the `PACKAGES` table in `src/engine.cpp`.

### Starting inventory

| Resource | Starting stock |
|---|---|
| Trucks | 20 |
| Supply packs | 120 |
| Water bottles | 500 |
| Food packets | 300 |
| Medicine kits | 200 |

Defined at the top of `src/inventory.cpp`. Change these numbers to test the program with sufficient or insufficient resources.

---

## Saved Requests (CSV)

Every request made with `get_help` is added as one line to **`data/requests.csv`**. The `data` folder is created automatically if it does not exist, and the header row is written only when the file is new.

**Columns:**

```
Time, Area, Damage Level, Families Affected, Route, Distance (km), Water Scarcity, Food Availability, Injured People, Previous Aid
```

**Example row:**

```
2026-10-05 20:01:06,"Riverside Village",High,150,Damaged,0,High,Low,40,No
```

Area names are wrapped in quotes so a comma inside a name does not break the columns. The file opens directly in Excel or Google Sheets.

---

## File Structure

```
CODE/
├── include/                 Header files (declarations)
│   ├── engine.h             HelpRequest, Level, Action, ReliefPackage, PACKAGES, engine functions
│   ├── inventory.h          Shared stock variables (extern) and showInventory()
│   ├── output.h             Colour constants and screen-drawing functions
│   ├── storage.h            saveRequest() and text helpers for the CSV
│   └── utility.h            cleanInput()
├── src/                     Source files (definitions)
│   ├── main.cpp             Program entry point and command loop
│   ├── engine.cpp           Input validation, need score, decision rules, dispatch, result screen
│   ├── inventory.cpp        Starting stock and the inventory table
│   ├── output.cpp           Colours, layout helpers and the instructions screen
│   ├── storage.cpp          Saves each request to data/requests.csv
│   └── utility.cpp          Lowercases input and strips spaces
├── data/
│   └── requests.csv         Saved requests (created when the program runs)
├── .vscode/                 VS Code build/debug settings
├── .gitignore               Keeps compiled .exe files out of Git
└── README.md                This file
```

### How the modules connect

```
main.cpp ──► engine.cpp ──► storage.cpp   (save request)
   │             │
   │             └────────► inventory.cpp (read/update stock)
   │
   ├──► inventory.cpp       (show_inventory)
   └──► output.cpp          (instructions, colours, bars - used by every module)
```

---

## Testing

### How to run the test sets

1. Set the starting stock in `src/inventory.cpp`.
2. Rebuild and run the program.
3. Enter each test request with `get_help`, then check `show_inventory`.

- **Test Set 1 (sufficient resources):** use the starting stock above, so every request can be served.
- **Test Set 2 (insufficient resources):** lower the stock (for example 5 trucks and 60 supply packs) so that later requests fall back to smaller packages or get deferred.

### Test requests

> **TODO:** The assignment's test set (R1–R5) only gives damage, families, route, flood warning, medical need and previous aid. This program also asks for distance, water scarcity, food availability and injured people, so choose and record values for those here.

| Request | Damage | Families | Route (km) | Water | Food | Injured | Prev. aid |
|---|---|---|---|---|---|---|---|
| R1 – Riverside Village | High | 150 | Damaged | _?_ | _?_ | _?_ | No |
| R2 – Hilltop Settlement | Medium | 60 | Open (_?_) | _?_ | _?_ | _?_ | Yes |
| R3 – Lakeside Camp | High | 90 | Open (_?_) | _?_ | _?_ | _?_ | No |
| R4 – Market Town | Low | 200 | Open (_?_) | _?_ | _?_ | _?_ | No |
| R5 – Bridge Colony | High | 40 | Damaged | _?_ | _?_ | _?_ | Yes |

### Results

> **TODO:** Paste the program output or fill in this table for both test sets.

| Request | Score | Recommended | Sent | Trucks used | Packs used | Trucks left | Packs left |
|---|---|---|---|---|---|---|---|
| R1 | | | | | | | |
| R2 | | | | | | | |
| R3 | | | | | | | |
| R4 | | | | | | | |
| R5 | | | | | | | |

**Summary:** _X_ requests served, _Y_ downgraded, _Z_ deferred.

---

## Flowchart

> **TODO:** Add the flowchart image (e.g. `docs/flowchart.png`) and link it here:
> `![Flowchart](docs/flowchart.png)`
>
> It should show: command loop → inputs and validation → need score → threshold decision → stock check and fallback loop → stock update → result.

---

## Known Limitations

- `remove_item` and `update_item` are recognised by `tocommands()` in `main.cpp` but not handled, so typing them currently does nothing.
- The deferral message for running out of stock says "trucks or supply packs" even when water, food or medicine is what ran out.
- `MAX_NEED_SCORE` in `engine.cpp` (used for the score meter) must be updated by hand if the formula changes.
- The worked example on the instructions screen is fixed text and must be updated if the prompts, formula or thresholds change.
- There is no end-of-session summary of how many requests were served or deferred yet (the assignment asks for one).
- The stock resets every time the program starts; only the requests are saved to disk.

---

## Use of AI

> **Draft. Review and edit this so it is accurate for your whole group.**

We used Claude (Anthropic) as a coding assistant during development, for:

- Setting up the Git repository and pushing it to GitHub
- Explaining compiler and linker errors (e.g. missing `std::`, `undefined reference`, `extern` variables)
- Writing helper code: the inventory table, the validated input functions, saving requests to CSV, the relief package table, the coloured result screen and the fallback message
- Finding a bug in our fallback loop (`while (relief)` skipped Full Relief because its value is `0`)
- Explaining concepts such as `extern`, references, CMake and CSV formatting
- Drafting this README

The **need score formula, its weights, and the decision thresholds were designed by the group.** AI was used to check the score range, not to choose the rules.
