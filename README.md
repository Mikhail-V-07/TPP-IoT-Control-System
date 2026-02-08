# TPP IoT Control System (Educational Project)

This repository contains a **console-based C++ project** developed as part of my first-semester studies.  
The goal of the project is **learning and practice**, not production readiness.

The system simulates sensor telemetry of a thermal power plant and provides basic analytical tools to work with this data.

All data is **artificially generated**. The project does **not** represent real industrial systems.

---

## Project Goals

This project was created to practice and better understand:

- basic C++ syntax and program structure  
- working with dynamic memory  
- binary and CSV file I/O  
- simple statistics and data aggregation  
- decomposition of a large task into modules  
- menu-driven console applications  

The main focus is **learning by building**, not perfect architecture or performance.

---

## General Idea

1. The program generates a synthetic history of sensor readings for a given number of days.
2. Data is stored in:
   - a binary file (`data.bin`) for internal processing  
   - a CSV file (`data.csv`) for external viewing  
3. The user interacts with the system through a console menu and can run different types of analysis on the generated data.

---

## Implemented Features

### Data Generation
- Simulation of multiple sensors with values, timestamps, and status codes  
- Continuous history generation starting from a user-defined date  

### Basic Statistics
- Minimum, maximum, average, median  
- Standard deviation  
- Status distribution (normal / warning / emergency)  

### Temporal Analysis
- Hour-of-day statistics  
- Day-of-week patterns  
- Monthly and seasonal aggregation  

### Anomaly Detection
- Simple threshold-based detection of abnormal values  

### Trend Analysis
- Basic linear regression to estimate trends over time  

### Correlation
- Correlation between two selected sensors  
- Optional time lag  
- Global correlation matrix  

### Unit Health Overview
- Sensors grouped into logical plant units  
- Summary health report per unit  

### Predictive Approximation
- Very simplified forecasting based on recent history  
- Intended only as a learning exercise  

---

## Project Structure (Conceptual)

- `generator` – synthetic telemetry generation  
- `storage` – binary and CSV file handling  
- `stats / analytics` – statistics and aggregation logic  
- `date` – custom date handling utilities  
- `menu / main` – user interaction and program flow  

The code favors **explicit logic** over advanced abstractions, as this project was written early in my studies.

---

## How to Run

The program is console-based.

### Windows
1. Download the executable from **Releases**
2. Run the `.exe`
3. Start by generating a new history when prompted

### macOS
1. Download the macOS build from **Releases**
2. Grant execution permission:
   ```bash
   chmod +x TPP_Analytics_Mac
3. Run:
  ```bash
  ./TPP_Analytics_Mac
```

Notes on Code Quality

This is an educational project

Some design decisions are intentionally simple

There are places that could be refactored or improved

Performance and scalability were not primary goals

The repository is published mainly to track learning progress and receive feedback.

---

#Use of AI Tools

During development, I used AI tools as a learning assistant to search for information, study materials, and explanations of concepts that were not yet covered in my university curriculum.

AI was used strictly as a support tool for learning, similar to documentation, textbooks, or online resources.

---

#Final Note

This is a small learning project, but it reflects my current level and the topics I am studying.
Any constructive feedback or suggestions are welcome.

---
