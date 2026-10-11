# Relief Supply Distribution System

A C++ console program that helps a district administration share limited relief supplies after a natural disaster such as a flood. For each request from an affected area, it asks a few questions, works out a **need score**, chooses a **relief package**, falls back to a smaller package when stock runs short, updates the inventory and saves the request to a CSV file.

Built for **CS 110: Fundamentals of Computer Programming, Assignment 1** (NUST SEECS).

**Repository:** https://github.com/Abdullah860259/focp-1

---

## Group Members

| Name | Contribution |
|---|---|
| Abdullah Anwar | Core engine architecture and overall control flow: request input and validation (`engine.cpp`), relief package table, stock checking and dispatch, fallback logic, saving requests to CSV (`storage.cpp`), GitHub repository setup |
| Hooria Mansoor | Inventory summary and relief request history (`inventory.cpp`, `storage.cpp`), command routing in `main.cpp`, updated instructions screen in `output.cpp`, compiled the assignment document |
| Abeera Ahsan | Flowcharts, the `status` and `update` commands (with admin password), new terminal output layout (boxed tables, cards, progress line, low-stock warnings), revised need score formula |

---

## Table of Contents

1. [Features](#features)
2. [Requirements](#requirements)
3. [Building and Running](#building-and-running)
4. [Commands](#commands)
5. [Making a Relief Request (`help`)](#making-a-relief-request-help)
6. [Other Commands in Detail](#other-commands-in-detail)
7. [Need Score Calculation](#need-score-calculation)
8. [Decision / Action Rules](#decision--action-rules)
9. [Relief Packages and Inventory](#relief-packages-and-inventory)
10. [Saved Requests (CSV)](#saved-requests-csv)
11. [File Structure](#file-structure)
12. [Flowchart](#flowchart)
13. [Testing](#testing)
14. [Known Limitations](#known-limitations)
15. [Use of AI](#use-of-ai)

---

## Features

- Colour-coded console interface with boxed tables, cards, a need-score meter and a dispatch progress line
- Input validation: every answer is checked and asked again until it is valid
- Need score built from 7 factors about the affected area
- Four relief packages, from Full Relief down to Defer / Monitor
- Automatic fallback to a smaller package when any resource runs short, with the exact missing items printed
- Stock can never go below zero
- Inventory screen with level bars and **LOW** stock warnings
- Distribution summary of every request processed in the session (served / deferred)
- Password-protected `update` command to add or remove stock
- Every request is saved to `data/requests.csv`
- Exit confirmation so the program is not closed by accident

---

## Requirements

- A C++ compiler with **C++11** support (developed with MinGW `g++` 6.3.0 on Windows)
- A terminal that supports ANSI colours and UTF-8 (Windows Terminal, PowerShell, VS Code terminal, Git Bash, or any Linux/macOS terminal)
- A terminal at least **80 characters wide** (the screens use a fixed 80-column layout)

---

## Building and Running

Run these commands from the project root (the folder that contains `src/` and `include/`).

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
g++ -std=c++11 -Wall -I ./include src/main.cpp src/engine.cpp src/inventory.cpp src/output.cpp src/storage.cpp src/utility.cpp -o build
```

### Run

```bash
./build
```

Run the program **from the project root**. The CSV file is saved relative to the folder you run it from, so it ends up in `./data/requests.csv`.

---

## Commands

Type a command at the `Enter a command (or 'exit' to quit):` prompt and press **Enter**. Capital letters and spaces are ignored, so `Help`, `HELP` and ` help ` all work.

| Command | What it does |
|---|---|
| `help` | Make a new relief request for an area |
| `resources` | Show the current stock of every resource, with level bars and low-stock warnings |
| `status` | Show the distribution summary: how many requests were served and deferred, and a table of every request |
| `update` | Add or remove stock *(admin only, password required)* |
| `history` (or `requests`) | Show where the saved request history is stored |
| `instructions` | Show the instructions screen again |
| `exit` | Quit the program (asks for confirmation first) |

Anything else shows an **Invalid command** message.

---

## Making a Relief Request (`help`)

`help` asks these questions in order:

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

After the questions, the request is saved to the CSV file and the program shows:

1. **Heading**: `REQUEST <number> - <AREA NAME>`
2. **Request summary card**: everything that was entered, colour-coded by severity
3. **Need assessment card**: the need score on a meter out of 25, the recommended package and what it needs
4. **Dispatch status card**: a progress line (`Requested → Processed → Dispatched/Deferred → On the way → Reached`), a red **NO** line for every package that had to be skipped with the exact items that were short, and a green **OK** line for the package that was sent
5. **Stock table**: before / used / left for all five resources
6. **Result bar**: `DISPATCHED: <package>` (with a note if it was downgraded) or `DEFERRED / MONITORING` with the reason
7. **Low-stock warnings** for any resource that is now below 25% of its starting stock

### Example

```
Enter a command (or 'exit' to quit): help
    Area name: Riverside Village
    Damage level (low/medium/high): high
    Families affected (10-200): 150
    Access route (open/damaged): damaged
    Water scarcity (low/medium/high): high
    Food availability (low/medium/high): low
    Number of injured people (0-1000): 40
    Previous aid received (yes/no): no

    Need score: 22 / 25
    DISPATCHED:  A. Full Relief
```

---

## Other Commands in Detail

### `resources`

Shows a table with one row per resource:

| Column | Meaning |
|---|---|
| RESOURCE | Trucks, Supply packs, Water bottles, Food packets, Medicine kits |
| IN STOCK | How many are left now |
| START | How many the program started with |
| LEVEL | A bar with one block per 10% of the starting stock (green ≥ 50%, yellow 25–49%, red < 25%) |
| STATUS | **LOW** if under 25% of the starting stock, otherwise **OK** |

Any LOW item is also listed underneath as `!! LOW STOCK: <item> (<left> of <start> left)`. Press **Enter** to return to the prompt.

### `status`

Shows the totals for this session (**Requests processed**, **Served**, **Deferred**) and a table of every request made with `help`:

| ID | AREA | SCORE | STATUS | PACKAGE |
|---|---|---|---|---|
| Request number | Area name (shortened if long) | Need score | SERVED or DEFERRED | Package sent |

If no requests have been made yet, it says so. This is the end-of-run summary required by the assignment.

### `update` (admin only)

1. Asks for the **admin password**: `admin@123`. Characters are shown as `*` on Windows. After **3 wrong tries**, access is denied and you return to the prompt.
2. **Choose action:** `1` = Add stock, `2` = Remove stock
3. **Select resource:** `1` Trucks, `2` Supply packs, `3` Water bottles, `4` Food packets, `5` Medicine kits
4. **Enter quantity:** a whole number from 1 to 1000

A green **UPDATED** box then shows the previous amount, the amount added or removed, and the new total. You cannot remove more than is in stock.

### `history` / `requests`

Shows a **Relief Request Statistics** banner with the location of the saved request file (`data/requests.csv`).

### `instructions`

Clears the screen and shows the welcome screen again: introduction, command menu, how to use, and a worked example.

### `exit`

Asks `Are you sure you want to exit? (y/n)`. Answering `y` / `yes` closes the program with a thank-you message; `n` / `no` returns to the prompt.

---

## Need Score Calculation

```
Need Score = Damage + Families + Route + Water + Food + Injured + Aid
```

| Factor | Rule | Points |
|---|---|---|
| Damage level | Low → 0, Medium → 3, High → 6 (`level × 3`) | 0 – 6 |
| Families affected | 10–50 → 1, 51–100 → 2, 101–200 → 3 | 1 – 3 |
| Access route | Open and ≤ 100 km → 1, Open and > 100 km → 2, Damaged → 3 | 1 – 3 |
| Water scarcity | Low → 0, Medium → 2, High → 4 (`level × 2`) | 0 – 4 |
| Food availability | High → 1, Medium → 2, Low → 3 (`3 − level`, less food means more need) | 1 – 3 |
| Injured people | 0 → 0, 1–100 → 1, 101–500 → 2, 501–1000 → 4 | 0 – 4 |
| Previous aid | Received → 0, Not received → 2 | 0 – 2 |
| **Total** | | **3 – 25** |

The formula uses all 7 inputs. The lowest possible score is **3** (low damage, few families, a short open route, plenty of food and water, no injuries, aid already received) and the highest is **25**.

### Why these weights

- **Damage level (up to 6), the heaviest factor.** Damage is the most direct measure of how badly an area has been hit: destroyed homes, roads, power and water systems. It drives almost every other need, so it is worth twice as much as any other single factor. Low damage gives 0 points because an area with little destruction usually has its own ways to cope.
- **Water scarcity (up to 4).** Clean water is the most urgent survival need after a flood. People can last only a few days without it, and contaminated water quickly spreads diseases such as cholera. That makes water scarcity more urgent than food, so it gets more points.
- **Injured people (up to 4).** Injuries mean lives are at immediate risk and medicine is needed now. An area with no injuries gets 0, so it is not pushed up the list unnecessarily. Over 500 injured is a mass-casualty situation, so the score jumps from 2 straight to 4 rather than 3.
- **Families affected (up to 3).** More families means more people to feed and shelter, so the score rises with size. It is grouped into three bands so a small difference (for example 99 vs 101 families) does not change the result much. It is weighted below damage and water because a small, badly damaged area can be in more danger than a large, lightly affected one.
- **Food availability (up to 3).** A lack of food is serious, but people can stretch food for longer than water, so it is weighted slightly lower than water. Even "high" food availability still gives 1 point, because food supply after a disaster is uncertain and can run out quickly.
- **Access route (up to 3).** A damaged route gives the most points because the area is cut off: people cannot leave to get help, and later deliveries will be hard, so it needs a bigger response now. An open route that is far away (over 100 km) still adds 2 points, because each extra trip takes longer and the area cannot be resupplied quickly.
- **Previous aid (0 or 2).** An area that has not received any aid yet gets 2 extra points, so help is shared fairly and areas that were missed get attention first. The bonus is small, so a severely affected area that already got some aid can still qualify for a large package.

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

1. If the recommended package can be fully supplied (enough of **all five** resources), it is sent.
2. If not, the program prints which items are short (for example `not enough packs (need 30, have 20)`) and tries the next smaller package: **A → B → C → D**.
3. If even C cannot be supplied, the request is **deferred**.
4. Stock is only subtracted once a package is actually sent, so no resource can ever go below zero.
5. The request is counted as **served** or **deferred** for the `status` summary.

### 3. Deferral reasons

| Situation | Message |
|---|---|
| Score was 3 – 7 | *the need score is too low for a relief package right now* |
| Score was 8 or more, but no package could be supplied | *not enough stock left for any package* |

### Why these rules

- **20 and above → Full Relief.** A score of 20 out of 25 can only be reached when several factors are severe at the same time, for example high damage *and* serious water shortage *and* many families or injuries. Full Relief uses the most stock, so it is kept for the most critical areas.
- **12 – 19 → Standard Relief.** This band covers areas with clear, serious needs but where not everything is at its worst, for example medium damage with water problems, or high damage in a small area. A standard package covers them without draining stock too fast.
- **8 – 11 → Emergency Kit.** At this level at least one factor is serious (such as medium damage plus a shortage of food or water), so the area needs something now, but a small kit is enough to cover immediate needs.
- **Below 8 → Defer / Monitor.** A score this low means every factor is mild: little damage, few people and no injuries. Sending supplies here would use stock that a more severe area may need later, so the area is monitored instead.
- **Falling back instead of refusing.** When stock is short, sending a smaller package is better than sending nothing, because the area still gets immediate help. Stepping down one package at a time means the area always gets the largest package that is still possible, and the program never promises supplies it does not have.

---

## Relief Packages and Inventory

### Packages

| Package | Trucks | Supply packs | Water bottles | Food packets | Medicine kits |
|---|---|---|---|---|---|
| A. Full Relief | 2 | 50 | 100 | 50 | 25 |
| B. Standard Relief | 1 | 30 | 60 | 30 | 15 |
| C. Emergency Kit Only | 1 | 10 | 20 | 10 | 5 |
| D. Defer / Monitor | 0 | 0 | 0 | 0 | 0 |

Defined in the `PACKAGES` table at the top of `src/engine.cpp`.

### Starting inventory

| Resource | Starting stock | Constant in `src/inventory.cpp` |
|---|---|---|
| Trucks | 20 | `START_TRUCKS` |
| Supply packs | 120 | `START_PACKS` |
| Water bottles | 500 | `START_WATER` |
| Food packets | 300 | `START_FOOD` |
| Medicine kits | 200 | `START_MEDICINE` |

Change these constants to test the program with sufficient or insufficient resources. A resource counts as **LOW** when it falls below 25% of its starting value (`LOW_STOCK_PERCENT`).

---

## Saved Requests (CSV)

Every request made with `help` is added as one line to **`data/requests.csv`**. The `data` folder is created automatically if it does not exist, and the header row is written only when the file is new.

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
│   ├── inventory.h          Shared stock variables and request counters (extern), inventory and status screens
│   ├── output.h             Colour constants, table, card and screen-drawing functions
│   ├── storage.h            saveRequest(), CSV text helpers, request history screen
│   └── utility.h            cleanInput(), upperCase(), shorten(), readPassword(), waitForEnter()
├── src/                     Source files (definitions)
│   ├── main.cpp             Program entry point, command loop and command routing
│   ├── engine.cpp           Input validation, need score, decision rules, dispatch, result screen,
│   │                        update command (admin password) and exit confirmation
│   ├── inventory.cpp        Starting stock, resources screen, low-stock warnings, status summary
│   ├── output.cpp           Colours, tables, cards, banners and the instructions screen
│   ├── storage.cpp          Saves each request to data/requests.csv, history screen
│   └── utility.cpp          Input cleaning, text helpers, password input, "Press Enter"
├── data/
│   └── requests.csv         Saved requests (created when the program runs)
├── .gitignore               Keeps compiled files and editor settings out of Git
└── README.md                This file
```

### How the modules connect

```
main.cpp ──► engine.cpp ──► storage.cpp     (save each request)
   │             │
   │             └────────► inventory.cpp   (check / update stock, record request for status)
   │
   ├──► inventory.cpp       (resources, status)
   ├──► storage.cpp         (history)
   └──► output.cpp          (instructions, colours, tables, cards - used by every module)
```


## Testing

### Test requests

The assignment's request set (R1–R5) gives damage, families, route, flood warning, medical need and previous aid. Our program asks for some different inputs, so the missing values were filled in like this:

- **Medical need → injured people:** requests with medical need *Yes* were given a number of injured people; requests with *No* were given 0.
- **Flood warning → water scarcity:** requests with a flood warning were given *High* water scarcity (flood water contaminates supplies); the others *Low* or *Medium*.
- **Food availability:** *Low* for the hardest-hit areas, otherwise *Medium* or *High*.
- **Distance** for open routes: a realistic value under 100 km.

| Request | Damage | Families | Route | Water | Food | Injured | Prev. aid | Need score |
|---|---|---|---|---|---|---|---|---|
| R1 – Riverside Village | High | 150 | Damaged | High | Low | 120 | No | **23** |
| R2 – Hilltop Settlement | Medium | 60 | Open (40 km) | Low | Medium | 0 | Yes | **8** |
| R3 – Lakeside Camp | High | 90 | Open (30 km) | High | Low | 60 | No | **19** |
| R4 – Market Town | Low | 200 | Open (15 km) | Low | High | 0 | No | **7** |
| R5 – Bridge Colony | High | 40 | Damaged | High | Low | 30 | Yes | **18** |

**Score breakdown** (Damage + Families + Route + Water + Food + Injured + Aid):

| Request | Calculation | Score | Recommended |
|---|---|---|---|
| R1 | 6 + 3 + 3 + 4 + 3 + 2 + 2 | 23 | A. Full Relief |
| R2 | 3 + 2 + 1 + 0 + 2 + 0 + 0 | 8 | C. Emergency Kit Only |
| R3 | 6 + 2 + 1 + 4 + 3 + 1 + 2 | 19 | B. Standard Relief |
| R4 | 0 + 3 + 1 + 0 + 1 + 0 + 2 | 7 | D. Defer / Monitor |
| R5 | 6 + 1 + 3 + 4 + 3 + 1 + 0 | 18 | B. Standard Relief |

### How to run the test sets

1. Set the starting stock constants in `src/inventory.cpp`.
2. Rebuild and run the program.
3. Enter R1 to R5 in order with `help`.
4. Type `status` to see the summary and `resources` to see the remaining stock.

### Test Set 1: sufficient resources

Starting stock: **20 trucks, 120 packs, 500 water, 300 food, 200 medicine** (the default values).

| Request | Score | Recommended | Sent | Used (trucks / packs / water / food / medicine) | Left (trucks / packs / water / food / medicine) |
|---|---|---|---|---|---|
| R1 | 23 | A. Full Relief | A. Full Relief | 2 / 50 / 100 / 50 / 25 | 18 / 70 / 400 / 250 / 175 |
| R2 | 8 | C. Emergency Kit | C. Emergency Kit | 1 / 10 / 20 / 10 / 5 | 17 / 60 / 380 / 240 / 170 |
| R3 | 19 | B. Standard Relief | B. Standard Relief | 1 / 30 / 60 / 30 / 15 | 16 / 30 / 320 / 210 / 155 |
| R4 | 7 | D. Defer | **Deferred** (score too low) | 0 / 0 / 0 / 0 / 0 | 16 / 30 / 320 / 210 / 155 |
| R5 | 18 | B. Standard Relief | B. Standard Relief | 1 / 30 / 60 / 30 / 15 | 15 / 0 / 260 / 180 / 140 |

**Summary (`status`):** 5 requests processed, **4 served**, **1 deferred**. Every request got the package its score recommended; R4 was deferred only because its need was low. After R5, supply packs reach 0 and show as **LOW**.

### Test Set 2: insufficient resources

Starting stock changed to: **4 trucks, 80 packs** (`START_TRUCKS = 4`, `START_PACKS = 80`; water, food and medicine unchanged).

| Request | Score | Recommended | What happened | Sent | Left (trucks / packs) |
|---|---|---|---|---|---|
| R1 | 23 | A. Full Relief | Enough stock | A. Full Relief | 2 / 30 |
| R2 | 8 | C. Emergency Kit | Enough stock | C. Emergency Kit | 1 / 20 |
| R3 | 19 | B. Standard Relief | `NO B. Standard Relief - not enough packs (need 30, have 20)` → falls back | **C. Emergency Kit** (downgraded) | 0 / 10 |
| R4 | 7 | D. Defer | Score too low | **Deferred** | 0 / 10 |
| R5 | 18 | B. Standard Relief | `NO B. Standard Relief - not enough trucks (need 1, have 0), packs (need 30, have 10)`<br/>`NO C. Emergency Kit Only - not enough trucks (need 1, have 0)` | **Deferred** (not enough stock) | 0 / 10 |

Water, food and medicine after Test Set 2: 360 water, 230 food, 165 medicine.

**Summary (`status`):** 5 requests processed, **3 served** (one of them downgraded), **2 deferred** (R4 because of a low score, R5 because stock ran out). Trucks reach 0 and both trucks and packs show as **LOW**. Stock never goes below zero.

These results were worked out by tracing the code by hand: the need score formula, the 20 / 12 / 8 thresholds, and the A → B → C → D fallback loop in `getHelp()`.

---

## Known Limitations

- The stock and the `status` summary reset every time the program starts; only the requests themselves are saved to disk.
- `history` only shows where the CSV file is; it does not read the file back or count the saved requests.
- `MAX_NEED_SCORE` in `engine.cpp` (used for the score meter) must be updated by hand if the formula changes.
- The worked example on the instructions screen is fixed text, so it must be updated if the prompts, formula or thresholds change.
- The admin password is stored in the source code, which is fine for a classroom project but not for real use.
- The `history` command is not listed on the instructions screen.

---

## Use of AI

We used Claude (Anthropic) as a helper during development, for:

- Learning about git commands ,github and version control;
- Explaining compiler and linker errors (for example a missing `std::`, `undefined reference`, `extern` variables)
- Understanding ANSII codes and varios other C++ concepts
- Explaining concepts such as `extern`, references, CMake and CSV formatting
- Proofreading this README and our codebase.
